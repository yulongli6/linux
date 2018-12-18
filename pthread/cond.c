#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

pthread_cond_t sale;
pthread_cond_t eat;
pthread_mutex_t mutex;
int have_noodle = 0;

void* sale_noodle(void* arg)
{
    int id = (int)arg;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(have_noodle == 1)
            pthread_cond_wait(&sale, &mutex);
        printf("pthread %d create noodle!!\n", id);
        have_noodle = 1;

        //生产出来后通知买方便面的人
        pthread_cond_signal(&eat);
        pthread_mutex_unlock(&mutex);

    }

    return NULL;
}

void* eat_noodle(void* arg)
{
    while(1)
    {
        //因为have_noodle的操作也是一个临界资源的操作，因此需要被
        //保护，使用互斥锁进行保护
        pthread_mutex_lock(&mutex);
        if (have_noodle == 0) 
        {
            //因为等待时间不确定，因此有可能会浪费很多等待时间
            //因此使用环境变量提供的死等操作，但是这个死等需要能够
            // 被唤醒，这样的话，一旦方便面生产出来直接唤醒我们的死
            //等，不会浪费多余的等待时间
            //防止不满足条件陷入休眠，没有解锁，对方无法获取锁，没
            //办法生产方便面，因此需要解锁，
            //但是解锁和休眠必须是原子操作
             pthread_cond_wait(&eat, &mutex);
            //被唤醒，这时候可以继续吃面，并且修改条件，但是条件是
            //临界资源，因此需要加锁，
            // pthread_cond_wait整体操作
            // 解锁-》休眠-》被唤醒后加锁(但是这不是一个阻塞操作,
            //而是直接计数器置0)

        }
        printf("eat noodle!! good!!\n");
        have_noodle = 0;
        pthread_mutex_unlock(&mutex);
        //吃完之后通知一下卖方便面的
        pthread_cond_signal(&sale);
    }
}

int main()
{
    pthread_t tid1, tid2;
    int ret;

    //条件变量初始化
    pthread_cond_init(&eat, NULL);
    pthread_cond_init(&sale, NULL);
    pthread_mutex_init(&mutex, NULL);

    ret = pthread_create(&tid1, NULL, sale_noodle, (void*)1);
    if(ret != 0)
        return -1;
    ret = pthread_create(&tid1, NULL, sale_noodle, (void*)2);
    if(ret != 0)
        return -1;
    ret = pthread_create(&tid2, NULL, eat_noodle, NULL);
    if(ret != 0)
        return -1;

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    //条件变量销毁
    pthread_cond_destroy(&eat);
    pthread_cond_destroy(&sale);
    pthread_mutex_destroy(&mutex);
    return 0;
}

