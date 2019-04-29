#include <future>
#include <queue>
#include <boost/log/trivial.hpp>
#include "kasp_sdk.h"
#include "kasp_db_mock.h"
#include "kasp_decorator.h"

void client(kasp::decorator *db, std::size_t count, char symbol)
{
    for (std::size_t i = 1; i <= count; ++i)
    {
        std::string key(i, symbol);
        std::string data(i, symbol+1);
        db->put(key, data);
        switch (i) {
        case 100:
        case 200:
        case 300:
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            break;
        }
    }
}

void service(kasp::decorator *db, std::size_t count, char symbol)
{
    for (std::size_t i = 1; i < count; ++i)
    {
        std::string key(i, symbol);
        std::string data = db->get(key, std::chrono::milliseconds(300));
        BOOST_LOG_TRIVIAL(info) << "[" << key << "," << data << "]\n";
    }
}

int main(int argc, char* argv[])
{
    boost::asio::io_context io;
    kasp::decorator m_db(new kasp::db_mock(),io, std::chrono::milliseconds(500));

    std::vector<std::future<void>> clients;
    for (char i = 'A'; i < 'N'; i += 2)
    {
        clients.push_back(std::async(std::launch::async, &client, &m_db, 300, i));
    }
    std::vector<std::future<void>> services;
    for (char i = 'A'; i < 'D'; i += 2)
    {
        services.push_back(std::async(std::launch::async, &service, &m_db, 300, i));
    }

    io.run();
    return 0;
}
