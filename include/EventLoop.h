#pragma once
#include "Acceptor.h"
#include "TcpConnection.h"
#include "MutexLockGuard.h"

#include <sys/epoll.h>
#include <vector>
#include <map>
using std::map;
using std::vector;

namespace wdcpp
{
class EventLoop
{
    friend void TcpConnection::notifyLoop(const string &);

public:
    using EventLoopCallBack = TcpConnection::TcpConnectionCallBack;

private:
    using EventList = vector<struct epoll_event>;
    using ConnectionMap = map<int, TcpConnectionPtr>;
    // using ConnectionMap = map<int, TcpConnection>;
    using PendingCallBack = function<void()>;

public:
    EventLoop(Acceptor &);
    ~EventLoop();

    void loop();
    void unloop();

    void setConnectionCallBack(EventLoopCallBack &&);
    void setMessageCallBack(EventLoopCallBack &&);
    void setCloseCallBack(EventLoopCallBack &&);
    void setPendingCallBack(PendingCallBack &&);

private:
    int createEpoll();
    void addEpollFd(int);
    void delEpollFd(int);

    int createEvent();
    void readCounter();
    void writeCounter();

    void waitEpoll();
    void handleNewConnection();
    void handleOldConnection(int);
    void handlePendingCbs();

private:
    const size_t INIT_EPOLLNUM = 1024; // the maximum number of listener events at the beginning 
    int _epFd;                         // listener collection
    int _eventFd;                      // kernel counter
    Acceptor &_acceptor;               
    bool _isLooping;                   
    EventList _eventList;              
    ConnectionMap _connMap;            // connected collection

    EventLoopCallBack _onConnectionCb;
    EventLoopCallBack _onMessageCb;
    EventLoopCallBack _onCloseCb;

    MutexLock _mutex;
    vector<PendingCallBack> _pendingCbs; // delayed events handler collection
};
};
