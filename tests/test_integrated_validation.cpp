/*
        oqdTradierpp - Full featured Tradier API library 
       
        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
        Version:           v1.0.0
        Release Date:  06/30/2025
        License: Apache 2.0
        Disclaimer: This software is provided "as-is" without warranties of any kind. 
                    Use at your own risk. The authors are not liable for any trading losses.
                    Not financial advice. By using this software, you accept all risks.

*/

#include <gtest/gtest.h>
#include <simdjson.h>
#include "oqdTradierpp/trading/order.hpp"
#include "oqdTradierpp/account/account_balances.hpp"
#include "oqdTradierpp/account/position.hpp"
#include "oqdTradierpp/response_validator.hpp"
#include "oqdTradierpp/error_codes.hpp"

using namespace oqd;

class IntegratedValidationTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = std::make_unique<simdjson::dom::parser>();
    }

    std::unique_ptr<simdjson::dom::parser> parser;
};

// ==========================================
// Order Validation Integration Tests
// ==========================================

TEST_F(IntegratedValidationTest, OrderValidation_ValidOrder_PassesAllLevels) {
    std::string json = R"({
        "id": "12345",
        "class": "equity",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": "100",
        "type": "limit",
        "duration": "day",
        "price": "150.50",
        "avg_fill_price": "0.00",
        "exec_quantity": "0",
        "last_fill_price": "0.00",
        "last_fill_quantity": "0",
        "remaining_quantity": "100",
        "status": "pending",
        "tag": "user_order",
        "create_date": "2023-01-01T09:30:00.000Z",
        "transaction_date": "2023-01-01T09:30:00.000Z"
    })";

    auto doc = parser->parse(json);
    auto order = Order::from_json(doc.value());

    // Test all validation levels
    EXPECT_TRUE(order.validate(ValidationLevel::None).is_valid);
    EXPECT_TRUE(order.validate(ValidationLevel::Basic).is_valid);
    EXPECT_TRUE(order.validate(ValidationLevel::Strict).is_valid);
    EXPECT_TRUE(order.validate(ValidationLevel::Paranoid).is_valid);
}

TEST_F(IntegratedValidationTest, OrderValidation_InvalidPrice_FailsStrictValidation) {
    std::string json = R"({
        "id": "12345",
        "class": "equity",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": "100",
        "type": "limit",
        "duration": "day",
        "price": "-10.50",
        "status": "pending"
    })";

    auto doc = parser->parse(json);
    auto order = Order::from_json(doc.value());

    EXPECT_TRUE(order.validate(ValidationLevel::Basic).is_valid);  // Basic doesn't check price logic
    
    auto strict_result = order.validate(ValidationLevel::Strict);
    EXPECT_FALSE(strict_result.is_valid);
    EXPECT_FALSE(strict_result.issues.empty());
    
    // Check that we have a price validation issue
    bool found_price_issue = false;
    for (const auto& issue : strict_result.issues) {
        if (issue.field_name == "price" && issue.severity == ValidationSeverity::Error) {
            found_price_issue = true;
            break;
        }
    }
    EXPECT_TRUE(found_price_issue);
}

TEST_F(IntegratedValidationTest, OrderValidation_GTCMarketOrder_FailsBusinessRules) {
    std::string json = R"({
        "id": "12345",
        "class": "equity",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": "100",
        "type": "market",
        "duration": "gtc",
        "status": "pending"
    })";

    auto doc = parser->parse(json);
    auto order = Order::from_json(doc.value());

    auto result = order.validate(ValidationLevel::Strict);
    EXPECT_FALSE(result.is_valid);
    
    // Check for business rule violation
    bool found_gtc_market_issue = false;
    for (const auto& issue : result.issues) {
        if (issue.message.find("GTC") != std::string::npos && 
            issue.message.find("market") != std::string::npos) {
            found_gtc_market_issue = true;
            break;
        }
    }
    EXPECT_TRUE(found_gtc_market_issue);
}

// ==========================================
// AccountBalances Validation Integration Tests
// ==========================================

