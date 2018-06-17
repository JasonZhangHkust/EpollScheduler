#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <thread>
#include <functional>
#include <vector>
#include <map>

#include "Epoller.h"

class Scheduler
{
public:

    struct  SchedulerEvent
    {
        std::function<void()>  fCallback;
        double                 cycle;
    };

public:
    Scheduler();
    ~Scheduler();
    void run();
    void initialize();
    void quit();
    void notify();
    //unsigned long int == uint64_t
    bool runAT(const uint64_t& lUs, std::function<void()> fCallback);
    bool runAfter(const double& dSec, std::function<void()> fCallback);
    bool runEvery(const double& dSec, std::function<void()> fCallback);

private:
    std::thread _tScheduler;
    std::mutex  _mLock;
    bool _bQuit;
    int  _iSchedulerFD;
    uint64_t _lInterval;
    Epoller _tEpoller;
    std::multimap<uint64_t, SchedulerEvent> _mTimedCallBacks;
private:
    uint64_t getNowUs();
    void addTimer(const uint64_t& lTime, SchedulerEvent);
    bool insertIntoTimeoutMap(const uint64_t& when, const SchedulerEvent& timer_struct);
    uint64_t timeLapsed(const uint64_t& lNow);
    void resetSchedulerfd();
    void readSchedulerfd();
    void setRepeatTimer(const std::vector<SchedulerEvent>& vTimeOutEvent);
    void handleRead();

};

#endif //SCHEDULER_H
