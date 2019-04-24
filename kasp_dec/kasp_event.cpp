#include "kasp_event.h"


kasp::event::event()
    : m_flag(false)
{
}

bool kasp::event::signal()
{
    bool was_signal;
    m_mutex.lock();
    was_signal = m_flag;
    m_flag = true;
    m_mutex.unlock();
    m_cv.notify_all();
    return was_signal == false;
}

bool kasp::event::reset()
{
    bool was_signal;
    m_mutex.lock();
    was_signal = m_flag;
    m_flag = false;
    m_mutex.unlock();
    return was_signal;
}

void kasp::event::wait() const
{
    std::unique_lock< std::mutex > lock(m_mutex);
    m_cv.wait(lock,[&]()->bool{ return m_flag; });
}