TEST_F(IntegratedValidationTest, AccountBalances_ValidBalances_PassesValidation) {
    std::string json = R"({
        "account_number": "123456789",
        "account_value": "25000.00",
        "buying_power": "50000.00",
        "cash": {
            "cash_available": "5000.00",
            "sweep": "0.00",
            "unsettled_funds": "1000.00"
        },
        "margin": {
            "fed_call": "0.00",
            "maintenance_call": "0.00",
            "option_buying_power": "25000.00",
            "stock_buying_power": "50000.00",
            "stock_long_value": "20000.00",
            "stock_short_value": "0.00"
        },
        "pdt": {
            "flag": false,
            "day_trade_buying_power": "100000.00"
        }
    })";

    auto doc = parser->parse(json);
    auto balances = AccountBalances::from_json(doc.value());

    EXPECT_TRUE(balances.validate(ValidationLevel::Basic).is_valid);
    EXPECT_TRUE(balances.validate(ValidationLevel::Strict).is_valid);
    EXPECT_TRUE(balances.has_sufficient_buying_power(1000.0));
    EXPECT_FALSE(balances.is_at_risk());
}

TEST_F(IntegratedValidationTest, AccountBalances_InconsistentBalances_FailsValidation) {
    std::string json = R"({
        "account_number": "123456789",
        "account_value": "1000.00",
        "buying_power": "50000.00",
        "cash": {
            "cash_available": "60000.00",
            "sweep": "0.00",
            "unsettled_funds": "0.00"
        },
        "margin": {
            "fed_call": "5000.00",
            "maintenance_call": "3000.00",
            "option_buying_power": "0.00",
            "stock_buying_power": "0.00",
            "stock_long_value": "0.00",
            "stock_short_value": "0.00"
        }
    })";

    auto doc = parser->parse(json);
    auto balances = AccountBalances::from_json(doc.value());

    auto result = balances.validate(ValidationLevel::Strict);
    EXPECT_FALSE(result.is_valid);
    EXPECT_TRUE(balances.is_at_risk()); // Has margin calls
}

// ==========================================
// Position Validation Integration Tests
// ==========================================

TEST_F(IntegratedValidationTest, Position_ValidPosition_PassesValidation) {
    std::string json = R"({
        "cost_basis": "15000.00",
        "date_acquired": "2023-01-01T00:00:00.000Z",
        "id": "pos123",
        "quantity": "100.0",
        "symbol": "AAPL"
    })";

    auto doc = parser->parse(json);
    auto position = Position::from_json(doc.value());

    EXPECT_TRUE(position.validate(ValidationLevel::Basic).is_valid);
    EXPECT_TRUE(position.validate(ValidationLevel::Strict).is_valid);
    EXPECT_TRUE(position.is_valid());
    
    // Test cost basis calculations
    EXPECT_DOUBLE_EQ(position.get_average_cost(), 150.0); // 15000 / 100
    EXPECT_DOUBLE_EQ(position.get_unrealized_pnl(160.0), 1000.0); // (160 - 150) * 100
}

TEST_F(IntegratedValidationTest, Position_InvalidCostBasis_FailsValidation) {
    std::string json = R"({
        "cost_basis": "-1000.00",
        "date_acquired": "2023-01-01T00:00:00.000Z",
        "id": "pos123",
        "quantity": "100.0",
        "symbol": "AAPL"
    })";

    auto doc = parser->parse(json);
    auto position = Position::from_json(doc.value());

    auto result = position.validate(ValidationLevel::Strict);
    EXPECT_FALSE(result.is_valid);
    EXPECT_FALSE(position.is_valid());
    
    // Check for cost basis validation issue
    bool found_cost_basis_issue = false;
    for (const auto& issue : result.issues) {
        if (issue.field_name == "cost_basis" && issue.severity == ValidationSeverity::Error) {
            found_cost_basis_issue = true;
            break;
        }
    }
    EXPECT_TRUE(found_cost_basis_issue);
}

TEST_F(IntegratedValidationTest, Position_ZeroQuantity_FailsValidation) {
    std::string json = R"({
        "cost_basis": "15000.00",
        "date_acquired": "2023-01-01T00:00:00.000Z",
        "id": "pos123",
        "quantity": "0.0",
        "symbol": "AAPL"
    })";

    auto doc = parser->parse(json);
    auto position = Position::from_json(doc.value());

    auto result = position.validate(ValidationLevel::Strict);
    EXPECT_FALSE(result.is_valid);
    
    // Check for quantity validation issue
    bool found_quantity_issue = false;
    for (const auto& issue : result.issues) {
        if (issue.field_name == "quantity" && issue.severity == ValidationSeverity::Error) {
            found_quantity_issue = true;
            break;
        }
    }
    EXPECT_TRUE(found_quantity_issue);
}

// ==========================================
// Cross-Class Integration Tests
// ==========================================

