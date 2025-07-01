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
#include <optional>
#include <functional>
#include <variant>
#include <regex>
#include <mutex>
#include <algorithm>

namespace oqd {

enum class FieldType {
    String,
    Integer,
    Double,
    Boolean,
    Date,
    DateTime,
    Enum,
    Array,
    Object,
    Optional
};

enum class ValidationRule {
    Required,
    MinLength,
    MaxLength,
    MinValue,
    MaxValue,
    Pattern,
    OneOf,
    Range,
    PositiveNumber,
    NonEmpty
};

using ValidationValue = std::variant<std::string, int, double, std::vector<std::string>>;

struct ValidationConstraint {
    ValidationRule rule;
    ValidationValue value;
    std::string error_message;
    
    ValidationConstraint(ValidationRule r, ValidationValue v, std::string msg)
        : rule(r), value(std::move(v)), error_message(std::move(msg)) {}
};

struct FieldInfo {
    std::string name;
    std::string description;
    FieldType type;
    bool required;
    std::vector<ValidationConstraint> constraints;
    std::optional<std::string> example_value;
    std::vector<std::string> valid_values;  // For enum types
    std::string category;
    
    FieldInfo() = default;
    
    FieldInfo(std::string n, std::string desc, FieldType t, bool req = false,
              std::vector<ValidationConstraint> cons = {},
              std::optional<std::string> example = {},
              std::vector<std::string> valid_vals = {},
              std::string cat = "")
        : name(std::move(n)), description(std::move(desc)), type(t), required(req),
          constraints(std::move(cons)), example_value(std::move(example)),
          valid_values(std::move(valid_vals)), category(std::move(cat)) {}
};

enum class ResponseType {
    Order,
    AccountBalances,
    Position,
    Quote,
    Historical,
    GainLoss,
    History,
    AccessToken,
    Clock,
    Calendar,
    Watchlist,
    SymbolSearch,
    OptionChain,
    TimeSales,
    Streaming
};

class FieldReference {
public:
    // Get field information for a specific response type
    static const std::unordered_map<std::string, FieldInfo>& get_fields(ResponseType type);
    
    // Get field info by name for a specific response type
    static const FieldInfo& get_field_info(ResponseType type, const std::string& field_name);
    
    // Check if a field is required for a response type
    static bool is_field_required(ResponseType type, const std::string& field_name);
    
    // Get all required fields for a response type
    static std::vector<std::string> get_required_fields(ResponseType type);
    
    // Get validation constraints for a field
    static std::vector<ValidationConstraint> get_field_constraints(ResponseType type, const std::string& field_name);
    
    // Validate a field value against its constraints
    static std::vector<std::string> validate_field_value(ResponseType type, const std::string& field_name, const std::string& value);
    
    // Get all fields by category
    static std::vector<std::string> get_fields_by_category(ResponseType type, const std::string& category);
    
    // Check if a field exists for a response type
    static bool field_exists(ResponseType type, const std::string& field_name);
    
    // Get field type
    static FieldType get_field_type(ResponseType type, const std::string& field_name);
    
    // Get valid enum values for a field
    static std::vector<std::string> get_valid_values(ResponseType type, const std::string& field_name);
    
private:
    static const std::unordered_map<ResponseType, std::unordered_map<std::string, FieldInfo>>& get_all_field_maps();
    static void initialize_field_maps();
    static std::once_flag fields_initialized_;
    
    // Helper functions for creating field maps
    static std::unordered_map<std::string, FieldInfo> create_order_fields();
    static std::unordered_map<std::string, FieldInfo> create_balance_fields();
    static std::unordered_map<std::string, FieldInfo> create_position_fields();
    static std::unordered_map<std::string, FieldInfo> create_quote_fields();
    static std::unordered_map<std::string, FieldInfo> create_historical_fields();
    static std::unordered_map<std::string, FieldInfo> create_gain_loss_fields();
    static std::unordered_map<std::string, FieldInfo> create_history_fields();
    static std::unordered_map<std::string, FieldInfo> create_access_token_fields();
    static std::unordered_map<std::string, FieldInfo> create_clock_fields();
    static std::unordered_map<std::string, FieldInfo> create_calendar_fields();
    static std::unordered_map<std::string, FieldInfo> create_watchlist_fields();
    static std::unordered_map<std::string, FieldInfo> create_symbol_search_fields();
    static std::unordered_map<std::string, FieldInfo> create_streaming_fields();
};

// Validation utilities
class FieldValidator {
public:
    static bool validate_string_length(const std::string& value, int min_length, int max_length);
    static bool validate_numeric_range(double value, double min_val, double max_val);
    static bool validate_pattern(const std::string& value, const std::string& pattern);
    static bool validate_enum_value(const std::string& value, const std::vector<std::string>& valid_values);
    static bool validate_date_format(const std::string& value);
    static bool validate_datetime_format(const std::string& value);
    static bool is_positive_number(const std::string& value);
    static bool is_valid_symbol_format(const std::string& symbol);
    static bool is_valid_option_symbol(const std::string& option_symbol);
    static bool is_valid_exchange_code(const std::string& exchange);
};

// Constants for common validation patterns
namespace ValidationPatterns {
    extern const std::string SYMBOL_PATTERN;
    extern const std::string OPTION_SYMBOL_PATTERN;
    extern const std::string DATE_PATTERN;
    extern const std::string DATETIME_PATTERN;
    extern const std::string EXCHANGE_CODE_PATTERN;
    extern const std::string ORDER_ID_PATTERN;
    extern const std::string ACCOUNT_ID_PATTERN;
}

// Exchange code mappings from reference data
namespace ExchangeCodes {
    extern const std::vector<std::string> EQUITY_EXCHANGES;
    extern const std::vector<std::string> OPTION_EXCHANGES;
    
    bool is_valid_equity_exchange(const std::string& code);
    bool is_valid_option_exchange(const std::string& code);
    std::string get_exchange_description(const std::string& code);
}

// Order status and side validation
namespace OrderValidation {
    extern const std::vector<std::string> VALID_ORDER_STATUSES;
    extern const std::vector<std::string> VALID_EQUITY_SIDES;
    extern const std::vector<std::string> VALID_OPTION_SIDES;
    extern const std::vector<std::string> VALID_ORDER_TYPES;
    extern const std::vector<std::string> VALID_ORDER_DURATIONS;
    extern const std::vector<std::string> VALID_ORDER_CLASSES;
    extern const std::vector<std::string> VALID_STRATEGIES;
    
    bool is_valid_order_status(const std::string& status);
    bool is_valid_order_side(const std::string& side, const std::string& security_type);
    bool is_valid_order_type(const std::string& type);
    bool is_valid_duration(const std::string& duration);
    bool is_valid_order_class(const std::string& order_class);
    bool is_valid_strategy(const std::string& strategy);
}

std::string to_string(FieldType type);
std::string to_string(ValidationRule rule);
std::string to_string(ResponseType type);

} // namespace oqd