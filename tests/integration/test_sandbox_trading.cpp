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
#include "oqdTradierpp/oqdTradierpp.hpp"
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace oqd;
using namespace std::chrono_literals;

class SandboxTradingTest : public ::testing::Test {
protected:
    std::shared_ptr<TradierClient> client;
    std::unique_ptr<ApiMethods> api;
    std::string sandbox_token;
    std::string sandbox_account_id;
    
    void SetUp() override {
        // Get credentials from environment variables
        const char* token_env = std::getenv("TRADIER_SANDBOX_TOKEN");
        const char* account_env = std::getenv("TRADIER_SANDBOX_ACCOUNT");
        
        if (!token_env || !account_env) {
            GTEST_SKIP() << "Sandbox credentials not found in environment variables. "
                         << "Set TRADIER_SANDBOX_TOKEN and TRADIER_SANDBOX_ACCOUNT to run these tests.";
        }
        
        sandbox_token = token_env;
        sandbox_account_id = account_env;
        
        // Create client for sandbox environment
        client = create_client(Environment::Sandbox);
        client->set_access_token(sandbox_token);
        api = create_api_methods(client);
    }
    
    void TearDown() override {
        // Cancel any open orders to clean up
        if (api && !sandbox_account_id.empty()) {
            try {
                auto orders = api->account_orders(sandbox_account_id);
                for (const auto& order : orders) {
                    if (order.status == "pending" || order.status == "open") {
                        api->cancel_order(sandbox_account_id, order.id);
                    }
                }
            } catch (...) {
                // Ignore cleanup errors
            }
        }
    }
    
    void wait_for_order_fill(const std::string& order_id, int timeout_seconds = 10) {
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeout_seconds)) {
            auto order = api->account_order(sandbox_account_id, order_id);
            if (order.status == "filled" || order.status == "rejected" || order.status == "canceled") {
                return;
            }
            std::this_thread::sleep_for(100ms);
        }
    }
};

TEST_F(SandboxTradingTest, GetAccountInfo) {
    auto profile = api->user_profile();
    EXPECT_FALSE(profile.id.empty());
    EXPECT_FALSE(profile.name.empty());
    
    auto balances = api->account_balances(sandbox_account_id);
    EXPECT_GE(balances.total_equity, 0.0);
    EXPECT_GE(balances.total_cash, 0.0);
}

TEST_F(SandboxTradingTest, GetMarketQuotes) {
    std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOGL", "SPY"};
    auto quotes = api->get_quotes(symbols);
    
    EXPECT_EQ(quotes.size(), symbols.size());
    for (const auto& quote : quotes) {
        EXPECT_FALSE(quote.symbol.empty());
        EXPECT_GT(quote.last, 0.0);
        EXPECT_GT(quote.bid, 0.0);
        EXPECT_GT(quote.ask, 0.0);
        EXPECT_GE(quote.volume, 0);
    }
}

TEST_F(SandboxTradingTest, PlaceAndCancelMarketOrder) {
    EquityOrderRequest request;
    request.symbol = "AAPL";
    request.side = OrderSide::Buy;
    request.quantity = 1;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    auto order_response = api->place_equity_order(sandbox_account_id, request);
    ASSERT_FALSE(order_response.order.id.empty());
    EXPECT_EQ(order_response.order.symbol, "AAPL");
    EXPECT_EQ(order_response.order.quantity, 1);
    
    // Cancel the order
    auto cancel_response = api->cancel_order(sandbox_account_id, order_response.order.id);
    EXPECT_FALSE(cancel_response.order.id.empty());
    
    // Verify cancellation
    auto order_status = api->account_order(sandbox_account_id, order_response.order.id);
    EXPECT_TRUE(order_status.status == "canceled" || order_status.status == "filled");
}

TEST_F(SandboxTradingTest, PlaceLimitOrder) {
    // Get current quote to set realistic limit price
    auto quotes = api->get_quotes({"TSLA"});
    ASSERT_FALSE(quotes.empty());
    double current_price = quotes[0].last;
    
    EquityOrderRequest request;
    request.symbol = "TSLA";
    request.side = OrderSide::Buy;
    request.quantity = 5;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::GTC;
    request.price = current_price * 0.95; // 5% below market
    
    auto order_response = api->place_equity_order(sandbox_account_id, request);
    ASSERT_FALSE(order_response.order.id.empty());
    EXPECT_EQ(order_response.order.type, "limit");
    EXPECT_DOUBLE_EQ(order_response.order.price, request.price.value());
    
    // Check order status
    auto order = api->account_order(sandbox_account_id, order_response.order.id);
    EXPECT_EQ(order.status, "pending");
    
    // Cancel the order
    api->cancel_order(sandbox_account_id, order_response.order.id);
}

TEST_F(SandboxTradingTest, PlaceStopOrder) {
    auto quotes = api->get_quotes({"SPY"});
    ASSERT_FALSE(quotes.empty());
    double current_price = quotes[0].last;
    
    EquityOrderRequest request;
    request.symbol = "SPY";
    request.side = OrderSide::Sell;
    request.quantity = 10;
    request.type = OrderType::Stop;
    request.duration = OrderDuration::Day;
    request.stop = current_price * 0.98; // 2% below market
    
    auto order_response = api->place_equity_order(sandbox_account_id, request);
    ASSERT_FALSE(order_response.order.id.empty());
    EXPECT_EQ(order_response.order.type, "stop");
    EXPECT_DOUBLE_EQ(order_response.order.stop, request.stop.value());
    
    // Cancel the order
    api->cancel_order(sandbox_account_id, order_response.order.id);
}

