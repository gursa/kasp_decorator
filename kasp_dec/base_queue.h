#pragma once

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

namespace base
{

enum class request_type
{
    req_get,
    req_put,
    req_remove
};

class queue
{
private:

    struct message
    {
        std::string key = std::string();
        std::string data = std::string();
        request_type type = request_type::req_get;
        std::chrono::milliseconds timeout;
        bool ready = false;
    };

public:
    // for client threads
    std::string call(const std::string &key, const std::string &data, const request_type type = request_type::req_get, const std::chrono::milliseconds timeout = std::chrono::milliseconds(500));

    // for service threads
    std::shared_ptr<message> take();

    void done(std::shared_ptr<queue::message> message);
private:
    std::queue<std::shared_ptr<message>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_input_cv;
    std::condition_variable m_output_cv;
};

}

