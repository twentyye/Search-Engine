#pragma once
#include "TcpConnection.h"
#include "WebPageSearcher.h"
#include "KeyRecommander.h"
#include "Dictionary.h"

#include <unistd.h>

#include <sw/redis++/redis++.h>
#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;
using namespace sw::redis;

namespace wdcpp
{
class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &connPtr, WebPageSearcher &webPageSearcher, KeyRecommander &recommander, sw::redis::Redis &redis)
        : _msg(msg),
          _connPtr(connPtr),
          _webPageSearcher(webPageSearcher),
          _recommander(recommander),
          _redis(redis)

    {
    }

    void process(); // called by child thread（TheadPoll）

private:
    string _msg;
    TcpConnectionPtr _connPtr;
    WebPageSearcher &_webPageSearcher;
    KeyRecommander &_recommander;
    sw::redis::Redis &_redis;
};
}; // namespace wdcpp
