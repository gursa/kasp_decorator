#pragma once

#include <chrono>
#include <functional>
#include <thread>

namespace kasp
{

class timer
{
public:
    timer() : m_stop_timer(false){}
    ~timer()
    {
        stop();
    }

    template<typename Function>
    void set_timeout(Function function, int delay)
    {
        this->m_stop_timer = false;
        std::thread t(
                    [=]()
                    {
                        if (this->m_stop_timer) return;
                        std::this_thread::sleep_for(std::chrono::seconds(delay));
                        if (this->m_stop_timer) return;
                        function();
                    }
        );
        t.detach();
    }
    void stop()
    {
        this->m_stop_timer = true;
    }
private:
    bool m_stop_timer;
};

}

