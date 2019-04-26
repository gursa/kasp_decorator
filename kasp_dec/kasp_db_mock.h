#pragma once

#include <map>
#include <utility>
#include "kasp_sdk.h"

namespace kasp
{

class db_mock : public db_interface
{
public:
    db_mock();
    virtual ~db_mock();
    virtual std::string get(const std::string& key, const int get_timeout);
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);

private:
    std::map<std::string, std::string> m_records;
};

}
