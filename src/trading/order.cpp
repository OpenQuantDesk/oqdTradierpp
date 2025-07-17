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

#include "oqdTradierpp/trading/order.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

Leg Leg::from_json(const simdjson::dom::element& elem) {
    Leg leg;
    leg.option_symbol = std::string(elem["option_symbol"].get_string().value_unsafe());
    leg.side = order_side_from_string(std::string(elem["side"].get_string().value_unsafe()));
    leg.quantity = elem["quantity"].get_int64().value_unsafe();
    return leg;
}

std::string Leg::to_json() const {
    return json::create_object()
        .field("option_symbol", option_symbol)
        .field("side", side)
        .field("quantity", quantity)
        .end_object()
        .str();
}

Order Order::from_json(const simdjson::dom::element& elem) {
    Order order;
    order.id = std::string(elem["id"].get_string().value_unsafe());
    order.type = order_type_from_string(std::string(elem["type"].get_string().value_unsafe()));
    order.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    order.side = order_side_from_string(std::string(elem["side"].get_string().value_unsafe()));
    order.quantity = elem["quantity"].get_int64().value_unsafe();
    order.status = order_status_from_string(std::string(elem["status"].get_string().value_unsafe()));
    order.duration = order_duration_from_string(std::string(elem["duration"].get_string().value_unsafe()));
    
    if (!elem["price"].is_null()) {
        order.price = elem["price"].get_double().value_unsafe();
    }
    if (!elem["stop_price"].is_null()) {
        order.stop_price = elem["stop_price"].get_double().value_unsafe();
    }
    if (!elem["avg_fill_price"].is_null()) {
        order.avg_fill_price = elem["avg_fill_price"].get_double().value_unsafe();
    }
    
    order.exec_quantity = elem["exec_quantity"].get_int64().value_unsafe();
    order.last_fill_price = elem["last_fill_price"].get_double().value_unsafe();
    order.last_fill_quantity = elem["last_fill_quantity"].get_int64().value_unsafe();
    order.remaining_quantity = elem["remaining_quantity"].get_int64().value_unsafe();
    order.create_date = std::string(elem["create_date"].get_string().value_unsafe());
    order.transaction_date = std::string(elem["transaction_date"].get_string().value_unsafe());
    order.order_class = order_class_from_string(std::string(elem["class"].get_string().value_unsafe()));
    
    auto legs_elem = elem["legs"];
    if (legs_elem.error() == simdjson::SUCCESS && legs_elem.is_array()) {
        for (const auto& leg : legs_elem.get_array()) {
            order.legs.push_back(Leg::from_json(leg));
        }
    }
    
    return order;
}

std::string Order::to_json() const {
    return json::create_object()
        .field("id", id)
        .field("type", type)
        .field("symbol", symbol)
        .field("side", side)
        .field("quantity", quantity)
        .field("status", status)
        .field("duration", duration)
        .field_optional("price", price)
        .field_optional("stop_price", stop_price)
        .field_optional("avg_fill_price", avg_fill_price)
        .field("exec_quantity", exec_quantity)
        .field("last_fill_price", last_fill_price)
        .field("last_fill_quantity", last_fill_quantity)
        .field("remaining_quantity", remaining_quantity)
        .field("create_date", create_date)
        .field("transaction_date", transaction_date)
        .field("class", order_class)
        .array_field("legs", legs)
        .end_object()
        .str();
}

} // namespace oqd