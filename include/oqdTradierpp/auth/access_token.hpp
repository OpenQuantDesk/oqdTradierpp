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

#include <string>
#include <simdjson.h>

namespace oqd {

struct AccessToken {
    std::string access_token;
    std::string token_type;
    std::string refresh_token;
    int expires_in = 0;
    std::string scope;
    
    static AccessToken from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

} // namespace oqd