#include "strict_pool.hpp"
#include <iostream>

#define PTHREAD_CHECK(predicate,message) if(false == predicate) {std::cout << __FUNCTION__ << message << "\n";}

namespace serene {
task::task(std::function<u64(std::vector<void*>)> function,std::vector<void*>& arguements)
    : task_function(function)
    , task_arguements(arguements)
    {};

void task::execute(strict_pool* thread) {
    for(auto arg: task_arguements) {
        thread->accquire_parameter(arg); 
    }

    task_function(task_arguements);

    for(auto arg: task_arguements) {
        thread->release_parameter(arg);
    }
}

strict_pool::strict_pool(u64 thread_count) {
    thread_pool = std::vector<pthread_t>(thread_count);

    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    for(auto& thread: thread_pool) {
        int up_status = pthread_create(&thread,&attributes,this->pool_execute,this);
        PTHREAD_CHECK(0 == up_status, "thread creation error");
    }

    pthread_mutex_init(&destruct_guard,nullptr);
    pthread_mutex_init(&staging_predicate,nullptr);
    pthread_mutex_init(&queue_guard,nullptr);
    pthread_cond_init(&queue_cv,nullptr);

    pthread_attr_destroy(&attributes);
}

strict_pool::strict_pool(strict_pool&& lhs) {
    thread_pool = std::move(lhs.thread_pool);

    staging_predicate = std::move(lhs.staging_predicate);
    staging_task = std::move(lhs.staging_task); 
        
    parameter_cv = std::move(lhs.parameter_cv); 

    queue_cv = std::move(lhs.queue_cv);
    queue_guard = std::move(lhs.queue_guard);
    task_queue = std::move(lhs.task_queue);
}

strict_pool& strict_pool::operator=(strict_pool&& lhs) {
    thread_pool = std::move(lhs.thread_pool);

    staging_predicate = std::move(lhs.staging_predicate);
    staging_task = std::move(lhs.staging_task); 
        
    parameter_cv = std::move(lhs.parameter_cv); 

    queue_cv = std::move(lhs.queue_cv);
    queue_guard = std::move(lhs.queue_guard);
    task_queue = std::move(lhs.task_queue);

    return *this;
}

strict_pool::~strict_pool() {
    s32 size = thread_pool.size();
    pthread_mutex_lock(&destruct_guard);
    destruct = true;
    pthread_mutex_unlock(&destruct_guard);

    for(s32 i = 0; i < size; i++) {
        void* ret_val = nullptr;
        pthread_join(thread_pool[i],&ret_val);
        if(ret_val) {
            std::cout << "error on pthread exit has happened..!!\n";
        }
    }
}

void strict_pool::add_task(task& task) {
    int lock_status = pthread_mutex_lock(&staging_predicate);
    PTHREAD_CHECK(0 != lock_status,"couldn't lock the thread");

    staging_task.push_back(task);
    
    int unlock_status = pthread_mutex_unlock(&staging_predicate);
    PTHREAD_CHECK(0 != unlock_status,"couldn't unlock the thread");
}

void strict_pool::compile() {
    int staging_lock = pthread_mutex_lock(&staging_predicate);
    PTHREAD_CHECK(0 != staging_lock,"couldn't lock the thread");
    
    int lock_status = pthread_mutex_lock(&queue_guard);
    PTHREAD_CHECK(0 != lock_status,"couldn't lock the thread");

    for(auto &task: staging_task) {
        task_queue.push(task);
    }
    staging_task.clear();

    pthread_cond_signal(&queue_cv);

    int unlock_status = pthread_mutex_lock(&queue_guard);
    PTHREAD_CHECK(0 != unlock_status,"couldn't unlock the thread");

    int staging_unlock = pthread_mutex_unlock(&staging_predicate);
    PTHREAD_CHECK(0 != staging_unlock,"couldn't unlock the thread");
}

void strict_pool::execute_tasks() {
    int lock_status = pthread_mutex_lock(&queue_guard);
    PTHREAD_CHECK(0 != lock_status,"couldn't lock the thread");

    int race_condition = pthread_cond_wait(&queue_cv,&queue_guard);
    // TODO : maybe check if the destructor is called?
    while(false == check_destructor() && 0 == race_condition) {
        int race_condition = pthread_cond_wait(&queue_cv,&queue_guard);
    }

    if(true == check_destructor()) {
        return;
    }

    if(false == task_queue.empty()) {
        task current_task = task_queue.front();
        task_queue.pop();

        current_task.execute(this);
    }

    if(false == task_queue.empty()) {
        pthread_cond_signal(&queue_cv);
    }

    int unlock_status = pthread_mutex_unlock(&queue_guard);
    PTHREAD_CHECK(0 != unlock_status,"couldn't unlock the thread");
}

void* strict_pool::pool_execute(void* pool_ptr) {
    strict_pool* pool = reinterpret_cast<strict_pool*>(pool_ptr);
    // TODO : handle the destructor condition here.
    while(false == pool->check_destructor()) {
        pool->execute_tasks();
    }

    pthread_exit(nullptr);
    return nullptr;
}

bool strict_pool::add_parameter(void* parameter) {
    auto expect_end = parameter_cv.find(parameter);

    if(parameter_cv.end() != expect_end) {
        std::cout << "multiple parameter addition of the same parameter \n" << "parameter : " << parameter << "\n";
        return false;
    }

    parameter_cv[parameter] = {};
    
    auto mutex_init = pthread_mutex_init(&parameter_cv[parameter],nullptr);

    return true;
}

bool strict_pool::accquire_parameter(void* parameter) {
    auto expect_no_end = parameter_cv.find(parameter);
    if(parameter_cv.end() == expect_no_end) {
        std::cout << "parameter : " << parameter << "lookup found the parameter to be not tracked..!!! this is a major error..!!\n";
        return false;
    }

    int parameter_lock = pthread_mutex_lock(&parameter_cv[parameter]);
    PTHREAD_CHECK(0 == parameter_lock, "failed to accquire parameter mutex");
    return true;
}

bool strict_pool::release_parameter(void* parameter) {
    auto expect_no_end = parameter_cv.find(parameter);
    if(parameter_cv.end() == expect_no_end) {
        std::cout << "parameter : " << parameter << "lookup found the parameter to be not tracked..!!! this is a major error..!!\n";
        return false;
    }

    int parameter_unlock = pthread_mutex_unlock(&parameter_cv[parameter]);
    PTHREAD_CHECK(0 == parameter_unlock, "failed to release parameter mutex");
    return true;
}

bool strict_pool::check_destructor() {
    pthread_mutex_lock(&destruct_guard);
    bool destruct_val = destruct;
    pthread_mutex_unlock(&destruct_guard);

    return destruct_val;
}

}; // serene