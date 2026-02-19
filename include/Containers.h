#include <vector>
#include <list> 
#include <memory>
#include <memory_resource>

#include "Order.h"
#include "Trade.h"

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::pmr::list<OrderPointer>;
using Trades = std::pmr::vector<Trade>;