#include "relaxed_graph.hpp"

#if defined(DEBUG)
#include "logger.hpp"
#endif

namespace serene {

void relaxed_graph::thread_fn() {
    while(true == is_running) {
        std::unique_lock<std::mutex> lock(queue_lock);
        queue_condition.wait(lock, [this]{ return has_tasks; });

        if(false == is_running) {
#if defined(DEBUG)
            TRACE << "Thread ID : " << std::this_thread::get_id() << " is joinable \n";
#endif            
            break;
        }

        auto task = excutable_tasks.front();
        excutable_tasks.pop();

        has_tasks = !excutable_tasks.empty();

#if defined(DEBUG)
        TRACE << "Thread ID : " << std::this_thread::get_id() << "Task :" <<  task << "\n";
#endif
        
        if(true == has_tasks) {
            lock.unlock();
            queue_condition.notify_one();
            goto invoke_task;
        }
        lock.unlock();

invoke_task:
        task->invoke();
        accquire_executable_tasks();
    }
}

relaxed_graph::relaxed_graph(u32 count) {
    has_tasks = false;   
    is_running = true; 

    for(int i = 0; i < count; i++) {
        threads.push_back(std::move(std::thread([this]{ this->thread_fn(); })));
    }
}

relaxed_graph::~relaxed_graph() {
    queue_lock.lock();
    is_running = false;
    has_tasks = true;
    queue_lock.unlock();
    queue_condition.notify_all();

    clean_up_threads();
}

void relaxed_graph::initiate_new_batch() {
    std::lock_guard<std::mutex> lock(staging_lock);
    staging_vec.clear();
}

void relaxed_graph::submit_task(std::shared_ptr<task_base>& task) {
    std::lock_guard<std::mutex> lock(staging_lock);
    staging_vec.push_back(task);
}

void relaxed_graph::clean_up_threads() {
    for(auto& thread: threads) {
        thread.join();
    }
#if defined(DEBUG) 
TRACE << "All threads have merged successfully!!!\n" << ANSI_COLOR_RESET;
#endif
}

void relaxed_graph::make_staging_ready() {
    std::lock_guard<std::mutex> lock(staging_lock);
    std::lock_guard<std::mutex> exec(vec_lock);
    
    
    for(auto task: staging_vec) {
        task_vec.push_back(task);
    }
    accquire_executable_tasks();
}

void relaxed_graph::accquire_executable_tasks() {
    std::lock_guard lock(vec_lock);

    for(tasks_vector::iterator it = task_vec.begin(); it != task_vec.end();) {
        auto last_pos = it;
        std::shared_ptr<task_base> task = *last_pos;
        ++it;
        if(0 == task->get_resolved_dependency()) {
            queue_lock.lock();
            excutable_tasks.push(task);
            has_tasks = !excutable_tasks.empty();
            queue_lock.unlock();

            task_vec.erase(last_pos);
        }
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