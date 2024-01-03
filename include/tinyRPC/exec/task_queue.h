//
// Created by just do it on 2024/1/1.
//

#ifndef TINYRPC_TASK_QUEUE_H
#define TINYRPC_TASK_QUEUE_H
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "tinyRPC/rpc/error.h"

namespace tinyRPC {
    class RpcResponse;

    using TaskFunc = std::function<std::shared_ptr<RpcResponse>()>;
    using TaskCallback = std::function<void(std::shared_ptr<RpcResponse>)>;

    struct Task {
        TaskFunc task_;
        TaskCallback callback_;

        Task(TaskFunc t, TaskCallback cb):
        task_(std::move(t)), callback_(std::move(cb)) {};

        void operator()();
    };

    class TaskQueue {
    public:
        TaskQueue(int max_size = 0);

        bool Put(std::shared_ptr<Task> task, bool discard_oldest = false);

        std::shared_ptr<Task> Get();

        std::shared_ptr<Task> GetWithTimeout(std::chrono::milliseconds timeout);

        void Shutdown();

        bool Empty();

    private:
        size_t Swap(std::chrono::milliseconds timeout);

        std::queue<std::shared_ptr<Task>> get_queue_;
        std::queue<std::shared_ptr<Task>> put_queue_;
        std::timed_mutex get_mutex_;
        std::mutex put_mutex_;
        std::condition_variable get_cond_;
        std::atomic_bool shutting_down_;
        std::atomic_int32_t size_;
        int cap_;
    };

}

#endif //TINYRPC_TASK_QUEUE_H
