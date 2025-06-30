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
#include "oqdTradierpp/endpoints.hpp"
#include <string>
#include <unordered_map>

using namespace oqd::endpoints;

class EndpointsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

// Basic Endpoint Configuration Tests
TEST_F(EndpointsTest, AuthenticationEndpoints) {
    // Test OAuth endpoints
    EXPECT_EQ(authentication::oauth_authorize.path, "/v1/oauth/authorize");
    EXPECT_EQ(authentication::oauth_authorize.method, "GET");
    EXPECT_EQ(authentication::oauth_authorize.auth_type, "none");
    
    EXPECT_EQ(authentication::oauth_accesstoken.path, "/v1/oauth/accesstoken");
    EXPECT_EQ(authentication::oauth_accesstoken.method, "POST");
    EXPECT_EQ(authentication::oauth_accesstoken.auth_type, "basic");
}

TEST_F(EndpointsTest, UserEndpoints) {
    EXPECT_EQ(user::profile.path, "/v1/user/profile");
    EXPECT_EQ(user::profile.method, "GET");
    EXPECT_EQ(user::profile.auth_type, "bearer");
    EXPECT_EQ(user::profile.rate_limit_per_second, 60);
}

TEST_F(EndpointsTest, MarketEndpoints) {
    // Test market data endpoints
    EXPECT_EQ(markets::quotes.path, "/v1/markets/quotes");
    EXPECT_EQ(markets::quotes.method, "GET");
    EXPECT_EQ(markets::quotes.auth_type, "bearer");
    EXPECT_EQ(markets::quotes.rate_limit_per_second, 120);
    
    EXPECT_EQ(markets::clock.path, "/v1/markets/clock");
    EXPECT_EQ(markets::clock.method, "GET");
    
    EXPECT_EQ(markets::history.path, "/v1/markets/history");
    EXPECT_EQ(markets::history.method, "GET");
}

TEST_F(EndpointsTest, OptionsEndpoints) {
    EXPECT_EQ(markets::options::chains.path, "/v1/markets/options/chains");
    EXPECT_EQ(markets::options::chains.method, "GET");
    
    EXPECT_EQ(markets::options::expirations.path, "/v1/markets/options/expirations");
    EXPECT_EQ(markets::options::strikes.path, "/v1/markets/options/strikes");
}

TEST_F(EndpointsTest, AccountEndpoints) {
    // Test parameterized account endpoints (these are structs, not constexpr configs)
    EXPECT_EQ(accounts::balances::method, "GET");
    EXPECT_EQ(accounts::positions::method, "GET");
    EXPECT_EQ(accounts::orders::method, "GET");
    
    // All should use bearer auth
    EXPECT_EQ(accounts::balances::auth_type, "bearer");
    EXPECT_EQ(accounts::positions::auth_type, "bearer");
    EXPECT_EQ(accounts::orders::auth_type, "bearer");
    
    // Test path generation
    EXPECT_EQ(accounts::balances::path("123456"), "/v1/accounts/123456/balances");
    EXPECT_EQ(accounts::positions::path("123456"), "/v1/accounts/123456/positions");
    EXPECT_EQ(accounts::orders::path("123456"), "/v1/accounts/123456/orders");
}

TEST_F(EndpointsTest, StreamingEndpoints) {
    EXPECT_EQ(markets::events::session.path, "/v1/markets/events/session");
    EXPECT_EQ(markets::events::session.method, "POST");
    
    // Test stream path generation
    EXPECT_EQ(markets::events::stream::path("abc123"), "/v1/markets/events/abc123");
}

TEST_F(EndpointsTest, BetaFundamentalEndpoints) {
    // Test beta endpoints
    EXPECT_EQ(beta::fundamentals::company.path, "/beta/markets/fundamentals/company");
    EXPECT_EQ(beta::fundamentals::company.method, "GET");
    EXPECT_EQ(beta::fundamentals::company.rate_limit_per_second, 30);
    
    EXPECT_EQ(beta::fundamentals::ratios.path, "/beta/markets/fundamentals/ratios");
    EXPECT_EQ(beta::fundamentals::financials.path, "/beta/markets/fundamentals/financials");
    EXPECT_EQ(beta::fundamentals::price_stats.path, "/beta/markets/fundamentals/price_stats");
    EXPECT_EQ(beta::fundamentals::corporate_calendar.path, "/beta/markets/fundamentals/corporate_calendar");
    EXPECT_EQ(beta::fundamentals::dividend.path, "/beta/markets/fundamentals/dividend");
}

// WebSocket Endpoint Tests
TEST_F(EndpointsTest, WebSocketEndpoints) {
    EXPECT_EQ(websocket::base_urls::production, "wss://ws.tradier.com");
    EXPECT_EQ(websocket::base_urls::sandbox, "wss://sandbox.tradier.com");
    
    EXPECT_EQ(websocket::markets, "/v1/markets/events");
    EXPECT_EQ(websocket::accounts, "/v1/accounts/events");
}

// Endpoint Builder Tests
TEST_F(EndpointsTest, EndpointBuilderPathSubstitution) {
    std::unordered_map<std::string, std::string> params = {
        {"account_id", "123456"}
    };
    
    std::string path = EndpointBuilder::build_path(accounts::balances_template, params);
    EXPECT_EQ(path, "/v1/accounts/123456/balances");
    
    path = EndpointBuilder::build_path(accounts::orders_template, params);
    EXPECT_EQ(path, "/v1/accounts/123456/orders");
}

