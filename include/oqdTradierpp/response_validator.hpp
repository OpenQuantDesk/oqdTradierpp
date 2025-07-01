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
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <algorithm>
#include <simdjson.h>
#include "field_reference.hpp"

namespace oqd {

enum class ValidationLevel {
    None,     // No validation
    Basic,    // Basic required field checking
    Strict,   // Full field validation with constraints
    Paranoid  // Exhaustive validation with all checks
};

enum class ValidationSeverity {
    Info,
    Warning,
    Error,
    Critical
};

struct ValidationIssue {
    std::string field_name;
    ValidationSeverity severity;
    std::string message;
    std::string expected_value;
    std::string actual_value;
    std::string suggestion;
    
    ValidationIssue(std::string field, ValidationSeverity sev, std::string msg,
                    std::string expected = "", std::string actual = "", std::string suggest = "")
        : field_name(std::move(field)), severity(sev), message(std::move(msg)),
          expected_value(std::move(expected)), actual_value(std::move(actual)),
          suggestion(std::move(suggest)) {}
};

struct ValidationResult {
    bool is_valid;
    std::vector<ValidationIssue> issues;
    std::vector<std::string> missing_required_fields;
    std::vector<std::string> unexpected_fields;
    std::vector<std::string> warnings;
    ResponseType response_type;
    
    ValidationResult(ResponseType type) : is_valid(true), response_type(type) {}
    
    void add_issue(const ValidationIssue& issue) {
        issues.push_back(issue);
        if (issue.severity == ValidationSeverity::Error || issue.severity == ValidationSeverity::Critical) {
            is_valid = false;
        }
    }
    
    void add_missing_field(const std::string& field_name) {
        missing_required_fields.push_back(field_name);
        add_issue(ValidationIssue{field_name, ValidationSeverity::Error, 
                                 "Required field is missing", "", "", "Ensure this field is present in the response"});
    }
    
    void add_unexpected_field(const std::string& field_name) {
        unexpected_fields.push_back(field_name);
        add_issue(ValidationIssue{field_name, ValidationSeverity::Warning, 
                                 "Unexpected field found", "", "", "This field is not part of the expected schema"});
    }
    
    void add_warning(const std::string& message) {
        warnings.push_back(message);
        add_issue(ValidationIssue{"", ValidationSeverity::Warning, message});
    }
    
    bool has_errors() const {
        return !is_valid;
    }
    
    bool has_warnings() const {
        return !warnings.empty() || std::any_of(issues.begin(), issues.end(),
            [](const ValidationIssue& issue) { return issue.severity == ValidationSeverity::Warning; });
    }
    
    size_t error_count() const {
        return std::count_if(issues.begin(), issues.end(),
            [](const ValidationIssue& issue) { 
                return issue.severity == ValidationSeverity::Error || issue.severity == ValidationSeverity::Critical; 
            });
    }
    
    size_t warning_count() const {
        return std::count_if(issues.begin(), issues.end(),
            [](const ValidationIssue& issue) { return issue.severity == ValidationSeverity::Warning; });
    }
    
    std::string get_summary() const;
    std::string get_detailed_report() const;
};

class ResponseValidator {
public:
    // Set global validation level
    static void set_validation_level(ValidationLevel level);
    static ValidationLevel get_validation_level();
    
    // Enable/disable specific validation features
    static void set_strict_type_checking(bool enabled);
    static void set_check_unexpected_fields(bool enabled);
    static void set_validate_constraints(bool enabled);
    
    // Main validation functions
    template<typename T>
    static ValidationResult validate_response(const T& response, ResponseType type, ValidationLevel level = ValidationLevel::Basic);
    
    static ValidationResult validate_json_response(const simdjson::dom::element& json, ResponseType type, ValidationLevel level = ValidationLevel::Basic);
    
    // Type-specific validators
    static ValidationResult validate_order(const simdjson::dom::element& json, ValidationLevel level = ValidationLevel::Basic);
    static ValidationResult validate_account_balances(const simdjson::dom::element& json, ValidationLevel level = ValidationLevel::Basic);
    static ValidationResult validate_position(const simdjson::dom::element& json, ValidationLevel level = ValidationLevel::Basic);
    static ValidationResult validate_quote(const simdjson::dom::element& json, ValidationLevel level = ValidationLevel::Basic);
    
