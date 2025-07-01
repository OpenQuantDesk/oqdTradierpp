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

#include "oqdTradierpp/trading/order.hpp"
#include "oqdTradierpp/core/enums.hpp"
#include "oqdTradierpp/core/json_builder.hpp"
#include "oqdTradierpp/account/account_balances.hpp"
#include "oqdTradierpp/account/position.hpp"
#include "oqdTradierpp/market/quote.hpp"
#include <iostream>
#include <sstream>

namespace oqd {

Leg Leg::from_json(const simdjson::dom::element& elem) {
    Leg leg;
    leg.option_symbol = std::string(elem["option_symbol"].get_string().value_unsafe());
    leg.side = order_side_from_string(std::string(elem["side"].get_string().value_unsafe()));
    leg.quantity = elem["quantity"].get_int64().value_unsafe();
    return leg;
}

std::string Leg::to_json() const {
    return json::create_object()
        .field("option_symbol", option_symbol)
        .field("side", side)
        .field("quantity", quantity)
        .end_object()
        .str();
}

Order Order::from_json(const simdjson::dom::element& elem) {
    Order order;
    order.id = std::string(elem["id"].get_string().value_unsafe());
    order.type = order_type_from_string(std::string(elem["type"].get_string().value_unsafe()));
    order.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    order.side = order_side_from_string(std::string(elem["side"].get_string().value_unsafe()));
    order.quantity = elem["quantity"].get_int64().value_unsafe();
    order.status = order_status_from_string(std::string(elem["status"].get_string().value_unsafe()));
    order.duration = order_duration_from_string(std::string(elem["duration"].get_string().value_unsafe()));
    
    if (!elem["price"].is_null()) {
        order.price = elem["price"].get_double().value_unsafe();
    }
    if (!elem["stop_price"].is_null()) {
        order.stop_price = elem["stop_price"].get_double().value_unsafe();
    }
    if (!elem["avg_fill_price"].is_null()) {
        order.avg_fill_price = elem["avg_fill_price"].get_double().value_unsafe();
    }
    
    order.exec_quantity = elem["exec_quantity"].get_int64().value_unsafe();
    order.last_fill_price = elem["last_fill_price"].get_double().value_unsafe();
    order.last_fill_quantity = elem["last_fill_quantity"].get_int64().value_unsafe();
    order.remaining_quantity = elem["remaining_quantity"].get_int64().value_unsafe();
    order.create_date = std::string(elem["create_date"].get_string().value_unsafe());
    order.transaction_date = std::string(elem["transaction_date"].get_string().value_unsafe());
    order.order_class = order_class_from_string(std::string(elem["class"].get_string().value_unsafe()));
    
    auto legs_elem = elem["legs"];
    if (legs_elem.error() == simdjson::SUCCESS && legs_elem.is_array()) {
        for (const auto& leg : legs_elem.get_array()) {
            order.legs.push_back(Leg::from_json(leg));
        }
    }
    
    return order;
}

std::string Order::to_json() const {
    return json::create_object()
        .field("id", id)
        .field("type", type)
        .field("symbol", symbol)
        .field("side", side)
        .field("quantity", quantity)
        .field("status", status)
        .field("duration", duration)
        .field_optional("price", price)
        .field_optional("stop_price", stop_price)
        .field_optional("avg_fill_price", avg_fill_price)
        .field("exec_quantity", exec_quantity)
        .field("last_fill_price", last_fill_price)
        .field("last_fill_quantity", last_fill_quantity)
        .field("remaining_quantity", remaining_quantity)
        .field("create_date", create_date)
        .field("transaction_date", transaction_date)
        .field("class", order_class)
        .array_field("legs", legs)
        .end_object()
        .str();
}

// ==========================================
// Leg Validation Methods
// ==========================================

ValidationResult Leg::validate(ValidationLevel level) const {
    auto result = ValidationResult(ResponseType::Order);
    
    // Basic validation
    if (option_symbol.empty()) {
        result.add_issue(ValidationIssue{"option_symbol", ValidationSeverity::Error, 
                                        "Option symbol is required for leg"});
    } else if (!FieldValidator::is_valid_option_symbol(option_symbol)) {
        result.add_issue(ValidationIssue{"option_symbol", ValidationSeverity::Error, 
                                        "Invalid option symbol format", "Valid OCC format", option_symbol,
                                        "Use proper OCC option symbol format"});
    }
    
    if (quantity <= 0) {
        result.add_issue(ValidationIssue{"quantity", ValidationSeverity::Error, 
                                        "Leg quantity must be positive", "> 0", std::to_string(quantity)});
    }
    
    if (level >= ValidationLevel::Strict) {
        auto leg_issues = validate_leg_constraints();
        for (const auto& issue : leg_issues) {
            result.add_issue(issue);
        }
    }
    
    return result;
}

bool Leg::is_valid() const {
    return validate().is_valid;
}

std::vector<ValidationIssue> Leg::validate_leg_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Validate option side for leg
    if (side != OrderSide::BuyToOpen && side != OrderSide::BuyToClose && 
        side != OrderSide::SellToOpen && side != OrderSide::SellToClose) {
        issues.emplace_back("side", ValidationSeverity::Error, 
                           "Invalid option side for leg", "Option side required", to_string(side));
    }
    
