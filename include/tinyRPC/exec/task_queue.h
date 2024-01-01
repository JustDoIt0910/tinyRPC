//
// Created by just do it on 2024/1/1.
//

#ifndef TINYRPC_TASK_QUEUE_H
#define TINYRPC_TASK_QUEUE_H
#include <queue>
#include <functional>
#include <condition_variable>
#include "tinyRPC/rpc/error.h"

namespace tinyRPC {
    class RpcResponse;

    using TaskFunc = std::function<void()>;
    using TaskCallback = std::function<void(rpc_error::error_code, std::shared_ptr<RpcResponse>)>;

    struct Task {
        TaskFunc task_;
        TaskCallback callback_;

        Task(TaskFunc t, TaskCallback cb):
        task_(std::move(t)), callback_(std::move(cb)) {};
    };

    class TaskQueue {
    public:
        TaskQueue(int max_size);
        bool Put(std::shared_ptr<Task> task, bool discard_oldest = false);
        std::shared_ptr<Task> Get();

    private:
        size_t Swap();

        std::queue<std::shared_ptr<Task>> get_queue_;
        std::queue<std::shared_ptr<Task>> put_queue_;
        std::mutex get_mutex_;
        std::mutex put_mutex_;
        std::condition_variable get_cond_;
        int cap_;
    };

}

#endif //TINYRPC_TASK_QUEUE_H
