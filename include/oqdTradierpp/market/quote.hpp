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

#pragma once

#include <string>
#include <optional>
#include <simdjson.h>

namespace oqd {

struct Quote {
    std::string symbol;
    std::string description;
    std::string exch;
    std::string type;
    double last;
    double change;
    double change_percentage;
    double volume;
    double average_volume;
    double last_volume;
    std::string trade_date;
    double open;
    double high;
    double low;
    double close;
    double prevclose;
    std::string week_52_high;
    std::string week_52_low;
    double bid;
    double bidsize;
    std::string bidexch;
    std::string bid_date;
    double ask;
    double asksize;
    std::string askexch;
    std::string ask_date;
    
    // Options-specific fields
    std::optional<double> strike;
    std::optional<std::string> contract_size;
    std::optional<std::string> expiration_date;
    std::optional<std::string> expiration_type;
    std::optional<std::string> option_type;
    std::optional<std::string> root_symbol;
    
    // Greeks (if requested)
    std::optional<double> delta;
    std::optional<double> gamma;
    std::optional<double> theta;
    std::optional<double> vega;
    std::optional<double> rho;
    std::optional<double> phi;
    std::optional<double> bid_iv;
    std::optional<double> mid_iv;
    std::optional<double> ask_iv;
    std::optional<double> smv_vol;
    std::optional<double> updated_at;
    std::optional<double> open_interest;
    
    static Quote from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

} // namespace oqd