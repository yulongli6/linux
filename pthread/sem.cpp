#include <iostream>
#include<unistd.h>
#include<stdlib.h>
#include<semaphore.h>
#include<errno.h>
#include<pthread.h>
using namespace std;

sem_t sem;
void* productor(void* arg)
{
    while(1)
    {
        sem_wait(&sem);
        cout << "create noodle!!!" << endl;
        sem_post(&sem);
        usleep(10);
    }
    return NULL;
}

void* consumer(void* arg)
{
    while(1)
    {
        sem_wait(&sem);
        cout << "eat noodle!!!" << endl;
        sem_post(&sem);
        usleep(10);
    }
    return NULL;
}
int main()
{
    pthread_t tid1,tid2;
    
    sem_init(&sem, 0, 1);

    pthread_create(&tid1, NULL, productor, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    sem_destroy(&sem);
    return 0;
}

