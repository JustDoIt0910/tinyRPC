//
// Created by just do it on 2023/12/8.
//

#ifndef TINYRPC_COUNTDOWN_LATCH_H
#define TINYRPC_COUNTDOWN_LATCH_H
#include <condition_variable>

namespace tinyRPC {

    class CountdownLatch {
    public:
        explicit CountdownLatch(int count): count_(count) {}

        void Wait() {
            std::unique_lock<std::mutex> lk(mu_);
            cv_.wait(lk, [this] () -> bool { return count_ == 0; });
        }

        void CountDown() {
            std::unique_lock<std::mutex> lk(mu_);
            if(--count_ == 0) { cv_.notify_all(); }
        }

    private:
        int count_;
        std::mutex mu_;
        std::condition_variable cv_;
    };

}

#endif //TINYRPC_COUNTDOWN_LATCH_H
