
#include <iostream>
#include "Epoller.h"


Epoller::Epoller(bool bET) : _iEpollFD(-1),_pPrevs(NULL), _bET(bET), _iMaxConn(1024)
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
    epoll_event ev;
    ev.data.u64 = lData;
    if(_bET)
    {
        ev.events = iEvents | EPOLLET;
    }
    else
    {
        ev.events = iEvents;
    }

    epoll_ctl(_iEpollFD, iOP, iFD, &ev);

}

void Epoller::create(int iMaxConn) {
    _iMaxConn = iMaxConn;
    try {
        _iEpollFD = epoll_create(_iMaxConn + 1);
    }
    catch (...)
    {
        std::cout << "EPOLL CREATE FAIL" << std::endl;
    }



    if(_pPrevs)
    {
        delete[] _pPrevs;
    }

    _pPrevs = new epoll_event[_iMaxConn + 1];
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
    return epoll_wait(_iEpollFD, _pPrevs, _iMaxConn + 1, iMs);
}




