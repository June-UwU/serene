#pragma once
#include "types.hpp"
#include <vector>

namespace serene {

template <typename T>
using task_fn = void()(std::shared_ptr<T>);

template <typename T>
using bind_type = std::bind<task_fn<T>,std::shared_ptr<T>>; 

class task_base {
    public:
        ~task_base() = 0;
        virtual void invoke() = 0;
        virtual void add_dependency() = 0;
        virtual void resolve_dependency() = 0;
        virtual u64 get_resolved_dependency() = 0;
        virtual void add_dependant(std::shared_ptr<task_base>&) = 0
};

using tasks_vector = std::vector<std::shared_ptr<task_base>>; 

};