#include <iostream>
#include <future>
#include <queue>
#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

//class test_keys
//{
//public:
//    test_keys() : m_queue(new base::msg_queue()){}
//    void start()
//    {
//        auto p1 = std::async(std::launch::async, &test_keys::producer, this, 20, 'A');
//        auto p2 = std::async(std::launch::async, &test_keys::producer, this, 20, 'B');
//        auto p3 = std::async(std::launch::async, &test_keys::producer, this, 20, 'C');
//        auto p4 = std::async(std::launch::async, &test_keys::producer, this, 20, 'D');
//        auto p5 = std::async(std::launch::async, &test_keys::producer, this, 20, 'E');
//        auto p6 = std::async(std::launch::async, &test_keys::producer, this, 20, 'F');
//
//
//        auto c1 = std::async(std::launch::async, &test_keys::consumer, this);
//        auto c2 = std::async(std::launch::async, &test_keys::consumer, this);
//    }
//private:
//    void producer(std::size_t count, char key_symbol)
//    {
//        for(std::size_t i = 1; i < count; ++i)
//        {
//            std::string key(i, key_symbol);
//            {
//                std::shared_ptr<base::message> temp = std::make_shared<base::message>(key);
//                m_queue->put(temp);
//            }
//            m_cv.notify_one();
//            std::this_thread::sleep_for(std::chrono::seconds(2));
//        }
//    }
//
//    void consumer()
//    {
//        while(true)
//        {
//            {
//                std::unique_lock<std::mutex> ul(m_mtx);
//                m_cv.wait(ul,
//                        [this]()
//                        {
//                            return !m_queue->empty();
//                        }
//                );
//                auto key = m_queue->pop();
//                std::cout << "consumer key = " << key->m_data.c_str() << std::endl;
//            }
//
//        }
//    }
//private:
//    std::mutex m_mtx;
//    std::condition_variable m_cv;
//    std::shared_ptr<base::msg_queue> m_queue;
//};

std::mutex m_mtx;
std::condition_variable m_cv;
std::shared_ptr<base::msg_queue> m_queue;

void producer(std::size_t count, char key_symbol)
{
    for(std::size_t i = 1; i < count; ++i)
    {
        std::string key(i, key_symbol);
        {
            std::shared_ptr<base::message> temp = std::make_shared<base::message>(key);
            m_queue->put(temp);
        }
        m_cv.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void consumer()
{
    while(true)
    {
        {
            std::unique_lock<std::mutex> ul(m_mtx);
            m_cv.wait(ul,
                    []()
                    {
                        return !m_queue->empty();
                    }
            );
            auto key = m_queue->pop();
            std::cout << "consumer key = " << key->m_data.c_str() << std::endl;
        }

    }
}

int main(int argc, char* argv[])
{
    m_queue = std::make_shared<base::msg_queue>();

    auto p1 = std::async(std::launch::async, &producer, 20, 'A');
    auto p2 = std::async(std::launch::async, &producer, 20, 'B');
    auto p3 = std::async(std::launch::async, &producer, 20, 'C');
    auto p4 = std::async(std::launch::async, &producer, 20, 'D');
    auto p5 = std::async(std::launch::async, &producer, 20, 'E');
    auto p6 = std::async(std::launch::async, &producer, 20, 'F');


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
