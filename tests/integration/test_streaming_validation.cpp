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

#include <gtest/gtest.h>
#include "oqdTradierpp/oqdTradierpp.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <unordered_set>
#include <mutex>

using namespace oqd;
using namespace std::chrono_literals;

class StreamingValidationTest : public ::testing::Test {
protected:
    std::shared_ptr<TradierClient> client;
    std::unique_ptr<ApiMethods> api;
    std::shared_ptr<StreamingSession> streaming;
    std::string production_token;
    std::string session_id;
    
    void SetUp() override {
        // Get credentials from environment variables
        const char* production_token_env = std::getenv("TRADIER_PRODUCTION_KEY");
        const char* sandbox_token_env = std::getenv("TRADIER_SANDBOX_KEY");
        
        // Prefer production for streaming tests, fallback to sandbox
        Environment env;
        std::string token;
        
        if (production_token_env) {
            env = Environment::Production;
            token = production_token_env;
            production_token = token;
        } else if (sandbox_token_env) {
            env = Environment::Sandbox;
            token = sandbox_token_env;
            production_token = token;
        } else {
            GTEST_SKIP() << "No API tokens found. Set TRADIER_PRODUCTION_KEY or TRADIER_SANDBOX_KEY";
        }
        
        // Create client
        client = create_client(env);
        client->set_access_token(token);
        api = create_api_methods(client);
        
        // Create streaming session - use client directly since create_streaming_session doesn't exist
        streaming = std::make_shared<StreamingSession>(client);
    }
    
    void TearDown() override {
        if (streaming) {
            streaming->stop_stream();
        }
    }
};

TEST_F(StreamingValidationTest, StreamingSessionCreation) {
    // Test that streaming session was created successfully
    EXPECT_TRUE(streaming != nullptr);
    EXPECT_EQ(streaming->get_connection_state(), ConnectionState::Disconnected);
}

TEST_F(StreamingValidationTest, WebSocketMarketStream) {
    std::atomic<int> data_count{0};
    std::mutex data_mutex;
    std::vector<std::string> received_symbols;
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        data_count++;
        std::lock_guard<std::mutex> lock(data_mutex);
        
        try {
            auto type = StreamingSession::determine_data_type_static(data);
            if (type == StreamingDataType::Quote) {
                StreamingQuote quote = StreamingQuote::from_json(data);
                received_symbols.push_back(quote.symbol);
            }
        } catch (const std::exception& e) {
            // Ignore parsing errors for this test
        }
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "Streaming error: " << error << std::endl;
    };
    
    // Test symbols
    std::vector<std::string> symbols = {"SPY", "QQQ", "AAPL"};
    
    // Start WebSocket stream
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    // Wait for data (market hours only)
    std::this_thread::sleep_for(5s);
    
    // Stop stream
    streaming->stop_stream();
    
    if (data_count > 0) {
        std::lock_guard<std::mutex> lock(data_mutex);
        std::cout << "Received " << data_count << " data messages" << std::endl;
        
        // Check we received data for multiple symbols
        std::unordered_set<std::string> unique_symbols(received_symbols.begin(), received_symbols.end());
        EXPECT_GT(unique_symbols.size(), 0);
    } else {
        std::cout << "No streaming data received - market may be closed or connection failed" << std::endl;
    }
}

TEST_F(StreamingValidationTest, HTTPMarketStream) {
    std::atomic<int> data_count{0};
    std::mutex data_mutex;
    std::vector<std::string> received_data_types;
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        data_count++;
        std::lock_guard<std::mutex> lock(data_mutex);
        
        try {
            auto type = StreamingSession::determine_data_type_static(data);
            switch (type) {
                case StreamingDataType::Quote:
                    received_data_types.push_back("quote");
                    break;
                case StreamingDataType::Trade:
                    received_data_types.push_back("trade");
                    break;
                case StreamingDataType::Summary:
                    received_data_types.push_back("summary");
                    break;
                default:
                    received_data_types.push_back("other");
                    break;
            }
        } catch (const std::exception& e) {
            received_data_types.push_back("parse_error");
        }
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "HTTP streaming error: " << error << std::endl;
    };
    
    // Test symbols
    std::vector<std::string> symbols = {"SPY", "AAPL"};
    
    // Start HTTP stream
    streaming->start_market_http_stream(symbols, data_callback, error_callback);
    
    // Wait for data
    std::this_thread::sleep_for(5s);
    
    // Stop stream
    streaming->stop_stream();
    
    if (data_count > 0) {
        std::lock_guard<std::mutex> lock(data_mutex);
        std::cout << "HTTP stream received " << data_count << " data messages" << std::endl;
        
        // Check data types received
        std::unordered_set<std::string> unique_types(received_data_types.begin(), received_data_types.end());
        EXPECT_GT(unique_types.size(), 0);
    } else {
        std::cout << "No HTTP streaming data received - may be expected in test environment" << std::endl;
    }
}

