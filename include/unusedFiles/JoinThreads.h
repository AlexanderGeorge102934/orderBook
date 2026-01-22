// Credit to Anthony Williams' book: C++ Concurrency in Action
// Chapter 8 Section 8.3 

#include <vector>
#include <thread>

class JoinThreads
{
    std::vector<std::thread>& threads_;

public:
    explicit JoinThreads(std::vector<std::thread>& threads) : threads_(threads)
    {
    }
    ~JoinThreads()
    {
        for (size_t i = 0; i < threads_.size(); ++i)
        {
            if (threads_[i].joinable())
                threads_[i].join();
        }
    }
};