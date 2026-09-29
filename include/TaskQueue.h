#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include <queue>
#include <functional>
using std::queue;

namespace wdcpp
{
class TaskQueue
{
private:
    using Task = std::function<void()>;

public:
    TaskQueue(size_t);

    bool full() const;
    bool empty() const;
    void push(Task &&); 
    Task pop();         
    void wakeupEmpty(); 

private:
    queue<Task> _queue;
    size_t _capacity;
    MutexLock _mutex;
    Condition _full;  // main thread
    Condition _empty; // child thread
    bool _isExiting;  //when the thread is awakened,if queue no task, it can exit directly 
};
};
