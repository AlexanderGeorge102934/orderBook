#include "Using.h"
#include <format>
class Trade{
	private:
		TradeId tradeId_;
		OrderId buyOrderId_;
		OrderId sellOrderId_;
		Quantity quantity_;
		Price price_;
	public:
		explicit Trade(const TradeId& tradeId, const OrderId& buyOrderId, const OrderId& sellOrderId, const Quantity& quantity, const Price& price)
			: tradeId_ { tradeId }
			, buyOrderId_ { buyOrderId }
			, sellOrderId_ { sellOrderId } 
			, quantity_ { quantity }
			, price_ { price } 
			{
				if(quantity == 0){
					throw std::invalid_argument(std::format("Quantity of trade Trade({}) cannot be 0", getTradeId()));
				}
			}	

		[[nodiscard]] const TradeId& getTradeId() const noexcept { return tradeId_; }
		[[nodiscard]] const OrderId& getBuyOrderId() const noexcept { return buyOrderId_; } 
		[[nodiscard]] const OrderId& getSellOrderId() const noexcept { return sellOrderId_; }  
		[[nodiscard]] const Quantity& getQuantity() const noexcept { return quantity_; }
		[[nodiscard]] const Price& getPrice() const noexcept { return price_; }


};