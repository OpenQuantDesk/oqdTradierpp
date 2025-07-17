/*
/        oqdTradierpp - Full featured Tradier API library 
/       
/        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
/        Version:           v1.1
/        Release Date:  06/30/2025
/        License: Apache 2.0
/        Disclaimer: This software is provided "as-is" without warranties of any kind. 
/                    Use at your own risk. The authors are not liable for any trading losses.
/                    Not financial advice. By using this software, you accept all risks.
/
*/

#pragma once

#include <string>
#include <vector>
#include <optional>
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/trading/order.hpp"

namespace oqd {

struct MultilegOrderRequest {
    OrderClass order_class = OrderClass::Multileg;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::vector<Leg> legs;
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

struct ComboOrderRequest {
    OrderClass order_class = OrderClass::Combo;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::vector<Leg> legs;
    std::optional<std::string> equity_symbol;
    std::optional<OrderSide> equity_side;
    std::optional<int> equity_quantity;
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

} // namespace oqd