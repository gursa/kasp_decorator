#pragma once

#include <memory>
#include "kasp_sdk.h"
#include "../object_threadsafe/safe_ptr.h"

namespace kasp
{

class decorator: public db_interface
{
public:
    decorator(db_interface *db);
    virtual ~decorator();
    virtual std::string get(const std::string& key);
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);
private:
    std::shared_ptr<db_interface> m_db;
    sf::safe_ptr<std::map<std::string, std::string>> m_cache;
};

}
