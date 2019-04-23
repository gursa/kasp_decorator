#include <algorithm>
#include <iostream>
#include "kasp_db_mock.h"

kasp::db_mock::db_mock()
{
    std::cout << __FUNCTION__ << std::endl;
}

kasp::db_mock::~db_mock()
{
    m_records->clear();
    std::cout << __FUNCTION__ << std::endl;
}

std::string kasp::db_mock::get(const std::string &key)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << std::endl;
    auto search = m_records->find(key);
    if (search != m_records->end())
    {
        std::string data = search->second;
        std::cout << __FUNCTION__ << ": data = " << data.c_str() << std::endl;
        return data;
    }
    std::cerr << __FUNCTION__ << ": could not find key " << key.c_str() << std::endl;
    return std::string();
}

void kasp::db_mock::put(const std::string &key, const std::string &data)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << "\tdata = " << data.c_str() << std::endl;
    m_records->insert(std::pair<std::string, std::string>(key, data));
}

void kasp::db_mock::remove(const std::string &key)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << std::endl;
    auto search = m_records->find(key);
    if (search != m_records->end())
        m_records->erase(search);
}
