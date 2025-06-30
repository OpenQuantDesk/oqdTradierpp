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

#include "oqdTradierpp/market/time_sales.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

TimeSales TimeSales::from_json(const simdjson::dom::element& elem) {
    TimeSales sales;
    sales.time = std::string(elem["time"].get_string().value_unsafe());
    sales.timestamp = elem["timestamp"].get_double().value_unsafe();
    sales.price = elem["price"].get_double().value_unsafe();
    sales.open = elem["open"].get_double().value_unsafe();
    sales.high = elem["high"].get_double().value_unsafe();
    sales.low = elem["low"].get_double().value_unsafe();
    sales.close = elem["close"].get_double().value_unsafe();
    sales.volume = elem["volume"].get_double().value_unsafe();
    sales.vwap = elem["vwap"].get_double().value_unsafe();
    return sales;
}

std::string TimeSales::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("time", time)
        .field("timestamp", timestamp)
        .field("price", price)
        .field("open", open)
        .field("high", high)
        .field("low", low)
        .field("close", close)
        .field("volume", volume)
        .field("vwap", vwap)
        .end_object()
        .str();
}

} // namespace oqd