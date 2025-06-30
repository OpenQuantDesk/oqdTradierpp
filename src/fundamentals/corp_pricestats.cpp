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

#include "oqdTradierpp/fundamentals/corp_pricestats.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

PriceStatistics PriceStatistics::from_json(const simdjson::dom::element& elem) {
    PriceStatistics stats;
    stats.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    stats.week_52_high = elem["week_52_high"].get_double().value_unsafe();
    stats.week_52_low = elem["week_52_low"].get_double().value_unsafe();
    stats.moving_avg_50 = elem["moving_avg_50"].get_double().value_unsafe();
    stats.moving_avg_200 = elem["moving_avg_200"].get_double().value_unsafe();
    stats.beta = elem["beta"].get_double().value_unsafe();
    stats.volatility = elem["volatility"].get_double().value_unsafe();
    return stats;
}

std::string PriceStatistics::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("week_52_high", week_52_high)
        .field("week_52_low", week_52_low)
        .field("moving_avg_50", moving_avg_50)
        .field("moving_avg_200", moving_avg_200)
        .field("beta", beta)
        .field("volatility", volatility)
        .end_object()
        .str();
}

} // namespace oqd