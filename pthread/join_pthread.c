#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<errno.h>
void* thr_start(void* arg)
{
    pthread_detach(pthread_self());
    printf("arg:%d\n",(int)arg);
    sleep(10);
    return NULL;
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thr_start, (void*)88);

    if(ret != 0)
    {
        printf("cteate pthread error\n");
        return -1;
    }

    sleep(2);
    pthread_cancel(tid);//PTHREAD_CANCELED
    char* retval;
    ret = pthread_join(tid, (void**)&retval);
    if(ret == EINVAL)
    {
        printf("pthread is not joinable\n");
    }

    printf("--------------[%d]\n",(int)retval);
    return 0;
}

