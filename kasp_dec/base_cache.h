#pragma once

#include <mutex>
#include <string>
#include <map>
#include "kasp_sdk.h"

namespace base
{

class cache
{
public:
    void put(const std::string &key, const std::string &value);

    bool find(const std::string &key, std::string &value);

    void remove(const std::string &key);

    void clear();
    int copy(kasp::db_interface *db);
private:
    std::map<std::string, std::string> m_cache;
    std::mutex m_mutex;
};

}
