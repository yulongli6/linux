#include <stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<syscall.h>
void* child_pthread(void* arg)
{
    while(1)
    {
        pthread_t tid1 = pthread_self();
        pid_t tid;
        tid = syscall(SYS_gettid);
        printf("i am child pthread!!pid=%d\n", tid);
        printf("子线程的地址空间pid : %p\n", tid1);
        sleep(1);
    }
    return NULL;
}
int main()
{
    pthread_t tid2;
    int ret = pthread_create(&tid2, NULL, child_pthread, NULL);
    if(ret != 0)
    {
        printf("crate pthread error\n");
        return -1;
    }

    pthread_t tid1 = pthread_self();
    while(1)
    {
        pid_t tid;
        tid = syscall(SYS_gettid);
        printf("i am main pthread!! pthread id:%p\n", tid2);
        printf("pid = %d\n", tid);
        printf("主线程地址空间地址pid: %p\n", tid1);
        sleep(1);
    }
    return 0;
}

