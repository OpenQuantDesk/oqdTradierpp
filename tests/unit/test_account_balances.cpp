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
#include "oqdTradierpp/account/account_balances.hpp"
#include <simdjson.h>

using namespace oqd;

class AccountBalancesTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AccountBalancesTest, DefaultConstruction) {
    AccountBalances balances;
    EXPECT_EQ(balances.account_number, 0.0);
    EXPECT_EQ(balances.total_equity, 0.0);
    EXPECT_EQ(balances.long_market_value, 0.0);
    EXPECT_EQ(balances.short_market_value, 0.0);
    EXPECT_EQ(balances.account_value, 0.0);
    EXPECT_EQ(balances.close_pl, 0.0);
    EXPECT_EQ(balances.current_requirement, 0.0);
    EXPECT_EQ(balances.equity, 0.0);
    EXPECT_EQ(balances.long_liquid_value, 0.0);
    EXPECT_EQ(balances.long_market_value_bp, 0.0);
    EXPECT_EQ(balances.short_liquid_value, 0.0);
    EXPECT_EQ(balances.short_market_value_bp, 0.0);
    EXPECT_EQ(balances.uncleared_funds, 0.0);
    EXPECT_EQ(balances.pending_orders_count, 0.0);
    EXPECT_EQ(balances.option_short_value, 0.0);
    EXPECT_EQ(balances.total_cash, 0.0);
    EXPECT_EQ(balances.unsettled_funds, 0.0);
    EXPECT_EQ(balances.dividend, 0.0);
    EXPECT_EQ(balances.cash, 0.0);
    EXPECT_EQ(balances.market_value, 0.0);
}

TEST_F(AccountBalancesTest, FieldAssignment) {
    AccountBalances balances;
    balances.account_number = 12345678.0;
    balances.total_equity = 50000.00;
    balances.total_cash = 25000.00;
    balances.pending_orders_count = 5.0;
    balances.long_market_value = 30000.00;
    balances.short_market_value = -5000.00;
    
    EXPECT_EQ(balances.account_number, 12345678.0);
    EXPECT_EQ(balances.total_equity, 50000.00);
    EXPECT_EQ(balances.total_cash, 25000.00);
    EXPECT_EQ(balances.pending_orders_count, 5.0);
    EXPECT_EQ(balances.long_market_value, 30000.00);
    EXPECT_EQ(balances.short_market_value, -5000.00);
}

TEST_F(AccountBalancesTest, FromJsonDeserialization) {
    std::string json_str = R"({
        "account_number": 87654321.0,
        "total_equity": 75000.25,
        "long_market_value": 50000.00,
        "short_market_value": -1000.00,
        "account_value": 74000.25,
        "close_pl": 1500.75,
        "current_requirement": 2500.00,
        "equity": 75000.25,
        "long_liquid_value": 49000.00,
        "long_market_value_bp": 48000.00,
        "short_liquid_value": -900.00,
        "short_market_value_bp": -1100.00,
        "uncleared_funds": 0.00,
        "pending_orders_count": 3.0,
        "option_short_value": -500.00,
        "total_cash": 25000.25,
        "unsettled_funds": 1000.00,
        "dividend": 150.00,
        "cash": 24000.25,
        "market_value": 49000.00
    })";
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json_str);
    AccountBalances balances = AccountBalances::from_json(doc.value());
    
    EXPECT_EQ(balances.account_number, 87654321.0);
    EXPECT_EQ(balances.total_equity, 75000.25);
    EXPECT_EQ(balances.long_market_value, 50000.00);
    EXPECT_EQ(balances.short_market_value, -1000.00);
    EXPECT_EQ(balances.account_value, 74000.25);
    EXPECT_EQ(balances.close_pl, 1500.75);
    EXPECT_EQ(balances.current_requirement, 2500.00);
    EXPECT_EQ(balances.equity, 75000.25);
    EXPECT_EQ(balances.long_liquid_value, 49000.00);
    EXPECT_EQ(balances.long_market_value_bp, 48000.00);
    EXPECT_EQ(balances.short_liquid_value, -900.00);
    EXPECT_EQ(balances.short_market_value_bp, -1100.00);
    EXPECT_EQ(balances.uncleared_funds, 0.00);
    EXPECT_EQ(balances.pending_orders_count, 3.0);
    EXPECT_EQ(balances.option_short_value, -500.00);
    EXPECT_EQ(balances.total_cash, 25000.25);
    EXPECT_EQ(balances.unsettled_funds, 1000.00);
    EXPECT_EQ(balances.dividend, 150.00);
    EXPECT_EQ(balances.cash, 24000.25);
    EXPECT_EQ(balances.market_value, 49000.00);
}

