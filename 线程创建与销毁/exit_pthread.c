#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

void* thr_start(void* arg)
{

    pthread_t tid = pthread_self();
    int ret = (int)arg;
    while(1)
    {
        printf("child pthread!! %d \n", ret);
        sleep(1);
        pthread_cancel(tid);
    }

    return NULL;
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thr_start, (void*)88);
    if(0 != ret)
    {
        printf("create pthread error\n");
        return -1;
    }
    
    while(1)
    {
        printf("main pthread\n");
        sleep(1);
    }

    return 0;
}

