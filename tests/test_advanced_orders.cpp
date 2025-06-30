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
#include "oqdTradierpp/validation.hpp"
#include <string>

using namespace oqd;

class AdvancedOrdersTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test data setup
    }
};

// Test OrderClass enum with new values
TEST_F(AdvancedOrdersTest, OrderClassToString) {
    EXPECT_EQ(to_string(OrderClass::Equity), "equity");
    EXPECT_EQ(to_string(OrderClass::Option), "option");
    EXPECT_EQ(to_string(OrderClass::Multileg), "multileg");
    EXPECT_EQ(to_string(OrderClass::Combo), "combo");
    EXPECT_EQ(to_string(OrderClass::OTO), "oto");
    EXPECT_EQ(to_string(OrderClass::OCO), "oco");
    EXPECT_EQ(to_string(OrderClass::OTOCO), "otoco");
}

TEST_F(AdvancedOrdersTest, OrderClassFromString) {
    EXPECT_EQ(order_class_from_string("equity"), OrderClass::Equity);
    EXPECT_EQ(order_class_from_string("option"), OrderClass::Option);
    EXPECT_EQ(order_class_from_string("multileg"), OrderClass::Multileg);
    EXPECT_EQ(order_class_from_string("combo"), OrderClass::Combo);
    EXPECT_EQ(order_class_from_string("oto"), OrderClass::OTO);
    EXPECT_EQ(order_class_from_string("oco"), OrderClass::OCO);
    EXPECT_EQ(order_class_from_string("otoco"), OrderClass::OTOCO);
}

// Test OTO Order Request
TEST_F(AdvancedOrdersTest, OTOOrderRequestToJson) {
    OTOOrderRequest oto_order;
    oto_order.tag = "test_oto";
    
    // First order
    oto_order.first_order.symbol = "AAPL";
    oto_order.first_order.side = OrderSide::Buy;
    oto_order.first_order.quantity = 100;
    oto_order.first_order.type = OrderType::Market;
    oto_order.first_order.duration = OrderDuration::Day;
    
    // Second order
    oto_order.second_order.symbol = "AAPL";
    oto_order.second_order.side = OrderSide::Sell;
    oto_order.second_order.quantity = 100;
    oto_order.second_order.type = OrderType::Limit;
    oto_order.second_order.duration = OrderDuration::GTC;
    oto_order.second_order.price = 160.00;
    
    std::string json = oto_order.to_json();
    EXPECT_TRUE(json.find("\"class\":\"oto\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"tag\":\"test_oto\"") != std::string::npos);
    EXPECT_TRUE(json.find("AAPL") != std::string::npos);
}

// Test OCO Order Request
TEST_F(AdvancedOrdersTest, OCOOrderRequestToJson) {
    OCOOrderRequest oco_order;
    oco_order.tag = "test_oco";
    
    // First order (profit target)
    oco_order.first_order.symbol = "MSFT";
    oco_order.first_order.side = OrderSide::Sell;
    oco_order.first_order.quantity = 50;
    oco_order.first_order.type = OrderType::Limit;
    oco_order.first_order.duration = OrderDuration::GTC;
    oco_order.first_order.price = 320.00;
    
    // Second order (stop loss)
    oco_order.second_order.symbol = "MSFT";
    oco_order.second_order.side = OrderSide::Sell;
    oco_order.second_order.quantity = 50;
    oco_order.second_order.type = OrderType::Stop;
    oco_order.second_order.duration = OrderDuration::GTC;
    oco_order.second_order.stop = 280.00;
    
    std::string json = oco_order.to_json();
    EXPECT_TRUE(json.find("\"class\":\"oco\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"tag\":\"test_oco\"") != std::string::npos);
    EXPECT_TRUE(json.find("MSFT") != std::string::npos);
}

