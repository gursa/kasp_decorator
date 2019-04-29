#include <future>
#include <queue>
#include <boost/log/trivial.hpp>
#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

int main(int argc, char* argv[])
{
    boost::asio::io_context io;
    kasp::decorator m_db(new kasp::db_mock(),io, std::chrono::milliseconds(500));

    auto client = [](kasp::decorator &db, std::size_t count, char symbol)
    {
        for (std::size_t i = 1; i <= count; ++i)
        {
            std::string key(i, symbol);
            std::string data(i, symbol+1);
            db.put(key, data);
            if(i % 100 == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    };

    auto service = [](kasp::decorator &db, std::size_t count, char symbol, std::chrono::milliseconds timeout)
    {
        for (std::size_t i = 1; i < count; ++i)
        {
            std::string key(i, symbol);
            std::string data = db.get(key, timeout);
            BOOST_LOG_TRIVIAL(info) << "db.get = [" << key << "," << data << "]";
        }
    };

    std::vector<std::future<void>> clients_services;
    for (char i = 'A'; i < 'N'; i += 2)
    {
        clients_services.push_back(std::async(std::launch::async, client, std::ref(m_db), 10, i));
    }

    for (char i = 'A'; i < 'D'; i += 2)
    {
        clients_services.push_back(std::async(std::launch::async, service, std::ref(m_db), 10, i, std::chrono::milliseconds(300)));
    }

    clients_services.push_back(std::async(std::launch::async, service, std::ref(m_db), 10, 'Y', std::chrono::milliseconds(10)));

    io.run();
    return 0;
}
