
#include <fstream> 
#include <iostream> 
#include <string> 
#include <sstream>
#include <cctype>
#include <atomic> 
#include <thread>

#include "TradingSystem.h"

int main() {
    try {
        TradingSystem tradingSystem{};
        std::cout << "Trading System Online. Listening on Port 13..." << std::endl;
        tradingSystem.startServer();
    }
    catch (const std::exception& e) {
        std::cerr << "Critical System Failure: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}