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

struct SpreadLeg {
    std::string option_symbol;
    OrderSide side;
    int quantity;
    std::optional<double> ratio;  // For uneven spreads
    
    static SpreadLeg from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct SpreadOrderRequest {
    OrderClass order_class = OrderClass::Multileg;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::vector<SpreadLeg> legs;
    std::string spread_type; // "vertical", "horizontal", "diagonal", "iron_condor", etc.
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

} // namespace oqd