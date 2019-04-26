#include "base_queue.h"

std::string base::queue::call(const std::string &key, const std::string &data, const request_type type, const std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto m = std::make_shared<message>();
    m->key = key;
    m->type = type;
    m->data = data;
    m->timeout = timeout;

    m_queue.push(m);
    m_input_cv.notify_one();

    if(!m_output_cv.wait_for(lock, timeout, [&]() { return m->ready; }))
        throw std::runtime_error("We have a timeout");


    return m->data;
}

std::shared_ptr<base::queue::message> base::queue::take()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    std::shared_ptr<message> m;
    m_input_cv.wait(lock,
            [&, this]()
            {
                if(!m_queue.empty())
                {
                    m = m_queue.front();
                    m_queue.pop();
                    return true;
                }

                return false;
            }
    );

    return m;
}

void base::queue::done(std::shared_ptr<base::queue::message> message)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    message->ready = true;
    m_output_cv.notify_all();
}
