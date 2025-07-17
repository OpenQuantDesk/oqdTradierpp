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
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include <chrono>

std::string get_env_var(const std::string& name, const std::string& default_value = "") {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : default_value;
}

int main() {
    try {
        oqd::initialize_library();

        // Get credentials from environment variables
        std::string sandbox_key = get_env_var("TRADIER_SANDBOX_KEY");
        std::string production_key = get_env_var("TRADIER_PRODUCTION_KEY");
        
        // Determine environment
        oqd::Environment env = oqd::Environment::Sandbox;
        std::string access_token = sandbox_key;
        
        if (sandbox_key.empty() && !production_key.empty()) {
            std::cout << "Using production environment for market data" << std::endl;
            env = oqd::Environment::Production;
            access_token = production_key;
        } else if (!sandbox_key.empty()) {
            std::cout << "Using sandbox environment for market data" << std::endl;
        } else {
            std::cerr << "Error: No API keys found!" << std::endl;
            std::cerr << "Please set TRADIER_SANDBOX_KEY or TRADIER_PRODUCTION_KEY" << std::endl;
            return 1;
        }

        // Create client and API methods
        auto client = oqd::create_client(env);
        client->set_access_token(access_token);
        auto api = oqd::create_api_methods(client);

        std::cout << "=== Tradier Market Data Example ===" << std::endl;
        std::cout << "Environment: " << (env == oqd::Environment::Production ? "Production" : "Sandbox") << std::endl;
        std::cout << "Base URL: " << client->get_base_url() << std::endl;

        // Example 1: Get market status
        std::cout << "\n=== Market Clock & Status ===" << std::endl;
        try {
            auto clock = api->get_market_clock();
            std::cout << "Market Date: " << clock.date << std::endl;
            std::cout << "Market State: " << clock.state << std::endl;
            std::cout << "Description: " << clock.description << std::endl;
            std::cout << "Timestamp: " << clock.timestamp << std::endl;
            if (!clock.next_state_change.empty()) {
                std::cout << "Next State Change: " << clock.next_state_change << std::endl;
                std::cout << "Next State: " << clock.next_state << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting market clock: " << e.what() << std::endl;
        }

        // Example 2: Get quotes for popular stocks
        std::cout << "\n=== Popular Stock Quotes ===" << std::endl;
        try {
            std::vector<std::string> popular_stocks = {
                "AAPL", "MSFT", "GOOGL", "AMZN", "TSLA", 
                "NVDA", "META", "SPY", "QQQ", "IWM"
            };
            
            std::cout << "Fetching real-time quotes for " << popular_stocks.size() << " symbols..." << std::endl;
            auto quotes = api->get_quotes(popular_stocks);
            
            std::cout << std::left << std::setw(8) << "Symbol" 
                      << std::right << std::setw(10) << "Last" 
                      << std::setw(10) << "Change" 
                      << std::setw(8) << "Change%" 
                      << std::setw(12) << "Volume" 
                      << std::setw(10) << "Bid"
                      << std::setw(10) << "Ask" << std::endl;
            std::cout << std::string(68, '-') << std::endl;
            
            for (const auto& quote : quotes) {
                std::cout << std::left << std::setw(8) << quote.symbol
                          << std::right << std::fixed << std::setprecision(2)
                          << std::setw(10) << quote.last
                          << std::setw(10) << quote.change
                          << std::setw(7) << quote.change_percentage << "%"
                          << std::setw(12) << std::setprecision(0) << quote.volume
                          << std::setw(10) << std::setprecision(2) << quote.bid
                          << std::setw(10) << quote.ask << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting quotes: " << e.what() << std::endl;
        }

        // Example 3: Get detailed option chain data
        std::cout << "\n=== AAPL Option Chain Analysis ===" << std::endl;
        try {
            // First get option expirations
            auto expirations = api->get_option_expirations("AAPL");
            std::cout << "AAPL has " << expirations.size() << " expiration dates available" << std::endl;
            
            if (!expirations.empty()) {
                // Use the first expiration (nearest)
                std::string exp_date = expirations[0];
                std::cout << "Analyzing options for expiration: " << exp_date << std::endl;
                
                // Get option chain with Greeks
                auto chain = api->get_option_chain("AAPL", exp_date, true);
                std::cout << "Found " << chain.options.size() << " options for this expiration" << std::endl;
                
                // Separate calls and puts
                std::vector<oqd::Quote> calls, puts;
                for (const auto& option : chain.options) {
                    if (option.option_type.has_value()) {
                        if (option.option_type.value() == "call") {
                            calls.push_back(option);
                        } else if (option.option_type.value() == "put") {
                            puts.push_back(option);
                        }
                    }
                }
                
                std::cout << "Calls: " << calls.size() << ", Puts: " << puts.size() << std::endl;
                
                // Show some in-the-money and out-of-the-money options
                std::cout << "\nTop 5 Call Options (by volume):" << std::endl;
                std::sort(calls.begin(), calls.end(), [](const auto& a, const auto& b) {
                    return a.volume > b.volume;
                });
                
                std::cout << std::left << std::setw(20) << "Strike" 
                          << std::setw(10) << "Last" 
                          << std::setw(12) << "Volume"
                          << std::setw(10) << "Delta"
                          << std::setw(10) << "IV" << std::endl;
                std::cout << std::string(62, '-') << std::endl;
                
                for (size_t i = 0; i < std::min(calls.size(), size_t(5)); ++i) {
                    const auto& call = calls[i];
                    std::cout << std::left << std::setw(20);
                    if (call.strike.has_value()) {
                        std::cout << ("$" + std::to_string(call.strike.value()));
                    } else {
                        std::cout << "N/A";
                    }
                    std::cout << std::right << std::fixed << std::setprecision(2)
                              << std::setw(10) << call.last
                              << std::setw(12) << std::setprecision(0) << call.volume;
                    
                    if (call.delta.has_value()) {
                        std::cout << std::setw(10) << std::setprecision(3) << call.delta.value();
                    } else {
                        std::cout << std::setw(10) << "N/A";
                    }
                    
                    if (call.mid_iv.has_value()) {
                        std::cout << std::setw(9) << std::setprecision(1) << (call.mid_iv.value() * 100) << "%";
                    } else {
                        std::cout << std::setw(10) << "N/A";
                    }
                    std::cout << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting option data: " << e.what() << std::endl;
        }

        // Example 4: Historical data analysis
        std::cout << "\n=== Historical Data Analysis ===" << std::endl;
        try {
            std::cout << "Fetching 30 days of SPY historical data..." << std::endl;
            auto historical = api->get_historical_data("SPY", "daily");
            
            if (!historical.empty()) {
                std::cout << "Received " << historical.size() << " days of data" << std::endl;
                
                // Calculate some basic statistics
                double max_high = 0, min_low = 999999;
                double total_volume = 0;
                size_t count = std::min(historical.size(), size_t(30)); // Last 30 days
                
                for (size_t i = 0; i < count; ++i) {
                    const auto& day = historical[i];
                    max_high = std::max(max_high, day.high);
                    min_low = std::min(min_low, day.low);
                    total_volume += day.volume;
                }
                
                double avg_volume = total_volume / count;
                
                std::cout << "30-Day Statistics for SPY:" << std::endl;
                std::cout << "  High: $" << std::fixed << std::setprecision(2) << max_high << std::endl;
                std::cout << "  Low: $" << min_low << std::endl;
                std::cout << "  Average Volume: " << std::setprecision(0) << avg_volume << std::endl;
                
                // Show last few days
                std::cout << "\nLast 5 trading days:" << std::endl;
                std::cout << std::left << std::setw(12) << "Date"
                          << std::right << std::setw(8) << "Open"
                          << std::setw(8) << "High" 
                          << std::setw(8) << "Low"
                          << std::setw(8) << "Close"
                          << std::setw(12) << "Volume" << std::endl;
                std::cout << std::string(56, '-') << std::endl;
                
                for (size_t i = 0; i < std::min(historical.size(), size_t(5)); ++i) {
                    const auto& day = historical[i];
                    std::cout << std::left << std::setw(12) << day.date
                              << std::right << std::fixed << std::setprecision(2)
                              << std::setw(8) << day.open
                              << std::setw(8) << day.high
                              << std::setw(8) << day.low
                              << std::setw(8) << day.close
                              << std::setw(12) << std::setprecision(0) << day.volume << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting historical data: " << e.what() << std::endl;
        }

        // Example 5: Company search and lookup
        std::cout << "\n=== Company Search ===" << std::endl;
        try {
            std::cout << "Searching for companies containing 'tech'..." << std::endl;
            auto search_results = api->search_companies("tech");
            
            std::cout << "Found " << search_results.size() << " results:" << std::endl;
            for (size_t i = 0; i < std::min(search_results.size(), size_t(10)); ++i) {
                const auto& result = search_results[i];
                std::cout << "  " << result.symbol << " - " << result.company 
                         << " (" << result.exchange << ")" << std::endl;
            }
            if (search_results.size() > 10) {
                std::cout << "  ... and " << (search_results.size() - 10) << " more results" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error searching companies: " << e.what() << std::endl;
        }

        // Example 6: Symbol lookup
        std::cout << "\n=== Symbol Lookup ===" << std::endl;
        try {
            std::cout << "Looking up symbols starting with 'APPL'..." << std::endl;
            auto lookup_results = api->lookup_symbols("APPL");
            
            for (const auto& result : lookup_results) {
                std::cout << "  " << result.symbol << " - " << result.company 
                         << " (" << result.type << ")" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error looking up symbols: " << e.what() << std::endl;
        }

        // Example 7: Async operations for better performance
        std::cout << "\n=== Async Performance Test ===" << std::endl;
        try {
            std::cout << "Launching multiple async requests..." << std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            
            // Launch multiple requests concurrently
            auto clock_future = api->get_market_clock_async();
            auto quotes_future = api->get_quotes_async({"AAPL", "MSFT", "GOOGL"});
            auto search_future = api->search_companies_async("bank");
            auto historical_future = api->get_historical_data_async("QQQ", "daily");
            
            // Wait for all results
            auto clock_result = clock_future.get();
            auto quotes_result = quotes_future.get();
            auto search_result = search_future.get();
            auto historical_result = historical_future.get();
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "Completed 4 async requests in " << duration.count() << "ms" << std::endl;
            std::cout << "Results: Market=" << clock_result.state 
                      << ", Quotes=" << quotes_result.size()
                      << ", Search=" << search_result.size()
                      << ", Historical=" << historical_result.size() << " days" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error with async operations: " << e.what() << std::endl;
        }

        // Example 8: Rate limit monitoring
        std::cout << "\n=== Rate Limit Status ===" << std::endl;
        auto market_rate_limit = client->get_rate_limit("market_data_endpoints");
        auto account_rate_limit = client->get_rate_limit("account_endpoints");
        
        if (market_rate_limit.has_value()) {
            std::cout << "Market Data Rate Limit:" << std::endl;
            std::cout << "  Available: " << market_rate_limit->available << std::endl;
            std::cout << "  Used: " << market_rate_limit->used << std::endl;
        } else {
            std::cout << "Market Data Rate Limit: No information available" << std::endl;
        }
        
        if (account_rate_limit.has_value()) {
            std::cout << "Account Rate Limit:" << std::endl;
            std::cout << "  Available: " << account_rate_limit->available << std::endl;
            std::cout << "  Used: " << account_rate_limit->used << std::endl;
        } else {
            std::cout << "Account Rate Limit: No information available" << std::endl;
        }

        oqd::cleanup_library();
        std::cout << "\nMarket data example completed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/*
Market Data Example Usage:

1. Set environment variables:
   export TRADIER_SANDBOX_KEY="your_sandbox_access_token"
   # or
   export TRADIER_PRODUCTION_KEY="your_production_access_token"

2. Build and run:
   cd build
   make
   ./examples/market_data_example

Features demonstrated:
- Real-time market clock and status
- Multi-symbol quote requests with formatting
- Comprehensive option chain analysis with Greeks
- Historical data analysis and statistics
- Company search and symbol lookup
- Async request performance testing
- Rate limit monitoring

This example is safe to run as it only reads market data and doesn't place any trades.
*/
