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

#include "oqdTradierpp/watchlist/watchlist.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

Watchlist Watchlist::from_json(const simdjson::dom::element& elem) {
    Watchlist watchlist;
    watchlist.id = std::string(elem["id"].get_string().value_unsafe());
    watchlist.name = std::string(elem["name"].get_string().value_unsafe());
    return watchlist;
}

std::string Watchlist::to_json() const {
    return json::create_object()
        .field("id", id)
        .field("name", name)
        .end_object()
        .str();
}

} // namespace oqd