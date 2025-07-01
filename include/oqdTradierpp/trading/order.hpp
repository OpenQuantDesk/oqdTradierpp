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

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <simdjson.h>
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/response_validator.hpp"

namespace oqd {

struct Leg {
    std::string option_symbol;
    OrderSide side;
    int quantity;
    
    static Leg from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
    
    // Validation methods
    ValidationResult validate(ValidationLevel level = ValidationLevel::Basic) const;
    bool is_valid() const;
    std::vector<ValidationIssue> validate_leg_constraints() const;
};

struct Order {
    std::string id;
    OrderType type;
    std::string symbol;
    OrderSide side;
    int quantity;
    OrderStatus status;
    OrderDuration duration;
    std::optional<double> price;
    std::optional<double> stop_price;
    std::optional<double> avg_fill_price;
    int exec_quantity;
    double last_fill_price;
    int last_fill_quantity;
    int remaining_quantity;
    std::string create_date;
    std::string transaction_date;
    OrderClass order_class;
    std::vector<Leg> legs;
    
    static Order from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
    
    // Core validation methods
    ValidationResult validate(ValidationLevel level = ValidationLevel::Basic) const;
    bool is_valid() const;
    std::vector<ValidationIssue> get_validation_issues() const;
    
    // Specific validation methods
    std::vector<ValidationIssue> validate_order_constraints() const;
    std::vector<ValidationIssue> validate_price_constraints() const;
    std::vector<ValidationIssue> validate_quantity_constraints() const;
    std::vector<ValidationIssue> validate_cross_field_constraints() const;
    std::vector<ValidationIssue> validate_business_rules() const;
    std::vector<ValidationIssue> validate_market_hours_rules() const;
    std::vector<ValidationIssue> validate_tradier_specific_rules() const;
    
    // Helper validation methods
    bool requires_price() const;
    bool requires_stop_price() const;
    bool is_market_hours_compatible() const;
    bool is_gtc_compatible() const;
    bool is_option_order() const;
    bool is_multileg_order() const;
    
    // Enhanced validation with external data
    ValidationResult validate_with_market_data(const struct Quote* quote = nullptr) const;
    ValidationResult validate_with_account_data(const struct AccountBalances* balances = nullptr) const;
    ValidationResult validate_with_positions(const std::vector<struct Position>* positions = nullptr) const;
    
    // Validation summary and reporting
    std::string get_validation_summary() const;
    std::string get_validation_report() const;
    void print_validation_issues() const;
};

} // namespace oqd