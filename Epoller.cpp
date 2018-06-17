
#include "Epoller.h"
#include <unistd.h>

Epoller::Epoller(bool bET) : _iEpollFD(-1), _pPrevs(nullptr), _bET(bET), _iMaxConn(1024)
{}

Epoller::~Epoller()
{

    if(_iEpollFD > 0)
    {
        close(_iEpollFD);
    }
}

void Epoller::ctrl(int iFD, long long lData, unsigned int iEvents, int iOP)
{
    struct  epoll_event ev;
    ev.data.u64 = data;
    if(_bET)
    {
        ev.events = iEvents | EPOLLET;
    }
    else
    {
        ev.events = events;
    }

    epoll_ctl(_iEpollFD, iOP, iFD, &ev);

}

void Epoller::create(int iMaxConn)
{
    _iMaxConn = iMaxConn;
    _iEpollFD =  epoll_create(_iMaxConn + 1);
    if(_pPrevs)
    {
        delete[] _pPrevs;
    }

    _pPrevs = std::make_shared<epoll_event>(_iMaxConn + 1);
}

void Epoller::add(int iFD, long long lData, unsigned int iEvent)
{
    ctrl(iFD, lData, iEvent, EPOLL_CTL_ADD);
}

void Epoller::modify(int iFD, long long lData, unsigned int iEvent)
{
    ctrl(iFD, lData, iEvent, EPOLL_CTL_MOD);
}

void Epoller::dele(int iFD, long long lData, unsigned int iEvent)
{
    ctrl(iFD, lData, iEvent, EPOLL_CTL_MOD);
}

int Epoller::wait(int iMs)
{
    return epoll_wait(_iEpollFD, _pPrevs.get(), _iMaxConn + 1, iMs);
}




