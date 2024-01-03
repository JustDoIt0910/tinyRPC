//
// Created by just do it on 2024/1/1.
//

#ifndef TINYRPC_EXECUTOR_H
#define TINYRPC_EXECUTOR_H
#include "tinyRPC/exec/task_queue.h"
#include <thread>
#include <chrono>
#include <atomic>

namespace tinyRPC {

    class ThreadPoolExecutor {
    public:
        enum class RejectPolicy { ABORT, CALLER_RUN, DISCARD, DISCARD_OLDEST };

        ThreadPoolExecutor(int queue_size, int core_pool_size, int max_pool_size,
                           std::chrono::milliseconds max_idle, RejectPolicy reject_policy);

        void Execute(TaskFunc func, TaskCallback cb) noexcept(false);

        void Shutdown();

        ~ThreadPoolExecutor();

    private:
        void WorkerRoutine();
        void Reject(std::shared_ptr<Task> task);

        TaskQueue task_queue_;
        std::mutex mu_;
        std::unordered_map<std::thread::id, std::shared_ptr<std::thread>> active_workers_;
        std::mutex stopped_workers_mu_;
        std::vector<std::shared_ptr<std::thread>> stopped_workers_;
        RejectPolicy reject_policy_;
        int core_pool_size_;
        int max_pool_size_;
        std::chrono::milliseconds max_idle_;
        std::atomic_int pool_size_;
        std::atomic_bool shutting_down_;
        std::thread cleanup_thread_;
        std::mutex cleanup_mu_;
        std::condition_variable cleanup_cond_;
    };

}

#endif //TINYRPC_EXECUTOR_H
