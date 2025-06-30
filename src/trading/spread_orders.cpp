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

#include "oqdTradierpp/trading/spread_orders.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

SpreadLeg SpreadLeg::from_json(const simdjson::dom::element& elem) {
    SpreadLeg leg;
    leg.option_symbol = std::string(elem["option_symbol"].get_string().value_unsafe());
    leg.side = order_side_from_string(std::string(elem["side"].get_string().value_unsafe()));
    leg.quantity = static_cast<int>(elem["quantity"].get_int64().value_unsafe());
    
    auto ratio_result = elem["ratio"];
    if (ratio_result.error() == simdjson::SUCCESS) {
        leg.ratio = ratio_result.value().get_double().value();
    }
    
    return leg;
}

std::string SpreadLeg::to_json() const {
    auto builder = json::create_object()
        .field("option_symbol", option_symbol)
        .field("side", side)
        .field("quantity", quantity);
    
    if (ratio.has_value()) {
        builder.set_fixed().set_precision(2).field("ratio", ratio.value());
    }
    
    return builder.end_object().str();
}

std::string SpreadOrderRequest::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("class", order_class)
        .field("type", type)
        .field("duration", duration);
    
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    
    builder.field("spread_type", spread_type)
           .array_field("legs", legs);
    
    if (tag.has_value()) {
        builder.field("tag", tag.value());
    }
    
    return builder.end_object().str();
}

} // namespace oqd