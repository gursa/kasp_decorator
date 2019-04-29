#include <future>
#include <queue>
#include <boost/log/trivial.hpp>
#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

kasp::decorator m_db(new kasp::db_mock(), std::chrono::milliseconds(2000));

void client(int count, char symbol)
{
    for (int i = 1; i < count; ++i)
    {
        std::string key(i, symbol);
        std::string data(i, symbol+1);

        m_db.put(key, data);
    }
}

void service(int count, char symbol)
{
    for (int i = 1; i < count; ++i)
    {
        std::string key(i, symbol);
        std::string data = m_db.get(key, std::chrono::milliseconds(300));
        BOOST_LOG_TRIVIAL(info) << "[" << key << "," << data << "]\n";
    }
}

int main(int argc, char* argv[])
{

    auto p1 = std::async(std::launch::async, &client, 5, 'A');
    auto p2 = std::async(std::launch::async, &client, 5, 'C');
    auto p3 = std::async(std::launch::async, &client, 5, 'E');
    auto p4 = std::async(std::launch::async, &client, 5, 'J');
    auto p5 = std::async(std::launch::async, &client, 5, 'K');
    auto p6 = std::async(std::launch::async, &client, 5, 'M');


    auto c1 = std::async(std::launch::async, &service, 5, 'A');
    auto c2 = std::async(std::launch::async, &service, 5, 'C');

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    p1.get();
    p2.get();
    p3.get();
    p4.get();
    p5.get();
    p6.get();
    c1.get();
    c2.get();
    return 0;
}
