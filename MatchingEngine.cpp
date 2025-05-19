#include <iostream>
#include <thread> 

#include "ThreadGuard.h"
#include "OrderBook.h"



class IdGenerator{
	private:
	    OrderId nextOrderId{1};
	    TradeId nextTradeId{1};

	public:
	    OrderId generateOrderId(){
		return nextOrderId++;
	    
	    }

	    TradeId generateTradeId(){
		return nextTradeId++;
	    }
};

class MatchingEngine{

    private:
        std::thread matchingThread_;
        ThreadGuard threadGuard_;
	    IdGenerator idGenerator_;
        OrderBook orderBook_;

        // Add a OrderBook object 

    public:

        MatchingEngine() 
        : matchingThread_ { }
        , threadGuard_ { matchingThread_ }
	    , idGenerator_ {}
        , orderBook_ {}
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
};
