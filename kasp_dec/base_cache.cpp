#include <boost/log/trivial.hpp>
#include "base_cache.h"


void base::cache::put(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_cache[key] = value;
}

bool base::cache::find(const std::string &key, std::string &value)
{
    try
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
    catch(std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << "[Exception] " << ex.what() << std::endl;
    }
    return false;
}

void base::cache::remove(const std::string &key)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto it = m_cache.find(key);
    if(it == m_cache.end())
        m_cache.erase(it);
}

void base::cache::clear()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_cache.clear();
}

int base::cache::copy(kasp::db_interface *db)
{
    try
    {
        std::unique_lock<std::mutex> ul(m_mutex);
        int rec_count = 0;
        for(auto const& item: m_cache)
        {
            std::string db_data = db->get(item.first, std::chrono::milliseconds(1000));
            std::string data = item.second;
            if(data.compare(db_data))
            {
                db->put(item.first, data);
                ++rec_count;
                BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " data = " << data.c_str() << std::endl;
            }
        }
        return rec_count;
    }
    catch (std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << "[Exception] " << ex.what() << std::endl;
    }
    return 0;
}
