#ifndef TRADINGSYSTEM_H
#define TRADINGSYSTEM_H

#include "Pipeline.h"
#include "OrderBook.h"

#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TradingSystem : private Pipeline
{
    private:
        OrderBook orderBook_; 
        OrderId nextOrderId_;

        // Functions pass info moving from top to bottom 
        void handleSequencing(std::string_view message);
        void handleMatching(Order order);

    public:
        TradingSystem()
        : orderBook_{}
        , nextOrderId_{1}
        {}
        
        ~TradingSystem(){
            // TODO add iocontext as member and destroy it ioContext_.stop()
        };

        void startServer();
        
        // No copying
        TradingSystem &operator=(const TradingSystem &other) = delete;
        TradingSystem(const TradingSystem &other) = delete;

        // No moving
        TradingSystem &operator=(const TradingSystem &&other) = delete;
        TradingSystem(const TradingSystem &&other) = delete;

};

#endif