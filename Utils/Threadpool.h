#ifndef ThreadPool_h
#define ThreadPool_h

#include <queue>
#include <pthread.h>

class ThreadJob{
    public:
        void* (*job)(void *arg);
        void *arg;
        ThreadJob(void* job(void *arg), void* arg);
}

class ThreadPool{
    public:
        int num_threads;
        int active;
        std::queue<ThreadJob*> work;
        pthread_t *pool;
        pthread_mutex_t lock;
        pthread_cond_t signal;

        ThreadPool(int num_thread);
        ~ThreadPool();
        void add_work(ThreadJob thread_job);
}

#endif