TEST_F(AccountBalancesTest, ToJsonSerialization) {
    AccountBalances balances;
    balances.account_number = 123456.0;
    balances.total_equity = 100000.50;
    balances.total_cash = 50000.25;
    balances.pending_orders_count = 7.0;
    balances.long_market_value = 75000.00;
    balances.market_value = 74500.00;
    
    std::string json = balances.to_json();
    
    EXPECT_TRUE(json.find("\"account_number\":123456") != std::string::npos);
    EXPECT_TRUE(json.find("\"total_equity\":100000.50") != std::string::npos);
    EXPECT_TRUE(json.find("\"total_cash\":50000.25") != std::string::npos);
    EXPECT_TRUE(json.find("\"pending_orders_count\":7") != std::string::npos);
    EXPECT_TRUE(json.find("\"long_market_value\":75000") != std::string::npos);
    EXPECT_TRUE(json.find("\"market_value\":74500") != std::string::npos);
}

TEST_F(AccountBalancesTest, RoundTripSerialization) {
    AccountBalances original;
    original.account_number = 999888.0;
    original.total_equity = 123456.78;
    original.total_cash = 65432.10;
    original.long_market_value = 58024.68;
    original.pending_orders_count = 10.0;
    original.close_pl = 1500.25;
    original.dividend = 250.50;
    original.cash = 65181.60;
    original.market_value = 58024.68;
    
    std::string json = original.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccountBalances deserialized = AccountBalances::from_json(doc.value());
    
    EXPECT_EQ(deserialized.account_number, original.account_number);
    EXPECT_EQ(deserialized.total_equity, original.total_equity);
    EXPECT_EQ(deserialized.total_cash, original.total_cash);
    EXPECT_EQ(deserialized.long_market_value, original.long_market_value);
    EXPECT_EQ(deserialized.pending_orders_count, original.pending_orders_count);
    EXPECT_EQ(deserialized.close_pl, original.close_pl);
    EXPECT_EQ(deserialized.dividend, original.dividend);
    EXPECT_EQ(deserialized.cash, original.cash);
    EXPECT_EQ(deserialized.market_value, original.market_value);
}

TEST_F(AccountBalancesTest, NegativeValues) {
    AccountBalances balances;
    balances.option_short_value = -5000.25;
    balances.short_market_value = -10000.50;
    balances.close_pl = -1000.00;
    balances.short_liquid_value = -4500.25;
    balances.short_market_value_bp = -9500.50;
    
    std::string json = balances.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccountBalances deserialized = AccountBalances::from_json(doc.value());
    
    EXPECT_EQ(deserialized.option_short_value, -5000.25);
    EXPECT_EQ(deserialized.short_market_value, -10000.50);
    EXPECT_EQ(deserialized.close_pl, -1000.00);
    EXPECT_EQ(deserialized.short_liquid_value, -4500.25);
    EXPECT_EQ(deserialized.short_market_value_bp, -9500.50);
}

TEST_F(AccountBalancesTest, ZeroValues) {
    AccountBalances balances;
    
    std::string json = balances.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccountBalances deserialized = AccountBalances::from_json(doc.value());
    
    EXPECT_EQ(deserialized.total_equity, 0.0);
    EXPECT_EQ(deserialized.total_cash, 0.0);
    EXPECT_EQ(deserialized.pending_orders_count, 0.0);
    EXPECT_EQ(deserialized.long_market_value, 0.0);
    EXPECT_EQ(deserialized.market_value, 0.0);
}

TEST_F(AccountBalancesTest, LargeValues) {
    AccountBalances balances;
    balances.total_equity = 999999999.99;
    balances.long_market_value = 500000000.00;
    balances.total_cash = 499999999.99;
    balances.account_number = 2147483647.0;
    balances.pending_orders_count = 1000.0;
    
    std::string json = balances.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccountBalances deserialized = AccountBalances::from_json(doc.value());
    
    EXPECT_EQ(deserialized.total_equity, 999999999.99);
    EXPECT_EQ(deserialized.long_market_value, 500000000.00);
    EXPECT_EQ(deserialized.total_cash, 499999999.99);
    EXPECT_EQ(deserialized.account_number, 2147483647.0);
    EXPECT_EQ(deserialized.pending_orders_count, 1000.0);
}