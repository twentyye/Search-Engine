#pragma once
#include <memory>
#include <vector>
#include "Thread.h"
#include "TaskQueue.h"
using std::unique_ptr;
using std::vector;

namespace wdcpp
{
class ThreadPool
{
    friend class WorkerThread;

private:
    using Task = std::function<void()>;

public:
    ThreadPool(size_t, size_t);
    ~ThreadPool();

public:
    void start();    
    void stop();          
    void addTask(Task &&); 

private:
    void doTask();  
    Task getTask(); 

private:
    vector<unique_ptr<Thread>> _workers;
    size_t _workerNum; 
    size_t _capacity;  
    TaskQueue _taskQueue;
    bool _isExiting; 
};
};
