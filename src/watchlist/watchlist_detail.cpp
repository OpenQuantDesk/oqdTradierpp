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

#include "oqdTradierpp/watchlist/watchlist_detail.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

WatchlistDetail WatchlistDetail::from_json(const simdjson::dom::element& elem) {
    WatchlistDetail detail;
    detail.id = std::string(elem["id"].get_string().value_unsafe());
    detail.name = std::string(elem["name"].get_string().value_unsafe());
    
    auto symbols_elem = elem["symbols"];
    if (symbols_elem.is_object()) {
        auto symbol_result = symbols_elem["symbol"];
        if (symbol_result.error() == simdjson::SUCCESS) {
            auto symbol_array = symbol_result.value();
            if (symbol_array.is_array()) {
                for (const auto& symbol : symbol_array.get_array()) {
                    detail.symbols.push_back(std::string(symbol.get_string().value_unsafe()));
                }
            } else {
                detail.symbols.push_back(std::string(symbol_array.get_string().value_unsafe()));
            }
        }
    } else if (symbols_elem.is_array()) {
        for (const auto& symbol : symbols_elem.get_array()) {
            detail.symbols.push_back(std::string(symbol.get_string().value_unsafe()));
        }
    }
    
    return detail;
}

std::string WatchlistDetail::to_json() const {
    return json::create_object()
        .field("id", id)
        .field("name", name)
        .array_field("symbols", symbols)
        .end_object()
        .str();
}

} // namespace oqd