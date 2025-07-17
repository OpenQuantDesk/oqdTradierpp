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

#pragma once

#include <simdjson.h>
#include <string>

namespace oqd {

struct AccountBalances {
    double account_number = 0.0;
    double total_equity = 0.0;
    double long_market_value = 0.0;
    double short_market_value = 0.0;
    double account_value = 0.0;
    double close_pl = 0.0;
    double current_requirement = 0.0;
    double equity = 0.0;
    double long_liquid_value = 0.0;
    double long_market_value_bp = 0.0;
    double short_liquid_value = 0.0;
    double short_market_value_bp = 0.0;
    double uncleared_funds = 0.0;
    double pending_orders_count = 0.0;
    double option_short_value = 0.0;
    double total_cash = 0.0;
    double unsettled_funds = 0.0;
    double dividend = 0.0;
    double cash = 0.0;
    double market_value = 0.0;
    
    static AccountBalances from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

}