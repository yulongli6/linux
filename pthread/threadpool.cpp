#include<iostream>
#include<time.h>
#include<queue>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
using namespace std;


class Task
{
public:
    void SetData(void* data)
    {
        _data = data;
    }

    bool Run()
    {
        srand(time(NULL));
        int nsec = rand()%5;
        cout <<"ID: " << pthread_self() << " run data:-------" 
            <<(char*)_data << "-------sleep " << nsec << endl;
        sleep(nsec);
        return true;
    }
private:
    void* _data;
};


class ThreadPool
{
public:
    ThreadPool(int max_thread = 5, int max_queue = 10)
        :_max_thread(max_thread)
         ,_cur_thread(max_thread)
         ,_stop_flag(0)
         ,_cap(max_queue)
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_empty, NULL);
        pthread_cond_init(&_full, NULL);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_empty);
        pthread_cond_destroy(&_full);
    }

    bool Init()
    {
        pthread_t tid;
        for(int i = 0; i < _max_thread; ++i)
        {
            int ret = pthread_create(&tid, NULL, thr_start, (void*)this);
            if(ret != 0)
            {
                cout << "pthread_create error" << endl;
                return false;
            }
            pthread_detach(tid);
        }
    }
    bool AddTask(Task* t)
    {
        pthread_mutex_lock(&_mutex);
        while(Full())
        {
            pthread_cond_wait(&_full, &_mutex);
        }

        QueuePush(t);
        pthread_cond_signal(&_empty);
        pthread_mutex_unlock(&_mutex);
        return true;
    }
    bool Stop()
    {
        pthread_mutex_lock(&_mutex);
        if(_stop_flag == 1)
        {
            pthread_mutex_unlock(&_mutex);
            return false;
        }
        _stop_flag = 1;
        while(_cur_thread > 0)
        {
            pthread_cond_broadcast(&_empty);
            pthread_cond_wait(&_full, &_mutex);
        }
        pthread_mutex_unlock(&_mutex);
        return false;
    }
private:

    bool Empty()
    {
        return _list.empty();
    }

    bool Full()
    {
        return (_cap == _list.size());
    }

    bool QueuePush(Task* task)
    {
        _list.push(task);
        return true;
    }

    bool QueuePop(Task** task)
    {
        *task = _list.front();
        _list.pop();
        return true;
    }

    static void* thr_start(void *arg)
    {
        ThreadPool* p = (ThreadPool*)arg;

        while(1)
        {

            pthread_mutex_lock(&p->_mutex);
            //如果队列为空，且不是退出状态，陷入等待
            while(p->Empty() && p->_stop_flag != 1)
                pthread_cond_wait(&p->_empty, &p->_mutex);
            //如果队列为空，且处于退出状态，则退出
            //退出前当前线程数-1，解锁
            if(p->Empty() && p->_stop_flag == 1)
            {
                cout << "--------thread exit" << endl;
                p->_cur_thread--;
                pthread_mutex_unlock(&p->_mutex);
                pthread_cond_signal(&p->_full);
                pthread_exit(NULL);
            }
            Task *task;
            p->QueuePop(&task);
            pthread_mutex_unlock(&p->_mutex);

            pthread_cond_signal(&p->_full);
            //任务的执行，需要放在解锁外面，因为我们不知道任务需要执行多长时间
            //如果先运行后解锁，有可能其他线程一直无法获取任务
            task->Run();
        }
        return NULL;
    }
private:
    int _max_thread;//最大线程数量
    int _cur_thread;//当前线程数量
    int _stop_flag;//线程池中线程的退出标志
    size_t _cap;       //队列最大节点数
    queue<Task*> _list;//线程池  任务队列
    pthread_mutex_t _mutex;
    pthread_cond_t _empty;
    pthread_cond_t _full;
};


int main()
{
    ThreadPool pool;
    pool.Init();
    Task task[10];
    char* c = "hello world!!!";
    for(int i = 0; i < 10; ++i)
    {
        sleep(1);
        task[i].SetData((void*)c);
        cout << "add task: " << c  <<endl;
        pool.AddTask(&task[i]);
    }
    pool.Stop();
    return 0;
}
