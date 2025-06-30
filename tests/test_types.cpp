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

using namespace oqd;

class TypesTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = std::make_unique<simdjson::dom::parser>();
    }

    std::unique_ptr<simdjson::dom::parser> parser_;
};

TEST_F(TypesTest, OrderClassEnumConversion) {
    EXPECT_EQ(to_string(OrderClass::Equity), "equity");
    EXPECT_EQ(to_string(OrderClass::Option), "option");
    EXPECT_EQ(to_string(OrderClass::Multileg), "multileg");
    EXPECT_EQ(to_string(OrderClass::Combo), "combo");

    EXPECT_EQ(order_class_from_string("equity"), OrderClass::Equity);
    EXPECT_EQ(order_class_from_string("option"), OrderClass::Option);
    EXPECT_EQ(order_class_from_string("multileg"), OrderClass::Multileg);
    EXPECT_EQ(order_class_from_string("combo"), OrderClass::Combo);
    EXPECT_EQ(order_class_from_string("invalid"), OrderClass::Equity); // Default
}

TEST_F(TypesTest, OrderTypeEnumConversion) {
    EXPECT_EQ(to_string(OrderType::Market), "market");
    EXPECT_EQ(to_string(OrderType::Limit), "limit");
    EXPECT_EQ(to_string(OrderType::Stop), "stop");
    EXPECT_EQ(to_string(OrderType::StopLimit), "stop_limit");

    EXPECT_EQ(order_type_from_string("market"), OrderType::Market);
    EXPECT_EQ(order_type_from_string("limit"), OrderType::Limit);
    EXPECT_EQ(order_type_from_string("stop"), OrderType::Stop);
    EXPECT_EQ(order_type_from_string("stop_limit"), OrderType::StopLimit);
    EXPECT_EQ(order_type_from_string("invalid"), OrderType::Market); // Default
}

TEST_F(TypesTest, OrderDurationEnumConversion) {
    EXPECT_EQ(to_string(OrderDuration::Day), "day");
    EXPECT_EQ(to_string(OrderDuration::GTC), "gtc");
    EXPECT_EQ(to_string(OrderDuration::Pre), "pre");
    EXPECT_EQ(to_string(OrderDuration::Post), "post");

    EXPECT_EQ(order_duration_from_string("day"), OrderDuration::Day);
    EXPECT_EQ(order_duration_from_string("gtc"), OrderDuration::GTC);
    EXPECT_EQ(order_duration_from_string("pre"), OrderDuration::Pre);
    EXPECT_EQ(order_duration_from_string("post"), OrderDuration::Post);
    EXPECT_EQ(order_duration_from_string("invalid"), OrderDuration::Day); // Default
}

TEST_F(TypesTest, OrderSideEnumConversion) {
    EXPECT_EQ(to_string(OrderSide::Buy), "buy");
    EXPECT_EQ(to_string(OrderSide::Sell), "sell");
    EXPECT_EQ(to_string(OrderSide::SellShort), "sell_short");
    EXPECT_EQ(to_string(OrderSide::BuyToOpen), "buy_to_open");
    EXPECT_EQ(to_string(OrderSide::BuyToClose), "buy_to_close");
    EXPECT_EQ(to_string(OrderSide::SellToOpen), "sell_to_open");
    EXPECT_EQ(to_string(OrderSide::SellToClose), "sell_to_close");

    EXPECT_EQ(order_side_from_string("buy"), OrderSide::Buy);
    EXPECT_EQ(order_side_from_string("sell"), OrderSide::Sell);
    EXPECT_EQ(order_side_from_string("sell_short"), OrderSide::SellShort);
    EXPECT_EQ(order_side_from_string("buy_to_open"), OrderSide::BuyToOpen);
    EXPECT_EQ(order_side_from_string("buy_to_close"), OrderSide::BuyToClose);
    EXPECT_EQ(order_side_from_string("sell_to_open"), OrderSide::SellToOpen);
    EXPECT_EQ(order_side_from_string("sell_to_close"), OrderSide::SellToClose);
    EXPECT_EQ(order_side_from_string("invalid"), OrderSide::Buy); // Default
}

TEST_F(TypesTest, OrderStatusEnumConversion) {
    EXPECT_EQ(to_string(OrderStatus::Open), "open");
    EXPECT_EQ(to_string(OrderStatus::PartiallyFilled), "partially_filled");
    EXPECT_EQ(to_string(OrderStatus::Filled), "filled");
    EXPECT_EQ(to_string(OrderStatus::Expired), "expired");
    EXPECT_EQ(to_string(OrderStatus::Canceled), "canceled");
    EXPECT_EQ(to_string(OrderStatus::Pending), "pending");
    EXPECT_EQ(to_string(OrderStatus::Rejected), "rejected");

    EXPECT_EQ(order_status_from_string("open"), OrderStatus::Open);
    EXPECT_EQ(order_status_from_string("partially_filled"), OrderStatus::PartiallyFilled);
    EXPECT_EQ(order_status_from_string("filled"), OrderStatus::Filled);
    EXPECT_EQ(order_status_from_string("expired"), OrderStatus::Expired);
    EXPECT_EQ(order_status_from_string("canceled"), OrderStatus::Canceled);
    EXPECT_EQ(order_status_from_string("pending"), OrderStatus::Pending);
    EXPECT_EQ(order_status_from_string("rejected"), OrderStatus::Rejected);
    EXPECT_EQ(order_status_from_string("invalid"), OrderStatus::Open); // Default
}

