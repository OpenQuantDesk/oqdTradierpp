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

#include "oqdTradierpp/auth/access_token.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

AccessToken AccessToken::from_json(const simdjson::dom::element& elem) {
    AccessToken token;
    token.access_token = std::string(elem["access_token"].get_string().value());
    token.token_type = std::string(elem["token_type"].get_string().value());
    token.refresh_token = std::string(elem["refresh_token"].get_string().value());
    token.expires_in = elem["expires_in"].get_int64().value();
    auto scope_result = elem["scope"].get_string();
    token.scope = scope_result.error() == simdjson::SUCCESS ? std::string(scope_result.value()) : "";
    return token;
}

std::string AccessToken::to_json() const {
    return json::create_object()
        .field("access_token", access_token)
        .field("token_type", token_type)
        .field("refresh_token", refresh_token)
        .field("expires_in", expires_in)
        .field("scope", scope)
        .end_object()
        .str();
}

} // namespace oqd