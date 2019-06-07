#include <iostream>
#include <unistd.h>
#include "ThreadPool.h"
using namespace std;

void * mytask(void *arg){
    //int num=voidptrtoint(arg);
    cout<< "excute "<< *(int *)arg <<" task..."<<endl;
}

int main(){
    ThreadPool threadpool(4,9999);
    Task tmp;
    //vector<int> vi;
    int vi[9999];

    for(int i=0;i<10;i++){
        //vi.emplace_back(i);
        vi[i]=i;
        tmp.function=mytask;
        tmp.arg=(void *)&(vi[i]);
        threadpool.addOneTask(tmp);
        sleep(2);
    }
    sleep(5);
    threadpool.destroyThreadPool();
    return 0;
}

