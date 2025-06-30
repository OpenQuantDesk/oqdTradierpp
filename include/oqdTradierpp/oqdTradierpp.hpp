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

#include "client.hpp"
#include "api.hpp"
#include "types.hpp"
#include "streaming.hpp"

namespace oqd {

constexpr const char* VERSION = "2.0.0";
constexpr int VERSION_MAJOR = 2;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

std::shared_ptr<TradierClient> create_client(Environment env = Environment::Production);
std::unique_ptr<ApiMethods> create_api_methods(std::shared_ptr<TradierClient> client);

std::string get_version();
void initialize_library();
void cleanup_library();

}