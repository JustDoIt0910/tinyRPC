//
// Created by just do it on 2024/1/1.
//
#include "tinyRPC/exec/task_queue.h"

namespace tinyRPC {

    TaskQueue::TaskQueue(int max_size): cap_(max_size) {}

    bool TaskQueue::Put(std::shared_ptr<Task> task, bool discard_oldest) {
        std::unique_lock<std::mutex> lk(put_mutex_);
        if(put_queue_.size() == cap_) {
            if(discard_oldest) {
                std::shared_ptr<Task> victim = put_queue_.front();
                rpc_error::error_code ec(rpc_error::error_code::RPC_THREAD_POOL_REJECTED);
                victim->callback_(std::move(ec), nullptr);
            }
            else { return false; }
        }
        put_queue_.push(task);
        lk.unlock();
        get_cond_.notify_one();
        return true;
    }

    size_t TaskQueue::Swap() {
        std::unique_lock<std::mutex> lk(put_mutex_);
        get_cond_.wait(lk, [this] () { return !put_queue_.empty(); });
        size_t sz = put_queue_.size();
        get_queue_.swap(put_queue_);
        return sz;
    }

    std::shared_ptr<Task> TaskQueue::Get() {
        std::unique_lock<std::mutex> lk(get_mutex_);
        if(!get_queue_.empty() || Swap() > 0) {
            std::shared_ptr<Task> task = get_queue_.front();
            get_queue_.pop();
            return task;
        }
        return std::shared_ptr<Task>();
    }

}