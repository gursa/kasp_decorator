#pragma once

#include <map>
#include <chrono>
#include "kasp_sdk.h"
#include "base_cache.h"

namespace kasp
{

class db_mock : public db_interface
{
public:
    db_mock();
    virtual ~db_mock();
    virtual std::string get(const std::string& key, const std::chrono::milliseconds get_timeout);
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);

private:
    base::cache m_records;
};

}