TEST_F(StreamingValidationTest, SubscribeToSummary) {
    std::atomic<int> summary_count{0};
    std::mutex summaries_mutex;
    std::vector<StreamingSummary> received_summaries;
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        auto type = StreamingSession::determine_data_type_static(data);
        if (type == StreamingDataType::Summary) {
            summary_count++;
            std::lock_guard<std::mutex> lock(summaries_mutex);
            try {
                StreamingSummary summary = StreamingSummary::from_json(data);
                received_summaries.push_back(summary);
            } catch (const std::exception& e) {
                // Ignore parsing errors for this test
            }
        }
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "Streaming error: " << error << std::endl;
    };
    
    std::vector<std::string> symbols = {"SPY", "AAPL", "GOOGL"};
    
    // Start streaming with summary data filter
    streaming->set_data_filter({StreamingDataType::Summary});
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    // Wait for summaries
    std::this_thread::sleep_for(5s);
    
    streaming->stop_stream();
    
    if (summary_count > 0) {
        std::lock_guard<std::mutex> lock(summaries_mutex);
        
        // Validate summary data
        for (const auto& summary : received_summaries) {
            EXPECT_FALSE(summary.symbol.empty());
            EXPECT_GT(summary.open, 0.0);
            EXPECT_GT(summary.high, 0.0);
            EXPECT_GT(summary.low, 0.0);
            EXPECT_GT(summary.close, 0.0);
            EXPECT_GE(summary.volume, 0);
        }
    }
}

TEST_F(StreamingValidationTest, UnsubscribeFromQuotes) {
    std::atomic<int> quote_count{0};
    std::atomic<bool> unsubscribed{false};
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        auto type = StreamingSession::determine_data_type_static(data);
        if (type == StreamingDataType::Quote && !unsubscribed) {
            try {
                StreamingQuote quote = StreamingQuote::from_json(data);
                if (quote.symbol == "AAPL") {
                    quote_count++;
                }
            } catch (const std::exception& e) {
                // Ignore parsing errors
            }
        }
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "Streaming error: " << error << std::endl;
    };
    
    // Set data filter for quotes only
    streaming->set_data_filter({StreamingDataType::Quote});
    
    // Start streaming with AAPL
    std::vector<std::string> symbols = {"AAPL"};
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    std::this_thread::sleep_for(2s);
    
    int initial_count = quote_count.load();
    
    // Remove AAPL symbol (equivalent to unsubscribe)
    streaming->remove_symbols({"AAPL"});
    unsubscribed = true;
    std::this_thread::sleep_for(2s);
    
    int final_count = quote_count.load();
    
    streaming->stop_stream();
    
    // Should not receive more quotes after removing symbol
    EXPECT_EQ(initial_count, final_count);
}

TEST_F(StreamingValidationTest, MultipleSubscriptions) {
    std::atomic<int> total_messages{0};
    std::unordered_set<std::string> symbols_with_data;
    std::mutex data_mutex;
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        auto type = StreamingSession::determine_data_type_static(data);
        
        if (type == StreamingDataType::Quote) {
            try {
                StreamingQuote quote = StreamingQuote::from_json(data);
                total_messages++;
                std::lock_guard<std::mutex> lock(data_mutex);
                symbols_with_data.insert(quote.symbol);
            } catch (const std::exception& e) {
                // Ignore parsing errors
            }
        } else if (type == StreamingDataType::Trade) {
            try {
                StreamingTrade trade = StreamingTrade::from_json(data);
                total_messages++;
                std::lock_guard<std::mutex> lock(data_mutex);
                symbols_with_data.insert(trade.symbol);
            } catch (const std::exception& e) {
                // Ignore parsing errors
            }
        }
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "Streaming error: " << error << std::endl;
    };
    
    // Set data filter for both quotes and trades
    streaming->set_data_filter({StreamingDataType::Quote, StreamingDataType::Trade});
    
    // Subscribe to multiple symbols
    std::vector<std::string> symbols = {"SPY", "QQQ", "IWM", "DIA", "AAPL", "MSFT"};
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    // Wait for data
    std::this_thread::sleep_for(10s);
    
    streaming->stop_stream();
    
    if (total_messages > 0) {
        std::lock_guard<std::mutex> lock(data_mutex);
        EXPECT_GT(symbols_with_data.size(), 2);
        std::cout << "Received data for " << symbols_with_data.size() 
                  << " symbols, total messages: " << total_messages << std::endl;
    }
}

