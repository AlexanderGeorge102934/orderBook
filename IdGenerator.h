#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <atomic>

#include "Using.h" 

class IdGenerator {
private:
    std::atomic<OrderId> nextOrderId{1};
    std::atomic<TradeId> nextTradeId{1};

public:
    OrderId generateOrderId() {
        return nextOrderId.fetch_add(1, std::memory_order_relaxed);
    }

    TradeId generateTradeId() {
        return nextTradeId.fetch_add(1, std::memory_order_relaxed);
    }
};

#endif
