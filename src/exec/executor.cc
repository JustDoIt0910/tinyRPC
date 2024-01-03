//
// Created by just do it on 2024/1/1.
//
#include "tinyRPC/exec/executor.h"

using namespace std::chrono_literals;

namespace tinyRPC {

    ThreadPoolExecutor::ThreadPoolExecutor(int queue_size, int core_pool_size, int max_pool_size,
                                           std::chrono::milliseconds max_idle,
                                           ThreadPoolExecutor::RejectPolicy reject_policy):
            task_queue_(queue_size), core_pool_size_(core_pool_size), max_pool_size_(max_pool_size),
            max_idle_(max_idle), reject_policy_(reject_policy), pool_size_(core_pool_size), shutting_down_(false) {
        for(int i = 0; i < core_pool_size_; i++) {
            auto worker = std::make_shared<std::thread>(&ThreadPoolExecutor::WorkerRoutine, this);
            active_workers_[worker->get_id()] = worker;
        }
        cleanup_thread_ = std::thread([this](){
            while(!shutting_down_.load()) {
                std::unique_lock<std::mutex> lk(stopped_workers_mu_);
                cleanup_cond_.wait_for(lk, 30s);
                for(auto& t: stopped_workers_) {
                    t->join();
                }
                stopped_workers_.clear();
                lk.unlock();
            }
        });
    }

    void ThreadPoolExecutor::WorkerRoutine() {
        while(!shutting_down_.load() || !task_queue_.Empty()) {
            std::shared_ptr<Task> task;
            if(pool_size_.load() > core_pool_size_) {
                task = task_queue_.GetWithTimeout(max_idle_);
            }
            else { task = task_queue_.Get(); }
            if(!task) {
                std::unique_lock<std::mutex> lk(mu_);
                if(!shutting_down_.load() && pool_size_.load() > core_pool_size_) {
                    std::thread::id tid = std::this_thread::get_id();
                    auto worker = active_workers_[tid];
                    active_workers_.erase(tid);
                    pool_size_--;
                    lk.unlock();
                    std::unique_lock<std::mutex> stopped_workers_lk(stopped_workers_mu_);
                    stopped_workers_.push_back(worker);
                    return;
                }
            }
            if(task) {
                (*task)();
            }
        }
    }

    void ThreadPoolExecutor::Execute(TaskFunc func, TaskCallback cb) noexcept(false) {
        auto task = std::make_shared<Task>(std::move(func), std::move(cb));
        if(shutting_down_.load()) {
            Reject(task);
            return;
        }
        if(!task_queue_.Put(task)) {
            std::unique_lock<std::mutex> lk(mu_);
            if(active_workers_.size() < max_pool_size_) {
                pool_size_++;
                auto new_worker = std::make_shared<std::thread>([this, task](){
                    (*task)();
                    WorkerRoutine();
                });
                active_workers_[new_worker->get_id()] = new_worker;
                return;
            }
            else {
                lk.unlock();
                Reject(task);
            }
        }
    }

    void ThreadPoolExecutor::Shutdown() {
        mu_.lock();
        shutting_down_.store(true);
        mu_.unlock();
        task_queue_.Shutdown();
        cleanup_cond_.notify_one();
        cleanup_thread_.join();
        for(auto& it: active_workers_) {
            it.second->join();
        }
        for(auto& t: stopped_workers_) {
            t->join();
        }
    }

    void ThreadPoolExecutor::Reject(std::shared_ptr<Task> task) {
        if(reject_policy_ == RejectPolicy::ABORT) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_THREAD_POOL_REJECTED);
            throw rpc_error(ec);
        }
        else if(reject_policy_ == RejectPolicy::CALLER_RUN) { (*task)(); }
        else if(reject_policy_ == RejectPolicy::DISCARD) { return; }
        else { task_queue_.Put(task, true); }
    }

    ThreadPoolExecutor::~ThreadPoolExecutor() {
        if(shutting_down_.load()) { return; }
        Shutdown();
    }
}