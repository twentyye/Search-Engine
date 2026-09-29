#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <utility>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <poll.h>
#include <ErrorCheck>
#include <algorithm>
using namespace std;

void test1()
{
    // splice
    list<int> list = {1, 2, 3, 34};
    auto it = list.begin();
    while (it != list.end())
    {
        if (*it == 2)
            break;
        ++it;
    }
    list.splice(list.begin(), list, it);
    for (auto &elem : list)
    {
        cout << elem << " ";
    }
}

void test2()
{
    // create
    int _timerFd = ::timerfd_create(CLOCK_REALTIME, 0);
    if (_timerFd < 0)
    {
        perror("timerfd_create");
    }

    // settime
    struct itimerspec value;
    value.it_value.tv_sec = 1;
    value.it_value.tv_nsec = 0;
    value.it_interval.tv_sec = 3;
    value.it_interval.tv_nsec = 0;
    int ret = ::timerfd_settime(_timerFd, 0, &value, nullptr);
    if (ret < 0)
    {
        perror("timerfd_settime");
    }

    // start poll
    struct pollfd pfd;
    pfd.events = POLLIN;
    pfd.fd = _timerFd;

    while (1)
    {
        int nready = -1;
        do
        {
            nready = ::poll(&pfd, 1, 5500);
        } while (nready == -1 && errno == EINTR);

        if (nready == -1)
        {
            perror("poll");
            return;
        }
        else if (0 == nready)
            printf("poll timeout!\n");
        else // timer expires
        {
#ifdef __DEBUG__
            printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
            cout << "timer thread: timer is on time" << endl;
#endif
            if (pfd.revents & POLLIN)
            {
                // handleRead
                uint64_t value = 2;
                int ret = ::read(_timerFd, &value, sizeof(value));
                if (ret != sizeof(value))
                {
                    perror("read");
                }
                // _timerCb
                cout << "working..." << endl;
            }
        }
    }
}

void test4()
{
    string s = "abc中国人cc哈哈";
    size_t pos = s.find("美国");
    cout << s.size() << endl;
    if (pos != s.npos)
        cout << s.substr(pos) << endl;
}

void test5()
{
    vector<int> v = {1, 3, 5, 7};
    remove(v.begin(), v.end(), 5);
    cout << v.size() << endl;
    for (auto &i : v)
        cout << i << " ";
}

int main(int argc, char *argv[])
{
    // test2();
    // test3();
    // test4();
    test5();
    return 0;
}