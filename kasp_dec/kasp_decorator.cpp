#include <utility>
#include <future>
#include <thread>
#include <chrono>
#include "kasp_decorator.h"

kasp::decorator::decorator(kasp::db_interface *db, const int get_timeout, const int db_timeout)
    : m_timeout(get_timeout),
      m_db(db),
      m_timer(new kasp::timer()),
      m_thread_pool(new kasp::thread_pool(5))
{
    std::cout << __FUNCTION__ << std::endl;

    m_thread_pool->run_task(
                [this]()
                {
                    while(true)
                    {
                        std::shared_ptr<kasp::request_item> request;
                        {
                            std::unique_lock<std::mutex> ul(m_request_mutex);
                            m_request_cv.wait(ul,
                                    [this]()
                                    {
                                        return !m_request_queue.empty();
                                    }
                            );
                            request = m_request_queue.front();
                            m_request_queue.pop();
                        }
                        if(request->m_type == kasp::request_type::req_get)
                        {
                            std::unique_lock<std::mutex> ul(m_cache_mutex);
                            if(m_cache_cv.wait_for(ul, std::chrono::seconds(request->m_timeout)) != std::cv_status::timeout)
                            {
                                auto search = m_cache.find(request->m_rec.key);
                                if (search != m_cache.end())
                                {
                                    auto timeout_result = search->second->m_event->wait(std::chrono::seconds(request->m_timeout));
                                    if(timeout_result)
                                    {
                                        search->second->m_event->reset();

                                        std::string db_data = m_db->get(request->m_rec.key, request->m_timeout);
                                        if(db_data.empty())
                                        {
                                            m_db->put(request->m_rec.key, search->second->m_record->data);
                                        }

                                        search->second->m_event->signal();

                                        std::lock_guard<std::mutex> lg(m_get_mutex);
                                        m_get_queue.push(std::make_shared<kasp::request_item>(search->second->m_record->key, search->second->m_record->data, kasp::request_type::req_get, request->m_timeout));
                                        m_get_cv.notify_one();
                                    }
                                }
                                else
                                {
                                    std::string db_data = m_db->get(request->m_rec.key, request->m_timeout);
                                    if(!db_data.empty())
                                    {
                                        this->put(request->m_rec.key, db_data);
                                        std::lock_guard<std::mutex> lg(m_get_mutex);
                                        m_get_queue.push(std::make_shared<kasp::request_item>(request->m_rec.key, db_data, kasp::request_type::req_get, request->m_timeout));
                                        m_get_cv.notify_one();
                                    }
                                }
                            }
                        }
                        else if(request->m_type == kasp::request_type::req_put)
                        {

                        }
                        else
                        {

                        }
                    }
                }
                );
    //m_thread_pool.

    m_timer->set_timeout(
                [this]()
                {
                    std::cout << "Copy cache to database ..." << std::endl;
                    int rec_count = 0;
                    auto it = m_cache.begin();
                    while(it != m_cache.end())
                    {
                        it->second->m_event->reset();
                        std::string db_data = m_db->get(it->first, 10);
                        if(it->second->m_record->data.compare(db_data))
                        {
                            m_db->put(it->first, it->second->m_record->data);
                            ++rec_count;
                        }
                        it->second->m_event->signal();
                        ++it;
                    }
                    std::cout << "Copy is success! Saved " << rec_count << " records" << std::endl;
                },
                db_timeout);
}

kasp::decorator::~decorator()
{
    //std::cout << __FUNCTION__ << std::endl;
    m_cache.clear();
}

std::string kasp::decorator::get(const std::string &key, const int get_timeout)
{
    std::future<std::string> future = std::async(
                                                std::launch::async,
                                                [&,this]()
                                                {
                                                    {
                                                        std::lock_guard<std::mutex> lg(m_request_mutex);
                                                        m_request_queue.push(std::make_shared<kasp::request_item>(key, "", kasp::request_type::req_get, get_timeout));
                                                    }
                                                    m_request_cv.notify_one();

                                                    std::shared_ptr<kasp::request_item> answer;
                                                    {
                                                        std::unique_lock<std::mutex> ul(m_get_mutex);
                                                        if(m_get_cv.wait_for(ul, std::chrono::seconds(get_timeout)) != std::cv_status::timeout)
                                                        {
                                                            answer = m_get_queue.front();
                                                            m_get_queue.pop();

                                                            return answer->m_rec.data;
                                                        }
                                                    }
                                                    return std::string();
                                                });

    std::future_status status;
    std::string error_info(__FUNCTION__);
    do
    {
        status = future.wait_for(std::chrono::seconds(get_timeout));
        if (status == std::future_status::deferred)
        {
            error_info.append(": async is deferred!");
            throw std::runtime_error(error_info.c_str());
        }
        else if (status == std::future_status::timeout)
        {
            error_info.append(": timeout!");
            throw std::runtime_error(error_info.c_str());
        }
    } while (status != std::future_status::ready);

    return future.get();
}

void kasp::decorator::put(const std::string &key, const std::string &data)
{
    //std::cout << __FUNCTION__ << ": key = " << key.c_str() << "\tdata = " << data.c_str() << std::endl;
    auto temp_rec = std::unique_ptr<kasp::records_event>(new kasp::records_event());
    temp_rec->m_event = std::unique_ptr<kasp::event>(new kasp::event());
    temp_rec->m_record = std::unique_ptr<kasp::records>(new kasp::records());
    temp_rec->m_event->reset();
    temp_rec->m_record->key = key;
    temp_rec->m_record->data = data;
    temp_rec->m_event->signal();
    m_cache.insert(std::make_pair(key, std::move(temp_rec)));


    std::future<int> future = std::async(
                                                std::launch::async,
                                                [&,this]()
                                                {
                                                    {
                                                        std::lock_guard<std::mutex> lg(m_request_mutex);
                                                        m_request_queue.push(std::make_shared<kasp::request_item>(key, data, kasp::request_type::req_put, 0));
                                                    }
                                                    m_request_cv.notify_one();
                                                    return 0;
                                                });

    future.get();
}

void kasp::decorator::remove(const std::string &key)
{
    //std::cout << __FUNCTION__ << ": key = " << key.c_str() << std::endl;
    auto search = m_cache.find(key);
    if (search != m_cache.end())
        m_cache.erase(search);
    m_db->remove(key);
}
