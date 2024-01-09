//
// Created by just do it on 2024/1/1.
//
#include <cassert>
#include "tinyRPC/rpc/message.h"
#include "tinyRPC/exec/task_queue.h"

namespace tinyRPC {

    void Task::operator()() const {
        std::shared_ptr<RpcResponse> res = task_();
        callback_(res);
    }

    TaskQueue::TaskQueue(int max_size): cap_(max_size), size_(0), shutting_down_(false) {}

    bool TaskQueue::Put(std::shared_ptr<Task> task, bool discard_oldest) {
        if(shutting_down_.load()) { return true; }
        std::unique_lock<std::mutex> lk(put_mutex_);
        if(cap_ > 0 && put_queue_.size() == cap_) {
            if(discard_oldest) {
                std::shared_ptr<Task> victim = put_queue_.front();
                auto response = std::make_shared<RpcResponse>();
                response->ec_ = rpc_error::error_code::RPC_THREAD_POOL_REJECTED;
                victim->callback_(response);
            }
            else { return false; }
        }
        put_queue_.push(task);
        lk.unlock();
        get_cond_.notify_one();
        size_++;
        return true;
    }

    size_t TaskQueue::Swap(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lk(put_mutex_);
        if(timeout == std::chrono::milliseconds::zero()) {
            get_cond_.wait(lk, [this] () { return !put_queue_.empty() || shutting_down_.load(); });
        }
        else {
            get_cond_.wait_for(lk, timeout, [this] () { return !put_queue_.empty() || shutting_down_.load(); });
        }
        size_t sz = put_queue_.size();
        get_queue_.swap(put_queue_);
        return sz;
    }

    std::shared_ptr<Task> TaskQueue::GetWithTimeout(std::chrono::milliseconds timeout) {
        std::unique_lock<std::timed_mutex> lk(get_mutex_, std::defer_lock);
        if(timeout == std::chrono::milliseconds::zero()) {
            lk.lock();
        }
        else if(!lk.try_lock_for(timeout)) {
            return {};
        }
        if(!get_queue_.empty() || Swap(timeout) > 0) {
            std::shared_ptr<Task> task = get_queue_.front();
            get_queue_.pop();
            size_--;
            return task;
        }
        if(!shutting_down_.load()) {
            assert(timeout != std::chrono::milliseconds::zero());
        }
        return {};
    }

    std::shared_ptr<Task> TaskQueue::Get() { return GetWithTimeout(std::chrono::milliseconds::zero()); }

    void TaskQueue::Shutdown() {
        shutting_down_.store(true);
        get_cond_.notify_one();
    }

    bool TaskQueue::Empty() { return size_.load() == 0; }

}