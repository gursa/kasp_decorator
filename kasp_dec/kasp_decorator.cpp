#include <boost/log/trivial.hpp>
#include <utility>
#include <future>
#include <thread>
#include <chrono>
#include "kasp_decorator.h"

kasp::decorator::decorator(kasp::db_interface *db, const std::chrono::milliseconds db_timeout)
    : m_db(db),
      m_timer(new kasp::timer()),
      m_thread_pool(new kasp::thread_pool(5))
{
    BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << std::endl;

    m_thread_pool->run_task(
                [&, this]()
                {
                    while(true)
                    {
                        auto request = m_request_queue.take();

                        if(request->type == base::request_type::req_get)
                        {
                            auto is_search = m_cache.find(request->key, request->data);
                            if(is_search)
                            {
                                std::string db_data = m_db->get(request->key, db_timeout);
                                if(db_data.empty())
                                {
                                    m_db->put(request->key, request->data);
                                }
                                m_request_queue.done(request);
                            }
                            else
                            {
                                std::string db_data = m_db->get(request->key, db_timeout);
                                if(!db_data.empty())
                                {
                                    this->put(request->key, db_data);
                                    m_request_queue.done(request);
                                }
                            }

                        }
                        else if(request->type == base::request_type::req_put)
                        {
                            m_cache.put(request->key, request->data);
                            m_request_queue.done(request);
                        }
                        else
                        {
                            m_cache.remove(request->key);
                            m_request_queue.done(request);
                        }
                    }
                }
                );

    m_timer->set_timeout(
                [this]()
                {
                    BOOST_LOG_TRIVIAL(info) << "Copy cache to database ...";
                    int rec_count = m_cache.copy(m_db.get());
                    BOOST_LOG_TRIVIAL(info) << "Copy is success! Saved " << rec_count << " records" << std::endl;
                },
                db_timeout);
}

kasp::decorator::~decorator()
{
    m_cache.clear();
}

std::string kasp::decorator::get(const std::string &key, const std::chrono::milliseconds get_timeout)
{
    return m_request_queue.call(key, "", base::request_type::req_get, std::chrono::milliseconds(get_timeout));
}

void kasp::decorator::put(const std::string &key, const std::string &data)
{
    m_request_queue.call(key, data, base::request_type::req_put, std::chrono::milliseconds(0xFFFFFFFF));
}

void kasp::decorator::remove(const std::string &key)
{
    m_request_queue.call(key, "", base::request_type::req_remove, std::chrono::milliseconds(0xFFFFFFFF));
}
