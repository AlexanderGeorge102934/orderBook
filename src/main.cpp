#include <fstream> 
#include <iostream> 
#include <string> 
#include <sstream>
#include <cctype>
#include <atomic> 
#include <thread>

#include "ThreadQueue.h"
#include "ThreadGuard.h"
#include "OrderBook.cpp"


// I know global variables are bad practice and i should opt for condition variables but I'm too lazy to do that 
std::atomic<bool> done{false};

struct Queues{
    ThreadQueue<std::string> createOrderQueue;
    ThreadQueue<Order> orderBookQueue;
};

bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool isValidSide(const std::string& side) {
    return side == "BUY" || side == "SELL";
}

bool isValidOrderType(const std::string& orderType) {
    return orderType == "LIMIT" || orderType == "MARKET";
}


// Got lazy so i just chatted the validation for the file line 
bool isValidLine(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() != 6) {
        return false; // malformed
    }

    // Validate Side
    if (!isValidSide(tokens[0])) return false;

    // Validate Price (uint32)
    if (!isNumber(tokens[1])) return false;
    Price price = std::stoull(tokens[1]);
    if (price > UINT32_MAX) return false;

    // Validate OrderId (uint64)
    if (!isNumber(tokens[2])) return false;

    // Validate OrderType
    if (!isValidOrderType(tokens[3])) return false;

    // Validate InitialQuantity and RemainingQuantity (uint32, positive)
    if (!isNumber(tokens[4]) || !isNumber(tokens[5])) return false;
    Quantity initialQty = static_cast<Quantity>(std::stoull(tokens[4]));
    Quantity remainingQty = static_cast<Quantity>(std::stoull(tokens[5]));
    if (initialQty < 1 || remainingQty < 1) return false;

    // On init, initial and remaining must be equal
    if (initialQty != remainingQty) return false;

    return true;
}

void openFile(Queues& queues){

	const std::string inputFile {"data.txt"};
	const std::string outputFile {"output.txt"};

	std::ifstream inFile(inputFile);
	if(!inFile.is_open()){
		std::cerr << "Failed to open input file: " << inputFile << "\n";
		return;
	}

	std::ofstream outFile(outputFile);
	if(!outFile.is_open()){
		std::cerr << "Failed to create output file: " << outputFile << "\n";
		return; 
	}

	std::string line;
	int32_t lineNumber {0};
	int32_t removedLines {0};

	while(std::getline(inFile, line)){
		++lineNumber;

		if(isValidLine(line)){
			outFile << line << "\n";
		}
		else{
			++removedLines;
		}
	}

    auto dataFileName = std::make_unique<std::string>(outputFile);

	std::cout << "# of removed lines: " << removedLines << "\n";
	inFile.close();
	outFile.close();
}

// Chatted this but i understand it 
void createOrderArgs(Queues& queues ) {
    const std::string fileText{"output.txt"};
    std::ifstream inFile(fileText);

    const int maxRetries = 100;  // 10 seconds max
    int attempts = 0;
    while (attempts++ < maxRetries) {
        inFile.open(fileText);
        if (inFile.is_open()) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!inFile.is_open()) {
        std::cerr << "Failed to open file: " << fileText << "\n";
        return;
    }

    std::string line;

    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string sideStr, priceStr, orderIdStr, orderTypeStr, initialQtyStr, remainingQtyStr;

        iss >> sideStr >> priceStr >> orderIdStr >> orderTypeStr >> initialQtyStr >> remainingQtyStr;

        // Convert to proper types
        Side side = (sideStr == "BUY") ? Side::Buy : Side::Sell;
        uint32_t price = static_cast<uint32_t>(std::stoul(priceStr));
        uint64_t orderId = std::stoull(orderIdStr);
        OrderType orderType = (orderTypeStr == "LIMIT") ? OrderType::Limit: OrderType::Market;
        uint32_t initialQty = static_cast<Quantity>(std::stoul(initialQtyStr));
        uint32_t remainingQty = static_cast<Quantity>(std::stoul(remainingQtyStr));

        // Create unique_ptr<Order>
        auto orderPtr = std::make_unique<Order>(side, price, orderId, orderType, initialQty, remainingQty);

        queues.orderBookQueue.push(std::move(orderPtr));
    }


    done.store(true, std::memory_order_release);
    //std::cout << "Created " << orders.size() << " valid orders.\n";

    // Example usage:
    //for (const auto& order : orders) {
      //  std::cout << "OrderId: " << order->getOrderId() << ", Side: "
     //             << (order->getSide() == Side::Buy ? "BUY" : "SELL") << "\n";
    //}
}

void orderBookProcessing(Queues& queues){
    OrderBook orderBook;

    while(true){
        auto uniOrderPointer = queues.orderBookQueue.pop();

        if(uniOrderPointer){

            //Changed it to a shared but it's safe to make it shared since this is the only thread accessing it now 
            OrderPointer sharedOrderPointer = std::move(uniOrderPointer);
            orderBook.processOrder(sharedOrderPointer);
        }
        else{
            // No item popped:
            if (done.load(std::memory_order_acquire)) {
                // Producer says no more items will come, so we can exit
                break;
            }
            // Sleep/yield briefly to avoid hogging CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    for(const auto& trade: orderBook.getTrades()){
        std::cout << "Trade " << trade.getTradeId() << " has a quantity of " << trade.getQuantity() << " for a price of " << trade.getPrice() 
                << ". The seller order id is " << trade.getSellOrderId() << " and the buyer orderId is " << trade.getBuyOrderId() << "\n";
    }



}
int main(){
    Queues queues;
	// Task 1 read file, validate format
	std::thread openFileThread(openFile, std::ref(queues));
    ThreadGuard guardFile(openFileThread);

	// Task 2 create order argumetns pass to task 3    
    std::thread createOrderArgThread(createOrderArgs, std::ref(queues));
    ThreadGuard gaurdArg(createOrderArgThread);
	// Task 3 takes order arguments creates order pointers and processes them 
    std::thread orderBookThread(orderBookProcessing, std::ref(queues));
    ThreadGuard gaurdOrder(orderBookThread);
    return 0;
}