#pragma once
#include "types.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace serene {

template <typename T>
using task_fn = void(*)(std::shared_ptr<T>);

template <typename T>
using bind_type = decltype(std::bind(std::declval<task_fn<T>>(), std::declval<std::shared_ptr<T>>())); 

class task_base {
    public:
        virtual~task_base() = default;
        virtual void invoke() = 0;
        virtual void add_dependency() = 0;
        virtual void resolve_dependency() = 0;
        virtual u64 get_resolved_dependency() = 0;
        virtual void add_dependant(std::shared_ptr<task_base>&) = 0;
};

using tasks_vector = std::vector<std::shared_ptr<task_base>>; 

};