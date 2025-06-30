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

#include "oqdTradierpp/account/position.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

Position Position::from_json(const simdjson::dom::element& elem) {
    Position position;
    position.cost_basis = elem["cost_basis"].get_double().value_unsafe();
    position.date_acquired = std::string(elem["date_acquired"].get_string().value_unsafe());
    position.id = std::string(elem["id"].get_string().value_unsafe());
    position.quantity = elem["quantity"].get_double().value_unsafe();
    position.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    return position;
}

std::string Position::to_json() const {
    return json::create_object()
        .field("cost_basis", cost_basis)
        .field("date_acquired", date_acquired)
        .field("id", id)
        .field("quantity", quantity)
        .field("symbol", symbol)
        .end_object()
        .str();
}

}