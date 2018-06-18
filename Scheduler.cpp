#include <sys/timerfd.h>
#include <iostream>
#include <sys/time.h>
#include <string.h>

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
        catch (...)
        {}
    }
    close(_iSchedulerFD);
}

void Scheduler::quit()
{
    std::lock_guard(_mLock);
    _bQuit = true;
    _mCond.notify_all();

}

void Scheduler::notify()
{
    std::lock_guard(_mLock);
    _mCond.notify_one();
}

bool Scheduler::runAT(const uint64_t &lUs, std::function<void()> fCallback)
{
    SchedulerEvent tSchedulerEvent;

    tSchedulerEvent.fCallback = fCallback;
    tSchedulerEvent.cycle     = 0.0;

    addTimer(lUs, tSchedulerEvent);

    return true;

}

bool Scheduler::runAfter(const double &dSec, std::function<void()> fCallback)
{
    SchedulerEvent tSchedulerEvent;
    tSchedulerEvent.fCallback = fCallback;
    tSchedulerEvent.cycle     = 0.0;

    addTimer(getNowUs() + dSec * 1000000, tSchedulerEvent);

    return true;
}

bool Scheduler::runEvery(const double &dSec, std::function<void()> fCallback)
{
    SchedulerEvent tSchedulerEvent;
    tSchedulerEvent.fCallback = fCallback;
    tSchedulerEvent.cycle     = 0.0;

    addTimer(getNowUs() + dSec * 1000000, tSchedulerEvent);

    return true;
}

inline uint64_t Scheduler::getNowUs()
{
    struct timeval tTtimeVal;
    gettimeofday(&tTtimeVal, nullptr);

    return tTtimeVal.tv_sec * 1000000 + tTtimeVal.tv_usec;
}

void Scheduler::addTimer(const uint64_t &lTime, const Scheduler::SchedulerEvent &tSchedulerEvent)
{
    if (insertIntoTimeoutMap(lTime, tSchedulerEvent))
    {
        resetSchedulerfd();
    }
}

bool Scheduler::insertIntoTimeoutMap(const uint64_t &lTime, const Scheduler::SchedulerEvent &tSchedulerEvent)
{
    bool bTimeOutChange = false;
    std::lock_guard(_mLock);
    auto iter = _mTimedCallBacks.begin();

    if(iter == _mTimedCallBacks.end() || lTime < (iter->first))
    {
        bTimeOutChange = true;
    }

    _mTimedCallBacks.insert(std::make_pair(lTime, tSchedulerEvent));

    return bTimeOutChange;
}

uint64_t Scheduler::timeLapsed(const uint64_t &lNow)
{
    std::lock_guard(_mLock);

    uint64_t lNextTimeOut = (_mTimedCallBacks.begin()->first);
    uint64_t lTimeLapsed  = lNextTimeOut -lNow;

    if(lNextTimeOut < lNow || lTimeLapsed < 100)
    {
        lTimeLapsed = 100;
    }

    return lTimeLapsed;
}

void Scheduler::resetSchedulerfd()
{
    itimerspec tNewTime;
    itimerspec tOldTime;
    bzero(&tNewTime, sizeof(tNewTime));
    bzero(&tOldTime, sizeof(tOldTime));

    timespec tTimeDiffvalue;

    uint64_t lTimeLapsed = timeLapsed(getNowUs());

    tTimeDiffvalue.tv_sec = static_cast<time_t>(lTimeLapsed / 1000000);
    tTimeDiffvalue.tv_nsec = static_cast<long>((lTimeLapsed % 1000000) * 1000);

    tNewTime.it_value = tTimeDiffvalue;

    if(timerfd_settime(_iSchedulerFD, 0, &tNewTime, &tOldTime) == -1)
    {
        std::cout << "TimeOut FD Setting Failed"  << std::endl;
    }

}


void Scheduler::readSchedulerfd()
{
    uint64_t lExpiredNum = 0;
    ssize_t n = read(_iSchedulerFD, &lExpiredNum, sizeof(lExpiredNum));

    //LOG->debug() << num_expire << " timer out" << endl;

    if (n != sizeof(lExpiredNum))
    {
        std::cout << "Read Timer FD ERROR"  << std::endl;
    }
}

void Scheduler::handleRead()
{
    vector<SchedulerEvent> vTimeOutEvent;

    { // lock

        std::lock_guard(_mLock);
        std::multimap<uint64_t, SchedulerEvent>::iterator iter = getTimedMap().lower_bound(getNowUs());

        for (auto it = getTimedMap().begin(); it != iter; ++it)
        {
            vTimeOutEvent.push_back(it->second);
        }

        //Delete TimeOut Event from Map
        getTimedMap().erase(getTimedMap().begin(), iter);

    } // unlock

    for (auto& k : vTimeOutEvent)
    {
        // CallBack;
        k.callback();
    }

    setRepeatTimer(vTimeOutEvent);
}



