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

#include "oqdTradierpp/fundamentals/corp_actions.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

CorporateActions CorporateActions::from_json(const simdjson::dom::element& elem) {
    CorporateActions actions;
    actions.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    actions.type = std::string(elem["type"].get_string().value_unsafe());
    actions.date = std::string(elem["date"].get_string().value_unsafe());
    actions.description = std::string(elem["description"].get_string().value_unsafe());
    
    auto value_result = elem["value"];
    if (value_result.error() == simdjson::SUCCESS) {
        actions.value = value_result.value().get_double().value();
    }
    
    return actions;
}

std::string CorporateActions::to_json() const {
    auto builder = json::create_object()
        .field("symbol", symbol)
        .field("type", type)
        .field("date", date)
        .field("description", description);
    
    if (value.has_value()) {
        builder.field("value", value.value());
    }
    
    return builder.end_object().str();
}

} // namespace oqd

