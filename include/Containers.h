#include <vector>
#include <list> 
#include <memory>

#include "Order.h"
#include "Trade.h"

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;
using Trades = std::vector<Trade>;