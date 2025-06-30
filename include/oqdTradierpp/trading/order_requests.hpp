/*
/        oqdTradierpp - Full featured Tradier API library 
/       
/        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
/        Version:           v1.0.0
/        Release Date:  06/30/2025
/        License: Apache 2.0
/        Disclaimer: This software is provided "as-is" without warranties of any kind. 
/                    Use at your own risk. The authors are not liable for any trading losses.
/                    Not financial advice. By using this software, you accept all risks.
/
*/

#pragma once

#include <string>
#include <optional>
#include "oqdTradierpp/core/enums.hpp"

namespace oqd {

// Base Order Request
struct OrderRequest {
    OrderClass order_class = OrderClass::Equity;
    std::string symbol;
    OrderSide side = OrderSide::Buy;
    int quantity = 0;
    OrderType type = OrderType::Market;
    OrderDuration duration = OrderDuration::Day;
    std::optional<double> price;
    std::optional<double> stop;
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

struct EquityOrderRequest : public OrderRequest {
    EquityOrderRequest() { order_class = OrderClass::Equity; }
};

struct OptionOrderRequest : public OrderRequest {
    std::string option_symbol;
    
    OptionOrderRequest() { order_class = OrderClass::Option; }
    std::string to_json() const;
};

} // namespace oqd