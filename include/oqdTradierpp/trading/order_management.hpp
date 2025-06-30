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
#include <simdjson.h>
#include "oqdTradierpp/core/enums.hpp"

namespace oqd {

struct OrderModification {
    std::optional<OrderType> type;
    std::optional<OrderDuration> duration;
    std::optional<double> price;
    std::optional<double> stop;
    std::optional<int> quantity;
    
    std::string to_json() const;
};

struct OrderPreview {
    double commission;
    double cost;
    double fees;
    std::string symbol;
    int quantity;
    OrderSide side;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::string result;
    std::string strategy_type;
    std::string last_day;
    double day_trades;
    double buying_power;
    double change;
    double amount;
    
    static OrderPreview from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct OrderResponse {
    std::string id;
    std::string status;
    
    static OrderResponse from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

} // namespace oqd