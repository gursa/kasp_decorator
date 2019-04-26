#include <iostream>
#include <future>
#include <queue>
#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

namespace base
{

struct message
{
    //std::string m_data;
    std::string m_key;
    std::string m_data;
    bool m_flag;
    message(const std::string &key, const std::string &data, const bool flag)
        : m_key(key),
          m_data(data),
          m_flag(flag)
    {}
};

class msg_queue
{
public:
    void put(std::shared_ptr<base::message> msg)
    {
        {
            std::lock_guard<std::mutex> lg(m_mutex);
            m_queue.push(msg);
            m_cv.notify_one();
        }

        std::unique_lock<std::mutex> ul(m_mutex);
        m_cv2.wait(ul,
                [&]()
                {
                    return msg->m_flag;
                }
        );
    }
    std::shared_ptr<base::message> take()
    {
        std::shared_ptr<base::message> result;
        std::unique_lock<std::mutex> ul(m_mutex);
        m_cv.wait(ul,
                [&, this]()
                {
                    if(!m_queue.empty())
                    {
                        result = m_queue.front();
                        m_queue.pop();
                        return true;
                    }
                    return false;
                }
        );
        return result;
    }

    void ready(std::shared_ptr<base::message> request)
    {
        request->m_flag = true;
        m_cv2.notify_all();
    }
private:
    std::queue<std::shared_ptr<base::message>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::condition_variable m_cv2;
};

class cache
{
public:
    void put(const std::string &key, const std::string &value)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_cache[key] = value;
    }
    bool find(const std::string &key, std::string &value)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto it = m_cache.find(key);
        if(it == m_cache.end())
        {
            value = std::string();
            return false;
        }

        value = it->second;
        return true;
    }
    void remove(const std::string &key)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto it = m_cache.find(key);
        if(it == m_cache.end())
            m_cache.erase(it);
    }
private:
    std::map<std::string, std::string> m_cache;
    std::mutex m_mutex;
};

}

base::msg_queue m_queue;
std::condition_variable m_cv;
std::mutex m_mutex;

void producer(int count, char in)
{
    for(int i = 1; i < count; ++i)
    {
        std::string key(i, in);
        auto request = std::make_shared<base::message>(key, "", false);
        m_queue.put(request);
        std::cout << "producer in = " << request->m_key.c_str()  << "\t out = " << request->m_data.c_str() << std::endl;
    }
}

int consumer()
{
    while(true)
    {
        auto request = m_queue.take();
        request->m_data = request->m_key + "Z";
        m_queue.ready(request);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    auto p1 = std::async(std::launch::async, &producer, 5, 'A');
    auto p2 = std::async(std::launch::async, &producer, 5, 'B');
    auto p3 = std::async(std::launch::async, &producer, 5, 'C');
    auto p4 = std::async(std::launch::async, &producer, 5, 'D');
    auto p5 = std::async(std::launch::async, &producer, 5, 'E');
    auto p6 = std::async(std::launch::async, &producer, 5, 'F');


    auto c1 = std::async(std::launch::async, &consumer);
    auto c2 = std::async(std::launch::async, &consumer);
    return 0;
}

//#include <boost/asio.hpp>
//
//boost::asio::io_service io_service;
//boost::posix_time::seconds interval(5);  // 1 second
//boost::asio::deadline_timer timer(io_service, interval);
//
//void tick(const boost::system::error_code& /*e*/) {
//
//    std::cout << "tick" << std::endl;
//
//    // Reschedule the timer for 1 second in the future:
//    timer.expires_at(timer.expires_at() + interval);
//    // Posts the timer event
//    timer.async_wait(tick);
//}
//
//int main(void) {
//
//    // Schedule the timer for the first time:
//    timer.async_wait(tick);
//    // Enter IO loop. The timer will fire for the first time 1 second from now:
//    io_service.run();
//    return 0;
//}
