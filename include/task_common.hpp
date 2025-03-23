#pragma once
#include "types.hpp"
#include "task_base.hpp"
#include "free_task.hpp"

#define MAKE_TASK_FN(class_name) \
template <typename T> \
std::shared_ptr<task_base> make_##class_name(task_fn<T>& fn, std::shared_ptr<T>& data) { \
    task_base* ptr = new class_name(fn, data); \
    return std::make_shared<task_base>(ptr);  \
} \

#define MAKE_TASK_BIND_FN(class_name) \
template<typename T> \
std::shared_ptr<task_base> make_##class_name(bind_type<T>& bound_fn) { \
    task_base* ptr = new class_name(bind_type& bind); \
    return std::make_shared<task_base>(ptr); \
} \

#define MAKE_TASK_FUNCTIONS(class_name) \
    MAKE_TASK_BIND_FN(class_name, template_type) \
    MAKE_TASK_FN(class_name, template_type) \ 

MAKE_TASK_FUNCTIONS(free_task)
