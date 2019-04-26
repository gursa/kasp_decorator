#pragma once

#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include "kasp_event.h"

namespace base
{

struct message
{
    std::string m_data;
    message(const std::string &data) : m_data(data) {}
};

class msg_queue
{
public:
    void put(std::shared_ptr<base::message> msg)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_queue.push(msg);
    }
    std::shared_ptr<base::message> pop()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto result = m_queue.front();
        m_queue.pop();
        return result;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        return m_queue.empty();
    }
private:
    std::queue<std::shared_ptr<base::message>> m_queue;
    std::mutex m_mutex;
};

}


namespace kasp
{

struct records
{
    std::string key;
    std::string data;
};

enum class request_type
{
    req_put,
    req_get,
    req_remove
};

struct request_item
{
    records m_rec;
    request_type m_type;
    int m_timeout;
    request_item(const std::string &key, const std::string &data, const request_type &rec_type, const int &timeout)
        : m_rec({key, data}),
          m_type(rec_type),
          m_timeout(timeout)
    {}
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
    virtual std::string get(const std::string& key, const int get_timeout) = 0;
    // Метод задания значения по ключу
    virtual void put(const std::string& key, const std::string& data) = 0;
    // Метод удаления по ключу
    virtual void remove(const std::string& key) = 0;
};

}
