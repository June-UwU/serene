#pragma once
#include "types.hpp"
#include "task_base.hpp"
#include <mutex>
#include <queue>
#include <atomic>
#include <vector>
#include <thread>
#include <conditional_variable>

namespace serene  {

class relaxed_pool {
    static void thread_fn(relaxed_pool* pool);
    public:
        relaxed_pool(u32 count = std::thread::hardware_concurrency);
        ~relaxed_pool();

        relaxed_pool(const relaxed_pool&) = delete;
        relaxed_pool&(relaxed_pool&&);

        relaxed_pool operator=(const relaxed_pool&) = delete;
        relaxed_pool& operator=(relaxed_pool&&);

        void initiate_new_batch();
        void submit_task(std::shared_ptr<task_base>& task);
        void mark_batch_ready();

    private:
        void clean_up_threads();
        void make_staging_ready();
        void accquire_executable_tasks();
    
    private:
        std::mutex vec_lock;
        task_vector task_vec;

        bool has_tasks;
        std::mutex queue_lock;
        std::queue<task_base> excutable_tasks;
        std::conditional_variable queue_condition;

    private:
        std::mutex staging_lock;
        task_vector staging_vec;
    
    private: 
        std::atomic<bool> is_running;
        std::vector<std::thread> threads;
};

};