    return issues;
}

// ==========================================
// Order Validation Methods
// ==========================================

ValidationResult Order::validate(ValidationLevel level) const {
    auto result = ValidationResult(ResponseType::Order);
    
    if (level == ValidationLevel::None) {
        return result;
    }
    
    // Convert order to JSON for framework validation
    std::string json_str = to_json();
    simdjson::dom::parser parser;
    auto parse_result = parser.parse(json_str);
    if (parse_result.error()) {
        result.add_issue(ValidationIssue{"", ValidationSeverity::Critical, "Failed to serialize order for validation"});
        return result;
    }
    
    // Use framework validation
    auto framework_result = ResponseValidator::validate_order(parse_result.value(), level);
    
    // Merge framework results
    for (const auto& issue : framework_result.issues) {
        result.add_issue(issue);
    }
    for (const auto& field : framework_result.missing_required_fields) {
        result.add_missing_field(field);
    }
    for (const auto& field : framework_result.unexpected_fields) {
        result.add_unexpected_field(field);
    }
    
    // Add our custom validations
    if (level >= ValidationLevel::Basic) {
        auto order_issues = validate_order_constraints();
        for (const auto& issue : order_issues) {
            result.add_issue(issue);
        }
    }
    
    if (level >= ValidationLevel::Strict) {
        auto price_issues = validate_price_constraints();
        auto quantity_issues = validate_quantity_constraints();
        auto cross_field_issues = validate_cross_field_constraints();
        auto business_issues = validate_business_rules();
        
        for (const auto& issue : price_issues) result.add_issue(issue);
        for (const auto& issue : quantity_issues) result.add_issue(issue);
        for (const auto& issue : cross_field_issues) result.add_issue(issue);
        for (const auto& issue : business_issues) result.add_issue(issue);
    }
    
    if (level >= ValidationLevel::Paranoid) {
        auto market_hours_issues = validate_market_hours_rules();
        auto tradier_issues = validate_tradier_specific_rules();
        
        for (const auto& issue : market_hours_issues) result.add_issue(issue);
        for (const auto& issue : tradier_issues) result.add_issue(issue);
    }
    
    return result;
}

bool Order::is_valid() const {
    return validate().is_valid;
}

std::vector<ValidationIssue> Order::get_validation_issues() const {
    return validate().issues;
}

