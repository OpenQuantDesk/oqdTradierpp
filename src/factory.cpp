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

#include "oqdTradierpp/oqdTradierpp.hpp"
#include <iostream>

namespace oqd {

std::shared_ptr<TradierClient> create_client(Environment env) {
    return std::make_shared<TradierClient>(env);
}

std::unique_ptr<ApiMethods> create_api_methods(std::shared_ptr<TradierClient> client) {
    return std::make_unique<ApiMethods>(std::move(client));
}

std::string get_version() {
    return std::string(VERSION);
}

void initialize_library() {
    std::cout << "Tradier C++ Library v" << VERSION << " initialized" << std::endl;
}

void cleanup_library() {
    std::cout << "Tradier C++ Library cleanup complete" << std::endl;
}

} // namespace oqd