#include <iostream>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
using namespace std;

pthread_rwlock_t rwlock;
void* reader(void* arg)
{
    cout << "start read" << endl;
    while(1)
    {
        pthread_rwlock_rdlock(&rwlock);
        cout << "read lock!!" <<pthread_self() << endl;
        sleep(1);
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

void* writer(void* arg)
{
    while(1)
    {
        pthread_rwlock_wrlock(&rwlock);
        cout << "------write lock!!" << endl;
        sleep(3);
        pthread_rwlock_unlock(&rwlock);
        cout << "------write over" << endl;
    }
    return NULL;
}

int main()
{
    pthread_t rtid[4], wtid;

    //设置写锁优先
    pthread_rwlockattr_t attr;
    //初始化读写锁属性
    pthread_rwlockattr_init(&attr);
    //设置写锁优先
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    int i;
    pthread_rwlock_init(&rwlock, &attr);
    //释放读写锁属性
    pthread_rwlockattr_destroy(&attr);
    for(i = 0; i < 4; ++i)
    {
        pthread_create(&rtid[i], NULL, reader, NULL);
    }

    pthread_create(&wtid, NULL, writer, NULL);

    pthread_join(wtid, NULL);

    for(i = 0; i < 4; ++i)
    {
        pthread_join(rtid[1], NULL);
    }


    pthread_rwlock_destroy(&rwlock);
    return 0;
}

