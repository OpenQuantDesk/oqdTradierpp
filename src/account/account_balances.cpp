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

#include "oqdTradierpp/account/account_balances.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

AccountBalances AccountBalances::from_json(const simdjson::dom::element& elem) {
    AccountBalances balances;
    
    balances.account_number = elem["account_number"].is_null() ? 0.0 : elem["account_number"].get_double().value_unsafe();
    balances.total_equity = elem["total_equity"].is_null() ? 0.0 : elem["total_equity"].get_double().value_unsafe();
    balances.long_market_value = elem["long_market_value"].is_null() ? 0.0 : elem["long_market_value"].get_double().value_unsafe();
    balances.short_market_value = elem["short_market_value"].is_null() ? 0.0 : elem["short_market_value"].get_double().value_unsafe();
    balances.account_value = elem["account_value"].is_null() ? 0.0 : elem["account_value"].get_double().value_unsafe();
    balances.close_pl = elem["close_pl"].is_null() ? 0.0 : elem["close_pl"].get_double().value_unsafe();
    balances.current_requirement = elem["current_requirement"].is_null() ? 0.0 : elem["current_requirement"].get_double().value_unsafe();
    balances.equity = elem["equity"].is_null() ? 0.0 : elem["equity"].get_double().value_unsafe();
    balances.long_liquid_value = elem["long_liquid_value"].is_null() ? 0.0 : elem["long_liquid_value"].get_double().value_unsafe();
    balances.long_market_value_bp = elem["long_market_value_bp"].is_null() ? 0.0 : elem["long_market_value_bp"].get_double().value_unsafe();
    balances.short_liquid_value = elem["short_liquid_value"].is_null() ? 0.0 : elem["short_liquid_value"].get_double().value_unsafe();
    balances.short_market_value_bp = elem["short_market_value_bp"].is_null() ? 0.0 : elem["short_market_value_bp"].get_double().value_unsafe();
    balances.uncleared_funds = elem["uncleared_funds"].is_null() ? 0.0 : elem["uncleared_funds"].get_double().value_unsafe();
    balances.pending_orders_count = elem["pending_orders_count"].is_null() ? 0.0 : elem["pending_orders_count"].get_double().value_unsafe();
    balances.option_short_value = elem["option_short_value"].is_null() ? 0.0 : elem["option_short_value"].get_double().value_unsafe();
    balances.total_cash = elem["total_cash"].is_null() ? 0.0 : elem["total_cash"].get_double().value_unsafe();
    balances.unsettled_funds = elem["unsettled_funds"].is_null() ? 0.0 : elem["unsettled_funds"].get_double().value_unsafe();
    balances.dividend = elem["dividend"].is_null() ? 0.0 : elem["dividend"].get_double().value_unsafe();
    balances.cash = elem["cash"].is_null() ? 0.0 : elem["cash"].get_double().value_unsafe();
    balances.market_value = elem["market_value"].is_null() ? 0.0 : elem["market_value"].get_double().value_unsafe();
    
    return balances;
}

std::string AccountBalances::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("account_number", account_number)
        .field("total_equity", total_equity)
        .field("long_market_value", long_market_value)
        .field("short_market_value", short_market_value)
        .field("account_value", account_value)
        .field("close_pl", close_pl)
        .field("current_requirement", current_requirement)
        .field("equity", equity)
        .field("long_liquid_value", long_liquid_value)
        .field("long_market_value_bp", long_market_value_bp)
        .field("short_liquid_value", short_liquid_value)
        .field("short_market_value_bp", short_market_value_bp)
        .field("uncleared_funds", uncleared_funds)
        .field("pending_orders_count", pending_orders_count)
        .field("option_short_value", option_short_value)
        .field("total_cash", total_cash)
        .field("unsettled_funds", unsettled_funds)
        .field("dividend", dividend)
        .field("cash", cash)
        .field("market_value", market_value)
        .end_object()
        .str();
}

}