
#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <format> 

#include "Side.h"
#include "Using.h"
#include "OrderType.h"

class Order{

	private:
		Side side_;
		Price price_;
		OrderId orderId_;
		OrderType orderType_;
		Quantity initialQuantity_;
		Quantity remainingQuantity_;
	
	public:
		Order(const Side& side, const Price& price, const OrderId& orderId, const OrderType& orderType, const Quantity& initialQuantity, const Quantity& remainingQuantity)
			: side_ { side }
			, price_ { price }
			, orderId_ { orderId }
			, orderType_ { orderType } 
			, initialQuantity_ { initialQuantity } 
			, remainingQuantity_ { remainingQuantity } 
			{
                // The quantity should never be less than 1 when initialized otherwise the order is filled             
				if (initialQuantity < 1) {
                    throw std::invalid_argument(std::format("Order({}) must have initial quantity >= 1 (got {})", orderId, initialQuantity));
                }
                if (remainingQuantity < 1) {
                    throw std::invalid_argument(std::format("Order({}) must have remaining quantity >= 1 (got {})", orderId, remainingQuantity));
                }
                if(remainingQuantity_ > initialQuantity_){
                    throw std::invalid_argument(std::format("Order({}) cannot have a greater remaining quantity than initial", orderId));
                }

            }

		[[nodiscard]] const Side& getSide() const noexcept { return side_; }
		[[nodiscard]] const Price& getPrice() const noexcept { return price_; } 
		[[nodiscard]] const OrderId& getOrderId() const noexcept { return orderId_; } 
		[[nodiscard]] const OrderType& getOrderType() const noexcept { return orderType_; }
	    [[nodiscard]] const Quantity& getInitialQuantity() const noexcept { return initialQuantity_; }
		[[nodiscard]] const Quantity& getRemainingQuantity() const noexcept { return remainingQuantity_; } 
		[[nodiscard]] Quantity getFilledQuantity() const noexcept { return initialQuantity_ - remainingQuantity_; } 
		[[nodiscard]] bool isFilled() const noexcept { return getFilledQuantity() == 0; }
		void Fill(const Quantity& quantity){

			// Assumption is that the quantity if greater should be handled before passing the quantity as an argument
			if(quantity > getRemainingQuantity()){
				throw std::logic_error(std::format("Quantity to fill is larger than the remaining quantity for Order({})", getOrderId()));
			}	
			
			remainingQuantity_ -= quantity;	
		}
};

#endif
