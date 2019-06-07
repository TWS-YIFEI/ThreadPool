//#include <iostream>
//#include <queue>
//#include <vector>
//#include <string>
//#include <pthread.h>
#include "ThreadPool.h"

//using namespace std;

ThreadPool::ThreadPool(int threadcount,int maxtaskcount):maxqueuetaskcount(maxtaskcount){
    initMutex();
    initThreadPool(threadcount);
}

void ThreadPool::initMutex(){
    excuteAndTest(pthread_mutex_init(&task_queue_mutex,NULL),
            "pthread_mutex_init(task_queue_mutex)");
    cout<<"task_queue_mutex init sucess"<<endl;
    excuteAndTest(pthread_mutex_init(&thread_pool_mutex,NULL),
            "pthread_mutex_init(thread_pool_mutex)");
    cout<<"thread_pool_mutex init sucess"<<endl;
}

void *ThreadPool::initThreadPool(unsigned int count){
    Thread tmp;
    PthreadCreateArgs pca;
    pca.tp=this;
    excuteAndTest(pthread_cond_init(&(tmp.cond),NULL),
            "pthread_cond_init(tmp,cond)");
    tmp.state=false;
    for(unsigned int i=0;i<count;i++){
        pca.index=i;
        excuteAndTest(pthread_create(&(tmp.threadid),NULL,threadFunction,(void *)&pca),
                "pthread_create()");
        excuteAndTest(pthread_detach((tmp.threadid)),
                "pthread_detach");
        thread_pool.emplace_back(tmp);
    }
}

int ThreadPool::chooseLeisureThread(){
    for(int i=0;i<thread_pool.size();i++){
        if(thread_pool[i].state==false) return i;
    }
    return -1;
}

void * ThreadPool::threadFunction(void *arg){
    PthreadCreateArgs pca=*(PthreadCreateArgs*)arg;
    while(1){
        pca.tp->excuteAndTest(pthread_mutex_lock(&(pca.tp->task_queue_mutex)),
            "pthread_mutex_lock in threadFunction");
        while(pca.tp->task_queue.size()==0){
            pca.tp->excuteAndTest(pthread_cond_wait(&(pca.tp->thread_pool[pca.index].cond),&(pca.tp->task_queue_mutex)),
                "pthread_cond_wait in threadFunction");
        }
        pthread_cleanup_push(cleanupFunction,arg);
        Task tmp=pca.tp->task_queue.front(); pca.tp->task_queue.pop();
        pca.tp->excuteAndTest(pthread_mutex_unlock(&(pca.tp->task_queue_mutex)),
            "pthread_mutex_unlock in threadFunction");
        pthread_cleanup_pop(0);
        (tmp.function)(tmp.arg);
        pca.tp->thread_pool[pca.index].state=false;
        pthread_testcancel();
    }
}

void ThreadPool::cleanupFunction(void *arg){
    PthreadCreateArgs pca=*(PthreadCreateArgs*)arg;
    pca.tp->excuteAndTest(pthread_mutex_unlock(&(pca.tp->task_queue_mutex)),
            "pthread_mutex_unlock in cleanupFunction");
}

bool ThreadPool::addOneTask(Task task){
    excuteAndTest(pthread_mutex_lock(&task_queue_mutex),
        "pthread_mutex_lock in addOneTask");
    task_queue.push(task);
    excuteAndTest(pthread_mutex_unlock(&task_queue_mutex),
        "pthread_mutex_unlock in addOneTask");

    excuteAndTest(pthread_mutex_lock(&thread_pool_mutex),
        "pthread_mutex_lock in addOneTask");
    int leisurethread=chooseLeisureThread();
    thread_pool[leisurethread].state=true;
    excuteAndTest(pthread_mutex_unlock(&thread_pool_mutex),
        "pthread_mutex_unlock in addOneTask");

    excuteAndTest(pthread_cond_signal(&(thread_pool[leisurethread].cond)),
        "pthread_cond_signal in addOneTask");
    return true;
}

void ThreadPool::destroyThreadPool(){
    for(int i=0;i<thread_pool.size();i++){
        excuteAndTest(pthread_cond_destroy(&thread_pool[i].cond),
            "pthread_cond_destory in ~ThreadPool");
        excuteAndTest(pthread_cancel(thread_pool[i].threadid),
                "pthread_cancel in ~ThreadPool");
        cout<<"thread "<<i<<" destroy..."<<endl;
    }
    excuteAndTest(pthread_mutex_unlock(&task_queue_mutex),
            "task_queue_mutex unlock in ~ThreadPool");
    excuteAndTest(pthread_mutex_unlock(&thread_pool_mutex),
            "thread_pool_mutex unlock in ~ThreadPool");

    excuteAndTest(pthread_mutex_destroy(&task_queue_mutex),
            "task_queue_mutex destory in ~ThreadPool");
    excuteAndTest(pthread_mutex_destroy(&thread_pool_mutex),
            "thread_pool_mutex destory in ~ThreadPool");

}

void ThreadPool::excuteAndTest(int s,string str){
    if(s!=0) cout<<str<<endl;
}
