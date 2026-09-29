#include "TcpServer.h"

namespace wdcpp
{
TcpServer::TcpServer(const string &ip, unsigned short port)
    : _acceptor(ip, port),
      _loop(_acceptor)
{
}

void TcpServer::start()
{
    _acceptor.prepare(); // 复用 + 绑定bind + 监听listen

    _loop.loop();
}

/**
 *  1. stop 不能和 start 由同一线程运行，否则没有效果
 */
void TcpServer::stop()
{
    _loop.unloop();
}

void TcpServer::setConnectionCallBack(TcpServerCallBack &&onConnectionCb)
{
    _loop.setConnectionCallBack(std::move(onConnectionCb));
}

void TcpServer::setMessageCallBack(TcpServerCallBack &&onMessageCb)
{
    _loop.setMessageCallBack(std::move(onMessageCb));
}

void TcpServer::setCloseCallBack(TcpServerCallBack &&onCloseCb)
{
    _loop.setCloseCallBack(std::move(onCloseCb));
}
};
