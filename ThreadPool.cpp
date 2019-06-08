#include "ThreadPool.h"

std::queue<Task> ThreadPool::task_queue;
pthread_mutex_t ThreadPool::task_queue_mutex=PTHREAD_MUTEX_INITIALIZER;
std::vector<Thread> ThreadPool::thread_pool;
pthread_mutex_t ThreadPool::thread_pool_mutex=PTHREAD_MUTEX_INITIALIZER;
bool ThreadPool::shutdown=false;

ThreadPool::ThreadPool(int threadcount,int maxtaskcount):maxqueuetaskcount(maxtaskcount){
    initThreadPool(threadcount);
}

void ThreadPool::initThreadPool(unsigned int count){
    Thread tmp;
    static int pthcrearg[100005];
    tmp.state=false;
    for(unsigned int i=0;i<count;i++){
        pthcrearg[i]=i;
        excuteAndTest(
                pthread_cond_init(&(tmp.cond),NULL),
                "pthread_cond_init(tmp,cond)"
        );
        cout<<i<<" pthread_cond init sucess"<<endl;
        excuteAndTest(
                pthread_create(&(tmp.threadid),NULL,threadFunction,(void *)&pthcrearg[i]),
                "pthread_create()"
        );
        thread_pool.emplace_back(tmp);
    }
}

int ThreadPool::chooseLeisureThread(){
    for(int i=0;i<(int)thread_pool.size();i++){
        if(thread_pool[i].state==false) return i;
    }
    return -1;
}

void * ThreadPool::threadFunction(void *arg){
    while(1){
        excuteAndTest(
            pthread_mutex_lock(&(task_queue_mutex)),
            "task_queu_mutex_lock in threadFunction"
        );
        while(task_queue.size()==0&&shutdown==false){
            excuteAndTest(
                pthread_cond_wait(&(thread_pool[*(int*)arg].cond),&(task_queue_mutex)),
                "pthread_cond_wait in threadFunction"
            );
        }
        if(shutdown==true){
            break;
        }
        Task tt;
        pthread_cleanup_push(cleanupFunction,arg);
        tt=task_queue.front();
        task_queue.pop();
        excuteAndTest(
            pthread_mutex_unlock(&(task_queue_mutex)),
            "pthread_mutex_unlock in threadFunction"
        );
        pthread_cleanup_pop(0);
        (tt.function)(tt.arg);
        thread_pool[*(int*)arg].state=false;
        pthread_testcancel();
    }
    cout<<"thread "<<*(int*)arg<<" ready to shutdown"<<endl;
    excuteAndTest(
        pthread_mutex_unlock(&(task_queue_mutex)),
        "pthread_mutex_unlock in threadFunction"
    );
    excuteAndTest(
        pthread_cond_destroy(&(thread_pool[*(int*)arg].cond)),
        "destroy pthread in threadFunction"
    );
    cout<<"thread "<<*(int*)arg<<" cond destroy sucess"<<endl;
    pthread_exit(NULL);
}

void ThreadPool::cleanupFunction(void *arg){
    excuteAndTest(
        pthread_mutex_unlock(&(task_queue_mutex)),
        "pthread_mutex_unlock in cleanupFunction"
    );
}

bool ThreadPool::addOneTask(Task task){
    excuteAndTest(
        pthread_mutex_lock(&task_queue_mutex),
        "pthread_mutex_lock in addOneTask"
    );
    task_queue.push(task);
    excuteAndTest(
        pthread_mutex_unlock(&task_queue_mutex),
        "pthread_mutex_unlock in addOneTask"
    );

    excuteAndTest(
        pthread_mutex_lock(&thread_pool_mutex),
        "pthread_mutex_lock in addOneTask"
    );
    int leisurethread=chooseLeisureThread();
    if(leisurethread>=0){
        thread_pool[leisurethread].state=true;
        excuteAndTest(
            pthread_mutex_unlock(&thread_pool_mutex),
            "pthread_mutex_unlock in addOneTask"
        );

        excuteAndTest(
            pthread_cond_signal(&(thread_pool[leisurethread].cond)),
            "pthread_cond_signal in addOneTask"
        );
        return true;
    }else{
        cout<<"no leisure thread"<<endl;
        excuteAndTest(
            pthread_mutex_unlock(&thread_pool_mutex),
            "pthread_mutex_unlock in addOneTask"
        );
        return true;
    }
}

void ThreadPool::destroyThreadPool(){
    shutdown=true;
    for(int i=0;i<(int)thread_pool.size();i++){
        excuteAndTest(
            pthread_cond_signal(&thread_pool[i].cond),
            "cond_signal in destroyThreadPool"
        );
        cout<<"notified "<<i<<" pthread_cond to shutdown"<<endl;
    }
}

void ThreadPool::excuteAndTest(int s,string str){
    if(s!=0) cout<<str<<endl;
}
