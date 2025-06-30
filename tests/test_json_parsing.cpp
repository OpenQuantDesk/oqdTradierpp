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
#include "oqdTradierpp/types.hpp"
#include <simdjson.h>
#include <string>

using namespace oqd;

class JsonParsingTest : public ::testing::Test {
protected:
    simdjson::dom::parser parser;
};

// Quote JSON Parsing Tests
TEST_F(JsonParsingTest, ParseValidQuote) {
    std::string json = R"({
        "symbol": "AAPL",
        "description": "Apple Inc.",
        "bid": 150.25,
        "ask": 150.50,
        "last": 150.35,
        "volume": 1000000,
        "open": 149.50,
        "high": 151.00,
        "low": 149.25,
        "close": 150.00,
        "bid_size": 100,
        "ask_size": 200,
        "change": 0.35,
        "change_percentage": 0.23
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto quote = Quote::from_json(doc.value());
    EXPECT_EQ(quote.symbol, "AAPL");
    EXPECT_EQ(quote.description, "Apple Inc.");
    EXPECT_DOUBLE_EQ(quote.bid, 150.25);
    EXPECT_DOUBLE_EQ(quote.ask, 150.50);
    EXPECT_DOUBLE_EQ(quote.last, 150.35);
    EXPECT_EQ(quote.volume, 1000000);
}

TEST_F(JsonParsingTest, ParseQuoteWithMissingOptionalFields) {
    std::string json = R"({
        "symbol": "AAPL",
        "bid": 150.25,
        "ask": 150.50,
        "last": 150.35
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto quote = Quote::from_json(doc.value());
    EXPECT_EQ(quote.symbol, "AAPL");
    EXPECT_DOUBLE_EQ(quote.bid, 150.25);
    EXPECT_EQ(quote.volume, 0); // Default value
}

// Order JSON Parsing Tests
TEST_F(JsonParsingTest, ParseValidOrder) {
    std::string json = R"({
        "id": 12345,
        "type": "limit",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": 100.0,
        "status": "filled",
        "duration": "day",
        "price": 150.00,
        "avg_fill_price": 150.10,
        "exec_quantity": 100.0,
        "last_fill_price": 150.10,
        "last_fill_quantity": 100.0,
        "remaining_quantity": 0.0,
        "create_date": "2023-01-01T10:00:00.000Z",
        "transaction_date": "2023-01-01T10:05:00.000Z",
        "class": "equity"
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto order = Order::from_json(doc.value());
    EXPECT_EQ(order.id, 12345);
    EXPECT_EQ(order.type, OrderType::Limit);
    EXPECT_EQ(order.symbol, "AAPL");
    EXPECT_EQ(order.side, OrderSide::Buy);
    EXPECT_DOUBLE_EQ(order.quantity, 100.0);
    EXPECT_EQ(order.status, OrderStatus::Filled);
}

// Options Chain JSON Parsing Tests
TEST_F(JsonParsingTest, ParseOptionContract) {
    std::string json = R"({
        "symbol": "AAPL210917C00150000",
        "description": "AAPL Sep 17 2021 $150 Call",
        "bid": 5.25,
        "ask": 5.50,
        "last": 5.40,
        "volume": 1000,
        "open_interest": 5000,
        "strike": 150.00,
        "option_type": "call",
        "expiration_date": "2021-09-17",
        "underlying": "AAPL"
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    // OptionContract not yet implemented in types.hpp
    // auto option = OptionContract::from_json(doc.value());
    // EXPECT_EQ(option.symbol, "AAPL210917C00150000");
    // EXPECT_DOUBLE_EQ(option.strike.value(), 150.00);
    // EXPECT_EQ(option.option_type.value(), "call");
}

// Account Balances JSON Parsing Tests
TEST_F(JsonParsingTest, ParseAccountBalances) {
    std::string json = R"({
        "total_equity": 100000.00,
        "cash": 50000.00,
        "long_market_value": 50000.00,
        "short_market_value": 0.00,
        "market_value": 50000.00,
        "pending_orders_count": 2,
        "unsettled_funds": 1000.00,
        "option_long_value": 5000.00,
        "option_short_value": 0.00,
        "long_market_value_bp": 200000.00
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto balances = AccountBalances::from_json(doc.value());
    EXPECT_DOUBLE_EQ(balances.total_equity, 100000.00);
    EXPECT_DOUBLE_EQ(balances.cash, 50000.00);
    EXPECT_DOUBLE_EQ(balances.market_value, 50000.00);
}

// Nested JSON Structure Tests
TEST_F(JsonParsingTest, ParseOptionsChainWithMultipleStrikes) {
    std::string json = R"({
        "symbol": "AAPL",
        "expiration_dates": ["2021-09-17", "2021-10-15"],
        "options": [
            {
                "symbol": "AAPL210917C00150000",
                "strike": 150.00,
                "option_type": "call",
                "bid": 5.25,
                "ask": 5.50
            },
            {
                "symbol": "AAPL210917P00150000",
                "strike": 150.00,
                "option_type": "put",
                "bid": 4.75,
                "ask": 5.00
            }
        ]
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    // OptionsChain not yet implemented in types.hpp
    // auto chain = OptionsChain::from_json(doc.value());
    // EXPECT_EQ(chain.symbol, "AAPL");
    // EXPECT_EQ(chain.expiration_dates.size(), 2);
    // EXPECT_EQ(chain.options.size(), 2);
}

