#ifndef THREAD_POOL
#define THREAD_POOL

#include <queue>
#include <vector>
#include <string>
#include <pthread.h>
using namespace std;

class ThreadPool{
private:
    typedef struct{
        void *(*function)(void *);
        void *arg;
    }Task;
    typedef struct{
        pthread_t threadid;
        bool state;//false休闲  true忙碌
        pthread_cond_t cond;
    }Thread;

    queue<Task> task_queue;
    pthread_mutex_t task_queue_mutex;
    //pthread_cond_t cond_to_thread;
    vector<Thread> thread_pool;
    pthread_mutex_t thread_pool_mutex;

private:
    void initMutex();
    void initThreadPool(unsigned int count);
    unsigned int chooseLeisureThread();
    static void *threadFunction(void *arg);
    inline void excuteAndTest(int s,string str);

public:
    ~ThreadPool();

    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;

    explicit ThreadPool(unsigned int count);
    bool addOneTask(Task task);
};

#endif
