#include "EchoServer.h"
#include "Configuration.h"
#include "CacheManager.h"

#include <iostream>
using std::cout;
using std::endl;
using namespace wdcpp;

void test1()
{

    const string ip = Configuration::getInstance()->getConfigMap()["ip"];
    const unsigned short port = (unsigned short)stoul(Configuration::getInstance()->getConfigMap()["port"]);

    EchoServer server(ip, port);
    server.start();
}

void test2()
{
    auto p = CacheManager::getInstance();
    auto cacheGroup = p->getCacheGroup(0);
    auto res = cacheGroup.getRecord("hello");
    cacheGroup.insertRecord("hello", "hello");
    cacheGroup.insertRecord("xixi", "xixi");
    cacheGroup.insertRecord("haha", "haha");
    cacheGroup.insertRecord("loulou", "loulou");
    auto res1 = cacheGroup.getRecord("xixi");
    cout << res1 << endl;
}

int main()
{
    test1();
    // test2();
    return 0;
}
