#pragma once

#include <boost/log/trivial.hpp>
#include <chrono>
#include <functional>
#include <thread>

namespace kasp
{

class timer
{
public:
    timer() : m_stop_timer(false)
    {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << std::endl;
    }
    ~timer()
    {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << std::endl;
        stop();
    }

    template<typename Function>
    void set_timeout(Function function, std::chrono::milliseconds delay)
    {
        this->m_stop_timer = false;
        std::thread t(
                    [=]()
                    {
                        while(true)
                        {
                            if(this->m_stop_timer) return;
                            std::this_thread::sleep_for(delay);
                            if(this->m_stop_timer) return;
                            function();
                        }
                    });
        t.detach();
    }
    void stop()
    {
        BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << std::endl;
        this->m_stop_timer = true;
    }
private:
    bool m_stop_timer;
};

}

