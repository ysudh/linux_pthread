#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
/*
多线程通信实例,传递参数
int pthread_self(void); 返回线程ID/getpid(进程ID)
在线程里用sleep()函数,让出线程执行权利
pthread_exit()线程/进程的主动退出方式,其占用的资源并未释放
pthread_cancel() pthread_setcancel() pthread_setcanceltype()  设置一个线程终止另一个线程
pthread_join() 将当前线程挂起,等待完成,是个线程阻塞函数,调用它的函数将一直等到被等待的线程函数结束为止.
若没有此函数或是sleep()函数,线程函数将只执行一个轮转时间而结束.
*/

/*
同步机制 ...共享资源...
B线程要在A线程完成之后,在执行:
利用互斥锁+条件变量

互斥锁:快速互斥锁,递归互斥锁,检错互斥锁.
简单的加锁方式来控制对共享资源的原子操作
初始化:pthread_mutex_init()
上锁:pthread_mutex_lock()
尝试上锁:pthread_mutex_trylock()
解锁:pthread_mutex_unlock()
消除:pthread_mutex_destroy()

条件变量 
pthread_cond_wait();
被阻塞的线程可以被 pthread_cond_signal()函数， pthread_cond_broadcast()函数唤醒，也可能在被信号中断后被唤醒.


信号量:
*/

void print_msg(void *arg);
//建立锁
pthread_mutex_t mutex;

int main()
{
    //printf("%d %d",sizeof(int),sizeof(void *));int 4字节 void *八个字节(指针64位,64位寻址)
    //把指针复制给int型,会丢数据?
    pthread_t thread[3];
    int no = 0,reback;  
    void * thrd_set;
    srand(time(NULL));
    /*锁初始化*/
    pthread_mutex_init(&mutex,NULL);
    for(no = 0;no<3;no++)
    {
        reback = pthread_create(&thread[no],NULL,(void *)(&print_msg),(void *)(intptr_t)no);
        //reback = pthread_create(&thread[no],NULL,(void *)(&print_msg),&no);
        if(reback != 0)
        {
            printf("Creat pthread error!\n");
            exit(1);//退出进程
        } 
    }
    printf("Creat threads ok,\n");
    //sleep(10);等待10s结束(不管线程函数执行到哪里)
    for(no = 0;no<3;no++)
    {
        //pthread_join() 使得线程函数可以执行完成
        reback = pthread_join(thread[no],&thrd_set);
        if(!reback)
        {
            printf("thread %d joined\n",no);
        }
        else
        {
            printf("thread %d joined FAILED\n",no);
        }
        /*解锁*/
        pthread_mutex_unlock(&mutex);
    }
    /*消除锁*/
    pthread_mutex_destroy(&mutex);
    return 0;
}
void print_msg(void *arg)
{
    //int thrd_num = *(int *)arg;//出错
    //intptr_t 和uintptr_t 类型用来存放指针地址。它们提供了一种可移植且安全的方法声明指针，
    //而且和系统中使用的指针长度相同，对于把指针转化成整数形式来说很有用。
    int thrd_num = (int)arg;//强制转换  warning
    int delay_time = 0,count = 0;
    int res;
    /*上锁*/
    res = pthread_mutex_lock(&mutex);
    if(res)
    {
        printf("thread %d lock failed\n",thrd_num);
        pthread_exit(NULL);
    }

    printf("i am pthread:%d\n",thrd_num);
    for(count = 0;count<5;count++)//类似5个任务
    {
        //如果把sleep()去掉,出现问题??????????????????????????????????????
        delay_time = (int)(rand()*5.0/(RAND_MAX))+1;
        sleep(delay_time);
        printf("\tThread %d:job %d delay = %d\n",thrd_num,count,delay_time);
    }
    printf("thread %d finished\n",thrd_num);
    pthread_exit(NULL);
}

/*
//定时器的使用实例
void sigalrm_fn(int sig)
{
    printf("alarm\n");
    alarm(2);
    return ;
}
int main()
{
    //定时函数的使用
    //使用alarm函数跟setitimer函数来实现定时功能
    signal(SIGALRM,sigalrm_fn);
    alarm(1);
    
    while(1)
    pause();//pause()会令目前的进程暂停(进入睡眠状态), 直到被信号(signal)所中断
    return 0;
}
*/