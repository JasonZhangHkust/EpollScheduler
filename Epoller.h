#ifndef __EPOLLER_H
#define __EPOLLER_H

#include <sys/epoll.h>
#include <cassert>
#include <memory>
#include <unistd.h>

class Epoller {
public:

    Epoller(bool bET = true);

    ~ Epoller();

    void create(int iMaxConn);

    void add(int iFD, long long lData, unsigned int iEvent);

    void modify(int iFD, long long lData, unsigned int iEvent);

    void dele(int iFD, long long lData, unsigned int iEvent);

    //millisecond
    int wait(int iMs);

    struct epoll_event& get(int iIndex) {assert(_pPrevs !=0); return _pPrevs[iIndex];}

protected:

    void ctrl(int iFD, long long lData, unsigned int iEvents, int iOP);

protected:


    int _iEpollFD;

    int _iMaxConn;

    epoll_event* _pPrevs;

    bool _bET;

};


#endif //TIMER_EPOLLER_H