std::vector<ValidationIssue> Order::validate_order_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Basic field validation
    if (id.empty()) {
        issues.emplace_back("id", ValidationSeverity::Error, "Order ID is required");
    }
    
    if (symbol.empty()) {
        issues.emplace_back("symbol", ValidationSeverity::Error, "Symbol is required");
    } else if (!FieldValidator::is_valid_symbol_format(symbol)) {
        issues.emplace_back("symbol", ValidationSeverity::Error, 
                           "Invalid symbol format", "Valid symbol", symbol);
    }
    
    if (quantity <= 0) {
        issues.emplace_back("quantity", ValidationSeverity::Error, 
                           "Quantity must be positive", "> 0", std::to_string(quantity));
    }
    
    if (quantity > 10000000) {
        issues.emplace_back("quantity", ValidationSeverity::Error, 
                           "Quantity exceeds maximum", "≤ 10,000,000", std::to_string(quantity));
    }
    
    return issues;
}

std::vector<ValidationIssue> Order::validate_price_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Price validation based on order type
    if (requires_price() && !price.has_value()) {
        issues.emplace_back("price", ValidationSeverity::Error, 
                           "Price is required for this order type", "numeric value", "missing");
    }
    
    if (requires_stop_price() && !stop_price.has_value()) {
        issues.emplace_back("stop_price", ValidationSeverity::Error, 
                           "Stop price is required for this order type", "numeric value", "missing");
    }
    
    // Price must be positive
    if (price.has_value() && price.value() <= 0) {
        issues.emplace_back("price", ValidationSeverity::Error, 
                           "Price must be positive", "> 0", std::to_string(price.value()));
    }
    
    if (stop_price.has_value() && stop_price.value() <= 0) {
        issues.emplace_back("stop_price", ValidationSeverity::Error, 
                           "Stop price must be positive", "> 0", std::to_string(stop_price.value()));
    }
    
    return issues;
}

std::vector<ValidationIssue> Order::validate_quantity_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Quantity consistency checks
    if (exec_quantity < 0) {
        issues.emplace_back("exec_quantity", ValidationSeverity::Error, 
                           "Executed quantity cannot be negative", "≥ 0", std::to_string(exec_quantity));
    }
    
    if (remaining_quantity < 0) {
        issues.emplace_back("remaining_quantity", ValidationSeverity::Error, 
                           "Remaining quantity cannot be negative", "≥ 0", std::to_string(remaining_quantity));
    }
    
    if (exec_quantity > quantity) {
        issues.emplace_back("exec_quantity", ValidationSeverity::Error, 
                           "Executed quantity exceeds total quantity", 
                           "≤ " + std::to_string(quantity), std::to_string(exec_quantity));
    }
    
    if (remaining_quantity > quantity) {
        issues.emplace_back("remaining_quantity", ValidationSeverity::Error, 
                           "Remaining quantity exceeds total quantity", 
                           "≤ " + std::to_string(quantity), std::to_string(remaining_quantity));
    }
    
    // Consistency check
    if (exec_quantity + remaining_quantity != quantity) {
        issues.emplace_back("quantity", ValidationSeverity::Warning, 
                           "Quantity consistency issue detected", 
                           std::to_string(exec_quantity + remaining_quantity), 
                           std::to_string(quantity),
                           "Verify quantity calculations");
    }
    
    return issues;
}

std::vector<ValidationIssue> Order::validate_cross_field_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Order type specific validations
    if (type == OrderType::Limit || type == OrderType::StopLimit) {
        if (!price.has_value()) {
            issues.emplace_back("price", ValidationSeverity::Error, 
                               "Limit orders require a price", "numeric value", "missing");
        }
    }
    
    if (type == OrderType::Stop || type == OrderType::StopLimit) {
        if (!stop_price.has_value()) {
            issues.emplace_back("stop_price", ValidationSeverity::Error, 
                               "Stop orders require a stop price", "numeric value", "missing");
        }
    }
    
    // Duration compatibility
    if (type == OrderType::Market && duration == OrderDuration::GTC) {
        issues.emplace_back("duration", ValidationSeverity::Error, 
                           "Market orders cannot use GTC duration", "day", "gtc",
                           "Use DAY duration for market orders");
    }
    
    // Side validation for order class
    if (order_class == OrderClass::Option) {
        if (side != OrderSide::BuyToOpen && side != OrderSide::BuyToClose && 
            side != OrderSide::SellToOpen && side != OrderSide::SellToClose) {
            issues.emplace_back("side", ValidationSeverity::Error, 
                               "Invalid side for option order", "option side", to_string(side));
        }
    } else if (order_class == OrderClass::Equity) {
        if (side != OrderSide::Buy && side != OrderSide::Sell && side != OrderSide::SellShort) {
            issues.emplace_back("side", ValidationSeverity::Error, 
                               "Invalid side for equity order", "equity side", to_string(side));
        }
    }
    
    return issues;
}

