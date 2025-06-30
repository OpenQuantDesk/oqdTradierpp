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

#include "oqdTradierpp/account/gain_loss_item.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

GainLossItem GainLossItem::from_json(const simdjson::dom::element& elem) {
    GainLossItem item;
    
    auto close_date_result = elem["close_date"].get_string();
    if (close_date_result.error() == simdjson::SUCCESS) {
        item.close_date = std::string(close_date_result.value());
    }
    
    auto cost_result = elem["cost"].get_double();
    if (cost_result.error() == simdjson::SUCCESS) {
        item.cost = cost_result.value();
    }
    
    auto gain_loss_result = elem["gain_loss"].get_double();
    if (gain_loss_result.error() == simdjson::SUCCESS) {
        item.gain_loss = gain_loss_result.value();
    }
    
    auto gain_loss_percent_result = elem["gain_loss_percent"].get_double();
    if (gain_loss_percent_result.error() == simdjson::SUCCESS) {
        item.gain_loss_percent = gain_loss_percent_result.value();
    }
    
    auto open_date_result = elem["open_date"].get_string();
    if (open_date_result.error() == simdjson::SUCCESS) {
        item.open_date = std::string(open_date_result.value());
    }
    
    auto proceeds_result = elem["proceeds"].get_double();
    if (proceeds_result.error() == simdjson::SUCCESS) {
        item.proceeds = proceeds_result.value();
    }
    
    auto quantity_result = elem["quantity"].get_double();
    if (quantity_result.error() == simdjson::SUCCESS) {
        item.quantity = quantity_result.value();
    }
    
    auto symbol_result = elem["symbol"].get_string();
    if (symbol_result.error() == simdjson::SUCCESS) {
        item.symbol = std::string(symbol_result.value());
    }
    
    auto term_result = elem["term"].get_int64();
    if (term_result.error() == simdjson::SUCCESS) {
        item.term = static_cast<int>(term_result.value());
    }
    
    return item;
}

std::string GainLossItem::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("close_date", close_date)
        .field("cost", cost)
        .field("gain_loss", gain_loss)
        .field("gain_loss_percent", gain_loss_percent)
        .field("open_date", open_date)
        .field("proceeds", proceeds)
        .field("quantity", quantity)
        .field("symbol", symbol)
        .field("term", term)
        .end_object()
        .str();
}

}