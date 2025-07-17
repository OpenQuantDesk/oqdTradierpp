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

#include "oqdTradierpp/fundamentals/corp_info.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

CompanyInfo CompanyInfo::from_json(const simdjson::dom::element& elem) {
    CompanyInfo info;
    info.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    info.name = std::string(elem["name"].get_string().value_unsafe());
    info.description = std::string(elem["description"].get_string().value_unsafe());
    info.exchange = std::string(elem["exchange"].get_string().value_unsafe());
    info.sector = std::string(elem["sector"].get_string().value_unsafe());
    info.industry = std::string(elem["industry"].get_string().value_unsafe());
    info.website = std::string(elem["website"].get_string().value_unsafe());
    info.ceo = std::string(elem["ceo"].get_string().value_unsafe());
    info.market_cap = elem["market_cap"].get_double().value_unsafe();
    info.pe_ratio = elem["pe_ratio"].get_double().value_unsafe();
    info.dividend_yield = elem["dividend_yield"].get_double().value_unsafe();
    return info;
}

std::string CompanyInfo::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("name", name)
        .field("description", description)
        .field("exchange", exchange)
        .field("sector", sector)
        .field("industry", industry)
        .field("website", website)
        .field("ceo", ceo)
        .field("market_cap", market_cap)
        .field("pe_ratio", pe_ratio)
        .field("dividend_yield", dividend_yield)
        .end_object()
        .str();
}

} // namespace oqd