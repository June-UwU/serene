#pragma once
#include "serene.hpp"
#include <pthread.h>
#include <queue>

namespace serene {

class task {

    public:
        task(std::function<std::vector<void*> function,std::vector<void*>& arguements);

        task(const task& lhs) = delete;
        task& operator=(const task& lhs) = delete;

        task(task&& lhs);
        task& operator=(task&& lhs);

        void execute(pthread* thread);
    private:
        pthread* handling_thread;
        std::function<u64(std::vector<void*>&) task_function;
        std::vector<void*> task_arguements;
};

class strict_pool {
    friend class task;

    public:
        strict_pool(u64 thread_count = (std::thread::hardware_concurrency() / 2));

        strict_pool(const strict_pool& lhs) = delete;
        strict_pool& operator=(const strict_pool& lhs) = delete;

        strict_pool(strict_pool&& lhs);
        strict_pool& operator=(strict_pool&& lhs);

        void add_task(task& task);
        void compile();

    private:
        bool add_parameter(void* parameter);
        void accquire_parameter(void* parameter);
        void release_parameter(void* parameter);
        
    private:
        void pool_execute(void* pool_ptr);
    private:
        std::vector<pthread_t> thread_pool;

        pthread_mutex_t staging_predicate;
        std::vector<task> staging_task; 
        
        std::unordered_map<void*,sync_primitives> parameter_cv; 

        pthread_cond_t task_predicate;
        pthread_mutex_t task_predicate_mutex;

        pthread_mutex_t queue_guard;
        std::queue<task> task_queue;
};

}; // serene