
#ifndef THREAD_GUARD_H
#define THREAD_GUARD_H

#include <thread>

class ThreadGuard{
    private:
        std::thread& t_;

    public:
        explicit ThreadGuard(std::thread& t)
        : t_ { t } 
        {

        }

        ~ThreadGuard(){
            if(t_.joinable()){
                t_.join();
            }
        }

        ThreadGuard(const ThreadGuard& threadGaurd) = delete;
        ThreadGuard operator=(const ThreadGuard& threadGuard) = delete;

};

#endif