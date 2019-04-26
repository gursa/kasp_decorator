#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <iostream>

namespace kasp
{

class timer
{
public:
    timer() : m_stop_timer(false)
    {
        std::cout << __FUNCTION__ << std::endl;
    }
    ~timer()
    {
        std::cout << __FUNCTION__ << std::endl;
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
        std::cout << __FUNCTION__ << std::endl;
        this->m_stop_timer = true;
    }
private:
    bool m_stop_timer;
};

}

