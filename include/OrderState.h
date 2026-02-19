#ifndef ORDERSTATE_H
#define ORDERSTATE_H

#include <cstdint>

enum class OrderState : uint8_t
{
    Processing,
    Filled,
    Cancelled,
    Rejected,
    Expired
};

#endif