#include <sys/timerfd.h>
#include <iostream>

#include "Scheduler.h"

Scheduler::Scheduler() : _bQuit(false)
{}


Scheduler::~Scheduler()
{
    if(!_bQuit)
    {
        quit();

        if(_tScheduler.joinable())
        {
            _tScheduler.join();
        }
    }
}

void Scheduler::initialize()
{
    _tEpoller.create(1024);

    _iSchedulerFD = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);

    if(_iSchedulerFD == -1)
    {
        std::cout << "Initial TimeOut Event Failed"<<std::endl;
    }

    _tEpoller.add(_iSchedulerFD, 0, EPOLLIN);
    
}

void Scheduler::run()
{
    while(!_bQuit)
    {
        try
        {
            int iEventNum = _tEpoller.wait(1000);

            for(int n = 0; n < iEventNum; ++n)
            {
                const epoll_event &ev = _tEpoller.get(n);
                if(ev.events & EPOLLIN)
                {
                    readSchedulerfd();
                    handleRead();
                }
                else
                {

                }
            }
        }
        catch(std::exception& ex)
        {
            std::cout << "Event Error"<<std::endl;
        }

    }
}


