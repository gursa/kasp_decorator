#pragma once

#include <string>
#include <memory>
#include <queue>
#include <mutex>

namespace kasp
{

class db_interface
{
public:
    virtual ~db_interface() = default;
    // Метод получение по ключу
    virtual std::string get(const std::string& key, const std::chrono::milliseconds get_timeout) = 0;
    // Метод задания значения по ключу
    virtual void put(const std::string& key, const std::string& data) = 0;
    // Метод удаления по ключу
    virtual void remove(const std::string& key) = 0;
};

}
