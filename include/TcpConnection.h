#pragma once
#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"

#include <memory>
#include <functional>
using std::function;
using std::shared_ptr;

namespace wdcpp
{
/*************************************************************
 *
 *  TCP connection class
 * 
 *************************************************************/
class EventLoop;
class TcpConnection;
using TcpConnectionPtr = shared_ptr<TcpConnection>; 

class TcpConnection
    : NonCopyable,
      public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionCallBack = function<void(const TcpConnectionPtr &)>;

public:
    explicit TcpConnection(int, EventLoop *);

    void send(const string &);
    string recv();
    string recvLine();
    string show();

    bool isClosed() const;

    void setConnectionCallBack(const TcpConnectionCallBack &);
    void setMessageCallBack(const TcpConnectionCallBack &);
    void setCloseCallBack(const TcpConnectionCallBack &);
    void handleConnectionCallBack();
    void handleMessageCallBack();
    void handleCloseCallBack();

    void notifyLoop(const string &);

private:
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

private:
    Socket _clientSock;
    SocketIO _sockIO;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutDownWrite;
    TcpConnectionCallBack _onConnectionCb; 
    TcpConnectionCallBack _onMessageCb;    
    TcpConnectionCallBack _onCloseCb;      
    EventLoop *_loopPtr;
};
};
