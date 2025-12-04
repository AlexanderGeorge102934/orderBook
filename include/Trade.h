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
		explicit Trade(const TradeId& tradeId, const OrderId& takerOrderId, const OrderId& makerOrderId, const Quantity& quantity, const Price& price)
			: tradeId_ { tradeId }
			, buyOrderId_ { takerOrderId }
			, sellOrderId_ { makerOrderId } 
			, quantity_ { quantity }
			, price_ { price } 
			{
				if(quantity == 0){
					throw std::invalid_argument(std::format("Quantity of trade Trade({}) cannot be 0", getTradeId()));
				}
			}	

		[[nodiscard]] const TradeId& getTradeId() const noexcept { return tradeId_; }
		[[nodiscard]] const OrderId& getTakerOrderId() const noexcept { return buyOrderId_; } 
		[[nodiscard]] const OrderId& getMakerOrderId() const noexcept { return sellOrderId_; }  
		[[nodiscard]] const Quantity& getQuantity() const noexcept { return quantity_; }
		[[nodiscard]] const Price& getPrice() const noexcept { return price_; }


};