std::vector<ValidationIssue> Order::validate_business_rules() const {
    std::vector<ValidationIssue> issues;
    
    // Market order duration recommendation
    if (type == OrderType::Market && duration != OrderDuration::Day) {
        issues.emplace_back("duration", ValidationSeverity::Warning, 
                           "Market orders should typically use DAY duration", "day", to_string(duration),
                           "Consider using DAY duration for market orders");
    }
    
    // Short sale restrictions
    if (side == OrderSide::SellShort && duration == OrderDuration::GTC) {
        issues.emplace_back("duration", ValidationSeverity::Error, 
                           "Short sales cannot use GTC duration", "day", "gtc",
                           "Use DAY duration for short sales");
    }
    
    // Multileg validation
    if (is_multileg_order() && legs.empty()) {
        issues.emplace_back("legs", ValidationSeverity::Error, 
                           "Multileg order must have legs", "array with legs", "empty");
    }
    
    // Validate legs
    for (size_t i = 0; i < legs.size(); ++i) {
        auto leg_result = legs[i].validate();
        for (const auto& issue : leg_result.issues) {
            ValidationIssue leg_issue = issue;
            leg_issue.field_name = "legs[" + std::to_string(i) + "]." + issue.field_name;
            issues.push_back(leg_issue);
        }
    }
    
    return issues;
}

std::vector<ValidationIssue> Order::validate_market_hours_rules() const {
    std::vector<ValidationIssue> issues;
    
    // Pre/post market order validation
    if (duration == OrderDuration::Pre || duration == OrderDuration::Post) {
        if (type == OrderType::Market) {
            issues.emplace_back("type", ValidationSeverity::Warning, 
                               "Market orders in pre/post market sessions may have limited liquidity", 
                               "limit", "market",
                               "Consider using limit orders outside regular hours");
        }
    }
    
    return issues;
}

std::vector<ValidationIssue> Order::validate_tradier_specific_rules() const {
    std::vector<ValidationIssue> issues;
    
    // Tradier-specific constraints from error codes
    
    // Price range validation (would need market data for full validation)
    if (price.has_value() && price.value() < 0.0001) {
        issues.emplace_back("price", ValidationSeverity::Error, 
                           "Price too low for Tradier", "≥ $0.0001", std::to_string(price.value()));
    }
    
    // Account type restrictions (would need account data)
    if (order_class == OrderClass::Option) {
        issues.emplace_back("", ValidationSeverity::Info, 
                           "Option trading requires appropriate account permissions",
                           "", "", "Verify account has option trading enabled");
    }
    
    if (side == OrderSide::SellShort) {
        issues.emplace_back("", ValidationSeverity::Info, 
                           "Short selling requires margin account", 
                           "", "", "Verify account supports short selling");
    }
    
    return issues;
}

// Helper validation methods
bool Order::requires_price() const {
    return type == OrderType::Limit || type == OrderType::StopLimit;
}

bool Order::requires_stop_price() const {
    return type == OrderType::Stop || type == OrderType::StopLimit;
}

bool Order::is_market_hours_compatible() const {
    return duration == OrderDuration::Day || duration == OrderDuration::GTC;
}

