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

#include "oqdTradierpp/market/symbol_search.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

CompanySearch CompanySearch::from_json(const simdjson::dom::element& elem) {
    CompanySearch search;
    search.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    search.company = std::string(elem["company"].get_string().value_unsafe());
    search.exchange = std::string(elem["exchange"].get_string().value_unsafe());
    search.type = std::string(elem["type"].get_string().value_unsafe());
    return search;
}

std::string CompanySearch::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("company", company)
        .field("exchange", exchange)
        .field("type", type)
        .end_object()
        .str();
}

SymbolLookup SymbolLookup::from_json(const simdjson::dom::element& elem) {
    SymbolLookup lookup;
    lookup.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    lookup.company = std::string(elem["company"].get_string().value_unsafe());
    lookup.exchange = std::string(elem["exchange"].get_string().value_unsafe());
    lookup.type = std::string(elem["type"].get_string().value_unsafe());
    return lookup;
}

std::string SymbolLookup::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("company", company)
        .field("exchange", exchange)
        .field("type", type)
        .end_object()
        .str();
}

} // namespace oqd