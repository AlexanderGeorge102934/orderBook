#include <iostream>
#include <thread> 
#include "ThreadGaurd.h"
#include "OrderBook.h"


class MatchingEngine{

    private:
        std::thread matchingThread_;
        ThreadGaurd threadGaurd_;

        OrderBook orderBook_;

        // Add a OrderBook object 

    public:

        MatchingEngine() 
        : matchingThread { }
        , threadGaurd { matchingThread }
        , orderBook_ { }
        {

        }
        
        MatchingEngine(const MatchingEngine& matchingEngine) = delete;
        MatchingEngine operator=(const MatchingEngine& matchingEngine) = delete;

        MatchingEngine(MatchingEngine&& matchingEngine) noexcept {


        }

        MatchingEngine operator=(MatchingEngine&& matchingEngine) noexcept {

        }

        ~MatchingEngine(){

        }
}