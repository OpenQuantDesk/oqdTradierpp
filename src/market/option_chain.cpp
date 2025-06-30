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

#include "oqdTradierpp/market/option_chain.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

OptionChain OptionChain::from_json(const simdjson::dom::element& elem) {
    OptionChain chain;
    chain.underlying = std::string(elem["underlying"].get_string().value_unsafe());
    
    auto options_elem = elem["options"];
    if (options_elem.is_object()) {
        auto option_result = options_elem["option"];
        if (option_result.error() == simdjson::SUCCESS) {
            auto option_array = option_result.value();
            if (option_array.is_array()) {
                for (const auto& option : option_array.get_array()) {
                    chain.options.push_back(Quote::from_json(option));
                }
            } else {
                chain.options.push_back(Quote::from_json(option_array));
            }
        }
    }
    
    return chain;
}

std::string OptionChain::to_json() const {
    return json::create_object()
        .field("underlying", underlying)
        .array_field("options", options)
        .end_object()
        .str();
}

} // namespace oqd