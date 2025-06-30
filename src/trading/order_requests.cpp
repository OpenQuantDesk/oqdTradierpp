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

#include "oqdTradierpp/trading/order_requests.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

std::string OrderRequest::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("class", order_class)
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
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

std::string OptionOrderRequest::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("class", order_class)
        .field("symbol", symbol)
        .field("side", side)
        .field("quantity", quantity)
        .field("type", type)
        .field("duration", duration)
        .field("option_symbol", option_symbol);
    
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    if (stop.has_value()) {
        builder.field("stop", stop.value());
    }
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

} // namespace oqd