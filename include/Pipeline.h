#ifndef PIPELINE_H
#define PIPELINE_H

#include "Stage.h"

#include <map>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <iostream>
#include <boost/lockfree/spsc_queue.hpp>

using boost::lockfree::spsc_queue;

class Pipeline {
    private:
        std::atomic_bool done_;
        std::map<Stage, std::unique_ptr<spsc_queue<std::function<void()>>>> queuesMap_;
        std::vector<std::thread> threads_;

        void workerThread(const Stage& stage) 
        {
            auto& queue = *(queuesMap_.at(stage));
            while (!done_) {
                std::function<void()> task;
                if(queue.pop(task)){ //what happens if there's nothing to pop on the spsc?
                    task();
                }
                else{
                    std::this_thread::yield();
                }
            }

            // Process remaining tasks after done_ is set
            std::function<void()> task;
            while (queue.pop(task)){
                task();
            }
        }

    protected:
        template <typename T>
        void submit(const Stage &stage, T&& task) // turn task into a rval to avoid copy 
        {
            // Gotta make sure it gets pushed in  
            while(!queuesMap_[stage]->push(std::move(task))){
                std::this_thread::yield();
            }
        }

    public:
        Pipeline() 
        : done_{false} 
        , threads_{
            []{
                std::vector<std::thread> temp{};
                temp.reserve(4); 
                return temp;
            }()
        }
        {
            try
                {
                    size_t const threadCount = std::thread::hardware_concurrency();
                    if (threadCount == 0)
                    {
                        throw std::runtime_error("Program could not determine count of threads. \n");
                    }
                }
            catch (std::exception &e)
                {
                    std::cerr << e.what();
                    std::abort();
                }

            // Init queues
            queuesMap_[Stage::Sequencer] = std::make_unique<spsc_queue<std::function<void()>>>(1024);
            queuesMap_[Stage::Matching] = std::make_unique<spsc_queue<std::function<void()>>>(1024);
            queuesMap_[Stage::Logger] = std::make_unique<spsc_queue<std::function<void()>>>(1024);

            // Thread for each stage
            threads_.emplace_back(&Pipeline::workerThread, this, Stage::Sequencer);
            threads_.emplace_back(&Pipeline::workerThread, this, Stage::Matching);
            threads_.emplace_back(&Pipeline::workerThread, this, Stage::Logger);
        }

        ~Pipeline()
        {
            done_ = true;
            for (auto &thread : threads_)
            {
                if (thread.joinable())
                {
                    thread.join();
                }
            }
        }
};

#endif // PIPELINE_H


