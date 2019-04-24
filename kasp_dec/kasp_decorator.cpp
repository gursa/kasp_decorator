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
        auto timeout_result = search->second->m_event->wait(std::chrono::seconds(10));//temp_event.wait(std::chrono::seconds(10));
        if(!timeout_result)
            throw std::runtime_error("we have timeout");

        search->second->m_event->reset();

        std::string db_data = m_db->get(key);
        if(db_data.empty())
        {
            m_db->put(key, search->second->m_record->data);
        }

        std::cout << __FUNCTION__ << ": data = " << search->second->m_record->data.c_str() << std::endl;
        search->second->m_event->signal();
        return search->second->m_record->data;
    }

    std::string db_data = m_db->get(key);
    if(db_data.empty())
    {
        std::cerr << __FUNCTION__ << ": could not find key " << key.c_str() << std::endl;
        return std::string();
    }

    this->put(key, db_data);
    return db_data;
}

void kasp::decorator::put(const std::string &key, const std::string &data)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << "\tdata = " << data.c_str() << std::endl;
    auto temp_rec = std::unique_ptr<kasp::records_event>(new kasp::records_event());
    temp_rec->m_event->reset();
    temp_rec->m_record->key = key;
    temp_rec->m_record->data = data;
    temp_rec->m_event->signal();
    m_cache->insert(std::make_pair(key, std::move(temp_rec)));
}

void kasp::decorator::remove(const std::string &key)
{
    std::cout << __FUNCTION__ << ": key = " << key.c_str() << std::endl;
    auto search = m_cache->find(key);
    if (search != m_cache->end())
        m_cache->erase(search);
    m_db->remove(key);
}
