#pragma once

#include <memory>
#include <chrono>
#include "kasp_sdk.h"
#include "kasp_thread_pool.h"
#include "base_queue.h"
#include "base_cache.h"

namespace kasp
{

class decorator: public db_interface
{
public:
    decorator(db_interface *db,boost::asio::io_context& io, const std::chrono::milliseconds db_timeout = std::chrono::milliseconds(60000));
    virtual ~decorator();
    virtual std::string get(const std::string& key, const std::chrono::milliseconds get_timeout = std::chrono::milliseconds(1000));
    virtual void put(const std::string& key, const std::string& data);
    virtual void remove(const std::string& key);
private:
    void copy_to_db(const std::chrono::milliseconds db_timeout);
private:
    std::shared_ptr<db_interface> m_db;
    boost::asio::steady_timer m_timer;
    std::unique_ptr<kasp::thread_pool> m_thread_pool;
    base::queue m_request_queue;
    base::cache m_cache;
};

}
