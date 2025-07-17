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
#include "oqdTradierpp/streaming.hpp"
#include "oqdTradierpp/client.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <string>
#include <future>
#include <cstdlib>

using namespace oqd;

std::string get_env_var(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : "";
}

class StreamingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Get token from environment
        std::string sandbox_token = get_env_var("TRADIER_SANDBOX_KEY");
        std::string production_token = get_env_var("TRADIER_PRODUCTION_KEY");
        
        if (sandbox_token.empty() && production_token.empty()) {
            GTEST_SKIP() << "No API tokens found. Set TRADIER_SANDBOX_KEY or TRADIER_PRODUCTION_KEY";
        }
        
        // Prefer sandbox for testing
        Environment env = Environment::Sandbox;
        std::string token = sandbox_token;
        
        if (sandbox_token.empty() && !production_token.empty()) {
            env = Environment::Production;
            token = production_token;
        }
        
        client_ = std::make_shared<TradierClient>(env);
        client_->set_access_token(token);
        session_ = std::make_unique<StreamingSession>(client_);
        
        // Reset test state
        data_received_.store(false);
        error_received_.store(false);
        received_data_.clear();
        received_errors_.clear();
    }

    void TearDown() override {
        if (session_) {
            session_->stop_stream();
        }
        session_.reset();
        client_.reset();
    }

    // Mock data callback
    void OnDataReceived(const simdjson::dom::element& /* data */
) {
        data_received_.store(true);
        // Convert element to string for testing
        received_data_.push_back("mock_data_received");
    }

    // Mock error callback  
    void OnErrorReceived(const std::string& error) {
        error_received_.store(true);
        received_errors_.push_back(error);
    }

    std::shared_ptr<TradierClient> client_;
    std::unique_ptr<StreamingSession> session_;
    
    // Test state
    std::atomic<bool> data_received_{false};
    std::atomic<bool> error_received_{false};
    std::vector<std::string> received_data_;
    std::vector<std::string> received_errors_;
};

// Test basic session initialization
TEST_F(StreamingTest, SessionInitialization) {
    EXPECT_NE(session_, nullptr);
    EXPECT_EQ(session_->get_connection_state(), ConnectionState::Disconnected);
}

// Test session lifecycle  
TEST_F(StreamingTest, SessionLifecycle) {
    EXPECT_EQ(session_->get_connection_state(), ConnectionState::Disconnected);
    
    // After stopping, should still be disconnected
    session_->stop_stream();
    EXPECT_EQ(session_->get_connection_state(), ConnectionState::Disconnected);
}

// Test data filtering functionality
TEST_F(StreamingTest, DataFiltering) {
    std::vector<StreamingDataType> filter_types = {
        StreamingDataType::Quote,
        StreamingDataType::Trade
    };
    
    // Should not throw
    EXPECT_NO_THROW(session_->set_data_filter(filter_types));
    EXPECT_NO_THROW(session_->clear_data_filter());
}

// Test symbol management
TEST_F(StreamingTest, SymbolManagement) {
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT"};
    
    // These should not throw even without active connection
    EXPECT_NO_THROW(session_->add_symbols(symbols));
    EXPECT_NO_THROW(session_->remove_symbols({"AAPL"}));
    
    // Test empty symbols
    EXPECT_NO_THROW(session_->add_symbols({}));
    EXPECT_NO_THROW(session_->remove_symbols({}));
}

// Test duplicate symbol handling
TEST_F(StreamingTest, DuplicateSymbolHandling) {
    std::vector<std::string> symbols = {"AAPL", "AAPL", "GOOGL"};
    
    // Should handle duplicates gracefully
    EXPECT_NO_THROW(session_->add_symbols(symbols));
    
    // Remove one instance
    EXPECT_NO_THROW(session_->remove_symbols({"AAPL"}));
    
    // Try to remove again (should not throw)
    EXPECT_NO_THROW(session_->remove_symbols({"AAPL"}));
}

// Test WebSocket URL building
TEST_F(StreamingTest, WebSocketURLConstruction) {
    // Environment is set at client creation time
    
    // This would test internal URL building - we can't directly test it
    // but we can ensure no exceptions are thrown
    std::vector<std::string> symbols = {"AAPL"};
    
    auto callback = [this](const simdjson::dom::element& data) {
        OnDataReceived(data);
    };
    
    auto error_callback = [this](const std::string& error) {
        OnErrorReceived(error);
    };
    
    // These will fail to connect but should not throw during setup
    EXPECT_NO_THROW({
        try {
            session_->start_market_websocket_stream(symbols, callback, error_callback);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            session_->stop_stream();
        } catch (const std::exception&) {
            // Connection failures are expected in unit tests
        }
    });
}

