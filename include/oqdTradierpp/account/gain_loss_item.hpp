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
#include <simdjson.h>

namespace oqd {

struct GainLossItem {
    std::string close_date;  // Date in YYYY-MM-DD format
    double cost;
    double gain_loss;
    double gain_loss_percent;
    std::string open_date;   // Date in YYYY-MM-DD format
    double proceeds;
    double quantity;
    std::string symbol;
    int term;                // Number of days
    
    static GainLossItem from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

}