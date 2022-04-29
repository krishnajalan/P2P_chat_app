#include "Threadpool.h"

#include <stdio.h>
#include <stdlib.h>


void* generic_thread_function(void *arg);

ThreadPool::ThreadPool(int num_threads){
    this->num_threads = num_threads;
    this->active = 1;
    this->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    this->signal = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    pthread_mutex_lock(&this->lock);

    this->pool = (pthread_t*)malloc(sizeof(pthread_t[num_threads]));
    for(int i=0; i<num_threads; ++i){
        pthread_create(&this->pool[i], NULL, generic_thread_function, &this->pool);
    }
    pthread_mutex_unlock(&this->lock);
}

ThreadPool::~ThreadPool(){
    this->active = 0;
    for(int i=0; i<this->num_threads; ++i){
        pthread_cond_signal(&this->signal);
    }
    for(int i=0; i<this->num_threads; ++i){
        pthread_join(this->pool[i], NULL);
    }
    free(this->pool);
}

ThreadJob::ThreadJob(void* (*job)(void *arg), void* arg){
    this->job = job;
    this->arg = arg;
}

void ThreadPool::add_work(ThreadJob thread_job){
    pthread_mutex_lock(&this->lock);
    this->work.push(&thread_job);
    pthread_cond_signal(&this->signal);
    pthread_mutex_unlock(&this->lock);
}

void* generic_thread_function(void *arg){
    ThreadPool *thread_pool = (ThreadPool*) arg;
    while(thread_pool->active == 1){
        // Lock the work queue.
        pthread_mutex_lock(&thread_pool->lock);
        pthread_cond_wait(&thread_pool->signal, &thread_pool->lock);
        // Get the job from the queue.
        ThreadJob thread_job = *(ThreadJob *)thread_pool->work.front();
        // Unlock the queue.
        thread_pool->work.pop();
        pthread_mutex_unlock(&thread_pool->lock);
        // Execute the job.
        if (thread_job.job)
        {
            thread_job.job(thread_job.arg);
        }
    }
    return NULL;
}