// Test OTOCO Order Request (Bracket Order)
TEST_F(AdvancedOrdersTest, OTOCOOrderRequestToJson) {
    OTOCOOrderRequest otoco_order;
    otoco_order.tag = "test_bracket";
    
    // Primary order (entry)
    otoco_order.primary_order.symbol = "TSLA";
    otoco_order.primary_order.side = OrderSide::Buy;
    otoco_order.primary_order.quantity = 25;
    otoco_order.primary_order.type = OrderType::Limit;
    otoco_order.primary_order.duration = OrderDuration::Day;
    otoco_order.primary_order.price = 200.00;
    
    // Profit order
    otoco_order.profit_order.symbol = "TSLA";
    otoco_order.profit_order.side = OrderSide::Sell;
    otoco_order.profit_order.quantity = 25;
    otoco_order.profit_order.type = OrderType::Limit;
    otoco_order.profit_order.duration = OrderDuration::GTC;
    otoco_order.profit_order.price = 230.00;
    
    // Stop order
    otoco_order.stop_order.symbol = "TSLA";
    otoco_order.stop_order.side = OrderSide::Sell;
    otoco_order.stop_order.quantity = 25;
    otoco_order.stop_order.type = OrderType::Stop;
    otoco_order.stop_order.duration = OrderDuration::GTC;
    otoco_order.stop_order.stop = 180.00;
    
    std::string json = otoco_order.to_json();
    EXPECT_TRUE(json.find("\"class\":\"otoco\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"tag\":\"test_bracket\"") != std::string::npos);
    EXPECT_TRUE(json.find("TSLA") != std::string::npos);
    EXPECT_TRUE(json.find("primary_order") != std::string::npos);
    EXPECT_TRUE(json.find("profit_order") != std::string::npos);
    EXPECT_TRUE(json.find("stop_order") != std::string::npos);
}

// Test Spread Order Request
TEST_F(AdvancedOrdersTest, SpreadOrderRequestToJson) {
    SpreadOrderRequest spread_order;
    spread_order.type = OrderType::Limit;
    spread_order.duration = OrderDuration::Day;
    spread_order.price = 2.50;
    spread_order.spread_type = "vertical_call_bull";
    spread_order.tag = "test_spread";
    
    // Long leg
    SpreadLeg long_leg;
    long_leg.option_symbol = "SPY240315C00450000";
    long_leg.side = OrderSide::BuyToOpen;
    long_leg.quantity = 1;
    long_leg.ratio = 1.0;
    
    // Short leg
    SpreadLeg short_leg;
    short_leg.option_symbol = "SPY240315C00460000";
    short_leg.side = OrderSide::SellToOpen;
    short_leg.quantity = 1;
    short_leg.ratio = 1.0;
    
    spread_order.legs.push_back(long_leg);
    spread_order.legs.push_back(short_leg);
    
    std::string json = spread_order.to_json();
    EXPECT_TRUE(json.find("\"class\":\"multileg\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"spread_type\":\"vertical_call_bull\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"tag\":\"test_spread\"") != std::string::npos);
    EXPECT_TRUE(json.find("SPY240315C00450000") != std::string::npos);
    EXPECT_TRUE(json.find("SPY240315C00460000") != std::string::npos);
}

// Test Order Validation
TEST_F(AdvancedOrdersTest, OrderValidation) {
    // Test equity order validation
    EquityOrderRequest equity_order;
    equity_order.symbol = "AAPL";
    equity_order.side = OrderSide::Buy;
    equity_order.quantity = 100;
    equity_order.type = OrderType::Market;
    equity_order.duration = OrderDuration::Day;
    
    auto result = OrderValidator::validate_equity_order(equity_order);
    EXPECT_TRUE(result.is_valid);
    EXPECT_TRUE(result.errors.empty());
    
    // Test invalid equity order
    equity_order.symbol = ""; // Invalid symbol
    result = OrderValidator::validate_equity_order(equity_order);
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(result.errors.empty());
}

TEST_F(AdvancedOrdersTest, OTOOrderValidation) {
    OTOOrderRequest oto_order;
    oto_order.tag = "test_oto";
    
    // Valid first order
    oto_order.first_order.symbol = "AAPL";
    oto_order.first_order.side = OrderSide::Buy;
    oto_order.first_order.quantity = 100;
    oto_order.first_order.type = OrderType::Market;
    oto_order.first_order.duration = OrderDuration::Day;
    
    // Valid second order
    oto_order.second_order.symbol = "AAPL";
    oto_order.second_order.side = OrderSide::Sell;
    oto_order.second_order.quantity = 100;
    oto_order.second_order.type = OrderType::Limit;
    oto_order.second_order.duration = OrderDuration::GTC;
    oto_order.second_order.price = 160.00;
    
    auto result = OrderValidator::validate_oto_order(oto_order);
    EXPECT_TRUE(result.is_valid);
}

