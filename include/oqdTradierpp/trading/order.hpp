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
#include <vector>
#include <optional>
#include <simdjson.h>
#include "oqdTradierpp/core/enums.hpp"

namespace oqd {

struct Leg {
    std::string option_symbol;
    OrderSide side;
    int quantity;
    
    static Leg from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct Order {
    std::string id;
    OrderType type;
    std::string symbol;
    OrderSide side;
    int quantity;
    OrderStatus status;
    OrderDuration duration;
    std::optional<double> price;
    std::optional<double> stop_price;
    std::optional<double> avg_fill_price;
    int exec_quantity;
    double last_fill_price;
    int last_fill_quantity;
    int remaining_quantity;
    std::string create_date;
    std::string transaction_date;
    OrderClass order_class;
    std::vector<Leg> legs;
    
    static Order from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

} // namespace oqd