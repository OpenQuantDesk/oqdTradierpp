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

#include "oqdTradierpp/account/position.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

Position Position::from_json(const simdjson::dom::element& elem) {
    Position position;
    position.cost_basis = elem["cost_basis"].get_double().value_unsafe();
    position.date_acquired = std::string(elem["date_acquired"].get_string().value_unsafe());
    position.id = std::string(elem["id"].get_string().value_unsafe());
    position.quantity = elem["quantity"].get_double().value_unsafe();
    position.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    return position;
}

std::string Position::to_json() const {
    return json::create_object()
        .field("cost_basis", cost_basis)
        .field("date_acquired", date_acquired)
        .field("id", id)
        .field("quantity", quantity)
        .field("symbol", symbol)
        .end_object()
        .str();
}

// ==========================================
// Position Validation Methods
// ==========================================

ValidationResult Position::validate(ValidationLevel level) const {
    auto result = ValidationResult(ResponseType::Position);
    
    if (level == ValidationLevel::None) {
        return result;
    }
    
    // Convert position to JSON for framework validation
    std::string json_str = to_json();
    simdjson::dom::parser parser;
    auto parse_result = parser.parse(json_str);
    if (parse_result.error()) {
        result.add_issue(ValidationIssue{"", ValidationSeverity::Critical, "Failed to serialize position for validation"});
        return result;
    }
    
    // Use framework validation
    auto framework_result = ResponseValidator::validate_position(parse_result.value(), level);
    
    // Merge framework results
    for (const auto& issue : framework_result.issues) {
        result.add_issue(issue);
    }
    for (const auto& field : framework_result.missing_required_fields) {
        result.add_missing_field(field);
    }
    
    // Add our custom validations
    if (level >= ValidationLevel::Basic) {
        auto position_issues = validate_position_constraints();
        for (const auto& issue : position_issues) {
            result.add_issue(issue);
        }
    }
    
    if (level >= ValidationLevel::Strict) {
        auto cost_basis_issues = validate_cost_basis_rules();
        for (const auto& issue : cost_basis_issues) {
            result.add_issue(issue);
        }
    }
    
    return result;
}

bool Position::is_valid() const {
    return validate().is_valid;
}

std::vector<ValidationIssue> Position::get_validation_issues() const {
    return validate().issues;
}

std::vector<ValidationIssue> Position::validate_position_constraints() const {
    std::vector<ValidationIssue> issues;
    
    // Basic validations
    if (id.empty()) {
        issues.emplace_back("id", ValidationSeverity::Error, "Position ID is required");
    }
    
    if (symbol.empty()) {
        issues.emplace_back("symbol", ValidationSeverity::Error, "Symbol is required");
    } else if (!FieldValidator::is_valid_symbol_format(symbol)) {
        issues.emplace_back("symbol", ValidationSeverity::Error, 
                           "Invalid symbol format", "Valid symbol", symbol);
    }
    
    if (quantity == 0) {
        issues.emplace_back("quantity", ValidationSeverity::Warning, 
                           "Position has zero quantity", "≠ 0", "0",
                           "Zero quantity positions may indicate closed positions");
    }
    
    // Large position warnings
    if (std::abs(quantity) > 1000000) {
        issues.emplace_back("quantity", ValidationSeverity::Warning, 
                           "Very large position detected", "reasonable size", std::to_string(quantity),
                           "Verify position size is correct");
    }
    
    return issues;
}

std::vector<ValidationIssue> Position::validate_cost_basis_rules() const {
    std::vector<ValidationIssue> issues;
    
    // Cost basis validation
    if (cost_basis < 0 && is_long_position()) {
        issues.emplace_back("cost_basis", ValidationSeverity::Warning, 
                           "Negative cost basis for long position", "≥ 0", std::to_string(cost_basis),
                           "May indicate data error or complex transaction history");
    }
    
    if (cost_basis > 0 && is_short_position()) {
        issues.emplace_back("cost_basis", ValidationSeverity::Warning, 
                           "Positive cost basis for short position", "≤ 0", std::to_string(cost_basis),
                           "May indicate data error or covered short position");
    }
    
    // Extreme cost basis values
    double average_cost = get_average_cost();
    if (average_cost > 10000) {
        issues.emplace_back("cost_basis", ValidationSeverity::Warning, 
                           "Very high average cost per share", "reasonable price", std::to_string(average_cost),
                           "Verify cost basis calculation");
    }
    
    if (average_cost < 0.001 && quantity != 0) {
        issues.emplace_back("cost_basis", ValidationSeverity::Warning, 
                           "Very low average cost per share", "reasonable price", std::to_string(average_cost),
                           "May indicate penny stock or data error");
    }
    
    return issues;
}

// Helper methods
bool Position::is_long_position() const {
    return quantity > 0;
}

bool Position::is_short_position() const {
    return quantity < 0;
}

double Position::get_current_value(double current_price) const {
    return quantity * current_price;
}

double Position::get_unrealized_pnl(double current_price) const {
    return get_current_value(current_price) - cost_basis;
}

double Position::get_average_cost() const {
    if (quantity == 0) {
        return 0.0;
    }
    return cost_basis / std::abs(quantity);
}

// Validation summary and reporting
std::string Position::get_validation_summary() const {
    return validate().get_summary();
}

std::string Position::get_validation_report() const {
    return validate().get_detailed_report();
}

void Position::print_validation_issues() const {
    auto result = validate();
    ValidationUtils::print_validation_report(result);
}

}