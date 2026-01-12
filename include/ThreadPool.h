#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include "ThreadSafeQueue.h"
#include <functional>
#include "JoinThreads.h"
class ThreadPool{

    private:
        std::atomic_bool done_;
        ThreadSafeQueue<std::function<void()>>& workQueue_; // This might need to change to something different in terms of void and no param
        std::vector<std::thread> threads_;
        JoinThreads joinThreads_;

        void workerThread(){
            while(!done_){
                std::function<void()> task;
                if(workQueue_.tryPop(task)){
                    task();
                }
                else{
                    std::this_thread::yield();
                }
            }
        }

    public:
        ThreadPool(ThreadSafeQueue<std::function<void()>>& workQueue, size_t threadCount)
        : done_{false}
        , workQueue_{workQueue}
        , threads_{
            [&threadCount]{
            std::vector<std::thread> temp{};
            temp.reserve(threadCount);
            return temp;}()
        }
        , joinThreads_{threads_}
        {
            try
            {
                for(size_t i {0}; i<threadCount; ++i){
                    threads_.emplace_back(&ThreadPool::workerThread, this);
                }
            }
            catch(...)
            {
                done_ = true;
                throw;
            } 
        }

        ~ThreadPool(){
            done_ = true;
        }

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool& operator=(const ThreadPool& other) = delete;

        //ThreadSafeQueue is not copyable or movable 
        ThreadPool(ThreadPool&& other) = delete;
        ThreadPool& operator=(ThreadPool&& other) = delete;

        template<typename FunctionType> 
        void submit(FunctionType f){
            workQueue_.push(std::function<void()>(f));
        }






};

#endif
