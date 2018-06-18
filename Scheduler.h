#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <thread>
#include <condition_variable>
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
    void start();
    void initialize();
    void quit();
    void notify();
    //unsigned long int == uint64_t
    bool runAT(const uint64_t& lUs, std::function<void()> fCallback);
    bool runAfter(const double& dSec, std::function<void()> fCallback);
    bool runEvery(const double& dSec, std::function<void()> fCallback);

private:
    std::thread _tSchedulerThread;
    std::mutex  _mLock;
    std::condition_variable _mCond;
    bool _bQuit;
    int  _iSchedulerFD;
    uint64_t _lInterval;
    Epoller _tEpoller;
    // Time Stored in Us
    std::multimap<uint64_t, SchedulerEvent> _mTimedCallBacks;

private:
    uint64_t getNowUs();
    void addTimer(const uint64_t& lTime, const SchedulerEvent& tSchedulerEvent);
    bool insertIntoTimeoutMap(const uint64_t& lTime, const SchedulerEvent& tSchedulerEvent);
    uint64_t timeLapsed(const uint64_t& lNow);
    void resetSchedulerFD();
    void readSchedulerFD();
    void setRepeatTimer(const std::vector<SchedulerEvent>& vTimeOutEvent);
    void handleRead();

    std::multimap<uint64_t, SchedulerEvent>& getTimedMap() { return _mTimedCallBacks; }
};

#endif //SCHEDULER_H
