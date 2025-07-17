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

#include "oqdTradierpp/account/user_profile.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

UserProfile UserProfile::from_json(const simdjson::dom::element& elem) {
    UserProfile profile;
    auto profile_elem = elem["profile"];
    profile.id = std::string(profile_elem["id"].get_string().value());
    profile.name = std::string(profile_elem["name"].get_string().value());
    
    auto accounts = profile_elem["account"];
    if (accounts.is_array()) {
        for (const auto& account : accounts.get_array()) {
            profile.account.push_back(std::string(account.get_string().value()));
        }
    }
    return profile;
}

std::string UserProfile::to_json() const {
    return json::create_object()
        .field("id", id)
        .field("name", name)
        .array_field("account", account)
        .end_object()
        .str();
}

}