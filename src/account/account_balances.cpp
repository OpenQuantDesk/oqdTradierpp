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

#include "oqdTradierpp/account/account_balances.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

AccountBalances AccountBalances::from_json(const simdjson::dom::element& elem) {
    AccountBalances balances;
    
    balances.account_number = elem["account_number"].is_null() ? 0.0 : elem["account_number"].get_double().value_unsafe();
    balances.total_equity = elem["total_equity"].is_null() ? 0.0 : elem["total_equity"].get_double().value_unsafe();
    balances.long_market_value = elem["long_market_value"].is_null() ? 0.0 : elem["long_market_value"].get_double().value_unsafe();
    balances.short_market_value = elem["short_market_value"].is_null() ? 0.0 : elem["short_market_value"].get_double().value_unsafe();
    balances.account_value = elem["account_value"].is_null() ? 0.0 : elem["account_value"].get_double().value_unsafe();
    balances.close_pl = elem["close_pl"].is_null() ? 0.0 : elem["close_pl"].get_double().value_unsafe();
    balances.current_requirement = elem["current_requirement"].is_null() ? 0.0 : elem["current_requirement"].get_double().value_unsafe();
    balances.equity = elem["equity"].is_null() ? 0.0 : elem["equity"].get_double().value_unsafe();
    balances.long_liquid_value = elem["long_liquid_value"].is_null() ? 0.0 : elem["long_liquid_value"].get_double().value_unsafe();
    balances.long_market_value_bp = elem["long_market_value_bp"].is_null() ? 0.0 : elem["long_market_value_bp"].get_double().value_unsafe();
    balances.short_liquid_value = elem["short_liquid_value"].is_null() ? 0.0 : elem["short_liquid_value"].get_double().value_unsafe();
    balances.short_market_value_bp = elem["short_market_value_bp"].is_null() ? 0.0 : elem["short_market_value_bp"].get_double().value_unsafe();
    balances.uncleared_funds = elem["uncleared_funds"].is_null() ? 0.0 : elem["uncleared_funds"].get_double().value_unsafe();
    balances.pending_orders_count = elem["pending_orders_count"].is_null() ? 0.0 : elem["pending_orders_count"].get_double().value_unsafe();
    balances.option_short_value = elem["option_short_value"].is_null() ? 0.0 : elem["option_short_value"].get_double().value_unsafe();
    balances.total_cash = elem["total_cash"].is_null() ? 0.0 : elem["total_cash"].get_double().value_unsafe();
    balances.unsettled_funds = elem["unsettled_funds"].is_null() ? 0.0 : elem["unsettled_funds"].get_double().value_unsafe();
    balances.dividend = elem["dividend"].is_null() ? 0.0 : elem["dividend"].get_double().value_unsafe();
    balances.cash = elem["cash"].is_null() ? 0.0 : elem["cash"].get_double().value_unsafe();
    balances.market_value = elem["market_value"].is_null() ? 0.0 : elem["market_value"].get_double().value_unsafe();
    
    return balances;
}

std::string AccountBalances::to_json() const {
    return json::create_object()
        .set_fixed().set_precision(2)
        .field("account_number", account_number)
        .field("total_equity", total_equity)
        .field("long_market_value", long_market_value)
        .field("short_market_value", short_market_value)
        .field("account_value", account_value)
        .field("close_pl", close_pl)
        .field("current_requirement", current_requirement)
        .field("equity", equity)
        .field("long_liquid_value", long_liquid_value)
        .field("long_market_value_bp", long_market_value_bp)
        .field("short_liquid_value", short_liquid_value)
        .field("short_market_value_bp", short_market_value_bp)
        .field("uncleared_funds", uncleared_funds)
        .field("pending_orders_count", pending_orders_count)
        .field("option_short_value", option_short_value)
        .field("total_cash", total_cash)
        .field("unsettled_funds", unsettled_funds)
        .field("dividend", dividend)
        .field("cash", cash)
        .field("market_value", market_value)
        .end_object()
        .str();
}

// ==========================================
// AccountBalances Validation Methods
// ==========================================

ValidationResult AccountBalances::validate(ValidationLevel level) const {
    auto result = ValidationResult(ResponseType::AccountBalances);
    
    if (level == ValidationLevel::None) {
        return result;
    }
    
    // Convert balances to JSON for framework validation
    std::string json_str = to_json();
    simdjson::dom::parser parser;
    auto parse_result = parser.parse(json_str);
    if (parse_result.error()) {
        result.add_issue(ValidationIssue{"", ValidationSeverity::Critical, "Failed to serialize balances for validation"});
        return result;
    }
    
    // Use framework validation
    auto framework_result = ResponseValidator::validate_account_balances(parse_result.value(), level);
    
    // Merge framework results
    for (const auto& issue : framework_result.issues) {
        result.add_issue(issue);
    }
    for (const auto& field : framework_result.missing_required_fields) {
        result.add_missing_field(field);
    }
    
    // Add our custom validations
    if (level >= ValidationLevel::Basic) {
        auto balance_issues = validate_balance_constraints();
        for (const auto& issue : balance_issues) {
            result.add_issue(issue);
        }
    }
    
    if (level >= ValidationLevel::Strict) {
        auto consistency_issues = validate_consistency_rules();
        auto account_type_issues = validate_account_type_rules();
        
        for (const auto& issue : consistency_issues) result.add_issue(issue);
        for (const auto& issue : account_type_issues) result.add_issue(issue);
    }
    
    return result;
}

