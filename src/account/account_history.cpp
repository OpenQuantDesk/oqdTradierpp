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

#include "oqdTradierpp/account/account_history.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

AccountHistory AccountHistory::from_json(const simdjson::dom::element& elem) {
    AccountHistory history;
    auto history_elem = elem["history"];
    if (history_elem.is_object()) {
        auto event_result = history_elem["event"];
        if (event_result.error() == simdjson::SUCCESS) {
            auto event_array = event_result.value();
            if (event_array.is_array()) {
                for (const auto& event : event_array.get_array()) {
                    history.history.push_back(HistoryItem::from_json(event));
                }
            } else {
                history.history.push_back(HistoryItem::from_json(event_array));
            }
        }
    }
    return history;
}

std::string AccountHistory::to_json() const {
    return json::create_object()
        .array_field("history", history)
        .end_object()
        .str();
}

}