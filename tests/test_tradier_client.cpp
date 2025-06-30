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

#include <gtest/gtest.h>
#include "oqdTradierpp/client.hpp"
#include "oqdTradierpp/endpoints.hpp"
#include <thread>
#include <chrono>
#include <functional>

using namespace oqd;

class TradierClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        client_ = std::make_unique<TradierClient>(Environment::Sandbox);
    }

    void TearDown() override {
        client_.reset();
    }

    std::unique_ptr<TradierClient> client_;
};

TEST_F(TradierClientTest, InitializationTest) {
    EXPECT_NE(client_, nullptr);
    EXPECT_EQ(client_->get_base_url(), "https://sandbox.tradier.com");
}

TEST_F(TradierClientTest, EnvironmentSwitching) {
    client_->set_environment(Environment::Production);
    EXPECT_EQ(client_->get_base_url(), "https://api.tradier.com");

    client_->set_environment(Environment::Sandbox);
    EXPECT_EQ(client_->get_base_url(), "https://sandbox.tradier.com");
}

TEST_F(TradierClientTest, AccessTokenSetting) {
    std::string test_token = "test_access_token_123";
    client_->set_access_token(test_token);
    
    // Token is set internally - we can't directly test it, but no exception should be thrown
    SUCCEED();
}

TEST_F(TradierClientTest, ClientCredentialsSetting) {
    std::string client_id = "test_client_id";
    std::string client_secret = "test_client_secret";
    
    client_->set_client_credentials(client_id, client_secret);
    
    // Credentials are set internally - we can't directly test them, but no exception should be thrown
    SUCCEED();
}

TEST_F(TradierClientTest, RateLimitTracking) {
    std::string endpoint_group = "test_group";
    
    // Initially, there should be no rate limit info
    auto rate_limit = client_->get_rate_limit(endpoint_group);
    EXPECT_FALSE(rate_limit.has_value());
    
    // Should not be rate limited initially
    EXPECT_FALSE(client_->is_rate_limited(endpoint_group));
}

TEST_F(TradierClientTest, EndpointsCompileTime) {
    // Test that endpoints are available at compile time
    // This validates our compile-time endpoint system
    EXPECT_EQ(std::string(oqd::endpoints::user::profile.path), "/v1/user/profile");
    EXPECT_EQ(std::string(oqd::endpoints::markets::quotes.path), "/v1/markets/quotes");
    EXPECT_EQ(oqd::endpoints::markets::quotes.rate_limit_per_second, 120);
}

TEST_F(TradierClientTest, URLBuilding) {
    // This tests internal URL building functionality indirectly
    std::unordered_map<std::string, std::string> params = {
        {"symbol", "AAPL"},
        {"interval", "daily"}
    };
    
    // Should not throw when building URLs
    EXPECT_NO_THROW({
        // This would normally test the private build_url method
        // For now, we test that the client doesn't crash with parameters
        auto future = client_->get_async("/test", params);
        // Don't wait for completion as this would likely fail without proper auth
    });
}

class TradierClientExceptionTest : public ::testing::Test {
protected:
    void SetUp() override {
        client_ = std::make_unique<TradierClient>(Environment::Sandbox);
    }

    std::unique_ptr<TradierClient> client_;
};

TEST_F(TradierClientExceptionTest, EndpointValidation) {
    // Test that endpoint validation works at compile time
    // This test validates our endpoint-based API methods
    EXPECT_NO_THROW({
        std::string user_path = std::string(oqd::endpoints::user::profile.path);
        std::string market_path = std::string(oqd::endpoints::markets::clock.path);
        EXPECT_FALSE(user_path.empty());
        EXPECT_FALSE(market_path.empty());
    });
}

TEST_F(TradierClientExceptionTest, RateLimitException) {
    // This would test rate limiting, but requires setting up mock rate limits
    // For now, just test that the exception type exists
    EXPECT_NO_THROW({
        RateLimitException ex("Test rate limit");
        EXPECT_STREQ(ex.what(), "Test rate limit");
    });
}

TEST_F(TradierClientExceptionTest, ApiException) {
    EXPECT_NO_THROW({
        ApiException ex("Test API error");
        EXPECT_STREQ(ex.what(), "Test API error");
    });
}

// Test environment-specific behavior
class TradierClientEnvironmentTest : public ::testing::TestWithParam<Environment> {
protected:
    void SetUp() override {
        env_ = GetParam();
        client_ = std::make_unique<TradierClient>(env_);
    }

    Environment env_;
    std::unique_ptr<TradierClient> client_;
};

TEST_P(TradierClientEnvironmentTest, CorrectBaseURL) {
    switch (env_) {
        case Environment::Production:
            EXPECT_EQ(client_->get_base_url(), "https://api.tradier.com");
            break;
        case Environment::Sandbox:
            EXPECT_EQ(client_->get_base_url(), "https://sandbox.tradier.com");
            break;
    }
}

INSTANTIATE_TEST_SUITE_P(
    EnvironmentTests,
    TradierClientEnvironmentTest,
    ::testing::Values(Environment::Production, Environment::Sandbox)
);

// Test concurrent usage
TEST_F(TradierClientTest, ConcurrentUsage) {
    const int num_threads = 5;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, &success_count]() {
            try {
                std::hash<std::thread::id> hasher;
                client_->set_access_token("test_token_" + std::to_string(hasher(std::this_thread::get_id())));
                success_count++;
            } catch (...) {
                // Ignore exceptions for this test
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(success_count.load(), num_threads);
}