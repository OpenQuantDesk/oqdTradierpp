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

#include "oqdTradierpp/account/history_item.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

HistoryItem HistoryItem::from_json(const simdjson::dom::element& elem) {
    HistoryItem item;
    item.amount = elem["amount"].get_double().value_unsafe();
    item.date = std::string(elem["date"].get_string().value_unsafe());
    item.type = std::string(elem["type"].get_string().value_unsafe());
    item.journal = std::string(elem["journal"].get_string().value_unsafe());
    item.description = std::string(elem["description"].get_string().value_unsafe());
    item.quantity = elem["quantity"].get_double().value_unsafe();
    item.price = elem["price"].get_double().value_unsafe();
    item.commission = elem["commission"].get_double().value_unsafe();
    item.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    return item;
}

std::string HistoryItem::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("amount", amount)
        .field("date", date)
        .field("type", type)
        .field("journal", journal)
        .field("description", description)
        .field("quantity", quantity)
        .field("price", price)
        .field("commission", commission)
        .field("symbol", symbol)
        .end_object()
        .str();
}

}