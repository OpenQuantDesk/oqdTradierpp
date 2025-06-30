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

#include "oqdTradierpp/trading/advanced_orders.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

std::string OrderComponent::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("symbol", symbol)
        .field("side", side)
        .field("quantity", quantity)
        .field("type", type)
        .field("duration", duration);
    
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    if (stop.has_value()) {
        builder.field("stop", stop.value());
    }
    if (option_symbol.has_value()) {
        builder.field("option_symbol", option_symbol.value());
    }
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

std::string OTOOrderRequest::to_json() const {
    auto builder = json::create_object()
        .field("class", order_class)
        .field("first_order", first_order)
        .field("second_order", second_order);
    
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

std::string OCOOrderRequest::to_json() const {
    auto builder = json::create_object()
        .field("class", order_class)
        .field("first_order", first_order)
        .field("second_order", second_order);
    
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

std::string OTOCOOrderRequest::to_json() const {
    auto builder = json::create_object()
        .field("class", order_class)
        .field("primary_order", primary_order)
        .field("profit_order", profit_order)
        .field("stop_order", stop_order);
    
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

} // namespace oqd