TEST_F(AdvancedOrdersTest, UtilityFunctions) {
    // Test symbol validation
    EXPECT_TRUE(OrderValidator::is_valid_symbol("AAPL"));
    EXPECT_TRUE(OrderValidator::is_valid_symbol("SPY"));
    EXPECT_TRUE(OrderValidator::is_valid_symbol("BRK.A"));
    EXPECT_FALSE(OrderValidator::is_valid_symbol(""));
    EXPECT_FALSE(OrderValidator::is_valid_symbol("TOOLONGSYMBOL"));
    
    // Test option symbol validation
    EXPECT_TRUE(OrderValidator::is_valid_option_symbol("AAPL240315C00150000"));
    EXPECT_TRUE(OrderValidator::is_valid_option_symbol("SPY240315P00450000"));
    EXPECT_FALSE(OrderValidator::is_valid_option_symbol("INVALID"));
    EXPECT_FALSE(OrderValidator::is_valid_option_symbol(""));
    
    // Test price validation
    EXPECT_TRUE(OrderValidator::is_valid_price(100.50));
    EXPECT_TRUE(OrderValidator::is_valid_price(0.01));
    EXPECT_FALSE(OrderValidator::is_valid_price(0.0));
    EXPECT_FALSE(OrderValidator::is_valid_price(-10.0));
    
    // Test quantity validation
    EXPECT_TRUE(OrderValidator::is_valid_quantity(100));
    EXPECT_TRUE(OrderValidator::is_valid_quantity(1));
    EXPECT_FALSE(OrderValidator::is_valid_quantity(0));
    EXPECT_FALSE(OrderValidator::is_valid_quantity(-100));
    
    // Test spread type validation
    EXPECT_TRUE(OrderValidator::is_spread_type_supported("vertical"));
    EXPECT_TRUE(OrderValidator::is_spread_type_supported("iron_condor"));
    EXPECT_TRUE(OrderValidator::is_spread_type_supported("butterfly"));
    EXPECT_FALSE(OrderValidator::is_spread_type_supported("invalid_spread"));
}

// Test risk calculations
TEST_F(AdvancedOrdersTest, RiskCalculations) {
    OTOCOOrderRequest bracket_order;
    
    // Entry order
    bracket_order.primary_order.symbol = "AAPL";
    bracket_order.primary_order.side = OrderSide::Buy;
    bracket_order.primary_order.quantity = 100;
    bracket_order.primary_order.type = OrderType::Limit;
    bracket_order.primary_order.price = 150.00;
    
    // Profit order
    bracket_order.profit_order.symbol = "AAPL";
    bracket_order.profit_order.side = OrderSide::Sell;
    bracket_order.profit_order.quantity = 100;
    bracket_order.profit_order.type = OrderType::Limit;
    bracket_order.profit_order.price = 160.00;
    
    // Stop order
    bracket_order.stop_order.symbol = "AAPL";
    bracket_order.stop_order.side = OrderSide::Sell;
    bracket_order.stop_order.quantity = 100;
    bracket_order.stop_order.type = OrderType::Stop;
    bracket_order.stop_order.stop = 140.00;
    
    double max_profit = OrderValidator::calculate_max_profit_otoco(bracket_order);
    double max_loss = OrderValidator::calculate_max_loss_otoco(bracket_order);
    double risk_reward = OrderValidator::calculate_risk_reward_ratio(bracket_order);
    
    EXPECT_DOUBLE_EQ(max_profit, 1000.0); // (160 - 150) * 100
    EXPECT_DOUBLE_EQ(max_loss, 1000.0);   // (150 - 140) * 100
    EXPECT_DOUBLE_EQ(risk_reward, 1.0);   // 1000 / 1000
}