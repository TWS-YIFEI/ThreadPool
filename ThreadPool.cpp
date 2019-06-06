#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <pthread.h>
#include "ThreadPool.h"

using namespace std;


ThreadPool::ThreadPool(unsigned int count){
    initMutex();
    initThreadPool(count);
}

void ThreadPool::initMutex(){
    excuteAndTest(pthread_mutex_init(&task_queue_mutex,NULL),
            "pthread_mutex_init(task_queue_mutex)");
    excuteAndTest(pthread_mutex_init(&thread_pool_mutex,NULL),
            "pthread_mutex_init(thread_pool_mutex)");
}

void ThreadPool::initThreadPool(unsigned int count){
    Thread tmp;
    void *arg;
    excuteAndTest(pthread_cond_init(&(tmp.cond),NULL),
            "pthread_cond_init(tmp,cond)");
    tmp.state=false;
    for(unsigned int i=0;i<count;i++){
        excuteAndTest(pthread_create(&(tmp.threadid),NULL,threadFunction,arg),
                "pthread_create()");
        excuteAndTest(pthread_detach((tmp.threadid)),
                "pthread_detach");
        thread_pool.emplace_back(tmp);
    }
}

unsigned int chooseLeisureThread(){
}



void * ThreadPool::threadFunction(void *arg){

}

inline void ThreadPool::excuteAndTest(int s,string str){
    if(s!=0) cout<<str<<endl;
}