TEST_F(IntegratedValidationTest, OrderWithMarketData_ValidatesCorrectly) {
    std::string order_json = R"({
        "id": "12345",
        "class": "equity",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": "100",
        "type": "limit",
        "duration": "day",
        "price": "150.50",
        "status": "pending"
    })";

    std::string quote_json = R"({
        "symbol": "AAPL",
        "last": "151.00",
        "bid": "150.75",
        "ask": "151.25",
        "bidsize": 100,
        "asksize": 200,
        "volume": 1000000
    })";

    auto order_doc = parser->parse(order_json);
    auto quote_doc = parser->parse(quote_json);
    
    auto order = Order::from_json(order_doc.value());
    // Note: Quote class integration would be implemented similarly
    
    // Test order validation without market data
    EXPECT_TRUE(order.validate(ValidationLevel::Strict).is_valid);
    
    // Test that order price is reasonable relative to market (would need Quote class)
    EXPECT_GT(order.price, 0.0);
    EXPECT_LT(order.price, 1000.0); // Sanity check
}

// ==========================================
// Error Code Integration Tests
// ==========================================

TEST_F(IntegratedValidationTest, ValidationWithErrorCodes_ProducesCorrectErrorInfo) {
    std::string json = R"({
        "id": "12345",
        "class": "invalid_class",
        "symbol": "",
        "side": "buy",
        "quantity": "-100",
        "type": "limit",
        "duration": "day",
        "price": "150.50",
        "status": "pending"
    })";

    auto doc = parser->parse(json);
    auto order = Order::from_json(doc.value());

    auto result = order.validate(ValidationLevel::Strict);
    EXPECT_FALSE(result.is_valid);
    
    // Check that we have multiple validation issues
    EXPECT_GT(result.issues.size(), 1);
    
    // Verify different types of validation errors
    bool has_field_error = false;
    bool has_quantity_error = false;
    
    for (const auto& issue : result.issues) {
        if (issue.field_name == "class" || issue.field_name == "symbol") {
            has_field_error = true;
        }
        if (issue.field_name == "quantity") {
            has_quantity_error = true;
        }
    }
    
    EXPECT_TRUE(has_field_error);
    EXPECT_TRUE(has_quantity_error);
}

// ==========================================
// Performance and Edge Case Tests
// ==========================================

TEST_F(IntegratedValidationTest, ValidationPerformance_HandlesLargeDataSets) {
    // Test validation performance with multiple objects
    std::vector<Order> orders;
    
    for (int i = 0; i < 100; ++i) {
        std::string json = R"({
            "id": ")" + std::to_string(i) + R"(",
            "class": "equity",
            "symbol": "AAPL",
            "side": "buy",
            "quantity": "100",
            "type": "limit",
            "duration": "day",
            "price": "150.50",
            "status": "pending"
        })";
        
        auto doc = parser->parse(json);
        orders.push_back(Order::from_json(doc.value()));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int valid_count = 0;
    for (const auto& order : orders) {
        if (order.validate(ValidationLevel::Strict).is_valid) {
            valid_count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(valid_count, 100);
    EXPECT_LT(duration.count(), 1000); // Should complete in under 1 second
}

TEST_F(IntegratedValidationTest, ValidationLevels_ProduceConsistentResults) {
    std::string json = R"({
        "id": "12345",
        "class": "equity",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": "100",
        "type": "limit",
        "duration": "day",
        "price": "150.50",
        "status": "pending"
    })";

    auto doc = parser->parse(json);
    auto order = Order::from_json(doc.value());

    // Validation levels should be hierarchical
    auto none_result = order.validate(ValidationLevel::None);
    auto basic_result = order.validate(ValidationLevel::Basic);
    auto strict_result = order.validate(ValidationLevel::Strict);
    auto paranoid_result = order.validate(ValidationLevel::Paranoid);

    // None should always pass (no validation)
    EXPECT_TRUE(none_result.is_valid);
    
    // If basic fails, all higher levels should fail
    if (!basic_result.is_valid) {
        EXPECT_FALSE(strict_result.is_valid);
        EXPECT_FALSE(paranoid_result.is_valid);
    }
    
    // If strict fails, paranoid should fail
    if (!strict_result.is_valid) {
        EXPECT_FALSE(paranoid_result.is_valid);
    }
    
    // Issue counts should be non-decreasing as validation level increases
    EXPECT_LE(none_result.issues.size(), basic_result.issues.size());
    EXPECT_LE(basic_result.issues.size(), strict_result.issues.size());
    EXPECT_LE(strict_result.issues.size(), paranoid_result.issues.size());
}