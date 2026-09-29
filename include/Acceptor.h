#pragma once
#include "Socket.h"
#include "InetAddress.h"

namespace wdcpp
{
class Acceptor
{
public:
    explicit Acceptor(const string &, unsigned short);

    void prepare();
    int accept();
    int fd();

private:
    void setReuseAddr(bool);
    void setReusePort(bool);
    void bind();
    void listen();

private:
    Socket _listenSock;
    InetAddress _serverAddr;
};
};
