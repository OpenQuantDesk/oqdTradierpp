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

#include "oqdTradierpp/fundamentals/corp_financials.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

CorporateFinancials CorporateFinancials::from_json(const simdjson::dom::element& elem) {
    CorporateFinancials financials;
    financials.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    financials.period = std::string(elem["period"].get_string().value_unsafe());
    financials.revenue = elem["revenue"].get_double().value_unsafe();
    financials.net_income = elem["net_income"].get_double().value_unsafe();
    financials.eps = elem["eps"].get_double().value_unsafe();
    financials.assets = elem["assets"].get_double().value_unsafe();
    financials.liabilities = elem["liabilities"].get_double().value_unsafe();
    financials.equity = elem["equity"].get_double().value_unsafe();
    financials.cash_flow = elem["cash_flow"].get_double().value_unsafe();
    return financials;
}

std::string CorporateFinancials::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("period", period)
        .field("revenue", revenue)
        .field("net_income", net_income)
        .field("eps", eps)
        .field("assets", assets)
        .field("liabilities", liabilities)
        .field("equity", equity)
        .field("cash_flow", cash_flow)
        .end_object()
        .str();
}

FinancialRatios FinancialRatios::from_json(const simdjson::dom::element& elem) {
    FinancialRatios ratios;
    ratios.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    ratios.price_to_earnings = elem["price_to_earnings"].get_double().value_unsafe();
    ratios.price_to_book = elem["price_to_book"].get_double().value_unsafe();
    ratios.price_to_sales = elem["price_to_sales"].get_double().value_unsafe();
    ratios.debt_to_equity = elem["debt_to_equity"].get_double().value_unsafe();
    ratios.return_on_equity = elem["return_on_equity"].get_double().value_unsafe();
    ratios.return_on_assets = elem["return_on_assets"].get_double().value_unsafe();
    ratios.current_ratio = elem["current_ratio"].get_double().value_unsafe();
    ratios.quick_ratio = elem["quick_ratio"].get_double().value_unsafe();
    return ratios;
}

std::string FinancialRatios::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("price_to_earnings", price_to_earnings)
        .field("price_to_book", price_to_book)
        .field("price_to_sales", price_to_sales)
        .field("debt_to_equity", debt_to_equity)
        .field("return_on_equity", return_on_equity)
        .field("return_on_assets", return_on_assets)
        .field("current_ratio", current_ratio)
        .field("quick_ratio", quick_ratio)
        .end_object()
        .str();
}

} // namespace oqd