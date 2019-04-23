#include <utility>
#include "kasp_decorator.h"

kasp::decorator::decorator(kasp::db_interface *db)
    : m_db(db)
{
    std::cout << __FUNCTION__ << std::endl;
}

kasp::decorator::~decorator()
{
    std::cout << __FUNCTION__ << std::endl;
    m_cache->clear();
}

std::string kasp::decorator::get(const std::string &key)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << std::endl;
    auto search = m_cache->find(key);
    if (search != m_cache->end())
    {
        std::string data = search->second;
        std::string db_data = m_db->get(key);
        if(db_data.empty())
            m_db->put(key, data);

        std::cout << __FUNCTION__ << ": data = " << data.c_str() << std::endl;
        return data;
    }

    std::string db_data = m_db->get(key);
    if(db_data.empty())
    {
        std::cerr << __FUNCTION__ << ": could not find key " << key.c_str() << std::endl;
        return std::string();
    }
    m_cache->insert(std::pair<std::string, std::string>(key, db_data));
    return db_data;
}

void kasp::decorator::put(const std::string &key, const std::string &data)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << "\tdata = " << data.c_str() << std::endl;
    m_cache->insert(std::pair<std::string, std::string>(key, data));
}

void kasp::decorator::remove(const std::string &key)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << std::endl;
    auto search = m_cache->find(key);
    if (search != m_cache->end())
        m_cache->erase(search);
    m_db->remove(key);
}
