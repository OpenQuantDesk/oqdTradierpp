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

#include "oqdTradierpp/trading/multileg_orders.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

std::string MultilegOrderRequest::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("class", order_class)
        .field("type", type)
        .field("duration", duration);
    
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    
    builder.array_field("legs", legs);
    
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

std::string ComboOrderRequest::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("class", order_class)
        .field("type", type)
        .field("duration", duration);
    
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    
    builder.array_field("legs", legs);
    
    if (equity_symbol.has_value()) {
        builder.field("equity_symbol", equity_symbol.value());
    }
    if (equity_side.has_value()) {
        builder.field("equity_side", equity_side.value());
    }
    if (equity_quantity.has_value()) {
        builder.field("equity_quantity", equity_quantity.value());
    }
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

} // namespace oqd