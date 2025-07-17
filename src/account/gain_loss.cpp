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

#include "oqdTradierpp/account/gain_loss.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

GainLoss GainLoss::from_json(const simdjson::dom::element& elem) {
    GainLoss gainloss;
    auto gainloss_elem = elem["gainloss"];
    if (gainloss_elem.is_object()) {
        auto closed_position_result = gainloss_elem["closed_position"];
        if (closed_position_result.error() == simdjson::SUCCESS) {
            auto position_array = closed_position_result.value();
            if (position_array.is_array()) {
                for (const auto& position : position_array.get_array()) {
                    gainloss.gainloss.push_back(GainLossItem::from_json(position));
                }
            } else {
                gainloss.gainloss.push_back(GainLossItem::from_json(position_array));
            }
        }
    }
    return gainloss;
}

std::string GainLoss::to_json() const {
    return json::create_object()
        .array_field("gainloss", gainloss)
        .end_object()
        .str();
}

}