bool Order::is_gtc_compatible() const {
    return type != OrderType::Market && side != OrderSide::SellShort;
}

bool Order::is_option_order() const {
    return order_class == OrderClass::Option || 
           side == OrderSide::BuyToOpen || side == OrderSide::BuyToClose ||
           side == OrderSide::SellToOpen || side == OrderSide::SellToClose;
}

bool Order::is_multileg_order() const {
    return order_class == OrderClass::Multileg || order_class == OrderClass::Combo ||
           order_class == OrderClass::OTO || order_class == OrderClass::OCO ||
           order_class == OrderClass::OTOCO;
}

// Enhanced validation with external data
ValidationResult Order::validate_with_market_data(const Quote* quote) const {
    auto result = validate();
    
    if (quote == nullptr) {
        return result;
    }
    
    // Stop price positioning validation
    if (stop_price.has_value()) {
        if (side == OrderSide::Buy || side == OrderSide::BuyToOpen) {
            // Buy stop must be above current ask
            if (stop_price.value() <= quote->ask) {
                result.add_issue(ValidationIssue{"stop_price", ValidationSeverity::Error,
                                               "Buy stop price must be above current ask",
                                               "> " + std::to_string(quote->ask), 
                                               std::to_string(stop_price.value()),
                                               "Set stop price above current ask price"});
            }
        } else if (side == OrderSide::Sell || side == OrderSide::SellToClose) {
            // Sell stop must be below current bid
            if (stop_price.value() >= quote->bid) {
                result.add_issue(ValidationIssue{"stop_price", ValidationSeverity::Error,
                                               "Sell stop price must be below current bid",
                                               "< " + std::to_string(quote->bid), 
                                               std::to_string(stop_price.value()),
                                               "Set stop price below current bid price"});
            }
        }
    }
    
    return result;
}

ValidationResult Order::validate_with_account_data(const AccountBalances* balances) const {
    auto result = validate();
    
    if (balances == nullptr) {
        return result;
    }
    
    // Buying power validation (simplified)
    if (side == OrderSide::Buy || side == OrderSide::BuyToOpen) {
        if (price.has_value()) {
            double estimated_cost = price.value() * quantity;
            if (estimated_cost > balances->total_cash) {
                result.add_issue(ValidationIssue{"", ValidationSeverity::Warning,
                                               "Order cost may exceed available cash",
                                               "≤ " + std::to_string(balances->total_cash),
                                               std::to_string(estimated_cost),
                                               "Verify sufficient buying power"});
            }
        }
    }
    
    return result;
}

ValidationResult Order::validate_with_positions(const std::vector<Position>* positions) const {
    auto result = validate();
    
    if (positions == nullptr) {
        return result;
    }
    
    // Find existing position for this symbol
    const Position* existing_position = nullptr;
    for (const auto& pos : *positions) {
        if (pos.symbol == symbol) {
            existing_position = &pos;
            break;
        }
    }
    
    // Position-aware validation
    if (side == OrderSide::Sell || side == OrderSide::SellToClose) {
        if (existing_position == nullptr || existing_position->quantity <= 0) {
            result.add_issue(ValidationIssue{"", ValidationSeverity::Warning,
                                           "Selling without long position",
                                           "long position", "no position",
                                           "Verify you have shares to sell"});
        } else if (quantity > existing_position->quantity) {
            result.add_issue(ValidationIssue{"quantity", ValidationSeverity::Error,
                                           "Sell quantity exceeds position size",
                                           "≤ " + std::to_string(static_cast<int>(existing_position->quantity)),
                                           std::to_string(quantity),
                                           "Reduce quantity or use sell_short"});
        }
    }
    
    return result;
}

// Validation summary and reporting
std::string Order::get_validation_summary() const {
    return validate().get_summary();
}

std::string Order::get_validation_report() const {
    return validate().get_detailed_report();
}

void Order::print_validation_issues() const {
    auto result = validate();
    ValidationUtils::print_validation_report(result);
}

} // namespace oqd