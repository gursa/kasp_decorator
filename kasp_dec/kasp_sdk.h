#pragma once

#include <string>

namespace kasp
{

struct records
{
    std::string key;
    std::string data;
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
