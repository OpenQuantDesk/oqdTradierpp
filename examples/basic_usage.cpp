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

#include "oqdTradierpp/oqdTradierpp.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>

std::string get_env_var(const std::string& name, const std::string& default_value = "") {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : default_value;
}

int main() {
    try {
        // Initialize the library
        oqd::initialize_library();

        // Get credentials from environment variables
        std::string sandbox_key = get_env_var("TRADIER_SANDBOX_KEY");
        std::string production_key = get_env_var("TRADIER_PRODUCTION_KEY");
        std::string sandbox_account = get_env_var("TRADIER_SANDBOX_ACCT");
        
        // Determine which environment to use
        oqd::Environment env = oqd::Environment::Sandbox;
        std::string access_token = sandbox_key;
        
        if (sandbox_key.empty() && !production_key.empty()) {
            std::cout << "No sandbox key found, using production environment" << std::endl;
            env = oqd::Environment::Production;
            access_token = production_key;
        } else if (!sandbox_key.empty()) {
            std::cout << "Using sandbox environment" << std::endl;
        } else {
            std::cerr << "Error: No API keys found in environment variables!" << std::endl;
            std::cerr << "Please set TRADIER_SANDBOX_KEY or TRADIER_PRODUCTION_KEY" << std::endl;
            return 1;
        }

        // Create client with determined environment
        auto client = oqd::create_client(env);
        client->set_access_token(access_token);

        // Create API methods wrapper
        auto api = oqd::create_api_methods(client);

        std::cout << "Tradier C++ Library Basic Usage Example" << std::endl;
        std::cout << "Library Version: " << oqd::get_version() << std::endl;
        std::cout << "Environment: " << (env == oqd::Environment::Production ? "Production" : "Sandbox") << std::endl;
        std::cout << "Base URL: " << client->get_base_url() << std::endl;

        // Example 1: Get user profile
        std::cout << "\n=== Getting User Profile ===" << std::endl;
        try {
            auto profile = api->get_user_profile();
            std::cout << "User ID: " << profile.id << std::endl;
            std::cout << "Name: " << profile.name << std::endl;
            std::cout << "Accounts: ";
            for (const auto& account : profile.account) {
                std::cout << account << " ";
            }
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error getting profile: " << e.what() << std::endl;
        }

        // Example 2: Get market clock
        std::cout << "\n=== Getting Market Clock ===" << std::endl;
        try {
            auto clock = api->get_market_clock();
            std::cout << "Date: " << clock.date << std::endl;
            std::cout << "Status: " << clock.state << std::endl;
            std::cout << "Description: " << clock.description << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error getting market clock: " << e.what() << std::endl;
        }

        // Example 3: Get quotes for multiple symbols (with real market data)
        std::cout << "\n=== Getting Real-Time Stock Quotes ===" << std::endl;
        try {
            std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT", "TSLA", "SPY", "QQQ"};
            std::cout << "Requesting quotes for: ";
            for (const auto& symbol : symbols) {
                std::cout << symbol << " ";
            }
            std::cout << std::endl;
            
            auto quotes = api->get_quotes(symbols);
            
            std::cout << "Received " << quotes.size() << " quotes:" << std::endl;
            for (const auto& quote : quotes) {
                std::cout << "\n" << quote.symbol << " (" << quote.description << ")" << std::endl;
                std::cout << "  Last: $" << std::fixed << std::setprecision(2) << quote.last;
                if (quote.change >= 0) {
                    std::cout << " +" << quote.change << " (+" << quote.change_percentage << "%)";
                } else {
                    std::cout << " " << quote.change << " (" << quote.change_percentage << "%)";
                }
                std::cout << std::endl;
                std::cout << "  Volume: " << std::fixed << std::setprecision(0) << quote.volume << std::endl;
                std::cout << "  Bid/Ask: $" << std::setprecision(2) << quote.bid << " / $" << quote.ask << std::endl;
                std::cout << "  Day Range: $" << quote.low << " - $" << quote.high << std::endl;
                std::cout << "  Previous Close: $" << quote.prevclose << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting quotes: " << e.what() << std::endl;
        }

        // Example 4: Search for companies
        std::cout << "\n=== Searching for Companies ===" << std::endl;
        try {
            auto results = api->search_companies("apple");
            std::cout << "Search results for 'apple':" << std::endl;
            for (const auto& result : results) {
                std::cout << "  " << result.symbol << " - " << result.company 
                         << " (" << result.exchange << ")" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error searching companies: " << e.what() << std::endl;
        }

        // Example 5: Get option expirations and chain data
        std::cout << "\n=== Getting Option Data ===" << std::endl;
        try {
            std::cout << "Getting AAPL option expirations..." << std::endl;
            auto expirations = api->get_option_expirations("AAPL");
            std::cout << "Found " << expirations.size() << " expiration dates:" << std::endl;
            for (size_t i = 0; i < std::min(expirations.size(), size_t(10)); ++i) {
                std::cout << "  " << expirations[i] << std::endl;
            }
            if (expirations.size() > 10) {
                std::cout << "  ... and " << (expirations.size() - 10) << " more" << std::endl;
            }
            
            // Get option chain for nearest expiration
            if (!expirations.empty()) {
                std::cout << "\nGetting option chain for nearest expiration: " << expirations[0] << std::endl;
                auto chain = api->get_option_chain("AAPL", expirations[0], true); // Include Greeks
                std::cout << "Found " << chain.options.size() << " options" << std::endl;
                
                // Show a few options with Greeks
                int count = 0;
                for (const auto& option : chain.options) {
                    if (count >= 5) break; // Show first 5
                    std::cout << "\n  " << option.symbol << std::endl;
                    if (option.strike.has_value()) {
                        std::cout << "    Strike: $" << option.strike.value() << std::endl;
                    }
                    std::cout << "    Last: $" << option.last << " Bid/Ask: $" << option.bid << "/$" << option.ask << std::endl;
                    if (option.delta.has_value() && option.gamma.has_value()) {
                        std::cout << "    Greeks - Delta: " << option.delta.value() << ", Gamma: " << option.gamma.value() << std::endl;
                    }
                    count++;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting option data: " << e.what() << std::endl;
        }

        // Example 6: Demonstrate asynchronous API calls
        std::cout << "\n=== Asynchronous API Calls ===" << std::endl;
        try {
            std::cout << "Starting asynchronous requests..." << std::endl;
            
            auto clock_future = api->get_market_clock_async();
            auto quotes_future = api->get_quotes_async({"SPY", "QQQ"});
            
            std::cout << "Waiting for results..." << std::endl;
            
            auto clock_result = clock_future.get();
            auto quotes_result = quotes_future.get();
            
            std::cout << "Market state: " << clock_result.state << std::endl;
            std::cout << "Got " << quotes_result.size() << " quotes" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error with async calls: " << e.what() << std::endl;
        }

        // Example 7: Rate limit checking
        std::cout << "\n=== Rate Limit Information ===" << std::endl;
        auto rate_limit = client->get_rate_limit("market_data_endpoints");
        if (rate_limit.has_value()) {
            std::cout << "Available requests: " << rate_limit->available << std::endl;
            std::cout << "Used requests: " << rate_limit->used << std::endl;
        } else {
            std::cout << "No rate limit information available" << std::endl;
        }

        // Cleanup
        oqd::cleanup_library();
        
        std::cout << "\nExample completed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/*
To run this example:

1. Build the library:
   mkdir build && cd build
   cmake ..
   make

2. Set your Tradier API credentials:
   - Get API credentials from https://tradier.com/
   - Replace "YOUR_ACCESS_TOKEN_HERE" with your actual token

3. Run the example:
   ./examples/basic_usage

Note: This example uses sandbox mode by default. To use production:
   auto client = oqd::create_client(oqd::Environment::Production);

Important: Never hardcode credentials in production code. Use environment
variables or secure configuration files instead.
*/