bool AccountBalances::is_valid() const {
    return validate().is_valid;
}

std::vector<ValidationIssue> AccountBalances::get_validation_issues() const {
    return validate().issues;
}

std::vector<ValidationIssue> AccountBalances::validate_balance_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Basic balance validations
    if (total_equity < 0) {
        issues.emplace_back("total_equity", ValidationSeverity::Warning, 
                           "Total equity is negative", "≥ 0", std::to_string(total_equity),
                           "Account may be in deficit");
    }
    
    if (current_requirement < 0) {
        issues.emplace_back("current_requirement", ValidationSeverity::Error, 
                           "Current requirement cannot be negative", "≥ 0", std::to_string(current_requirement));
    }
    
    if (uncleared_funds < 0) {
        issues.emplace_back("uncleared_funds", ValidationSeverity::Error, 
                           "Uncleared funds cannot be negative", "≥ 0", std::to_string(uncleared_funds));
    }
    
    if (pending_orders_count < 0) {
        issues.emplace_back("pending_orders_count", ValidationSeverity::Error, 
                           "Pending orders count cannot be negative", "≥ 0", std::to_string(pending_orders_count));
    }
    
    // Pattern Day Trader rule
    if (total_equity > 0 && total_equity < 25000 && has_day_trading_buying_power()) {
        issues.emplace_back("total_equity", ValidationSeverity::Warning, 
                           "Account below PDT minimum equity requirement", "≥ $25,000", std::to_string(total_equity),
                           "Day trading may be restricted");
    }
    
    return issues;
}

std::vector<ValidationIssue> AccountBalances::validate_consistency_rules() const {
    std::vector<ValidationIssue> issues;
    
    // Basic consistency checks
    double calculated_equity = long_market_value + short_market_value + total_cash;
    double equity_diff = std::abs(calculated_equity - total_equity);
    
    if (equity_diff > 0.01) { // Allow small rounding differences
        issues.emplace_back("total_equity", ValidationSeverity::Warning, 
                           "Equity calculation inconsistency detected",
                           std::to_string(calculated_equity), std::to_string(total_equity),
                           "Verify equity calculations");
    }
    
    // Market value consistency
    if (market_value != long_market_value + short_market_value) {
        issues.emplace_back("market_value", ValidationSeverity::Warning, 
                           "Market value inconsistency detected",
                           std::to_string(long_market_value + short_market_value), 
                           std::to_string(market_value),
                           "Verify market value calculations");
    }
    
    // Cash availability
    double available_cash = total_cash - uncleared_funds;
    if (available_cash < 0) {
        issues.emplace_back("total_cash", ValidationSeverity::Warning, 
                           "Available cash is negative after uncleared funds",
                           "≥ uncleared funds", std::to_string(total_cash),
                           "May indicate settlement issues");
    }
    
    return issues;
}

std::vector<ValidationIssue> AccountBalances::validate_account_type_rules() const {
    std::vector<ValidationIssue> issues;
    
    // Margin requirements
    if (is_margin_account()) {
        if (current_requirement > 0 && total_equity < current_requirement) {
            issues.emplace_back("total_equity", ValidationSeverity::Critical, 
                               "Account below maintenance margin requirement",
                               "≥ " + std::to_string(current_requirement), 
                               std::to_string(total_equity),
                               "Margin call may be issued");
        }
        
        if (short_market_value < 0) {
            double short_requirement = std::abs(short_market_value) * 0.3; // 30% requirement
            if (total_cash < short_requirement) {
                issues.emplace_back("total_cash", ValidationSeverity::Warning, 
                                   "Insufficient cash for short position maintenance",
                                   "≥ " + std::to_string(short_requirement), 
                                   std::to_string(total_cash),
                                   "Monitor short position requirements");
            }
        }
    }
    
    // Account risk assessment
    if (is_at_risk()) {
        issues.emplace_back("", ValidationSeverity::Critical, 
                           "Account appears to be at risk",
                           "positive equity", "negative or low equity",
                           "Contact support immediately");
    }
    
    return issues;
}

// Helper methods
bool AccountBalances::has_sufficient_buying_power(double required_amount) const {
    return get_available_buying_power() >= required_amount;
}

bool AccountBalances::is_margin_account() const {
    // Simplified check - in real implementation, this would check account type
    return short_market_value != 0 || current_requirement > 0;
}

bool AccountBalances::has_day_trading_buying_power() const {
    // Simplified check for day trading buying power
    return total_equity >= 25000 && is_margin_account();
}

bool AccountBalances::is_at_risk() const {
    return total_equity <= 0 || 
           (current_requirement > 0 && total_equity < current_requirement * 1.1);
}

double AccountBalances::get_available_buying_power() const {
    double available_cash = total_cash - uncleared_funds;
    if (is_margin_account()) {
        // Simplified buying power calculation
        return available_cash + (long_market_value * 0.5); // 50% margin
    }
    return available_cash;
}

double AccountBalances::get_maintenance_excess() const {
    if (current_requirement <= 0) {
        return total_equity;
    }
    return total_equity - current_requirement;
}

// Validation summary and reporting
std::string AccountBalances::get_validation_summary() const {
    return validate().get_summary();
}

std::string AccountBalances::get_validation_report() const {
    return validate().get_detailed_report();
}

void AccountBalances::print_validation_issues() const {
    auto result = validate();
    ValidationUtils::print_validation_report(result);
}

}