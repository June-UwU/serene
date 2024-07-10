#include "strict_pool.hpp"

namespace serene {
task::task(std::function<std::vector<void*> function,std::vector<void*>& arguements) 
    : task_function(function)
    , task_arguements(arguements)
    {};
 
task::task(task&& lhs) {
    task_function = lhs.task_function;
    task_arguements = lhs.task_arguements;
}

task::task& operator=(task&& lhs) {
    task_function = lhs.task_function;
    task_arguements = lhs.task_arguements;

    return *this;
}

void task::execute(pthread* thread) {
    handling_thread = thread;
    task_function(task_arguements);
}

strict_pool::strict_pool(u64 thread_count) {
    thread_pool = std::vector<pthread_t>(thread_count);

    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    for(auto& thread: thread_pool) {
        int up_status = pthread_create(&thread,&attributes,this->pool_execute,this);
        if(0 != up_status) {
            std::cout << "thread creation error : " << up_status << "\n";
        }
    }

    pthread_attr_destroy(&attributes);
}

strict_pool::strict_pool(strict_pool&& lhs) {
    thread_pool = std::move(lhs.thread_pool);

    staging_predicate = std::move(lhs.staging_predicate);
    staging_task = std::move(lhs.staging_task); 
        
    parameter_cv = std::move(lhs.parameter_cv); 

    task_predicate = std::move(lhs.task_predicate);
    task_predicate_mutex = std::move(lhs.task_predicate_mutex);

    queue_guard = std::move(lhs.queue_guard);
    task_queue = std::move(lhs.task_queue);
}

strict_pool& strict_pool::operator=(strict_pool&& lhs) {
    thread_pool = std::move(lhs.thread_pool);

    staging_predicate = std::move(lhs.staging_predicate);
    staging_task = std::move(lhs.staging_task); 
        
    parameter_cv = std::move(lhs.parameter_cv); 

    task_predicate = std::move(lhs.task_predicate);
    task_predicate_mutex = std::move(lhs.task_predicate_mutex);

    queue_guard = std::move(lhs.queue_guard);
    task_queue = std::move(lhs.task_queue);

    return *this;
}

void strict_pool::add_task(task& task) {
    int lock_status = pthread_mutex_lock(&staging_predicate);
    if(0 != lock_status) {
        std::cout << "couldn't lock the thread : " << lock_status << "\n";
    }

    staging_task.push_back(task);
    
    int unlock_status = pthread_mutex_unlock(&staging_predicate);
    if(0 != unlock_status) {
        std::cout << "couldn't lock the thread : " << unlock_status << "\n";
    }
}

void compile() {
    int lock_status = pthread_mutex_lock(&queue_guard);
    if(0 != lock_status) {
        std::cout << "couldn't lock the thread : " << lock_status << "\n";
    }

    for(auto &task: staging_task) {
        
    }

    int unlock_status = pthread_mutex_unlock(&staging_predicate);
    if(0 != unlock_status) {
        std::cout << "couldn't lock the thread : " << unlock_status << "\n";
    }
}


}; // serene