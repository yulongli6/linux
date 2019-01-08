#include <iostream>
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define QUEUE_MAX 10
class BlockQueue
{
private:
    std::queue<int> _list;
    int _cap;
    pthread_mutex_t _mutex;
    pthread_cond_t  _full;
    pthread_cond_t  _empty;
public:
    BlockQueue(int cap = QUEUE_MAX):_cap(cap) 
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_full, NULL);
        pthread_cond_init(&_empty, NULL);

    }
    ~BlockQueue() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_full);
        pthread_cond_destroy(&_empty);

    }
    bool QueuePush(int data)
    {
        pthread_mutex_lock(&_mutex);
        if(_cap == _list.size()) {
            printf("-------------------full\n");
            pthread_cond_wait(&_full, &_mutex);

        }
        _list.push(data);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_empty);
        return true;

    }
    bool QueuePop(int *data)
    {
        pthread_mutex_lock(&_mutex);
        if(_list.empty()) {
            printf("-------------------empty\n");
            pthread_cond_wait(&_empty, &_mutex);

        }
        *data = _list.front();
        _list.pop();
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_full);
        return true;

    }

};

void *productor(void *arg)
{
    BlockQueue *q = (BlockQueue*)arg;
    int i = 0;
    while(1) {
        sleep(1);
        printf("put data:%d\n", i);
        q->QueuePush(i++);

    }
    return NULL;

}
void *consumer(void *arg)
{
    BlockQueue *q = (BlockQueue*)arg;
    while(1) {
        sleep(1);
        int data;
        q->QueuePop(&data);
        printf("get data:%d\n", data);

    }
    return NULL;

}
int main()
{
    pthread_t tid1, tid2;
    BlockQueue q;

    int ret = pthread_create(&tid1, NULL, productor, (void*)&q);
    if (ret != 0) {
        printf("create pthread error\n");
        return -1;

    }
    ret = pthread_create(&tid2, NULL, consumer, (void*)&q);
    if (ret != 0) {
        printf("create pthread error\n");
        return -1;

    }
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;

}




0x00, 0x40, 0x00, 0x40, 0x78, 0xA0, 0x49, 0x10,
0x4A, 0x08, 0x4C, 0x06, 0x4B, 0xF8, 0x48, 0x00,
0x48, 0x00, 0x4B, 0xF8, 0x7A, 0x08, 0x4A, 0x08,
0x02, 0x08, 0x02, 0x08, 0x03, 0xF8, 0x02, 0x08,
