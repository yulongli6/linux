/*用vector实现环形队列
 * 使用信号量来对这个环形队列资源进行计数
 * 两个信号量：
 *  一个是空闲空间资源计数       等于vector节点数
 *  一个是写入数据的空间计数     初始值为0
*/
#include <iostream>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<vector>
using namespace std;
#define MAX_QUEUE 10
class RingQueue
{
private:
    vector<int> _q;
    int _cap;
    int _consumer_step;
    int _productor_step;
    sem_t _idle;
    sem_t _data;
public:
    RingQueue(int cap = MAX_QUEUE):_cap(cap),_q(cap)
    {
        _consumer_step = 0;
        _productor_step = 0;
        sem_init(&_idle, 0, cap);
        sem_init(&_data, 0, 0);
    }

    ~RingQueue()
    {
        sem_destroy(&_data);
        sem_destroy(&_idle);
    }

    bool GetData(int& data)
    {
        sem_wait(&_data);
        data = _q[_consumer_step++];
        _consumer_step %= _cap;
        sem_post(&_idle);
        return true;
    }

    bool PutData(int data)
    {
        sem_wait(&_idle);
        _q[_productor_step++] = data;
        _productor_step %= _cap;
        sem_post(&_data);
        return true;
    }
};

void* consumer(void* arg)
{
    RingQueue *q = (RingQueue*)arg;
    while(1)
    {
        int data;
        q->GetData(data);
        cout << "data:" << data << endl;
    }
    return NULL;
}

void* productor(void* arg)
{
    RingQueue *q = (RingQueue*)arg;
    int i = 0;
    while(1)
    {
        q->PutData(i++);
        cout << "put data:" << i << endl;
    }
    return NULL;
}

int main()
{
    RingQueue q;
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, consumer, (void*)&q);
    pthread_create(&tid2, NULL, productor, (void*)&q);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}

