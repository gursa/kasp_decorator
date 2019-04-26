#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace kasp
{

class thread_pool
{
public:
    thread_pool(std::size_t pool_size);
    ~thread_pool();

    template <typename Task>
    void run_task(Task task)
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);

        if (!m_available)
            return;

        --m_available;

        m_io_service.post(boost::bind(&thread_pool::wrap_task, this, boost::function<void()>(task)));
    }
private:
  void wrap_task(boost::function<void()> task);

private:
    boost::asio::io_service m_io_service;
    boost::asio::io_service::work m_work;
    boost::thread_group m_threads;
    std::size_t m_available;
    boost::mutex m_mutex;
};

}


