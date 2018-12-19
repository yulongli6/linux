#include <iostream>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<queue>
using namespace std;
#define QUEUE_MAX 10

class BlockQueue
{
public:
    BlockQueue(int capacity = QUEUE_MAX):_capacity(capacity)
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_full, NULL);
        pthread_cond_init(&_empty, NULL);
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_full);
        pthread_cond_destroy(&_empty);
    }

    void push(int data)
    {
        pthread_mutex_lock(&_mutex);
        //用if是错误的
        while(_capacity == _list.size())
        {
            cout << "---------------full" << endl;
            pthread_cond_wait(&_full, &_mutex);
        }

        _list.push(data);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_empty);
    }

    void pop(int* data)
    {
        pthread_mutex_lock(&_mutex);
        while(_list.empty())
        {
            cout << "---------------empty" << endl;
            pthread_cond_wait(&_empty, &_mutex);
        }

        *data = _list.front();
        _list.pop();
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_full);
    }
private:
    queue<int> _list;
    size_t _capacity;
    pthread_mutex_t _mutex;
    pthread_cond_t _full;
    pthread_cond_t _empty;
};


BlockQueue q;
int i = 1;
void* productor(void *arg)
{
    while(1)
    {
        cout << "productor_pthread:"<< arg  << " product:" << i << endl;
        q.push(i++);
    }
    return NULL;
}


void* consumer(void* arg)
{
    while(1)
    {
        int data;
        q.pop(&data);
        cout << "consumer_pthread:" << arg << " consume:" << data << endl;
    }
    return NULL;
}

int main()
{
    pthread_t tid1[4];
    pthread_t tid2[4];
    int ret;
    for(int i = 0; i < 4; ++i)
    {
        ret = pthread_create(&tid1[i], NULL, productor, (void*)i);
        if(0 != ret)
        {
            cout << "create pthread error" << endl;
            return -1;
        }
    }

    for(int i = 0; i < 4; ++i)
    {
        ret = pthread_create(&tid2[i], NULL, consumer, (void*)i);
        if(0 != ret)
        {
            cout << "create pthread error" << endl;
            return -1;
        }
    }

    for(int i = 0; i < 4; i++)
    {
        pthread_join(tid1[i], NULL);
        pthread_join(tid2[i], NULL);
    }
    return 0;
}

