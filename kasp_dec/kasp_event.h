#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace kasp
{

class event
{
public:
    event();
    bool signal();
    bool reset();
    void wait() const;
    template< typename R,typename P >
    bool wait(const std::chrono::duration<R,P>& timeout) const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_cv.wait_for(lock,timeout,[&]()->bool{ return m_flag; }))
            return false;
        return true;
    }
private:
    bool m_flag;
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cv;
};

}
