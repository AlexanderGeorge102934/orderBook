
#ifndef THREAD_GAURD_H
#define THREAD_GAURD_H

#include <thread>

class ThreadGaurd{
    private:
        std::thread& t_;

    public:
        explicit ThreadGaurd(std::thread& t)
        : t_ { t } 
        {

        }

        ~ThreadGaurd(){
            if(t_.joinable()){
                t_.join();
            }
        }

        ThreadGaurd(const ThreadGaurd& threadGaurd) = delete;
        ThreadGaurd operator=(const ThreadGuard& threadGaurd) = delete;

}

#endif