TEST_F(TypesTest, AccessTokenSerialization) {
    AccessToken token;
    token.access_token = "abc123";
    token.token_type = "Bearer";
    token.refresh_token = "refresh123";
    token.expires_in = 3600;
    token.scope = "read write";

    std::string json = token.to_json();
    EXPECT_TRUE(json.find("\"access_token\":\"abc123\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"token_type\":\"Bearer\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"refresh_token\":\"refresh123\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"expires_in\":3600") != std::string::npos);
    EXPECT_TRUE(json.find("\"scope\":\"read write\"") != std::string::npos);
}

TEST_F(TypesTest, AccessTokenDeserialization) {
    std::string json = R"({
        "access_token": "test_token",
        "token_type": "Bearer",
        "refresh_token": "test_refresh",
        "expires_in": 7200,
        "scope": "read"
    })";

    auto doc = parser_->parse(json);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    AccessToken token = AccessToken::from_json(doc.value());
    EXPECT_EQ(token.access_token, "test_token");
    EXPECT_EQ(token.token_type, "Bearer");
    EXPECT_EQ(token.refresh_token, "test_refresh");
    EXPECT_EQ(token.expires_in, 7200);
    EXPECT_EQ(token.scope, "read");
}

TEST_F(TypesTest, QuoteSerialization) {
    Quote quote;
    quote.symbol = "AAPL";
    quote.description = "Apple Inc";
    quote.last = 150.25;
    quote.bid = 150.20;
    quote.ask = 150.30;
    quote.volume = 1000000;

    std::string json = quote.to_json();
    EXPECT_TRUE(json.find("\"symbol\":\"AAPL\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"description\":\"Apple Inc\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"last\":150.25") != std::string::npos);
}

TEST_F(TypesTest, QuoteWithOptionsFields) {
    Quote quote;
    quote.symbol = "AAPL220121C00150000";
    quote.strike = 150.0;
    quote.expiration_date = "2022-01-21";
    quote.option_type = "call";
    quote.delta = 0.6;
    quote.gamma = 0.02;

    std::string json = quote.to_json();
    EXPECT_TRUE(json.find("\"strike\":150.00") != std::string::npos);
    EXPECT_TRUE(json.find("\"expiration_date\":\"2022-01-21\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"delta\":0.60") != std::string::npos);
    EXPECT_TRUE(json.find("\"gamma\":0.02") != std::string::npos);
}

TEST_F(TypesTest, UserProfileSerialization) {
    UserProfile profile;
    profile.id = "user123";
    profile.name = "John Doe";
    profile.account = {"account1", "account2"};

    std::string json = profile.to_json();
    EXPECT_TRUE(json.find("\"id\":\"user123\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"name\":\"John Doe\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"account\":[\"account1\",\"account2\"]") != std::string::npos);
}

TEST_F(TypesTest, CompanyInfoPlaceholder) {
    CompanyInfo info;
    info.symbol = "AAPL";
    info.name = "Apple Inc.";
    info.market_cap = 2800000000000.0; // 2.8T
    info.pe_ratio = 28.5;

    std::string json = info.to_json();
    EXPECT_TRUE(json.find("\"symbol\":\"AAPL\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"name\":\"Apple Inc.\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"market_cap\":2800000000000") != std::string::npos);
    EXPECT_TRUE(json.find("\"pe_ratio\":28.5") != std::string::npos);
}

TEST_F(TypesTest, FinancialRatiosPlaceholder) {
    FinancialRatios ratios;
    ratios.symbol = "AAPL";
    ratios.price_to_earnings = 28.5;
    ratios.price_to_book = 39.4;
    ratios.debt_to_equity = 1.73;

    std::string json = ratios.to_json();
    EXPECT_TRUE(json.find("\"symbol\":\"AAPL\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"price_to_earnings\":28.5") != std::string::npos);
    EXPECT_TRUE(json.find("\"price_to_book\":39.4") != std::string::npos);
    EXPECT_TRUE(json.find("\"debt_to_equity\":1.73") != std::string::npos);
}

// Test order request types
TEST_F(TypesTest, EquityOrderRequest) {
    EquityOrderRequest order;
    order.symbol = "AAPL";
    order.side = OrderSide::Buy;
    order.quantity = 100;
    order.type = OrderType::Limit;
    order.duration = OrderDuration::Day;
    order.price = 150.00;

    // Test that it's properly set as equity class
    EXPECT_EQ(order.order_class, OrderClass::Equity);
}

TEST_F(TypesTest, OptionOrderRequest) {
    OptionOrderRequest order;
    order.option_symbol = "AAPL220121C00150000";
    order.side = OrderSide::BuyToOpen;
    order.quantity = 1;
    order.type = OrderType::Limit;
    order.duration = OrderDuration::Day;
    order.price = 5.50;

    // Test that it's properly set as option class
    EXPECT_EQ(order.order_class, OrderClass::Option);

    std::string json = order.to_json();
    EXPECT_TRUE(json.find("\"option_symbol\":\"AAPL220121C00150000\"") != std::string::npos);
}