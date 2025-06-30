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

#include "oqdTradierpp/market/quote.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

Quote Quote::from_json(const simdjson::dom::element& elem) {
    Quote quote;
    quote.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    quote.description = elem["description"].is_null() ? "" : std::string(elem["description"].get_string().value_unsafe());
    quote.exch = elem["exch"].is_null() ? "" : std::string(elem["exch"].get_string().value_unsafe());
    quote.type = elem["type"].is_null() ? "" : std::string(elem["type"].get_string().value_unsafe());
    quote.last = elem["last"].is_null() ? 0.0 : elem["last"].get_double().value_unsafe();
    quote.change = elem["change"].is_null() ? 0.0 : elem["change"].get_double().value_unsafe();
    quote.change_percentage = elem["change_percentage"].is_null() ? 0.0 : elem["change_percentage"].get_double().value_unsafe();
    quote.volume = elem["volume"].is_null() ? 0.0 : elem["volume"].get_double().value_unsafe();
    quote.average_volume = elem["average_volume"].is_null() ? 0.0 : elem["average_volume"].get_double().value_unsafe();
    quote.last_volume = elem["last_volume"].is_null() ? 0.0 : elem["last_volume"].get_double().value_unsafe();
    quote.trade_date = elem["trade_date"].is_null() ? "" : std::string(elem["trade_date"].get_string().value_unsafe());
    quote.open = elem["open"].is_null() ? 0.0 : elem["open"].get_double().value_unsafe();
    quote.high = elem["high"].is_null() ? 0.0 : elem["high"].get_double().value_unsafe();
    quote.low = elem["low"].is_null() ? 0.0 : elem["low"].get_double().value_unsafe();
    quote.close = elem["close"].is_null() ? 0.0 : elem["close"].get_double().value_unsafe();
    quote.prevclose = elem["prevclose"].is_null() ? 0.0 : elem["prevclose"].get_double().value_unsafe();
    quote.week_52_high = elem["week_52_high"].is_null() ? "" : std::string(elem["week_52_high"].get_string().value_unsafe());
    quote.week_52_low = elem["week_52_low"].is_null() ? "" : std::string(elem["week_52_low"].get_string().value_unsafe());
    quote.bid = elem["bid"].is_null() ? 0.0 : elem["bid"].get_double().value_unsafe();
    quote.bidsize = elem["bidsize"].is_null() ? 0.0 : elem["bidsize"].get_double().value_unsafe();
    quote.bidexch = elem["bidexch"].is_null() ? "" : std::string(elem["bidexch"].get_string().value_unsafe());
    quote.bid_date = elem["bid_date"].is_null() ? "" : std::string(elem["bid_date"].get_string().value_unsafe());
    quote.ask = elem["ask"].is_null() ? 0.0 : elem["ask"].get_double().value_unsafe();
    quote.asksize = elem["asksize"].is_null() ? 0.0 : elem["asksize"].get_double().value_unsafe();
    quote.askexch = elem["askexch"].is_null() ? "" : std::string(elem["askexch"].get_string().value_unsafe());
    quote.ask_date = elem["ask_date"].is_null() ? "" : std::string(elem["ask_date"].get_string().value_unsafe());
    
    quote.strike = elem["strike"].is_null() ? 0.0 : elem["strike"].get_double().value_unsafe();
    quote.contract_size = elem["contract_size"].is_null() ? "" : std::string(elem["contract_size"].get_string().value_unsafe());
    quote.expiration_date = elem["expiration_date"].is_null() ? "" : std::string(elem["expiration_date"].get_string().value_unsafe());
    quote.expiration_type = elem["expiration_type"].is_null() ? "" : std::string(elem["expiration_type"].get_string().value_unsafe());
    quote.option_type = elem["option_type"].is_null() ? "" : std::string(elem["option_type"].get_string().value_unsafe());
    quote.root_symbol = elem["root_symbol"].is_null() ? "" : std::string(elem["root_symbol"].get_string().value_unsafe());
    
    quote.delta = elem["delta"].is_null() ? 0.0 : elem["delta"].get_double().value_unsafe();
    quote.gamma = elem["gamma"].is_null() ? 0.0 : elem["gamma"].get_double().value_unsafe();
    quote.theta = elem["theta"].is_null() ? 0.0 : elem["theta"].get_double().value_unsafe();
    quote.vega = elem["vega"].is_null() ? 0.0 : elem["vega"].get_double().value_unsafe();
    quote.rho = elem["rho"].is_null() ? 0.0 : elem["rho"].get_double().value_unsafe();
    quote.phi = elem["phi"].is_null() ? 0.0 : elem["phi"].get_double().value_unsafe();
    quote.bid_iv = elem["bid_iv"].is_null() ? 0.0 : elem["bid_iv"].get_double().value_unsafe();
    quote.mid_iv = elem["mid_iv"].is_null() ? 0.0 : elem["mid_iv"].get_double().value_unsafe();
    quote.ask_iv = elem["ask_iv"].is_null() ? 0.0 : elem["ask_iv"].get_double().value_unsafe();
    quote.smv_vol = elem["smv_vol"].is_null() ? 0.0 : elem["smv_vol"].get_double().value_unsafe();
    quote.updated_at = elem["updated_at"].is_null() ? 0.0 : elem["updated_at"].get_double().value_unsafe();
    quote.open_interest = elem["open_interest"].is_null() ? 0.0 : elem["open_interest"].get_double().value_unsafe();
    
    return quote;
}

std::string Quote::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("symbol", symbol)
        .field("description", description)
        .field("exch", exch)
        .field("type", type)
        .field("last", last)
        .field("change", change)
        .field("change_percentage", change_percentage)
        .field("volume", volume)
        .field("average_volume", average_volume)
        .field("last_volume", last_volume)
        .field("trade_date", trade_date)
        .field("open", open)
        .field("high", high)
        .field("low", low)
        .field("close", close)
        .field("prevclose", prevclose)
        .field("week_52_high", week_52_high)
        .field("week_52_low", week_52_low)
        .field("bid", bid)
        .field("bidsize", bidsize)
        .field("bidexch", bidexch)
        .field("bid_date", bid_date)
        .field("ask", ask)
        .field("asksize", asksize)
        .field("askexch", askexch)
        .field("ask_date", ask_date)
        .field_optional("strike", strike)
        .field_optional("contract_size", contract_size)
        .field_optional("expiration_date", expiration_date)
        .field_optional("delta", delta)
        .field_optional("gamma", gamma)
        .field_optional("theta", theta)
        .field_optional("vega", vega)
        .end_object()
        .str();
}

} // namespace oqd