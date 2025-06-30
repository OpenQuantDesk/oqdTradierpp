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

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>
#include "oqdTradierpp/oqdTradierpp.hpp"

using namespace oqd;
using namespace std::chrono_literals;

std::atomic<bool> running{true};

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nShutting down..." << std::endl;
        running = false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <production_token>" << std::endl;
        std::cerr << "Note: This requires a production API token with market data access" << std::endl;
        return 1;
    }
    
    std::string token = argv[1];
    
    // Set up signal handler for graceful shutdown
    std::signal(SIGINT, signal_handler);
    
    try {
        // Initialize library
        initialize_library();
        
        // Create client for production environment
        auto client = create_client(Environment::Production);
        client->set_access_token(token);
        
        // Create API methods instance
        auto api = create_api_methods(client);
        
        // Check market status
        auto market_clock = api->get_market_clock();
        std::cout << "Market State: " << market_clock.state << std::endl;
        std::cout << "Description: " << market_clock.description << std::endl;
        
        // Create streaming session
        std::cout << "\nCreating streaming session..." << std::endl;
        auto streaming = std::make_shared<StreamingSession>(client);
        
        // Set up event handlers
        std::cout << std::fixed << std::setprecision(2);
        
        // Set up data callback
        auto data_callback = [](const simdjson::dom::element& data) {
            try {
                auto type = StreamingSession::determine_data_type_static(data);
                
                if (type == StreamingDataType::Quote) {
                    Quote quote = Quote::from_json(data);
                    std::cout << "[QUOTE] " << quote.symbol 
                              << " Bid: $" << quote.bid << " x " << quote.bidsize
                              << " Ask: $" << quote.ask << " x " << quote.asksize
                              << " Last: $" << quote.last
                              << " Vol: " << quote.volume << std::endl;
                } else if (type == StreamingDataType::Trade) {
                    // Parse trade data manually as we don't have a Trade struct
                    std::string symbol = std::string(data["symbol"].get_string().value_unsafe());
                    double price = data["price"].get_double().value_unsafe();
                    double size = data["size"].get_double().value_unsafe();
                    std::string exchange = data["exch"].is_null() ? "" : std::string(data["exch"].get_string().value_unsafe());
                    
                    std::cout << "[TRADE] " << symbol 
                              << " Price: $" << price 
                              << " Size: " << size
                              << " Exch: " << exchange << std::endl;
                } else if (type == StreamingDataType::Summary) {
                    // Parse summary data manually
                    std::string symbol = std::string(data["symbol"].get_string().value_unsafe());
                    double open = data["open"].get_double().value_unsafe();
                    double high = data["high"].get_double().value_unsafe();
                    double low = data["low"].get_double().value_unsafe();
                    double close = data["close"].get_double().value_unsafe();
                    double volume = data["volume"].get_double().value_unsafe();
                    
                    std::cout << "[SUMMARY] " << symbol
                              << " Open: $" << open
                              << " High: $" << high
                              << " Low: $" << low
                              << " Close: $" << close
                              << " Volume: " << volume << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "[PARSE ERROR] " << e.what() << std::endl;
            }
        };
        
        // Error handler
        auto error_callback = [](const std::string& error) {
            std::cerr << "[ERROR] " << error << std::endl;
        };
        
        // Subscribe to symbols
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOGL", "AMZN", "TSLA"};
        std::cout << "\nSubscribing to: ";
        for (const auto& symbol : symbols) {
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
        
        // Start the WebSocket stream
        std::cout << "\nStarting WebSocket stream..." << std::endl;
        streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
        
        std::cout << "Streaming active. Press Ctrl+C to stop." << std::endl;
        
        // Keep the main thread alive
        while (running && streaming->is_streaming()) {
            std::this_thread::sleep_for(100ms);
        }
        
        // Clean shutdown
        std::cout << "Stopping stream..." << std::endl;
        streaming->stop_stream();
        
        std::cout << "Stream stopped successfully." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}