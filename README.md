# ThreadPool
c++/threadpool/线程池

### 简介
这个线程池是在学习完《Linux/UNIX系统编程手册》中线程相关知识后用来练手的小项目，线程相关函数都是直接调用Linux的API，并且使用了C++中的queue和vector。
虽然C++中也提供了线程创建、互斥锁等函数库，但是也是对系统函数的封装。并且作为初学，先学会用原生函数比较好。

### 基础知识
* C++面向对象、queue、vector
* pthread API
* 互斥锁、条件变量

### 结构
为了缓存添加进来的任务，需要创建一个队列来存储任务，并且还用一个vector数组来存储线程。
使用的是生产者-消费者模型，其中addOneTask()是往队列中添加任务，是生产者。而执行任务的线程有多个，所以每一个线程都是消费者。
每次添加完任务之后，会使用条件变量通知“一个”空闲线程来执行任务
![架构图](https://i.loli.net/2019/06/08/5cfb92147394049618.png)

### 遇到的问题
* 惊群问题
在每次添加任务之后，需要通知一个线程来执行任务，这里如果使用pthread_cond_broadcast(),
就会唤醒所有线程，而只能有一个线程得到任务，其他线程只能回去继续等待。这样就造成了不必要的浪费。
但即使使用pthread_cond_signal(),好像有些系统的实现，也可能会唤醒不止一个线程。
为了解决这个问题，可以给每一个线程添加一个条件变量，如果有任务被添加，此时只要看哪个线程处于空闲状态，只通知那一个线程即可。

* 静态函数访问非静态成员
在使用pthread_create()创建线程的时候，往里传的函数必须是静态函数，但是我们经常会需要在这个静态函数里访问类的非静态成员变量，那怎么办呢？
这里已知有两种方法解决这个问题：
1.创建线程时，需要用arg往里传递回调函数的参数，可以在这里把当前对象的地址封装到回调函数的参数arg里面，然后在回调函数中使用这个对象地址来调用他的非静态成员变量
2.直接把需要访问的普通成员变量改成静态的。由于这种方法比较简单，并且已经满足当前需求，在线程池使用的这种方法。

### 改进
* queue锁的细粒度
STL中queue不是线程安全的，所以如果加锁的话只能给整个队列加锁，而不能给入队和出队两个操作分别加锁。
所以添加任务和执行任务两个操作并不能同时进行。
等以后写一个线程安全的队列时，再来改进这个线程池。

* 线程优先级
有机会再扩展。

### 其他
* 关于线程数量的设置
N核服务器，通过执行业务的单线程分析出本地计算时间为x，等待时间为y，则工作线程数（线程池线程数）设置为 N*(x+y)/x，能让CPU的利用率最大化。

* 思路不难，但是真正写代码才发现有好多需要考虑的细节。

* 待学习:使用gdb调试多线程
[线程的查看以及利用gdb调试多线程](https://blog.csdn.net/zhangye3017/article/details/80382496)

### 使用方法
```cpp
 #include <iostream>
 #include <unistd.h>
 #include "ThreadPool.h"
using namespace std;

void * mytask(void *arg){
    cout<< "excute "<< *(int *)arg <<" task..."<<endl;
}

int main(){
    ThreadPool threadpool(20,100005);
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
```
```

### 编译运行
```shell
cd ThreadPool
g++ Test.cpp ThreadPool.cpp -o test -lpthread
./test
```

### 参考
* [线程数究竟设多少合理](https://blog.csdn.net/u010942020/article/details/52299281)
* [C++并发实战17：线程安全的stack和queue](https://blog.csdn.net/liuxuejiang158blog/article/details/17523477)
* [linyacool WebServer中的线程池](https://github.com/linyacool/WebServer/tree/master/WebServer)
* [用C++写线程池是怎样一种体验？](https://www.zhihu.com/question/27908489)
* [基于c++11的100行实现简单线程池](https://blog.csdn.net/gcola007/article/details/78750220)
* [使用C++11实现线程池的两种方法](https://blog.csdn.net/liushengxi_root/article/details/83932654)
