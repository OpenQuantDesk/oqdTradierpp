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

struct MarketClock {
    std::string date;
    std::string description;
    std::string state;
    std::string timestamp;
    std::string next_state_change;
    std::string next_state;
    
    static MarketClock from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct MarketDay {
    std::string date;
    std::string status;
    std::string description;
    std::optional<std::string> premarket;
    std::optional<std::string> open;
    std::optional<std::string> close;
    std::optional<std::string> postmarket;
    
    static MarketDay from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

} // namespace oqd