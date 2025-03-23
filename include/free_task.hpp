#pragma once
#include "task_base.hpp"
#include <memory>
#include <vector>
#include <atomic>

#if defined(_DEBUG)
#include "logger.hpp"
#define FREE_TASK_DEBUG TRACE << __FUNCTION__ << "Task ID : " << std::hex << this << "\n"
#elif
define FREE_TASK_DEBUG
#endif

#if defined(_DEBUG) \
#endif 

template <typename T>
class free_task : public task_base {
    public:
        free_task(std::function<task_fn>& fn, std::shared_ptr<T>& data) 
            : bound_fn(fn, data)
            , need_resolving {0} 
            {}
        
        free_task(const free_task& rhs) = delete;

        free_task&(free_task&& rhs) {
            bound_fn    = rhs.bound_fn;
            task_vec = rhs.task_vec;
            need_resolving = rhs.need_resolving;
            return *this;
        }

        free_task operator=(const free_task& rhs) = delete;

        free_task& operator=(free_task&& rhs) {
            bound_fn    = rhs.bound_fn;
            task_vec = rhs.task_vec;
            need_resolving = rhs.need_resolving;
            return *this;
        }

        void invoke() override {
            FREE_TASK_DEBUG;
            bound_fn();
            resolve_dependant_tasks();
        }

        void add_dependency() override {
            FREE_TASK_DEBUG;
            need_resolving++;
        }

        void resolve_dependency() override {
            FREE_TASK_DEBUG;
            need_resolving--;
        }

        u64 get_resolved_dependency() override {
            FREE_TASK_DEBUG;
            return need_resolving.load();
        }

        void add_dependant(std::shared_ptr<task_base>& dependant) override {
            FREE_TASK_DEBUG;
            task_vec.push_back(dependant);
            dependant->add_dependency();
        }

    private:
        void resolve_dependant_tasks() {
            FREE_TASK_DEBUG
            for(auto& task: task_vec) {
                task->resolve_dependency();
            }
        }

    private:
        bind_type<T> bound_fn;
        task_vector task_vec;
        std::atomic<u64> need_resolving;
};