    // Field-level validation
    static std::vector<ValidationIssue> validate_field(const std::string& field_name, 
                                                      const simdjson::dom::element& value,
                                                      const FieldInfo& field_info);
    
    static std::vector<ValidationIssue> validate_field_value(const std::string& field_name,
                                                            const std::string& value,
                                                            const FieldInfo& field_info);
    
    // Constraint validation
    static ValidationIssue validate_constraint(const std::string& field_name,
                                              const std::string& value,
                                              const ValidationConstraint& constraint);
    
    // Schema validation
    static ValidationResult validate_schema(const simdjson::dom::element& json, ResponseType type);
    
    // Business logic validation
    static ValidationResult validate_business_rules(const simdjson::dom::element& json, ResponseType type);
    
    // Cross-field validation
    static std::vector<ValidationIssue> validate_cross_field_constraints(const simdjson::dom::element& json, ResponseType type);
    
private:
    static ValidationLevel global_validation_level_;
    static bool strict_type_checking_;
    static bool check_unexpected_fields_;
    static bool validate_constraints_;
    
    // Helper functions
    static bool is_json_type_compatible(const simdjson::dom::element& element, FieldType expected_type);
    static std::string extract_string_value(const simdjson::dom::element& element);
    static ValidationResult create_validation_result(ResponseType type);
    
    // Type conversion helpers
    static std::optional<std::string> json_to_string(const simdjson::dom::element& element);
    static std::optional<double> json_to_double(const simdjson::dom::element& element);
    static std::optional<int> json_to_int(const simdjson::dom::element& element);
    static std::optional<bool> json_to_bool(const simdjson::dom::element& element);
};

// Validation configuration
struct ValidationConfig {
    ValidationLevel level = ValidationLevel::Basic;
    bool strict_types = false;
    bool check_unexpected = true;
    bool validate_constraints = true;
    bool validate_business_rules = true;
    bool validate_cross_fields = false;
    
    static ValidationConfig strict() {
        return ValidationConfig{ValidationLevel::Strict, true, true, true, true, true};
    }
    
    static ValidationConfig permissive() {
        return ValidationConfig{ValidationLevel::Basic, false, false, false, false, false};
    }
    
    static ValidationConfig paranoid() {
        return ValidationConfig{ValidationLevel::Paranoid, true, true, true, true, true};
    }
};

// Validation utilities
class ValidationUtils {
public:
    static std::string format_validation_report(const ValidationResult& result);
    static std::string format_issue(const ValidationIssue& issue);
    static std::vector<ValidationIssue> filter_by_severity(const std::vector<ValidationIssue>& issues, ValidationSeverity severity);
    static void print_validation_report(const ValidationResult& result);
    static ValidationResult merge_results(const std::vector<ValidationResult>& results);
};

// Response validation traits for compile-time type checking
template<typename T>
struct ResponseValidationTraits {
    static constexpr ResponseType response_type = ResponseType::Order; // Default
    static constexpr bool has_validation = false;
};

// Specializations for known types (would be defined in response headers)
template<>
struct ResponseValidationTraits<struct Order> {
    static constexpr ResponseType response_type = ResponseType::Order;
    static constexpr bool has_validation = true;
};

template<>
struct ResponseValidationTraits<struct AccountBalances> {
    static constexpr ResponseType response_type = ResponseType::AccountBalances;
    static constexpr bool has_validation = true;
};

template<>
struct ResponseValidationTraits<struct Position> {
    static constexpr ResponseType response_type = ResponseType::Position;
    static constexpr bool has_validation = true;
};

template<>
struct ResponseValidationTraits<struct Quote> {
    static constexpr ResponseType response_type = ResponseType::Quote;
    static constexpr bool has_validation = true;
};

std::string to_string(ValidationLevel level);
std::string to_string(ValidationSeverity severity);

} // namespace oqd