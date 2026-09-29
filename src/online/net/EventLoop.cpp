#include "EventLoop.h"

#include <ErrorCheck>
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

namespace wdcpp
{
EventLoop::EventLoop(Acceptor &acceptor)
    : _epFd(createEpoll()),
      _eventFd(createEvent()),
      _acceptor(acceptor),
      _isLooping(false),
      _eventList(INIT_EPOLLNUM) // 为 _eventList 初始化（即插入 INIT_EPOLLNUM 个空的 epoll_event）
{
    addEpollFd(_acceptor.fd()); // 将 _listenSock._fd 加入监听集合 _epFd
    addEpollFd(_eventFd);       // 将 _eventFd 加入监听集合 _epFd
}

EventLoop::~EventLoop()
{
    while (::close(_epFd) < 0 && errno == EINTR) // 回收 _epFd
        ;
}

/**
 *  开启 loop
 */
void EventLoop::loop()
{
    _isLooping = true;

    while (_isLooping)
        waitEpoll(); // loop 的执行体
}

/**
 *  关闭 loop，即设置 _isLooping 为 false
 *
 *  1. 让运行在 loop 中的线程退出 while 循环
 *  2. 执行 unloop 的线程和 执行 loop 的线程不是同一个，否则没有效果
 *  3. 执行 unloop 后，服务器只是暂时停止 epoll 监听，_listenSock._fd 和 _epFd 和 peerFd 都没有 close
 *     此时只要再将 _isLooping 设为 true，服务器可以照常运行
 */
void EventLoop::unloop()
{
    _isLooping = false;
}

/**
 *  注册 _onConnectionCb
 */
void EventLoop::setConnectionCallBack(EventLoopCallBack &&onConnectionCb)
{
    _onConnectionCb = std::move(onConnectionCb);
    // _onConnectionCb = onConnectionCb;
}

/**
 *  注册 _onMessageCb
 */
void EventLoop::setMessageCallBack(EventLoopCallBack &&onMessageCb)
{
    _onMessageCb = std::move(onMessageCb);
    // _onMessageCb = onMessageCb;
}

/**
 *  注册 _onCloseCb
 */
void EventLoop::setCloseCallBack(EventLoopCallBack &&onCloseCb)
{
    _onCloseCb = std::move(onCloseCb);
    // _onCloseCb = onCloseCb;
}

/**
 *  注册 onPengingCb
 *
 *  1. 将 onPengingCb 插入 _pengdingCbs
 */
void EventLoop::setPendingCallBack(PendingCallBack &&onPendingCb)
{
    // cout << "Worker Thread: setPendingCallBack" << endl;

    MutexLockGuard autolock(_mutex);

    _pendingCbs.push_back(std::move(onPendingCb));
}

/**
 *  创建监听集合，即在内核中创建一个套接字 _epFd（可将它视为那棵红黑树的根节点）
 */
int EventLoop::createEpoll()
{
    int fd = ::epoll_create1(0);
    ERROR_CHECK(fd, -1, "epoll_create1");

    return fd;
}

/**
 *  将 fd 加入监听集合 _epFd（让 epoll 开始监听 fd）
 */
void EventLoop::addEpollFd(int fd)
{
    struct epoll_event event;

    event.events = EPOLLIN; // 读就绪（该事件会因为读操作而就绪）
    event.data.fd = fd;

    int ret = ::epoll_ctl(_epFd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
}

/**
 *  将 fd 从监听集合 _epFd 中删除（让 epoll 不再监听 fd）
 */
void EventLoop::delEpollFd(int fd)
{
    int ret = ::epoll_ctl(_epFd, EPOLL_CTL_DEL, fd, nullptr);
    ERROR_CHECK(ret, -1, "epoll_ctl");
}

int EventLoop::createEvent()
{
    int fd = ::eventfd(0, 0);
    ERROR_CHECK(fd, -1, "eventfd");

    return fd;
}

/**
 *  从 _eventFd 中读出内核计数器的值
 *
 *  1. 主线程调用
 */
void EventLoop::readCounter()
{
    // cout << "Main Thread: readCounter" << endl;
    uint64_t val = 0;
    int ret = ::read(_eventFd, &val, sizeof(val));
    ERROR_CHECK(ret, -1, "read");
}

/**
 *  向 _eventFd 的内核计数器中写入整数 1
 *
 *  1. 子线程调用
 *  2. 用于子线程通知主线程，让主线程从 loop 中得以唤醒，去执行 onPendingCb
 */
void EventLoop::writeCounter()
{
    // cout << "Worker Thread: writeCounter" << endl;
    uint64_t one = 1;
    int ret = ::write(_eventFd, &one, sizeof(one));
    ERROR_CHECK(ret, -1, "write");
}

/**
 *  执行所有的 onPendingCb
 *
 *  1. 主线程调用
 *  2. 执行所有本该在 onMessageCb 中执行却延后的 send 操作
 */
void EventLoop::handlePendingCbs()
{
    // cout << "Main Thread: handlePendingCbs" << endl;
    vector<PendingCallBack> tmp;
    {
        MutexLockGuard autolock(_mutex);
        tmp.swap(_pendingCbs); // 将 _pendingCbs 复制给 tmp
    }

    for (auto &onPendingCb : tmp) // 执行所有的 pendingCbs，即 send(response)
        onPendingCb();
}

/**
 *  loop 的循环体
 *
 *  1. 仅由 loop 调用
 *  2. 开始监听（epoll_wait）
 *  3. 遍历就绪事件链表
 *     发生新连接事件 -> handleNewConnection
 *     发生旧连接事件（新消息到来或连接已断开) -> handleOldConnection
 *     收到子线程的通知 -> readCounter -> handlePendingCbs
 */
void EventLoop::waitEpoll()
{
    int readyNum = 0;

    do
    {
        readyNum = ::epoll_wait(_epFd, &_eventList[0], _eventList.size(), 5000);
    } while (readyNum == -1 && errno == EINTR); // 收到中断信号直接忽略，重新监听

    if (readyNum == -1) // 出错
        ::perror("epoll_wait");
    else if (readyNum == 0) // 超时
    {
        ERROR_PRINT("epoll_wait: timeout\n");
    }
    else // 正常情况：readyNum > 0
    {
        if ((size_t)readyNum == _eventList.size()) // 就绪事件链表已满，可能还有一些就绪事件未从就绪集合中拷贝出来，因此需立即对 _eventList 扩容
            _eventList.resize(2 * readyNum);       // 一定得用 resize，要保证 _eventList.size 成功变为 2 * readyNum

        for (int idx = 0; idx < readyNum; ++idx)
        {
            if (_eventList[idx].data.fd == _acceptor.fd()) // 发生新连接事件
            {
                if (_eventList[idx].events & EPOLLIN) // 确认事件类型是否为写就绪
                    handleNewConnection();
            }
            else if (_eventList[idx].data.fd == _eventFd) // 收到子线程的通知
            {
                if (_eventList[idx].events & EPOLLIN) // 确认事件类型是否为写就绪
                {
                    // cout << "Main Thread: I'm wakeup in _eventFd!" << endl;
                    readCounter(); // 将内核计数器的值取出

                    handlePendingCbs(); // 执行所有的 pendingCb，即 send(response)
                }
            }
            else // 发生旧连接事件（新消息到来或连接已断开)
            {
                if (_eventList[idx].events & EPOLLIN) // 确认事件类型是否为写就绪
                    handleOldConnection(_eventList[idx].data.fd);
            }
        }
    }
}

/**
 *  发生新连接事件，EventLoop 中负责的执行逻辑
 *
 *  1. 获取新的已连接套接字 peerFd
 *  2. 建立新的 conn，并为其中的三个事件处理器进行注册
 *  3. 更新已连接集合 _connMap
 *  4. 执行该事件的事件处理器 conn->handleConnectionCallBack
 */
void EventLoop::handleNewConnection()
{
    int peerFd = _acceptor.accept(); // 获取新的已连接套接字（即从 _listenSock._fd 中的全连接队列中取出一个连接，并为它分配新的已连接套接字）

    addEpollFd(peerFd); // 将新的已连接套接字加入监听集合 _epFd

#if 0 // map<int, TcpConnection> + 移动语义

    auto pair = std::make_pair(peerFd, TcpConnection(peerFd));
    _connMap.insert(pair);

    auto conn = _connMap[peerFd];

    conn->setConnectionCallBack(_onConnectionCb);
    conn->setMessageCallBack(_onMessageCb);
    conn->setCloseCallBack(_onCloseCb);

#endif

#if 1 // map<int, TcpConnectionPtr>

    TcpConnectionPtr conn(new TcpConnection(peerFd, this)); // 建议一条新的连接

    // 将三个 EventLoopCallBack 传给 conn 让其注册它的三个 TcpConnectionCallBack
    conn->setConnectionCallBack(_onConnectionCb);
    conn->setMessageCallBack(_onMessageCb);
    conn->setCloseCallBack(_onCloseCb);

    _connMap.insert(std::make_pair(peerFd, conn)); // 将该条新连接加入已连接集合 _connMap

#endif

    conn->handleConnectionCallBack(); // 执行新连接事件的事件处理器
}

/**
 *  发生旧连接事件，EventLoop 中负责的执行逻辑
 *
 *  1. 可能是发生新消息到来事件，也可能是发生连接断开事件
 *  2. 在 _connMap 中查找 peerFd 的那条连接 conn
 *  3. 判断该 conn 是否断开
 *  4. 若断开，则执行连接断开事件的事件处理器 conn->handleCloseCallBack
 *  5. 若没有断开，则执行新消息事件的事件处理器 conn->handleMessageCallBack
 */
void EventLoop::handleOldConnection(int peerFd)
{
    auto conn_it = _connMap.find(peerFd); // 查找 peerFd 的那条连接 conn

    if (conn_it != _connMap.end())
    {
        if (conn_it->second->isClosed()) // conn 已断开
        {
            conn_it->second->handleCloseCallBack(); // 执行连接断开事件的事件处理器

            delEpollFd(peerFd);      // 将 peerFd 从监听集合 _epFd 中删除，即不再监听它了
            _connMap.erase(conn_it); // 将这条已断开的连接 conn 从已连接集合 _connMap 中删除（完成 TCP 挥手）
        }
        else                                          // conn 没有断开
            conn_it->second->handleMessageCallBack(); // 执行新消息事件的事件处理器
    }
    else
        ERROR_PRINT("can not find peerFd(%d)'s connection in _connMap", peerFd);
}
};
