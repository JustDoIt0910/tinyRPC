//
// Created by just do it on 2024/1/1.
//

#ifndef TINYRPC_EXECUTOR_H
#define TINYRPC_EXECUTOR_H
#include "tinyRPC/exec/task_queue.h"
#include <thread>
#include <chrono>

namespace tinyRPC {

    class ThreadPoolExecutor {
    public:
        enum class RejectPolicy { ABORT, CALLER_RUN, DISCARD, DISCARD_OLDEST };

        ThreadPoolExecutor(int queue_size, int core_pool_size, int max_pool_size,
                           std::chrono::milliseconds max_idle, RejectPolicy reject_policy);
        void Execute(TaskFunc func, TaskCallback cb) noexcept(false);

    private:
        TaskQueue task_queue_;
    };

}

#endif //TINYRPC_EXECUTOR_H
