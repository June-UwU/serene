#pragma once
#include <types.hpp>

namespace serene {

typedef struct sync_primitives {
    pthread_cond_t cv;
    pthread_mutex_t mutex;
} sync_primitives;

}; // serene