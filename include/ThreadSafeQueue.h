// Credit to Anthony Williams' book: C++ Concurrency in Action
// Chapter 6 Section 6.2.2 A thread-safe queue using locks and condition variables  

#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class ThreadSafeQueue{

    private:

        // Note to self: mutex and condition variables can't be copied OR moved so this class can't be copied OR moved
        mutable std::mutex mut_; 
        std::queue<T> dataQueue_;
        std::condition_variable dataCondition_;

    public:

        // Rule of Zero 
        ThreadSafeQueue() = default;
        ~ThreadSafeQueue() = default;

        // No copying 
        ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;

        // No moving 
        ThreadSafeQueue(ThreadSafeQueue&& other) = delete;
        ThreadSafeQueue& operator=(ThreadSafeQueue&& other) = delete;

        void push(T newValue){
            std::lock_guard<std::mutex> lockGaurd(mut_);
            dataQueue.push(std::move(newValue));
            dataCondition_.notify_one(); // Note to self: Need only one waiting thread needs to be woken up not all 
        }

        void waitAndPop(T& value){ // Never knew you could just pass by ref rather than the overhead of returning a value lol
            std::unique_lock<std::mutex> uniqueLock(mut_);
            dataCondition_.wait(uniqueLock, 
                [this]
                    {
                        !dataQueue_.empty();
                    }
            );

            value = std::move(dataQueue_.front());
            dataQueue_.pop();
        }

        std::shared_ptr<T> waitAndPop(){ // If you want to return a value instead same as above really
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, 
                [this]
                    { 
                        return !data_queue.empty(); 
                    }
                );
            std::shared_ptr<T> result(
                std::make_shared<T>(std::move( data_queue.front() ))
            );

            data_queue.pop();
            return result;
        }

        bool tryPop(T& value){
            std::lock_guard<std::mutex> lockGaurd(mut_);
            if(dataQueue_.empty()){
                return false;
            }
            value = std::move(dataQueue_.front());
            dataQueue_.pop();
        }

        std::shared_ptr<T> tryPop()
        {
            std::lock_guard<std::mutex> lk(mut_);
            if (data_queue.empty()){
                return std::shared_ptr<T>();
            }
            std::shared_ptr<T> result(
                std::make_shared<T>(std::move( data_queue.front() ))
            );
            data_queue.pop();
            return result;
        }

        bool empty() const {
            std::lock_guard<std::mutex> lockGaurd(mut_);
            return dataQueue_.empty();
        }
};
#endif