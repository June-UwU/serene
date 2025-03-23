#pragma once
#include "types.hpp"
#include "task_base.hpp"
#include <mutex>
#include <queue>
#include <atomic>
#include <vector>
#include <thread>
#include <condition_variable>

namespace serene  {

class relaxed_graph {
    void thread_fn();
    public:
        relaxed_graph(u32 count = std::thread::hardware_concurrency());
        ~relaxed_graph();

        relaxed_graph(const relaxed_graph&) = delete;
        relaxed_graph(relaxed_graph&&) = delete;

        relaxed_graph operator=(const relaxed_graph&) = delete;
        relaxed_graph& operator=(relaxed_graph&&) = delete;

        void initiate_new_batch();
        void submit_task(std::shared_ptr<task_base>& task);
        void mark_batch_ready();

    private:
        void clean_up_threads();
        void make_staging_ready();
        void accquire_executable_tasks();
    
    private:
        std::mutex vec_lock;
        tasks_vector task_vec;

        bool has_tasks;
        std::mutex queue_lock;
        std::condition_variable queue_condition;
        std::queue<std::shared_ptr<task_base>> excutable_tasks;

    private:
        std::mutex staging_lock;
        tasks_vector staging_vec;
    
    private: 
        std::atomic<bool> is_running;
        std::vector<std::thread> threads;
};

};