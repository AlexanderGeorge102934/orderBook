#include <iostream>
#include <thread> 

#include "ThreadGaurd.h"
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
        ThreadGaurd threadGaurd_;
	IdGenerator idGenerator_;
        OrderBook orderBook_;

        // Add a OrderBook object 

    public:

        MatchingEngine() 
        : matchingThread { }
        , threadGaurd { matchingThread }
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
}
