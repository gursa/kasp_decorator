#include <iostream>
#include "kasp_thread_pool.h"

kasp::thread_pool::thread_pool(std::size_t pool_size)
    : m_work(m_io_service),
      m_available(pool_size)
{
    for (std::size_t i = 0; i < pool_size; ++i)
    {
        m_threads.create_thread(boost::bind(&boost::asio::io_service::run, &m_io_service));
    }
}

kasp::thread_pool::~thread_pool()
{
    m_io_service.stop();

    try
    {
        m_threads.join_all();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[Exception] " << __FUNCTION__ << ex.what() << std::endl;
    }
}

void kasp::thread_pool::wrap_task(boost::function<void()> task)
{
    try
    {
      task();
    }
    catch (const std::exception &ex)
    {
          std::cerr << "[Exception] " << __FUNCTION__ << ex.what() << std::endl;
    }

    boost::unique_lock<boost::mutex> lock(m_mutex);
    ++m_available;
}
