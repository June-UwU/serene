#include "relaxed_pool.hpp"

#if defined(_DEBUG)
#include "logger.hpp"
#endif
    
static void relaxed_pool::thread_fn(relaxed_pool* pool) {
    pool->accquire_executable_tasks();

    while(true == is_running) {
        std::unique_lock<std::mutex> lock(pool->queue_lock);
        pool->queue_condition.wait(lock, []{ return pool->queue_condition; });

        if(false == is_running) {
            break;
        }

        auto task = pool->excutable_tasks.front();
        pool->excutable_tasks.pop();

        pool->has_tasks = !pool->excutable_tasks.empty();

#if defined(_DEBUG)
        TRACE << "Thread ID : " << std::this_thread::get_id() << "Task :" <<  task << "\n";
#endif
        
        if(true == pool->has_tasks) {
            lock.unlock();
            pool->queue_condition.notify_one();
            goto invoke_task;
        }
        lock.unlock();

invoke_task:
        task->invoke();
        pool->accquire_executable_tasks();
    }
}

relaxed_pool::relaxed_pool(u32 count = std::thread::hardware_concurrency) {
    has_tasks = false;    
    for(int i = 0; i < count; i++) {
        threads.push_back(std::move(std::thread(relaxed_pool::thread_fn, *this)));
    }
}

relaxed_pool::~relaxed_pool() {
    queue_lock.lock();
    has_tasks = false;
    queue_lock.unlock();

    clean_up_threads();
}

relaxed_pool::relaxed_pool&(relaxed_pool&& rhs) {
    vec_lock = rhs.vec_lock;
    task_vec = rhs.task_vec;
    has_tasks = rhs.has_tasks;
    queue_lock = rhs.queue_lock;
    excutable_tasks = rhs.excutable_tasks;
    queue_condition = rhs.queue_condition;    
    staging_lock = rhs.staging_lock;
    staging_vec = rhs.staging_vec;
    is_running = rhs.is_running;
    threads = rhs.threads;
    
    return *this;
}

relaxed_pool& relaxed_pool::operator=(relaxed_pool&& rhs) {
    vec_lock = rhs.vec_lock;
    task_vec = rhs.task_vec;
    has_tasks = rhs.has_tasks;
    queue_lock = rhs.queue_lock;
    excutable_tasks = rhs.excutable_tasks;
    queue_condition = rhs.queue_condition;    
    staging_lock = rhs.staging_lock;
    staging_vec = rhs.staging_vec;
    is_running = rhs.is_running;
    threads = rhs.threads;
    
    return *this;
}

void relaxed_pool::initiate_new_batch() {
    std::lock_guard<std::mutex> lock(staging_lock);
    staging_vec.clear();
}

void relaxed_pool::submit_task(std::shared_ptr<task_base>& task) {
    std::lock_guard<std::mutex> lock(staging_lock);
    staging_vec.push_back(task);
}

void relaxed_pool::clean_up_threads() {
    for(auto& thread: threads) {
        thread.join();
    }
}

void relaxed_pool::make_staging_ready() {
    std::lock_guard<std::mutex> lock(staging_lock);
    std::lock_guard<std::mutex> vec_lock(vec_lock);
    
    
    for(auto task: staging_vec) {
        task_vec.push_back(task);
    }
}

void relaxed_pool::accquire_executable_tasks() {
    std::lock_guard lock(vec_lock);

    for(task_vector::iterator it = task_vec.begin(); it != task_vec.end();) {
        auto last_pos = it;
        std::shared_ptr<task_base> task = *last_pos;
        ++it;
        if(0 == task->get_resolved_dependency()) {
            queue_lock.lock();
            excutable_tasks.push_back(task);
            has_tasks = !excutable_tasks.empty();
            queue_lock.unlock();

            task_vec.erase(last_pos);
        }
    }
}

// private:
//     std::mutex vec_lock;
//     task_vector task_vec;

//     bool has_tasks;
//     std::mutex queue_lock;
//     std::queue<task_base> excutable_tasks;
//     std::conditional_variable queue_condition;

// private:
//     std::mutex staging_lock;
//     task_vector staging_vec;

// private: 
//     std::atomic<bool> is_running;
//     std::vector<std::thread> threads;