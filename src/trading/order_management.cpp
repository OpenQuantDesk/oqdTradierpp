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

#include "oqdTradierpp/trading/order_management.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

std::string OrderModification::to_json() const {
    auto builder = json::create_object().set_fixed().set_precision(2);
    
    if (type.has_value()) {
        builder.field("type", type.value());
    }
    if (duration.has_value()) {
        builder.field("duration", duration.value());
    }
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    if (stop.has_value()) {
        builder.field("stop", stop.value());
    }
    if (quantity.has_value()) {
        builder.field("quantity", quantity.value());
    }
    
    return builder.end_object().str();
}

OrderPreview OrderPreview::from_json(const simdjson::dom::element& elem) {
    OrderPreview preview;
    preview.commission = elem["commission"].get_double().value_unsafe();
    preview.cost = elem["cost"].get_double().value_unsafe();
    preview.fees = elem["fees"].get_double().value_unsafe();
    preview.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    preview.quantity = static_cast<int>(elem["quantity"].get_int64().value_unsafe());
    preview.side = order_side_from_string(std::string(elem["side"].get_string().value_unsafe()));
    preview.type = order_type_from_string(std::string(elem["type"].get_string().value_unsafe()));
    preview.duration = order_duration_from_string(std::string(elem["duration"].get_string().value_unsafe()));
    
    auto price_result = elem["price"];
    if (price_result.error() == simdjson::SUCCESS) {
        preview.price = price_result.value().get_double().value();
    }
    
    preview.result = std::string(elem["result"].get_string().value_unsafe());
    preview.strategy_type = std::string(elem["strategy_type"].get_string().value_unsafe());
    preview.last_day = std::string(elem["last_day"].get_string().value_unsafe());
    preview.day_trades = elem["day_trades"].get_double().value_unsafe();
    preview.buying_power = elem["buying_power"].get_double().value_unsafe();
    preview.change = elem["change"].get_double().value_unsafe();
    preview.amount = elem["amount"].get_double().value_unsafe();
    
    return preview;
}

std::string OrderPreview::to_json() const {
    auto builder = json::create_object()
        .set_fixed().set_precision(2)
        .field("commission", commission)
        .field("cost", cost)
        .field("fees", fees)
        .field("symbol", symbol)
        .field("quantity", quantity)
        .field("side", side)
        .field("type", type)
        .field("duration", duration);
    
    if (price.has_value()) {
        builder.field("price", price.value());
    }
    
    builder.field("result", result)
           .field("strategy_type", strategy_type)
           .field("last_day", last_day)
           .field("day_trades", day_trades)
           .field("buying_power", buying_power)
           .field("change", change)
           .field("amount", amount);
    
    return builder.end_object().str();
}

OrderResponse OrderResponse::from_json(const simdjson::dom::element& elem) {
    OrderResponse response;
    response.id = std::string(elem["id"].get_string().value_unsafe());
    response.status = std::string(elem["status"].get_string().value_unsafe());
    return response;
}

std::string OrderResponse::to_json() const {
    return json::create_object()
        .field("id", id)
        .field("status", status)
        .end_object()
        .str();
}

} // namespace oqd