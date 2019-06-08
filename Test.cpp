#include <iostream>
#include <unistd.h>
#include "ThreadPool.h"
using namespace std;

void * mytask(void *arg){
    cout<< "excute "<< *(int *)arg <<" task..."<<endl;
}

int main(){
    ThreadPool threadpool(5,100005);
    Task tmp;
    int vi[100005];

    for(int i=0;i<100;i++){
        vi[i]=i;
        tmp.function=mytask;
        tmp.arg=(void *)&(vi[i]);
        threadpool.addOneTask(tmp);
    }
    sleep(4);
    cout<<"ready to destroy"<<endl;
    threadpool.destroyThreadPool();
    sleep(4);
    return 0;
}