TEST_F(EndpointsTest, EndpointBuilderMultipleSubstitutions) {
    std::unordered_map<std::string, std::string> params = {
        {"account_id", "123456"},
        {"order_id", "789"}
    };
    
    std::string template_path = "/v1/accounts/{account_id}/orders/{order_id}";
    std::string path = EndpointBuilder::build_path(template_path, params);
    EXPECT_EQ(path, "/v1/accounts/123456/orders/789");
}

TEST_F(EndpointsTest, EndpointBuilderMissingParameter) {
    std::unordered_map<std::string, std::string> params = {
        {"other_param", "value"}
    };
    
    // Missing parameter should leave placeholder unchanged
    std::string path = EndpointBuilder::build_path(accounts::balances.path, params);
    EXPECT_EQ(path, "/v1/accounts/{account_id}/balances");
}

// Rate Limit Tests
TEST_F(EndpointsTest, RateLimitConfiguration) {
    // Verify rate limits are reasonable
    EXPECT_GT(markets::quotes.rate_limit_per_second, 0);
    EXPECT_LE(markets::quotes.rate_limit_per_second, 200);
    
    EXPECT_GT(user::profile.rate_limit_per_second, 0);
    EXPECT_LE(user::profile.rate_limit_per_second, 100);
    
    // Beta endpoints should have lower rate limits
    EXPECT_LE(beta::fundamentals::company.rate_limit_per_second, 60);
}

// Compile-time Validation Tests
TEST_F(EndpointsTest, CompileTimeEndpointAccess) {
    // Test compile-time endpoint access
    // Note: Testing constexpr access patterns
    EXPECT_EQ(user::profile.path, "/v1/user/profile");
    EXPECT_EQ(user::profile.method, "GET");
}

// Registry Tests
TEST_F(EndpointsTest, EndpointRegistryCompleteness) {
    // Verify all endpoints are in registry
    EXPECT_GT(registry::all_endpoints.size(), 0);
    
    // Check that key endpoints are included
    bool has_quotes = false;
    bool has_orders = false;
    bool has_beta = false;
    
    for (const auto* endpoint : registry::all_endpoints) {
        if (endpoint->path == markets::quotes.path) has_quotes = true;
        if (endpoint->path == accounts::orders.path) has_orders = true;
        if (endpoint->path == beta::fundamentals::company.path) has_beta = true;
    }
    
    EXPECT_TRUE(has_quotes);
    EXPECT_TRUE(has_orders);
    EXPECT_TRUE(has_beta);
}

// HTTP Method Validation Tests
TEST_F(EndpointsTest, HttpMethodConsistency) {
    // Read operations should use GET
    EXPECT_EQ(markets::quotes.method, "GET");
    EXPECT_EQ(markets::history.method, "GET");
    EXPECT_EQ(accounts::balances.method, "GET");
    EXPECT_EQ(accounts::positions.method, "GET");
    
    // Write operations should use POST/PUT/DELETE
    EXPECT_EQ(accounts::orders::create::method, "POST");
    // TODO: Add watchlist endpoints when implemented
    // EXPECT_EQ(watchlists::create.method, "POST");
    // EXPECT_EQ(watchlists::remove.method, "DELETE");
}

// Authentication Type Tests
TEST_F(EndpointsTest, AuthenticationTypeConsistency) {
    // OAuth endpoints should not require auth
    EXPECT_EQ(authentication::oauth_authorize.auth_type, "none");
    
    // OAuth token endpoint should use basic auth
    EXPECT_EQ(authentication::oauth_accesstoken.auth_type, "basic");
    
    // Most endpoints should use bearer auth
    EXPECT_EQ(user::profile.auth_type, "bearer");
    EXPECT_EQ(markets::quotes.auth_type, "bearer");
    EXPECT_EQ(accounts::balances.auth_type, "bearer");
}

// Path Pattern Tests
TEST_F(EndpointsTest, PathPatternConsistency) {
    // All paths should start with /v1 or /beta
    auto validate_path = [](std::string_view path) {
        return path.starts_with("/v1/") || path.starts_with("/beta/");
    };
    
    EXPECT_TRUE(validate_path(user::profile.path));
    EXPECT_TRUE(validate_path(markets::quotes.path));
    EXPECT_TRUE(validate_path(accounts::balances.path));
    EXPECT_TRUE(validate_path(beta::fundamentals::company.path));
}

// Parameterized Endpoint Tests
TEST_F(EndpointsTest, ParameterizedEndpointPatterns) {
    // Account endpoints should have {account_id} parameter
    EXPECT_TRUE(std::string(accounts::balances.path).find("{account_id}") != std::string::npos);
    EXPECT_TRUE(std::string(accounts::positions.path).find("{account_id}") != std::string::npos);
    EXPECT_TRUE(std::string(accounts::orders.path).find("{account_id}") != std::string::npos);
    
    // Individual order endpoints should have both parameters
    std::string order_path = "/v1/accounts/{account_id}/orders/{order_id}";
    EXPECT_TRUE(order_path.find("{account_id}") != std::string::npos);
    EXPECT_TRUE(order_path.find("{order_id}") != std::string::npos);
}

// Edge Case Tests
TEST_F(EndpointsTest, EmptyParameterMap) {
    std::unordered_map<std::string, std::string> empty_params;
    
    // Non-parameterized endpoint should remain unchanged
    std::string path = EndpointBuilder::build_path(markets::quotes.path, empty_params);
    EXPECT_EQ(path, markets::quotes.path);
}

TEST_F(EndpointsTest, SpecialCharactersInParameters) {
    std::unordered_map<std::string, std::string> params = {
        {"account_id", "test%20account"}  // URL encoded space
    };
    
    // Builder should not double-encode
    std::string path = EndpointBuilder::build_path(accounts::balances.path, params);
    EXPECT_EQ(path, "/v1/accounts/test%20account/balances");
}