#pragma once
#include <pthread.h>
#include "NonCopyable.h"
#include <functional>

namespace wdcpp
{
class Thread
    : NonCopyable
{
private:
    using ThreadCallBack = std::function<void()>;

public:
    Thread(ThreadCallBack &&);
    Thread(size_t, ThreadCallBack &&);
    virtual ~Thread();

    void create(); 
    void join();   

private:
    static void *threadFunc(void *); // thread entry function 

private:
    size_t _id;
    pthread_t _thid;
    bool _isRunning;
    ThreadCallBack _cb; 
};
};
