#pragma once

#include <memory>
#include <map>
#include <queue>
#include "kasp_sdk.h"
#include "kasp_event.h"
#include "kasp_timer.h"
#include "kasp_thread_pool.h"

namespace kasp
{

class decorator: public db_interface
{
public:
    decorator(db_interface *db, const int get_timeout = 10, const int db_timeout = 60);
    virtual ~decorator();
    virtual std::string get(const std::string& key, const int get_timeout = 10);
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);
private:
    const int m_timeout;
    std::shared_ptr<db_interface> m_db;
    std::unique_ptr<kasp::timer> m_timer;
    std::unique_ptr<kasp::thread_pool> m_thread_pool;

    std::mutex m_request_mutex;
    std::condition_variable m_request_cv;
    std::queue<std::shared_ptr<kasp::request_item>> m_request_queue;

    std::mutex m_cache_mutex;
    std::condition_variable m_cache_cv;
    std::map<std::string, std::unique_ptr<kasp::records_event>> m_cache;

    std::mutex m_get_mutex;
    std::condition_variable m_get_cv;
    std::queue<std::shared_ptr<kasp::request_item>> m_get_queue;
};

}
