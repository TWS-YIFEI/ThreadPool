#ifndef THREAD_POOL
#define THREAD_POOL

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <pthread.h>
using namespace std;

class ThreadPool;

typedef struct{
    void *(*function)(void *);
    void *arg;
}Task;

typedef struct{
    pthread_t threadid;
    bool state;//false休闲  true忙碌
    pthread_cond_t cond;
 }Thread;

typedef struct{
    ThreadPool *tp;
    int index;
}PthreadCreateArgs;

class ThreadPool{
private:
    static queue<Task> task_queue;
    static pthread_mutex_t task_queue_mutex;
    int maxqueuetaskcount;
    static vector<Thread> thread_pool;
    static pthread_mutex_t thread_pool_mutex;
    static bool shutdown;

private:
    void initMutex();
    void initThreadPool(unsigned int count);
    int chooseLeisureThread();
    static void *threadFunction(void *arg);
    static void cleanupFunction(void *arg);
    static void excuteAndTest(int s,string str);

public:
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;

    explicit ThreadPool(int threadcount,int maxtaskcount);
    bool addOneTask(Task task);
    void destroyThreadPool();
};

#endif
