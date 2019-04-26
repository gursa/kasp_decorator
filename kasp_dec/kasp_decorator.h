#pragma once

#include <memory>
#include <map>
#include <queue>
#include "kasp_sdk.h"
#include "kasp_event.h"
#include "kasp_timer.h"
#include "kasp_thread_pool.h"
#include "base_queue.h"
#include "base_cache.h"

namespace kasp
{

class decorator: public db_interface
{
public:
    decorator(db_interface *db, const int get_timeout = 10, const int db_timeout = 60);
    virtual ~decorator();
    virtual std::string get(const std::string& key, const std::chrono::milliseconds get_timeout = std::chrono::milliseconds(10));
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);
private:
    const int m_timeout;
    std::shared_ptr<db_interface> m_db;
    std::unique_ptr<kasp::timer> m_timer;
    std::unique_ptr<kasp::thread_pool> m_thread_pool;

    std::mutex m_request_mutex;
    std::condition_variable m_request_cv;
    base::queue m_request_queue;
    base::cache m_cache;
};

}
