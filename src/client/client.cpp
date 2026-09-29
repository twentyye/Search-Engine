#include "Configuration.h"
using namespace wdcpp;
#include "nlohmann/json.hpp"
#include "fifo_map.hpp"
using namespace nlohmann;
/* The following is used by nlohmann/json，ensure that the insertion order remains unchanged */
template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;

#include <ErrorCheck>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using std::cin;
using std::cout;
using std::endl;
using std::istringstream;
using std::string;
using std::vector;

int netFd; // global network sockets

void showMenu()
{
    printf("******* My tiny search engine *******\n");
    printf("*                                   *\n");
    printf("*     1: Keyword recommendation     *\n");
    printf("*     2: Web page search            *\n");
    printf("*     3: Quit                       *\n");
    printf("*                                   *\n");
    printf("*************************************\n");
    printf("\n");
}

/**
 *  Display web search results in pages
 */
void showWithPaging(Json &root)
{
    size_t pageNum = root["msg"].size();
    if (pageNum <= 1)
        cout << "[ " << pageNum << " page were found ]" << endl;
    else
        cout << "[ " << pageNum << " pages were found ]" << endl;

    size_t restNum = pageNum; 

    for (size_t idx = 0; idx < 5 && idx < pageNum; ++idx, --restNum)
    {
        auto &page = root["msg"][idx];
        cout << "[Title] " << page["title"] << endl;
        cout << "[url] " << page["url"] << endl;
        cout << "[Summary] " << page["summary"] << endl
             << endl;
    }

    while (restNum > 0)
    {
        if (restNum <= 1)
            cout << "[ " << restNum << " page behind, please input 'n' to show the rest, or 'c' to continue ]" << endl;
        else
            cout << "[ " << restNum << " pages behind, please input 'n' to show the rest, or 'c' to continue ]" << endl;

        char opt;
        cin >> opt;
        if (opt == 'n')
        {
            size_t start = pageNum - restNum;
            for (size_t idx = start; idx < start + 5 && idx < pageNum; ++idx, --restNum)
            {
                auto &page = root["msg"][idx];
                cout << "[Title] " << page["title"] << endl;
                cout << "[url] " << page["url"] << endl;
                cout << "[Summary] " << page["summary"] << endl
                     << endl;
            }
        }
        else
            goto end;
    }

end:;
}

size_t sendm(const void *buf, size_t count)
{
    int left = count;              
    const char *ptr = (char *)buf; 

    int ret = 0;

    while (left > 0)
    {
        do
        {
            ret = ::send(netFd, ptr, left, 0);
        } while (-1 == ret && errno == EINTR); // Ignore the interrupt event directly

        if (-1 == ret) 
        {
            ::perror("send");
            return count - ret; 
        }
        else if (0 == ret) 
        {
            ERROR_PRINT("send: server disconnected\n");
            break;
        }
        else 
        {
            ptr += ret;
            left -= ret;
        }
    }

    return count;
}

size_t recvm(void *buf, size_t count)
{
    int left = count;        
    char *ptr = (char *)buf; 

    int ret = 0;

    while (left > 0)
    {
        do
        {
            ret = ::recv(netFd, ptr, left, 0);
        } while (-1 == ret && errno == EINTR); 

        if (-1 == ret)
        {
            ::perror("recv");
            return count - ret; 
        }
        else if (0 == ret) 
        {
            ERROR_PRINT("recv: server disconnected\n");
            break;
        }
        else 
        {
            ptr += ret;
            left -= ret;
        }
    }

    return count;
}

void sendKey(string &key)
{
    Json root;
    root["msgID"] = 1;
    root["msg"] = key;
    string msg = root.dump(4);
#ifdef __DEBUG__
    printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
    cout << msg << endl;
#endif

    const size_t length = msg.size();
    sendm(&length, sizeof(size_t)); 
    sendm(msg.c_str(), length);    
}

void sendQuery(string &query)
{
    Json root;
    root["msgID"] = 2;
    root["msg"] = query;
    string msg = root.dump(4);

#ifdef __DEBUG__
    printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
    cout << msg << endl;
#endif

    const size_t length = msg.size();
    sendm(&length, sizeof(size_t)); 
    sendm(msg.c_str(), length);     
}

void recvKeys()
{

    size_t length = 0;
    recvm(&length, sizeof(size_t)); 
    char buf[length + 1] = {0};
    recvm(buf, length); 

    string msg(buf);
#ifdef __DEBUG__
    printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
    cout << msg << endl;
#endif
    Json root = json::parse(msg); 
    if (100 == root["msgID"])
    {
        cout << "Response from server: " << endl;
        for (auto &key : root["msg"])
        {
            cout << key << endl;
        }
    }
    else if (404 == root["msgID"])
    {
        cout << "Response from server: " << endl;
        cout << root["msg"] << endl;
    }
    else
    {
        cout << "Something Error! System close!" << endl;
        close(netFd);
        exit(EXIT_FAILURE);
    }
}

void recvWebPages()
{
    size_t length = 0;
    recvm(&length, sizeof(size_t)); /
    char buf[length + 1] = {0};
    recvm(buf, length); 

    string msg(buf);
#ifdef __DEBUG__
    printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
    cout << msg << endl;
#endif
    Json root = json::parse(msg); 
    if (200 == root["msgID"])
    {
        cout << "Response from server: " << endl;
        showWithPaging(root);
    }
    else if (404 == root["msgID"])
    {
        cout << "Response from server: " << endl;
        cout << root["msg"] << endl;
    }
    else
    {
        cout << "Something Error! System close!" << endl;
        close(netFd);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    string ip = Configuration::getInstance()->getConfigMap()["ip"];
    string port = Configuration::getInstance()->getConfigMap()["port"];

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(atoi(port.c_str()));

    netFd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(netFd, -1, "socket");

    int ret = connect(netFd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "connect");

    while (1)
    {
        showMenu();
        cout << "Please input your option: " << endl;

        int opt;
        cin >> opt;

        string msg;

        switch (opt)
        {
        case 1:
            cout << "Please input a key: " << endl;
            cin >> msg;
            sendKey(msg);
            recvKeys();
            break;
        case 2:
            cout << "Please input a query: " << endl;
            cin >> msg;
            sendQuery(msg);
            recvWebPages();
            break;
        case 3:
            close(netFd);
            exit(EXIT_SUCCESS);
        default:
            cout << "Error option! System close!" << endl;
            close(netFd);
            exit(EXIT_FAILURE);
            break;
        }
    }

    close(netFd);

    return 0;
}
