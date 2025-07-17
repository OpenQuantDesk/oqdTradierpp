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

#include "oqdTradierpp/market/historical_data.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

HistoricalData HistoricalData::from_json(const simdjson::dom::element& elem) {
    HistoricalData data;
    data.date = std::string(elem["date"].get_string().value_unsafe());
    data.open = elem["open"].get_double().value_unsafe();
    data.high = elem["high"].get_double().value_unsafe();
    data.low = elem["low"].get_double().value_unsafe();
    data.close = elem["close"].get_double().value_unsafe();
    data.volume = elem["volume"].get_double().value_unsafe();
    return data;
}

std::string HistoricalData::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("date", date)
        .field("open", open)
        .field("high", high)
        .field("low", low)
        .field("close", close)
        .field("volume", volume)
        .end_object()
        .str();
}

} // namespace oqd