TEST_F(StreamingValidationTest, ReconnectionHandling) {
    std::atomic<int> state_change_count{0};
    std::atomic<ConnectionState> last_state{ConnectionState::Disconnected};
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        // Just receive data to keep stream active
    };
    
    auto error_callback = [&](const std::string& error) {
        state_change_count++;
        std::cout << "Connection state change: " << error << std::endl;
    };
    
    // Start streaming session
    std::vector<std::string> symbols = {"SPY"};
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    // Wait for connection to establish
    std::this_thread::sleep_for(2s);
    
    EXPECT_EQ(streaming->get_connection_state(), ConnectionState::Connected);
    
    // Stop and restart to test reconnection behavior
    streaming->stop_stream();
    std::this_thread::sleep_for(500ms);
    
    EXPECT_EQ(streaming->get_connection_state(), ConnectionState::Disconnected);
    
    // Restart streaming
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    std::this_thread::sleep_for(2s);
    
    EXPECT_EQ(streaming->get_connection_state(), ConnectionState::Connected);
    
    streaming->stop_stream();
    
    // Should have experienced state changes
    EXPECT_GT(state_change_count.load(), 0);
}

TEST_F(StreamingValidationTest, ErrorHandling) {
    std::atomic<int> data_count{0};
    std::atomic<int> error_count{0};
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        data_count++;
    };
    
    auto error_callback = [&](const std::string& error) {
        error_count++;
        std::cout << "Expected error: " << error << std::endl;
    };
    
    // Test invalid symbols - should not crash but may not receive data
    std::vector<std::string> invalid_symbols = {"INVALID_SYMBOL_XYZ", "ANOTHER_INVALID_123"};
    streaming->start_market_websocket_stream(invalid_symbols, data_callback, error_callback);
    
    std::this_thread::sleep_for(2s);
    
    // Test should complete without crashes
    EXPECT_TRUE(streaming->get_connection_state() == ConnectionState::Connected || 
                streaming->get_connection_state() == ConnectionState::Error);
    
    streaming->stop_stream();
    
    // May have received errors, but should not have crashed
    std::cout << "Data count: " << data_count << ", Error count: " << error_count << std::endl;
}

TEST_F(StreamingValidationTest, HighFrequencyData) {
    std::atomic<int> message_count{0};
    
    auto start_time = std::chrono::steady_clock::now();
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        auto type = StreamingSession::determine_data_type_static(data);
        if (type == StreamingDataType::Quote || type == StreamingDataType::Trade) {
            message_count++;
        }
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "High frequency streaming error: " << error << std::endl;
    };
    
    // Set data filter for both quotes and trades
    streaming->set_data_filter({StreamingDataType::Quote, StreamingDataType::Trade});
    
    // Subscribe to the most liquid ETF
    std::vector<std::string> symbols = {"SPY"};
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    // Measure message rate for 30 seconds
    std::this_thread::sleep_for(30s);
    
    streaming->stop_stream();
    
    auto duration = std::chrono::steady_clock::now() - start_time;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    if (message_count > 0) {
        double rate = static_cast<double>(message_count) / seconds;
        std::cout << "Message rate: " << rate << " messages/second" << std::endl;
        std::cout << "Total messages: " << message_count << std::endl;
        
        // During market hours, SPY should have high message rate
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        bool is_market_hours = (tm.tm_hour >= 9 && tm.tm_hour < 16) && 
                               (tm.tm_wday >= 1 && tm.tm_wday <= 5);
        
        if (is_market_hours) {
            EXPECT_GT(rate, 1.0); // At least 1 message per second during market hours
        }
    }
}

TEST_F(StreamingValidationTest, MemoryStability) {
    std::atomic<int> data_count{0};
    
    auto data_callback = [&](const simdjson::dom::element& data) {
        data_count++;
    };
    
    auto error_callback = [](const std::string& error) {
        std::cout << "Memory stability test error: " << error << std::endl;
    };
    
    // Test for memory leaks with rapid symbol add/remove operations
    std::vector<std::string> symbols = {"AAPL"};
    streaming->start_market_websocket_stream(symbols, data_callback, error_callback);
    
    // Wait for initial connection
    std::this_thread::sleep_for(1s);
    
    for (int i = 0; i < 100; ++i) {
        streaming->add_symbols({"MSFT"});
        std::this_thread::sleep_for(10ms);
        streaming->remove_symbols({"MSFT"});
        std::this_thread::sleep_for(10ms);
    }
    
    streaming->stop_stream();
    
    // If we get here without crashes or exceptions, memory handling is stable
    EXPECT_TRUE(streaming->get_connection_state() == ConnectionState::Disconnected);
    std::cout << "Memory stability test completed with " << data_count << " messages processed" << std::endl;
}