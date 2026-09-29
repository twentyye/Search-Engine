#pragma once
#include "Acceptor.h"
#include "EventLoop.h"

namespace wdcpp
{
class TcpServer
{
public:
    using TcpServerCallBack = EventLoop::EventLoopCallBack;

public:
    TcpServer(const string &, unsigned short);

    void start();
    void stop();

    void setConnectionCallBack(TcpServerCallBack &&);
    void setMessageCallBack(TcpServerCallBack &&);
    void setCloseCallBack(TcpServerCallBack &&);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};
};