TEST_F(SandboxTradingTest, PlaceOptionOrder) {
    // Get option chain
    auto chain = api->option_chain("AAPL", "2024-12-20");
    ASSERT_FALSE(chain.empty());
    
    // Find an ATM call option
    auto quotes = api->get_quotes({"AAPL"});
    ASSERT_FALSE(quotes.empty());
    double stock_price = quotes[0].last;
    
    std::string option_symbol;
    for (const auto& option : chain) {
        if (option.option_type == "call" && 
            std::abs(option.strike - stock_price) < 5.0) {
            option_symbol = option.symbol;
            break;
        }
    }
    
    if (!option_symbol.empty()) {
        OptionOrderRequest request;
        request.option_symbol = option_symbol;
        request.side = OrderSide::BuyToOpen;
        request.quantity = 1;
        request.type = OrderType::Market;
        request.duration = OrderDuration::Day;
        
        auto order_response = api->place_option_order(sandbox_account_id, request);
        ASSERT_FALSE(order_response.order.id.empty());
        EXPECT_EQ(order_response.order.option_symbol, option_symbol);
        
        // Wait for fill and then close position
        wait_for_order_fill(order_response.order.id);
        
        auto order_status = api->account_order(sandbox_account_id, order_response.order.id);
        if (order_status.status == "filled") {
            // Close the position
            OptionOrderRequest close_request;
            close_request.option_symbol = option_symbol;
            close_request.side = OrderSide::SellToClose;
            close_request.quantity = 1;
            close_request.type = OrderType::Market;
            close_request.duration = OrderDuration::Day;
            
            api->place_option_order(sandbox_account_id, close_request);
        }
    }
}

TEST_F(SandboxTradingTest, GetPositions) {
    auto positions = api->account_positions(sandbox_account_id);
    
    for (const auto& position : positions) {
        EXPECT_FALSE(position.symbol.empty());
        EXPECT_NE(position.quantity, 0);
        EXPECT_GT(position.cost_basis, 0.0);
    }
}

TEST_F(SandboxTradingTest, GetOrderHistory) {
    auto orders = api->account_orders(sandbox_account_id);
    
    if (!orders.empty()) {
        // Check first few orders
        int count = std::min(5, static_cast<int>(orders.size()));
        for (int i = 0; i < count; ++i) {
            const auto& order = orders[i];
            EXPECT_FALSE(order.id.empty());
            EXPECT_FALSE(order.symbol.empty());
            EXPECT_GT(order.quantity, 0);
            EXPECT_FALSE(order.status.empty());
            EXPECT_FALSE(order.type.empty());
        }
    }
}

TEST_F(SandboxTradingTest, GetAccountHistory) {
    auto history = api->account_history(sandbox_account_id);
    
    if (!history.empty()) {
        for (const auto& item : history) {
            EXPECT_NE(item.amount, 0.0);
            EXPECT_FALSE(item.type.empty());
            EXPECT_FALSE(item.trade_date.empty());
        }
    }
}

TEST_F(SandboxTradingTest, ModifyOrder) {
    auto quotes = api->get_quotes({"MSFT"});
    ASSERT_FALSE(quotes.empty());
    double current_price = quotes[0].last;
    
    // Place initial limit order
    EquityOrderRequest request;
    request.symbol = "MSFT";
    request.side = OrderSide::Buy;
    request.quantity = 10;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::GTC;
    request.price = current_price * 0.90; // 10% below market
    
    auto order_response = api->place_equity_order(sandbox_account_id, request);
    ASSERT_FALSE(order_response.order.id.empty());
    
    // Modify the order with new price
    ModifyOrderRequest modify_request;
    modify_request.type = OrderType::Limit;
    modify_request.duration = OrderDuration::GTC;
    modify_request.price = current_price * 0.92; // Update to 8% below market
    
    auto modify_response = api->modify_order(sandbox_account_id, order_response.order.id, modify_request);
    EXPECT_FALSE(modify_response.order.id.empty());
    EXPECT_DOUBLE_EQ(modify_response.order.price, modify_request.price.value());
    
    // Cancel the modified order
    api->cancel_order(sandbox_account_id, order_response.order.id);
}

TEST_F(SandboxTradingTest, MarketStatus) {
    auto status = api->market_status();
    
    EXPECT_FALSE(status.date.empty());
    EXPECT_FALSE(status.state.empty());
    EXPECT_TRUE(status.state == "open" || status.state == "closed" || 
                status.state == "premarket" || status.state == "postmarket");
}

TEST_F(SandboxTradingTest, SymbolLookup) {
    auto results = api->symbol_lookup("AAP");
    
    EXPECT_FALSE(results.empty());
    bool found_apple = false;
    for (const auto& result : results) {
        if (result.symbol == "AAPL") {
            found_apple = true;
            EXPECT_EQ(result.exchange, "NASDAQ");
            EXPECT_EQ(result.type, "stock");
        }
    }
    EXPECT_TRUE(found_apple);
}

TEST_F(SandboxTradingTest, ErrorHandling) {
    // Test invalid symbol
    EquityOrderRequest request;
    request.symbol = "INVALID_SYMBOL_XYZ";
    request.side = OrderSide::Buy;
    request.quantity = 1;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    EXPECT_THROW(api->place_equity_order(sandbox_account_id, request), std::runtime_error);
    
    // Test invalid order ID
    EXPECT_THROW(api->cancel_order(sandbox_account_id, "invalid_order_id"), std::runtime_error);
    
    // Test invalid account ID
    EXPECT_THROW(api->account_balances("invalid_account"), std::runtime_error);
}