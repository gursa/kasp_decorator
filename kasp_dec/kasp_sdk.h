#pragma once

#include <string>
#include <memory>
#include "kasp_event.h"

namespace kasp
{

struct records
{
    std::string key;
    std::string data;
};

struct records_event
{
    std::unique_ptr<kasp::records> m_record;
    std::unique_ptr<kasp::event> m_event;
};

class db_interface
{
public:
    virtual ~db_interface() = default;
    // Метод получение по ключу
    virtual std::string get(const std::string& key) = 0;
    // Метод задания значения по ключу
    virtual void put(const std::string& key, const std::string& data) = 0;
    // Метод удаления по ключу
    virtual void remove(const std::string& key) = 0;
};

}
