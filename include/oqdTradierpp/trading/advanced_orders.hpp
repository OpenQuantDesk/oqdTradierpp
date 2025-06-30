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

struct OrderComponent {
    std::string symbol;
    OrderSide side;
    int quantity;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::optional<double> stop;
    std::optional<std::string> option_symbol;
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

struct OTOOrderRequest {
    OrderClass order_class = OrderClass::OTO;
    OrderComponent first_order;   // Primary order
    OrderComponent second_order;  // Triggered order
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

struct OCOOrderRequest {
    OrderClass order_class = OrderClass::OCO;
    OrderComponent first_order;   // First order
    OrderComponent second_order;  // Alternate order (cancels first when filled)
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

struct OTOCOOrderRequest {
    OrderClass order_class = OrderClass::OTOCO;
    OrderComponent primary_order;    // Primary order that triggers the bracket
    OrderComponent profit_order;     // Profit-taking order
    OrderComponent stop_order;       // Stop-loss order
    std::optional<std::string> tag;
    
    std::string to_json() const;
};

} // namespace oqd