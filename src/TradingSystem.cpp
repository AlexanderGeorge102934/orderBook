#include "TradingSystem.h"

void TradingSystem::startServer(){

    try{
        uint16_t port {1030};
        boost::asio::io_context ioContext;
        tcp::acceptor acceptor{ioContext, tcp::endpoint(tcp::v4(), port)};
        for (;;)
            {
                // Create a socket 
                tcp::socket socket(acceptor.get_executor());

                // Wait until someone connects 
                acceptor.accept(socket);

                std::array<char, 128> buf;
                boost::system::error_code error;

                // Read some data into buffer
                size_t len = socket.read_some(boost::asio::buffer(buf), error);

                if (error == boost::asio::error::eof)
                {
                    // Connection closed cleanly by peer
                    std::cout << "Connection closed" << std::endl;
                }
                else if (error)
                {
                    throw boost::system::system_error(error); // Some other error
                }

                std::string message(buf.data(), len); // std move this into ondata recieved argument 

                Pipeline::submit(Stage::Sequencer, [this, movedMessage{std::move(message)}]()
                                 { handleSequencing(std::string_view(movedMessage)); });

            }
    }
    catch (std::exception &e){
        std::cerr << e.what();
        std::abort();
    }
    // take in a tcp connection 
    // pass connection to a sequencer to handle creating an order through queue
}

void TradingSystem::handleSequencing(std::string_view message) {

    // Chatted this since I don't know how to break up a string 
    // Not the most efficient use, since I'm copying and probably not using sso so I'm defeating he whole point of string view but oh well
    try {
        // 1. Setup stream for parsing
        std::stringstream ss{ std::string(message) };
        std::string sideStr, typeStr;
        Price price;
        Quantity qty;

        // Format: BUY LIMIT 100 50 
        // "I want to create a buy limit order for 100 dollars with a qty of 50" 
        if (!(ss >> sideStr >> typeStr >> price >> qty)) {
            return; 
        }

        Side side {(sideStr == "BUY") ? Side::Buy : Side::Sell};
        OrderType type {(typeStr == "LIMIT") ? OrderType::Limit : OrderType::Market};

        OrderId id {nextOrderId_++};
        Quantity initQty {qty};
        Quantity remQty {qty};

        // 4. Construct the Order object 
        Order newOrder{ side, price, id, type, initQty, remQty };

        // 5. Pass to matching stage
        Pipeline::submit(Stage::Matching, [this, copiedOrder {newOrder}]() mutable { // Realized Order is just a bunch of PODs so moving would have no benefit 
            handleMatching(copiedOrder);
        });

    } catch (...) {
        // TODO see if you can try again         
    }
}

// [NOTE]: Later down the road I could remove this function and just simply call process order directly but for better 
//         readability this will do for now  
void TradingSystem::handleMatching(Order order){ 
    orderBook_.processOrder(order);
    // submit orderbooks trade to handle logging through queue 
}
