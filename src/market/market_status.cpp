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

#include "oqdTradierpp/market/market_status.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

MarketClock MarketClock::from_json(const simdjson::dom::element& elem) {
    MarketClock clock;
    auto clock_elem = elem["clock"];
    clock.date = std::string(clock_elem["date"].get_string().value_unsafe());
    clock.description = std::string(clock_elem["description"].get_string().value_unsafe());
    clock.state = std::string(clock_elem["state"].get_string().value_unsafe());
    clock.timestamp = std::string(clock_elem["timestamp"].get_string().value_unsafe());
    clock.next_state_change = std::string(clock_elem["next_state_change"].get_string().value_unsafe());
    clock.next_state = std::string(clock_elem["next_state"].get_string().value_unsafe());
    return clock;
}

std::string MarketClock::to_json() const {
    return json::create_object()
        .field("date", date)
        .field("description", description)
        .field("state", state)
        .field("timestamp", timestamp)
        .field("next_state_change", next_state_change)
        .field("next_state", next_state)
        .end_object()
        .str();
}

MarketDay MarketDay::from_json(const simdjson::dom::element& elem) {
    MarketDay day;
    day.date = std::string(elem["date"].get_string().value_unsafe());
    day.status = std::string(elem["status"].get_string().value_unsafe());
    day.description = std::string(elem["description"].get_string().value_unsafe());
    
    if (!elem["premarket"].is_null()) {
        day.premarket = std::string(elem["premarket"].get_string().value_unsafe());
    }
    if (!elem["open"].is_null()) {
        day.open = std::string(elem["open"].get_string().value_unsafe());
    }
    if (!elem["close"].is_null()) {
        day.close = std::string(elem["close"].get_string().value_unsafe());
    }
    if (!elem["postmarket"].is_null()) {
        day.postmarket = std::string(elem["postmarket"].get_string().value_unsafe());
    }
    
    return day;
}

std::string MarketDay::to_json() const {
    auto builder = json::create_object()
        .field("date", date)
        .field("status", status)
        .field("description", description);
    
    if (premarket.has_value()) {
        builder.field("premarket", premarket.value());
    }
    if (open.has_value()) {
        builder.field("open", open.value());
    }
    if (close.has_value()) {
        builder.field("close", close.value());
    }
    if (postmarket.has_value()) {
        builder.field("postmarket", postmarket.value());
    }
    
    return builder.end_object().str();
}

} // namespace oqd