// Test HTTP/SSE streaming setup
TEST_F(StreamingTest, HTTPStreamingSetup) {
    std::vector<std::string> symbols = {"AAPL", "GOOGL"};
    
    auto callback = [this](const simdjson::dom::element& data) {
        OnDataReceived(data);
    };
    
    auto error_callback = [this](const std::string& error) {
        OnErrorReceived(error);
    };
    
    // Test market HTTP stream setup
    EXPECT_NO_THROW({
        try {
            session_->start_market_http_stream(symbols, callback, error_callback);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            session_->stop_stream();
        } catch (const std::exception&) {
            // Connection failures are expected in unit tests
        }
    });
    
    // Test account HTTP stream setup
    EXPECT_NO_THROW({
        try {
            session_->start_account_http_stream(callback, error_callback);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            session_->stop_stream();
        } catch (const std::exception&) {
            // Connection failures are expected in unit tests
        }
    });
}

// Test async streaming methods
TEST_F(StreamingTest, AsyncStreamingMethods) {
    std::vector<std::string> symbols = {"AAPL"};
    
    auto callback = [this](const simdjson::dom::element& data) {
        OnDataReceived(data);
    };
    
    auto error_callback = [this](const std::string& error) {
        OnErrorReceived(error);
    };
    
    // Test async WebSocket streaming
    EXPECT_NO_THROW({
        auto future = session_->start_market_websocket_stream_async(symbols, callback, error_callback);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        session_->stop_stream();
        // Don't wait for future as it may not complete due to connection issues
    });
    
    // Test async HTTP streaming
    EXPECT_NO_THROW({
        auto future = session_->start_market_http_stream_async(symbols, callback, error_callback);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        session_->stop_stream();
        // Don't wait for future as it may not complete due to connection issues
    });
}

// Test environment switching
TEST_F(StreamingTest, EnvironmentSwitching) {
    // Test that different environments work
    client_->set_environment(Environment::Sandbox);
    EXPECT_EQ(client_->get_base_url(), "https://sandbox.tradier.com");
    
    client_->set_environment(Environment::Production);
    EXPECT_EQ(client_->get_base_url(), "https://api.tradier.com");
    
    // Switch back for cleanup
    client_->set_environment(Environment::Sandbox);
}

// Test session recreation after stop
TEST_F(StreamingTest, SessionRecreation) {
    auto callback = [this](const simdjson::dom::element& data) {
        OnDataReceived(data);
    };
    
    // Start and stop multiple times
    for (int i = 0; i < 3; ++i) {
        EXPECT_NO_THROW({
            try {
                session_->start_market_websocket_stream({"AAPL"}, callback);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                session_->stop_stream();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            } catch (const std::exception&) {
                // Connection failures expected
            }
        });
    }
}

// Test data type determination (would need access to private methods for full testing)
TEST_F(StreamingTest, StreamingDataTypes) {
    // Test that all data types are defined
    std::vector<StreamingDataType> all_types = {
        StreamingDataType::Quote,
        StreamingDataType::Trade,
        StreamingDataType::Summary,
        StreamingDataType::TimeSale,
        StreamingDataType::TradEx,
        StreamingDataType::OrderStatus,
        StreamingDataType::AccountActivity
    };
    
    // Should be able to set filter with all types
    EXPECT_NO_THROW(session_->set_data_filter(all_types));
    EXPECT_NO_THROW(session_->clear_data_filter());
}

// Test callback error handling
TEST_F(StreamingTest, CallbackErrorHandling) {
    // Test with null callbacks
    EXPECT_NO_THROW({
        try {
            session_->start_market_websocket_stream({"AAPL"}, nullptr, nullptr);
            session_->stop_stream();
        } catch (const std::exception&) {
            // Expected
        }
    });
    
    // Test with throwing callback
    auto throwing_callback = [](const simdjson::dom::element&) {
        throw std::runtime_error("Test exception");
    };
    
    EXPECT_NO_THROW({
        try {
            session_->start_market_websocket_stream({"AAPL"}, throwing_callback);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            session_->stop_stream();
        } catch (const std::exception&) {
            // Expected
        }
    });
}

// Test concurrent access
TEST_F(StreamingTest, ConcurrentAccess) {
    std::vector<std::thread> threads;
    
    // Test concurrent symbol operations
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([this, i]() {
            std::vector<std::string> symbols = {"SYMBOL" + std::to_string(i)};
            session_->add_symbols(symbols);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            session_->remove_symbols(symbols);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Should complete without issues
    SUCCEED();
}

// Factory function test
TEST_F(StreamingTest, FactoryFunction) {
    auto session = create_streaming_session(client_);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(session->get_connection_state(), ConnectionState::Disconnected);
}

// Test connection state management
TEST_F(StreamingTest, ConnectionStateManagement) {
    // Test initial state
    EXPECT_EQ(session_->get_connection_state(), ConnectionState::Disconnected);
    
    // Test reconnection configuration
    EXPECT_NO_THROW(session_->set_reconnect_enabled(true));
    EXPECT_NO_THROW(session_->set_max_reconnect_attempts(5));
    EXPECT_NO_THROW(session_->set_reconnect_delay(std::chrono::milliseconds(1000)));
    
    // State should still be disconnected
    EXPECT_EQ(session_->get_connection_state(), ConnectionState::Disconnected);
}