// Error Handling Tests
TEST_F(JsonParsingTest, ParseMalformedJson) {
    std::string json = R"({
        "symbol": "AAPL",
        "bid": 150.25,
        // Invalid comment
        "ask": 150.50
    })";
    
    auto doc = parser.parse(json);
    EXPECT_NE(doc.error(), simdjson::SUCCESS);
}

TEST_F(JsonParsingTest, ParseEmptyJson) {
    std::string json = "{}";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto quote = Quote::from_json(doc.value());
    EXPECT_TRUE(quote.symbol.empty());
}

// Array Parsing Tests
TEST_F(JsonParsingTest, ParseQuoteArray) {
    std::string json = R"([
        {
            "symbol": "AAPL",
            "bid": 150.25,
            "ask": 150.50,
            "last": 150.35
        },
        {
            "symbol": "GOOGL",
            "bid": 2800.00,
            "ask": 2801.00,
            "last": 2800.50
        }
    ])";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    std::vector<Quote> quotes;
    for (const auto& elem : doc.value().get_array()) {
        quotes.push_back(Quote::from_json(elem));
    }
    
    ASSERT_EQ(quotes.size(), 2);
    EXPECT_EQ(quotes[0].symbol, "AAPL");
    EXPECT_EQ(quotes[1].symbol, "GOOGL");
}

// Beta Fundamental Data JSON Parsing Tests
TEST_F(JsonParsingTest, ParseCorporateActions) {
    std::string json = R"({
        "symbol": "AAPL",
        "type": "dividend",
        "date": "2023-08-10",
        "description": "Quarterly dividend payment",
        "value": 0.24
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto action = CorporateActions::from_json(doc.value());
    EXPECT_EQ(action.symbol, "AAPL");
    EXPECT_EQ(action.type, "dividend");
    EXPECT_TRUE(action.value.has_value());
    EXPECT_DOUBLE_EQ(action.value.value(), 0.24);
}

TEST_F(JsonParsingTest, ParsePriceStatistics) {
    std::string json = R"({
        "symbol": "AAPL",
        "week_52_high": 180.00,
        "week_52_low": 120.00,
        "moving_avg_50": 150.00,
        "moving_avg_200": 145.00,
        "beta": 1.2,
        "volatility": 0.25
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto stats = PriceStatistics::from_json(doc.value());
    EXPECT_EQ(stats.symbol, "AAPL");
    EXPECT_DOUBLE_EQ(stats.week_52_high, 180.00);
    EXPECT_DOUBLE_EQ(stats.beta, 1.2);
}

// JSON Serialization Tests
TEST_F(JsonParsingTest, SerializeQuote) {
    Quote quote;
    quote.symbol = "AAPL";
    quote.bid = 150.25;
    quote.ask = 150.50;
    quote.last = 150.35;
    quote.volume = 1000000;
    
    std::string json = quote.to_json();
    
    // Parse it back to verify
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto parsed_quote = Quote::from_json(doc.value());
    EXPECT_EQ(parsed_quote.symbol, quote.symbol);
    EXPECT_DOUBLE_EQ(parsed_quote.bid, quote.bid);
}

TEST_F(JsonParsingTest, SerializeOrder) {
    Order order;
    order.id = "12345";
    order.type = OrderType::Limit;
    order.symbol = "AAPL";
    order.side = OrderSide::Buy;
    order.quantity = 100;
    order.status = OrderStatus::Pending;
    order.duration = OrderDuration::Day;
    order.price = 150.00;
    
    std::string json = order.to_json();
    
    // Verify it's valid JSON
    auto doc = parser.parse(json);
    EXPECT_EQ(doc.error(), simdjson::SUCCESS);
}

// Large JSON Handling Tests
TEST_F(JsonParsingTest, ParseLargeHistoricalDataset) {
    // Create a large historical data JSON
    std::string json = R"({"history": [)";
    for (int i = 0; i < 100; ++i) {
        if (i > 0) json += ",";
        json += R"({"date": "2023-01-)" + std::to_string(i+1) + R"(", "open": 150.0, "high": 151.0, "low": 149.0, "close": 150.5, "volume": 1000000})";
    }
    json += "]}";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto history_array = doc["history"].get_array();
    ASSERT_EQ(history_array.error(), simdjson::SUCCESS);
    
    int count = 0;
    for (const auto& elem : history_array.value()) {
        (void)elem; // Suppress unused warning
        count++;
    }
    EXPECT_EQ(count, 100);
}

// Special Character Handling Tests
TEST_F(JsonParsingTest, ParseJsonWithSpecialCharacters) {
    std::string json = R"({
        "symbol": "BRK.A",
        "description": "Berkshire Hathaway Inc. Class A\n\"Warren Buffett's Company\"",
        "last": 450000.00
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto quote = Quote::from_json(doc.value());
    EXPECT_EQ(quote.symbol, "BRK.A");
    EXPECT_TRUE(quote.description.find("Warren Buffett") != std::string::npos);
}

// Numeric Edge Cases Tests
TEST_F(JsonParsingTest, ParseNumericEdgeCases) {
    std::string json = R"({
        "symbol": "TEST",
        "bid": 0.0001,
        "ask": 999999.99,
        "volume": 9223372036854775807,
        "change_percentage": -99.99
    })";
    
    auto doc = parser.parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);
    
    auto quote = Quote::from_json(doc.value());
    EXPECT_DOUBLE_EQ(quote.bid, 0.0001);
    EXPECT_DOUBLE_EQ(quote.ask, 999999.99);
}