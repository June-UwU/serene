#pragma once
#include "serene.hpp"
#include <pthread.h>
#include <queue>
#include <vector>
#include <functional>
#include <thread>

namespace serene {

class strict_pool;
class task {

    public:
        task(std::function<u64(std::vector<void*>)> function,std::vector<void*>& arguements);
        void execute(strict_pool* thread);
    public:
        std::function<u64(std::vector<void*>&)> task_function;
        std::vector<void*> task_arguements;
};

class strict_pool {
    public:
        strict_pool(u64 thread_count = (std::thread::hardware_concurrency() / 2));

        strict_pool(const strict_pool& lhs) = delete;
        strict_pool& operator=(const strict_pool& lhs) = delete;

        strict_pool(strict_pool&& lhs);
        strict_pool& operator=(strict_pool&& lhs);
        ~strict_pool();

        void add_task(task& task);
        void compile();

        bool check_destructor();
        bool add_parameter(void* parameter);
        bool accquire_parameter(void* parameter);
        bool release_parameter(void* parameter);
        void execute_tasks();

    public:
        static void* pool_execute(void* pool_ptr);
    private:
        pthread_mutex_t destruct_guard;
        bool destruct = false;

        std::vector<pthread_t> thread_pool;

        pthread_mutex_t staging_predicate;
        std::vector<task> staging_task; 
        
        std::unordered_map<void*,pthread_mutex_t> parameter_cv; 

        pthread_mutex_t queue_guard;
        pthread_cond_t queue_cv;
        std::queue<task> task_queue;
};

}; // serene