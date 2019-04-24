#include <iostream>
#include <future>
#include <queue>
#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

class test_keys
{
public:
    test_keys() : m_db(new kasp::db_mock(), 5, 10){}
    void start()
    {
        auto p1 = std::async(std::launch::async, &test_keys::producer, this, 10, 'A', 'Z');
        auto p2 = std::async(std::launch::async, &test_keys::producer, this, 20, 'B', 'Y');
        auto p3 = std::async(std::launch::async, &test_keys::producer, this, 30, 'C', 'X');

        auto c1 = std::async(std::launch::async, &test_keys::consumer, this);
        auto c2 = std::async(std::launch::async, &test_keys::consumer, this);
    }
private:
    void producer(std::size_t count, char key_symbol, char data_symbol)
    {
        for(std::size_t i = 1; i < count; ++i)
        {
            std::string key(i, key_symbol);
            std::string data(i, data_symbol);
            {
                std::lock_guard<std::mutex> lg(m_mtx);
                m_keys.push(key);
                std::cout << "producer [key, data] = [" << key.c_str() << ", " << data.c_str() << "]" << std::endl;
            }
            m_db.put(key, data);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        m_cv.notify_one();
    }

    void consumer()
    {
        while(true)
        {
            std::string key;
            {
                std::unique_lock<std::mutex> ul(m_mtx);
                m_cv.wait(ul,
                        [this]()
                        {
                            return !m_keys.empty();
                        }
                );
                key = m_keys.front();
                m_keys.pop();
                try
                {
                    std::string data = m_db.get(key);
                    std::cout << "consumer [key, data] = [" << key.c_str() << ", " << data.c_str() << "]" << std::endl;
                }
                catch (std::exception &ex)
                {
                    std::cerr << ex.what() << std::endl;
                }
            }

        }
    }
private:
    std::queue<std::string> m_keys;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    kasp::decorator m_db;
};



int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    test_keys test;
    test.start();

    return 0;
}
