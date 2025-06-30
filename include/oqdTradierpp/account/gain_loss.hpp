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

#include <vector>
#include <simdjson.h>
#include "gain_loss_item.hpp"

namespace oqd {

struct GainLoss {
    std::vector<GainLossItem> gainloss;
    
    static GainLoss from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

}