#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>


int ticket = 100;
pthread_mutex_t mutex;
void* route(void* arg)
{
    int id = (int)arg;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(ticket > 0)
        {
            usleep(1000);
            printf("pthread %d -> %d\n", id, ticket);
            ticket--;
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}



int main()
{
    pthread_t tid[4];
    int i, ret;

    pthread_mutex_init(&mutex, NULL);
    for(i = 0; i < 4; ++i)
    {
        ret = pthread_create(&tid[i], NULL, route, (void*)i);
        if(ret != 0)
            return -1;
    }

    for(i = 0; i < 4; ++i)
        pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}

