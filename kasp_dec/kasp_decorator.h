#pragma once

#include <memory>
#include "kasp_sdk.h"
#include "kasp_event.h"
#include "kasp_timer.h"
#include "../object_threadsafe/safe_ptr.h"

namespace kasp
{

class decorator: public db_interface
{
public:
    decorator(db_interface *db, const int get_timeout = 10, const int db_timeout = 60);
    virtual ~decorator();
    virtual std::string get(const std::string& key);
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);
private:
    const int m_timeout;
    std::shared_ptr<db_interface> m_db;
    std::unique_ptr<kasp::timer> m_timer;
    sf::safe_ptr<std::map<std::string, std::unique_ptr<kasp::records_event>>> m_cache;
};

}
