#include <algorithm>
#include <boost/log/trivial.hpp>
#include "kasp_db_mock.h"

kasp::db_mock::db_mock()
{
}

kasp::db_mock::~db_mock()
{
    m_records.clear();
}

std::string kasp::db_mock::get(const std::string &key, const std::chrono::milliseconds get_timeout)
{
    try
    {
        std::string data;
        m_records.find(key, data);
        return data;
    }
    catch(std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << "[Exception] " << ex.what() << std::endl;
    }
}

void kasp::db_mock::put(const std::string &key, const std::string &data)
{
    try {
        m_records.put(key, data);
    }
    catch(std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << "[Exception] " << ex.what() << std::endl;
    }

}

void kasp::db_mock::remove(const std::string &key)
{
    try {
        m_records.remove(key);
    }
    catch(std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << "[Exception] " << ex.what() << std::endl;
    }

}
