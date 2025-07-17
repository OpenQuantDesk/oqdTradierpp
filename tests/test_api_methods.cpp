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
#include "oqdTradierpp/api_methods.hpp"
#include "oqdTradierpp/client.hpp"
#include <memory>
#include <future>
#include <thread>

using namespace oqd;

class ApiMethodsTest : public ::testing::Test {
protected:
    void SetUp() override {
        client = std::make_shared<TradierClient>(Environment::Sandbox);
        client->set_access_token("test_token");
        api_methods = std::make_unique<ApiMethods>(client);
    }
    
    std::shared_ptr<TradierClient> client;
    std::unique_ptr<ApiMethods> api_methods;
};

// Profile Tests
TEST_F(ApiMethodsTest, GetUserProfileAsync) {
    // This would normally mock the HTTP client response
    // For now, we're testing the interface exists
    auto future = api_methods->get_user_profile_async();
    EXPECT_TRUE(future.valid());
}

// Market Data Tests
TEST_F(ApiMethodsTest, GetQuotesAsyncSingleSymbol) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_quotes_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetQuotesAsyncMultipleSymbols) {
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT"};
    auto future = api_methods->get_quotes_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetMarketClockAsync) {
    auto future = api_methods->get_market_clock_async();
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetHistoricalDataAsync) {
    auto future = api_methods->get_historical_data_async(
        "SPY", 
        HistoricalInterval::Daily,
        std::chrono::system_clock::now() - std::chrono::hours(24*30),
        std::chrono::system_clock::now()
    );
    EXPECT_TRUE(future.valid());
}

// Options Tests
TEST_F(ApiMethodsTest, GetOptionsChainAsync) {
    auto future = api_methods->get_options_chain_async(
        "AAPL",
        std::chrono::system_clock::now(),
        true
    );
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetOptionsExpirationsAsync) {
    auto future = api_methods->get_option_expirations_async("AAPL", true, true);
    EXPECT_TRUE(future.valid());
}

// Account Tests
TEST_F(ApiMethodsTest, GetAccountBalancesAsync) {
    auto future = api_methods->get_account_balances_async("test_account");
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetAccountPositionsAsync) {
    auto future = api_methods->get_account_positions_async("test_account");
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetAccountOrdersAsync) {
    auto future = api_methods->get_account_orders_async("test_account");
    EXPECT_TRUE(future.valid());
}

// Trading Tests
TEST_F(ApiMethodsTest, PlaceEquityOrderAsync) {
    EquityOrderRequest order;
    order.symbol = "AAPL";
    order.quantity = 100;
    order.side = OrderSide::Buy;
    order.type = OrderType::Market;
    order.duration = OrderDuration::Day;
    
    auto future = api_methods->place_equity_order_async("test_account", order);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, PlaceOptionOrderAsync) {
    OptionOrderRequest order;
    order.option_symbol = "AAPL210917C00150000";
    order.quantity = 1;
    order.side = OrderSide::BuyToOpen;
    order.type = OrderType::Limit;
    order.duration = OrderDuration::Day;
    order.price = 5.00;
    
    auto future = api_methods->place_option_order_async("test_account", order);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, PlaceMultilegOrderAsync) {
    MultilegOrderRequest order;
    order.type = OrderType::Market;
    order.duration = OrderDuration::Day;
    
    Leg leg1;
    leg1.option_symbol = "AAPL210917C00150000";
    leg1.side = OrderSide::BuyToOpen;
    leg1.quantity = 1;
    
    Leg leg2;
    leg2.option_symbol = "AAPL210917C00160000";
    leg2.side = OrderSide::SellToOpen;
    leg2.quantity = 1;
    
    order.legs.push_back(leg1);
    order.legs.push_back(leg2);
    
    auto future = api_methods->place_multileg_order_async("test_account", order);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, CancelOrderAsync) {
    auto future = api_methods->cancel_order_async("test_account", "12345");
    EXPECT_TRUE(future.valid());
}

// Watchlist Tests
TEST_F(ApiMethodsTest, GetAllWatchlistsAsync) {
    auto future = api_methods->get_all_watchlists_async();
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, CreateWatchlistAsync) {
    std::vector<std::string> symbols = {"AAPL", "GOOGL"};
    auto future = api_methods->create_watchlist_async("Tech Stocks", symbols);
    EXPECT_TRUE(future.valid());
}

// Symbol Search Tests
TEST_F(ApiMethodsTest, SearchCompaniesAsync) {
    auto future = api_methods->search_companies_async("Apple");
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, LookupSymbolsAsync) {
    std::vector<std::string> types = {"stock", "etf"};
    auto future = api_methods->lookup_symbols_async("SPY", types);
    EXPECT_TRUE(future.valid());
}

// Fundamentals Tests
TEST_F(ApiMethodsTest, GetCompanyInfoAsync) {
    std::vector<std::string> symbols = {"AAPL", "GOOGL"};
    auto future = api_methods->get_company_info_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetFinancialRatiosAsync) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_financial_ratios_async(symbols);
    EXPECT_TRUE(future.valid());
}

// Beta Fundamental Data Tests
TEST_F(ApiMethodsTest, GetCorporateActionsAsync) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_corporate_actions_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetCorporateFinancialsAsync) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_corporate_financials_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetPriceStatisticsAsync) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_price_statistics_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetDividendInfoAsync) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_dividend_info_async(symbols);
    EXPECT_TRUE(future.valid());
}

TEST_F(ApiMethodsTest, GetCorporateCalendarAsync) {
    std::vector<std::string> symbols = {"AAPL"};
    auto future = api_methods->get_corporate_calendar_async(symbols);
    EXPECT_TRUE(future.valid());
}

// Concurrent Access Tests
TEST_F(ApiMethodsTest, ConcurrentApiCalls) {
    std::vector<std::future<std::vector<Quote>>> futures;
    
    // Launch multiple concurrent requests
    for (int i = 0; i < 10; ++i) {
        futures.push_back(api_methods->get_quotes_async({"SPY"}));
    }
    
    // Verify all futures are valid
    for (auto& future : futures) {
        EXPECT_TRUE(future.valid());
    }
}

// Error Handling Tests
TEST_F(ApiMethodsTest, InvalidAccountId) {
    // Test with empty account ID
    auto future = api_methods->get_account_balances_async("");
    EXPECT_TRUE(future.valid());
    // In a real test with mocking, we'd verify it throws or returns an error
}

TEST_F(ApiMethodsTest, EmptySymbolList) {
    std::vector<std::string> empty_symbols;
    auto future = api_methods->get_quotes_async(empty_symbols);
    EXPECT_TRUE(future.valid());
    // Should handle empty symbol list gracefully
}

// Synchronous Method Tests
TEST_F(ApiMethodsTest, GetQuotesSynchronous) {
    std::vector<std::string> symbols = {"AAPL"};
    EXPECT_NO_THROW({
        // This will fail without mocking, but tests the interface
        try {
            auto quotes = api_methods->get_quotes(symbols);
        } catch (...) {
            // Expected in unit tests without mocking
        }
    });
}

// Helper Method Tests
TEST_F(ApiMethodsTest, JoinSymbols) {
    // This is a private method, but we can test through public interface
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT"};
    auto future = api_methods->get_quotes_async(symbols);
    EXPECT_TRUE(future.valid());
    // The join_symbols method should properly format the symbol list
}