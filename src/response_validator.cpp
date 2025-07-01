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

#include "../include/oqdTradierpp/response_validator.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <algorithm>

namespace oqd {

// Static member initialization
ValidationLevel ResponseValidator::global_validation_level_ = ValidationLevel::Basic;
bool ResponseValidator::strict_type_checking_ = false;
bool ResponseValidator::check_unexpected_fields_ = true;
bool ResponseValidator::validate_constraints_ = true;

void ResponseValidator::set_validation_level(ValidationLevel level) {
    global_validation_level_ = level;
}

ValidationLevel ResponseValidator::get_validation_level() {
    return global_validation_level_;
}

void ResponseValidator::set_strict_type_checking(bool enabled) {
    strict_type_checking_ = enabled;
}

void ResponseValidator::set_check_unexpected_fields(bool enabled) {
    check_unexpected_fields_ = enabled;
}

void ResponseValidator::set_validate_constraints(bool enabled) {
    validate_constraints_ = enabled;
}

ValidationResult ResponseValidator::create_validation_result(ResponseType type) {
    return ValidationResult(type);
}

ValidationResult ResponseValidator::validate_json_response(const simdjson::dom::element& json, ResponseType type, ValidationLevel level) {
    auto result = create_validation_result(type);
    
    if (level == ValidationLevel::None) {
        return result;
    }
    
    // Get field definitions for this response type
    const auto& field_definitions = FieldReference::get_fields(type);
    
    if (field_definitions.empty()) {
        result.add_warning("No field definitions available for response type: " + to_string(type));
        return result;
    }
    
    // Check if JSON is an object
    if (!json.is_object()) {
        result.add_issue(ValidationIssue{"", ValidationSeverity::Critical, 
                                        "Response must be a JSON object", "object", "non-object"});
        return result;
    }
    
    auto json_obj = json.get_object();
    std::unordered_set<std::string> found_fields;
    
    // Validate each field in the JSON
    for (auto [key, value] : json_obj) {
        std::string field_name = std::string(key);
        found_fields.insert(field_name);
        
        auto field_iter = field_definitions.find(field_name);
        if (field_iter == field_definitions.end()) {
            // Unexpected field
            if (check_unexpected_fields_) {
                result.add_unexpected_field(field_name);
            }
            continue;
        }
        
        const FieldInfo& field_info = field_iter->second;
        
        // Validate this field
        auto field_issues = validate_field(field_name, value, field_info);
        for (const auto& issue : field_issues) {
            result.add_issue(issue);
        }
    }
    
    // Check for missing required fields
    if (level >= ValidationLevel::Basic) {
        for (const auto& [field_name, field_info] : field_definitions) {
            if (field_info.required && found_fields.find(field_name) == found_fields.end()) {
                result.add_missing_field(field_name);
            }
        }
    }
    
    // Perform cross-field validation for strict levels
    if (level >= ValidationLevel::Strict) {
        auto cross_field_issues = validate_cross_field_constraints(json, type);
        for (const auto& issue : cross_field_issues) {
            result.add_issue(issue);
        }
    }
    
    // Business rules validation for paranoid level
    if (level >= ValidationLevel::Paranoid) {
        auto business_result = validate_business_rules(json, type);
        for (const auto& issue : business_result.issues) {
            result.add_issue(issue);
        }
    }
    
    return result;
}

std::vector<ValidationIssue> ResponseValidator::validate_field(const std::string& field_name, 
                                                              const simdjson::dom::element& value,
                                                              const FieldInfo& field_info) {
    std::vector<ValidationIssue> issues;
    
    // Type validation
    if (strict_type_checking_ && !is_json_type_compatible(value, field_info.type)) {
        issues.emplace_back(field_name, ValidationSeverity::Error, 
                           "Type mismatch", to_string(field_info.type), 
                           "actual_type", "Ensure field has correct type");
    }
    
    // Convert to string for constraint validation
    std::string string_value = extract_string_value(value);
    
    // Validate constraints
    if (validate_constraints_) {
        for (const auto& constraint : field_info.constraints) {
            auto issue = validate_constraint(field_name, string_value, constraint);
            if (!issue.message.empty()) {
                issues.push_back(issue);
            }
        }
    }
    
    // Enum validation
    if (field_info.type == FieldType::Enum && !field_info.valid_values.empty()) {
        if (!FieldValidator::validate_enum_value(string_value, field_info.valid_values)) {
            std::string expected_values;
            for (size_t i = 0; i < field_info.valid_values.size(); ++i) {
                if (i > 0) expected_values += ", ";
                expected_values += field_info.valid_values[i];
            }
            issues.emplace_back(field_name, ValidationSeverity::Error,
                               "Invalid enum value", expected_values, string_value,
                               "Use one of the valid values");
        }
    }
    
    return issues;
}

ValidationIssue ResponseValidator::validate_constraint(const std::string& field_name,
                                                      const std::string& value,
                                                      const ValidationConstraint& constraint) {
    bool is_valid = true;
    std::string error_msg;
    
    switch (constraint.rule) {
        case ValidationRule::Required:
            is_valid = !value.empty();
            error_msg = constraint.error_message;
            break;
            
        case ValidationRule::MinLength:
            if (auto min_len = std::get_if<int>(&constraint.value)) {
                is_valid = FieldValidator::validate_string_length(value, *min_len, INT_MAX);
                error_msg = "Value too short (minimum " + std::to_string(*min_len) + " characters)";
            }
            break;
            
        case ValidationRule::MaxLength:
            if (auto max_len = std::get_if<int>(&constraint.value)) {
                is_valid = FieldValidator::validate_string_length(value, 0, *max_len);
                error_msg = "Value too long (maximum " + std::to_string(*max_len) + " characters)";
            }
            break;
            
        case ValidationRule::MinValue:
            try {
                double val = std::stod(value);
                if (auto min_val = std::get_if<double>(&constraint.value)) {
                    is_valid = val >= *min_val;
                    error_msg = "Value too small (minimum " + std::to_string(*min_val) + ")";
                } else if (auto min_val_int = std::get_if<int>(&constraint.value)) {
                    is_valid = val >= *min_val_int;
                    error_msg = "Value too small (minimum " + std::to_string(*min_val_int) + ")";
                }
            } catch (const std::exception&) {
                is_valid = false;
                error_msg = "Invalid numeric value";
            }
            break;
            
        case ValidationRule::MaxValue:
            try {
                double val = std::stod(value);
                if (auto max_val = std::get_if<double>(&constraint.value)) {
                    is_valid = val <= *max_val;
                    error_msg = "Value too large (maximum " + std::to_string(*max_val) + ")";
                } else if (auto max_val_int = std::get_if<int>(&constraint.value)) {
                    is_valid = val <= *max_val_int;
                    error_msg = "Value too large (maximum " + std::to_string(*max_val_int) + ")";
                }
            } catch (const std::exception&) {
                is_valid = false;
                error_msg = "Invalid numeric value";
            }
            break;
            
        case ValidationRule::Pattern:
            if (auto pattern = std::get_if<std::string>(&constraint.value)) {
                is_valid = FieldValidator::validate_pattern(value, *pattern);
                error_msg = "Value does not match required pattern";
            }
            break;
            
        case ValidationRule::OneOf:
            if (auto valid_values = std::get_if<std::vector<std::string>>(&constraint.value)) {
                is_valid = FieldValidator::validate_enum_value(value, *valid_values);
                error_msg = "Value not in allowed list";
            }
            break;
            
        case ValidationRule::PositiveNumber:
            try {
                double val = std::stod(value);
                is_valid = val > 0.0;
                error_msg = "Value must be positive";
            } catch (const std::exception&) {
                is_valid = false;
                error_msg = "Invalid numeric value";
            }
            break;
            
        case ValidationRule::NonEmpty:
            is_valid = !value.empty();
            error_msg = "Value cannot be empty";
            break;
            
        default:
            return ValidationIssue{"", ValidationSeverity::Info, ""};
    }
    
    if (!is_valid) {
        return ValidationIssue{field_name, ValidationSeverity::Error, 
                              error_msg.empty() ? constraint.error_message : error_msg};
    }
    
    return ValidationIssue{"", ValidationSeverity::Info, ""};
}

std::vector<ValidationIssue> ResponseValidator::validate_cross_field_constraints(const simdjson::dom::element& json, ResponseType type) {
    std::vector<ValidationIssue> issues;
    
    if (type == ResponseType::Order) {
        // Order-specific cross-field validations
        auto obj = json.get_object();
        
        // Check if stop orders have stop_price
        auto type_elem = obj["type"];
        auto stop_price_elem = obj["stop_price"];
        
        if (type_elem.is_string()) {
            auto type_result = type_elem.get_string();
            if (type_result.error()) return issues;
            std::string order_type = std::string(type_result.value());
            if ((order_type == "stop" || order_type == "stop_limit") && 
                (stop_price_elem.is_null() || stop_price_elem.error())) {
                issues.emplace_back("stop_price", ValidationSeverity::Error,
                                   "Stop orders must have a stop price",
                                   "numeric value", "missing",
                                   "Set a valid stop price for stop orders");
            }
        }
        
        // Check if limit orders have price
        if (type_elem.is_string()) {
            auto type_result2 = type_elem.get_string();
            if (type_result2.error()) return issues;
            std::string order_type = std::string(type_result2.value());
            auto price_elem = obj["price"];
            if ((order_type == "limit" || order_type == "stop_limit") && 
                (price_elem.is_null() || price_elem.error())) {
                issues.emplace_back("price", ValidationSeverity::Error,
                                   "Limit orders must have a price",
                                   "numeric value", "missing",
                                   "Set a valid limit price for limit orders");
            }
        }
        
        // Check quantity consistency
        auto quantity_elem = obj["quantity"];
        auto exec_quantity_elem = obj["exec_quantity"];
        auto remaining_quantity_elem = obj["remaining_quantity"];
        
        if (quantity_elem.is_number() && exec_quantity_elem.is_number() && remaining_quantity_elem.is_number()) {
            int total_qty = static_cast<int>(quantity_elem.get_double());
            int exec_qty = static_cast<int>(exec_quantity_elem.get_double());
            int remaining_qty = static_cast<int>(remaining_quantity_elem.get_double());
            
            if (exec_qty + remaining_qty != total_qty) {
                issues.emplace_back("quantity", ValidationSeverity::Warning,
                                   "Quantity inconsistency detected",
                                   std::to_string(exec_qty + remaining_qty),
                                   std::to_string(total_qty),
                                   "Verify quantity calculations");
            }
        }
    }
    
    return issues;
}

ValidationResult ResponseValidator::validate_business_rules(const simdjson::dom::element& json, ResponseType type) {
    auto result = create_validation_result(type);
    
    if (type == ResponseType::Order) {
        auto obj = json.get_object();
        
        // Business rule: Market orders should be DAY only
        auto type_elem = obj["type"];
        auto duration_elem = obj["duration"];
        
        if (type_elem.is_string() && duration_elem.is_string()) {
            auto type_result = type_elem.get_string();
            auto duration_result = duration_elem.get_string();
            if (type_result.error() || duration_result.error()) return result;
            std::string order_type = std::string(type_result.value());
            std::string duration = std::string(duration_result.value());
            
            if (order_type == "market" && duration != "day") {
                result.add_issue(ValidationIssue{"duration", ValidationSeverity::Warning,
                                                "Market orders should typically be DAY orders",
                                                "day", duration,
                                                "Consider using DAY duration for market orders"});
            }
        }
        
        // Business rule: Stop price positioning
        auto side_elem = obj["side"];
        auto stop_price_elem = obj["stop_price"];
        auto symbol_elem = obj["symbol"];
        
        if (side_elem.is_string() && stop_price_elem.is_number()) {
            auto side_result = side_elem.get_string();
            if (side_result.error()) return result;
            std::string side = std::string(side_result.value());
            double stop_price = stop_price_elem.get_double();
            
            // This would need current market data for full validation
            // For now, just check that stop price is positive
            if (stop_price <= 0) {
                result.add_issue(ValidationIssue{"stop_price", ValidationSeverity::Error,
                                                "Stop price must be positive",
                                                "> 0", std::to_string(stop_price),
                                                "Set a positive stop price"});
            }
        }
    }
    
    return result;
}

// Type-specific validation functions
ValidationResult ResponseValidator::validate_order(const simdjson::dom::element& json, ValidationLevel level) {
    return validate_json_response(json, ResponseType::Order, level);
}

ValidationResult ResponseValidator::validate_account_balances(const simdjson::dom::element& json, ValidationLevel level) {
    return validate_json_response(json, ResponseType::AccountBalances, level);
}

ValidationResult ResponseValidator::validate_position(const simdjson::dom::element& json, ValidationLevel level) {
    return validate_json_response(json, ResponseType::Position, level);
}

ValidationResult ResponseValidator::validate_quote(const simdjson::dom::element& json, ValidationLevel level) {
    return validate_json_response(json, ResponseType::Quote, level);
}

// Helper functions
bool ResponseValidator::is_json_type_compatible(const simdjson::dom::element& element, FieldType expected_type) {
    switch (expected_type) {
        case FieldType::String:
        case FieldType::Date:
        case FieldType::DateTime:
        case FieldType::Enum:
            return element.is_string();
            
        case FieldType::Integer:
            return element.is_number();
            
        case FieldType::Double:
            return element.is_number();
            
        case FieldType::Boolean:
            return element.is_bool();
            
        case FieldType::Array:
            return element.is_array();
            
        case FieldType::Object:
            return element.is_object();
            
        case FieldType::Optional:
            return true; // Optional can be any type or null
            
        default:
            return false;
    }
}

std::string ResponseValidator::extract_string_value(const simdjson::dom::element& element) {
    if (element.is_string()) {
        auto str_result = element.get_string();
        if (!str_result.error()) {
            return std::string(str_result.value());
        }
    } else if (element.is_number()) {
        return std::to_string(element.get_double());
    } else if (element.is_bool()) {
        return element.get_bool() ? "true" : "false";
    } else if (element.is_null()) {
        return "";
    }
    return ""; // Default for complex types
}

// ValidationResult methods
std::string ValidationResult::get_summary() const {
    std::ostringstream oss;
    oss << "Validation Result for " << to_string(response_type) << ": ";
    
    if (is_valid) {
        oss << "VALID";
    } else {
        oss << "INVALID (" << error_count() << " errors)";
    }
    
    if (has_warnings()) {
        oss << " with " << warning_count() << " warnings";
    }
    
    return oss.str();
}

std::string ValidationResult::get_detailed_report() const {
    std::ostringstream oss;
    
    oss << "=== Validation Report ===\n";
    oss << "Response Type: " << to_string(response_type) << "\n";
    oss << "Status: " << (is_valid ? "VALID" : "INVALID") << "\n";
    oss << "Errors: " << error_count() << "\n";
    oss << "Warnings: " << warning_count() << "\n\n";
    
    if (!missing_required_fields.empty()) {
        oss << "Missing Required Fields:\n";
        for (const auto& field : missing_required_fields) {
            oss << "  - " << field << "\n";
        }
        oss << "\n";
    }
    
    if (!unexpected_fields.empty()) {
        oss << "Unexpected Fields:\n";
        for (const auto& field : unexpected_fields) {
            oss << "  - " << field << "\n";
        }
        oss << "\n";
    }
    
    if (!issues.empty()) {
        oss << "Issues:\n";
        for (const auto& issue : issues) {
            oss << ValidationUtils::format_issue(issue) << "\n";
        }
    }
    
    return oss.str();
}

// Validation utilities
std::string ValidationUtils::format_issue(const ValidationIssue& issue) {
    std::ostringstream oss;
    oss << "[" << to_string(issue.severity) << "] ";
    
    if (!issue.field_name.empty()) {
        oss << issue.field_name << ": ";
    }
    
    oss << issue.message;
    
    if (!issue.expected_value.empty() && !issue.actual_value.empty()) {
        oss << " (expected: " << issue.expected_value << ", got: " << issue.actual_value << ")";
    }
    
    if (!issue.suggestion.empty()) {
        oss << " - " << issue.suggestion;
    }
    
    return oss.str();
}

void ValidationUtils::print_validation_report(const ValidationResult& result) {
    std::cout << result.get_detailed_report() << std::endl;
}

// String conversion functions
std::string to_string(ValidationLevel level) {
    switch (level) {
        case ValidationLevel::None: return "None";
        case ValidationLevel::Basic: return "Basic";
        case ValidationLevel::Strict: return "Strict";
        case ValidationLevel::Paranoid: return "Paranoid";
        default: return "Unknown";
    }
}

std::string to_string(ValidationSeverity severity) {
    switch (severity) {
        case ValidationSeverity::Info: return "INFO";
        case ValidationSeverity::Warning: return "WARNING";
        case ValidationSeverity::Error: return "ERROR";
        case ValidationSeverity::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

} // namespace oqd