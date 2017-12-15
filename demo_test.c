
/*
 * Author:donghao
 *要求： “编写一个程序，开启3个线程，这3个线程的ID分别为A、B、C，每个线程将自己的ID在屏幕上打印10遍，
          要求输出结果必须按ABC的顺序显示；如：ABCABC….依次递推。” 
 */
 
 /*
 互斥锁和条件变量的应用 条件变量常常实在互斥锁的保护下进行如:
 如:
 thread1()
 {
     ...
     pthread_mutex_lock();
     if()
        pthread_cond_signal()//条件改变，发送信号，通知thread2进程,结束thread1
     pthread_mutex_unlock();
     ...
 }
 thread2()
 {
    ...
    pthread_mutex_lock();
    while (condition_is_false)
        pthread_cond_wait();//阻塞函数,条件不满足时,阻塞结束,(与thread1相反)
    pthread_mutex_unlock();
    ...
 }
 */
 #include<stdio.h>
 #include<stdlib.h>
 #include<pthread.h>
 #include<unistd.h>

int num=0;//global
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //静态初始化锁   protect global
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //global变量共享

void *thread_fanc(void *arg);

 int main()
 {
     pthread_t id[3];
     int reback;
     int i=0;
     for(i = 0; i<3; i++)
     {
         reback = pthread_create(&id[i],NULL,thread_fanc,(void *)(intptr_t)i);
         if(reback != 0)
         {
             printf("create thread error!\n");
             exit(1);
         }
     }
     for(i = 0;i<3;i++)
        pthread_join(id[i],NULL);
     printf("\n");

     pthread_mutex_destroy(&mutex);
     pthread_cond_destroy(&cond);
     return 0;
 }
 
 void *thread_fanc(void *arg)
 {
     int info = (int)arg;
     int i=0;
     for(i=0;i<10;i++)
     {
         //sleep(1);位置不一样,效果不一样
         //互斥锁保护条件表达式的判断
         pthread_mutex_lock(&mutex);
         while(num != info)
            pthread_cond_wait(&cond, &mutex);
         printf("%c",info+'A');
         num = (num+1)%3;
         //解除阻塞,其他的线程继续竞争
         //linux下pthread_cond_broadcast(&cond)写在前面, cond_signal(cond_broadcast)只是让线程从cond_wait队列移到mutex_lock队列
         pthread_cond_broadcast(&cond);
         pthread_mutex_unlock(&mutex);
         //缺点:如果unlock和signal之前，有个低优先级的线程正在mutex上等待的话，那么这个低优先级的线程就会抢占高优先级的线程（cond_wait的线程).
         //pthread_cond_broadcast(&cond);
     }
     pthread_exit(NULL);
     return ((void *) 0);
 }
