#ifndef THREAD_QUEUE_H
#define THREAD_QUEUE_H

#include <queue>
#include <memory>
#include <mutex>

template<typename T>
class ThreadQueue{

    private:
        mutable std::mutex mtx_;
        std::queue<std::unique_ptr<T>> queue_;

    public:
        ThreadQueue() = default;
        ThreadQueue(ThreadQueue&& threadQueue) noexcept = delete;
        ThreadQueue& operator=(ThreadQueue&& threadQueue) noexcept = delete;

        void push(std::unique_ptr<T> data) {
           std::lock_guard<std::mutex> lock(mtx_);
           queue_.push(std::move(data));
        } 
        std::unique_ptr<T> pop(){
           std::lock_guard<std::mutex> lock(mtx_);
           if(queue_.empty()) return nullptr;

           auto item = std::move(queue_.front());
           queue_.pop();
           return item;
        }

        bool empty() const{
           std::lock_guard<std::mutex> lock(mtx_);
           return queue_.empty();
        }

        // This is unique so copying shouldn't be allowed 
        ThreadQueue(const ThreadQueue& threadQueue) = delete;
        ThreadQueue& operator=(const ThreadQueue& threadQueue) = delete;
};



#endif