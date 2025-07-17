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

#include "oqdTradierpp/fundamentals/corp_dividends.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

DividendInfo DividendInfo::from_json(const simdjson::dom::element& elem) {
    DividendInfo dividend;
    dividend.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    dividend.dividend_per_share = elem["dividend_per_share"].get_double().value_unsafe();
    dividend.ex_dividend_date = std::string(elem["ex_dividend_date"].get_string().value_unsafe());
    dividend.payment_date = std::string(elem["payment_date"].get_string().value_unsafe());
    dividend.record_date = std::string(elem["record_date"].get_string().value_unsafe());
    dividend.declaration_date = std::string(elem["declaration_date"].get_string().value_unsafe());
    dividend.yield = elem["yield"].get_double().value_unsafe();
    return dividend;
}

std::string DividendInfo::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("dividend_per_share", dividend_per_share)
        .field("ex_dividend_date", ex_dividend_date)
        .field("payment_date", payment_date)
        .field("record_date", record_date)
        .field("declaration_date", declaration_date)
        .field("yield", yield)
        .end_object()
        .str();
}

} // namespace oqd