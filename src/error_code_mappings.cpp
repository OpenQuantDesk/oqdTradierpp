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

#include "../include/oqdTradierpp/error_codes.hpp"

namespace oqd {

// Helper function to create the code map
static std::unordered_map<TradierErrorCode, ErrorCodeInfo> create_code_map() {
    using namespace std::chrono_literals;
    
    std::unordered_map<TradierErrorCode, ErrorCodeInfo> map;
    
    // Account Related Errors
    map[TradierErrorCode::AccountDisabled] = ErrorCodeInfo{
        TradierErrorCode::AccountDisabled,
        "AccountDisabled",
        "Account is disabled for trading. Please contact 980-272-3880 for questions or concerns.",
        ErrorCategory::Account,
        ErrorSeverity::Critical,
        RetryBehavior::NoRetry,
        0ms,
        {"Contact support at 980-272-3880", "Verify account status", "Check account permissions"},
        "980-272-3880",
        false
    };
    
    map[TradierErrorCode::AccountIsNotApproved] = ErrorCodeInfo{
        TradierErrorCode::AccountIsNotApproved,
        "AccountIsNotApproved",
        "Account is not approved for trading. Please contact 980-272-3880 for questions or concerns",
        ErrorCategory::Account,
        ErrorSeverity::Critical,
        RetryBehavior::NoRetry,
        0ms,
        {"Contact support at 980-272-3880", "Complete account approval process", "Submit required documentation"},
        "980-272-3880",
        false
    };
    
    map[TradierErrorCode::DayTradingBuyingPowerExceeded] = ErrorCodeInfo{
        TradierErrorCode::DayTradingBuyingPowerExceeded,
        "DayTradingBuyingPowerExceeded",
        "There is not enough day trading buying power for the requested order",
        ErrorCategory::Margin,
        ErrorSeverity::Error,
        RetryBehavior::NoRetry,
        0ms,
        {"Reduce order size", "Close existing positions", "Add funds to account", "Wait for settlements"},
        "980-272-3880",
        false
    };
    
    map[TradierErrorCode::BuyStopOrderStopPriceLessAsk] = ErrorCodeInfo{
        TradierErrorCode::BuyStopOrderStopPriceLessAsk,
        "BuyStopOrderStopPriceLessAsk",
        "Buy Stop order must have a Stop price greater than the current Ask price",
        ErrorCategory::Validation,
        ErrorSeverity::Error,
        RetryBehavior::RetryWithDelay,
        1s,
        {"Set stop price above current ask", "Use limit order instead", "Check current market price"},
        "",
        true
    };
    
    map[TradierErrorCode::InitialMargin] = ErrorCodeInfo{
        TradierErrorCode::InitialMargin,
        "InitialMargin",
        "You do not have enough buying power for this trade",
        ErrorCategory::Margin,
        ErrorSeverity::Error,
        RetryBehavior::NoRetry,
        0ms,
        {"Reduce order size", "Add funds to account", "Close existing positions"},
        "",
        false
    };
    
    map[TradierErrorCode::OmsInternalError] = ErrorCodeInfo{
        TradierErrorCode::OmsInternalError,
        "OmsInternalError",
        "Your order could not be processed. Please contact 980-272-3880 for questions or concerns",
        ErrorCategory::System,
        ErrorSeverity::Critical,
        RetryBehavior::RetryWithBackoff,
        5s,
        {"Retry order", "Contact support at 980-272-3880", "Check system status"},
        "980-272-3880",
        true
    };
    
    map[TradierErrorCode::Unknown] = ErrorCodeInfo{
        TradierErrorCode::Unknown,
        "Unknown",
        "An unknown error occurred",
        ErrorCategory::System,
        ErrorSeverity::Error,
        RetryBehavior::RetryWithDelay,
        2s,
        {"Retry operation", "Check request parameters", "Contact support if persists"},
        "980-272-3880",
        true
    };
    
    return map;
}

// Helper function to create the string map
static std::unordered_map<std::string, TradierErrorCode> create_string_map() {
    std::unordered_map<std::string, TradierErrorCode> string_map;
    
    string_map["AccountDisabled"] = TradierErrorCode::AccountDisabled;
    string_map["AccountIsNotApproved"] = TradierErrorCode::AccountIsNotApproved;
    string_map["DayTradingBuyingPowerExceeded"] = TradierErrorCode::DayTradingBuyingPowerExceeded;
    string_map["BuyStopOrderStopPriceLessAsk"] = TradierErrorCode::BuyStopOrderStopPriceLessAsk;
    string_map["InitialMargin"] = TradierErrorCode::InitialMargin;
    string_map["OmsInternalError"] = TradierErrorCode::OmsInternalError;
    string_map["Unknown"] = TradierErrorCode::Unknown;
    
    return string_map;
}

const std::unordered_map<TradierErrorCode, ErrorCodeInfo>& ErrorCodeMapper::get_code_map() {
    static const auto code_map = create_code_map();
    return code_map;
}

const std::unordered_map<std::string, TradierErrorCode>& ErrorCodeMapper::get_string_map() {
    static const auto string_map = create_string_map();
    return string_map;
}

void ErrorCodeMapper::initialize_maps() {
    // Force initialization by calling the getters
    get_code_map();
    get_string_map();
}

const ErrorCodeInfo& ErrorCodeMapper::get_error_info(TradierErrorCode code) {
    const auto& map = get_code_map();
    auto it = map.find(code);
    if (it != map.end()) {
        return it->second;
    }
    return get_error_info(TradierErrorCode::Unknown);
}

const ErrorCodeInfo& ErrorCodeMapper::get_error_info(const std::string& string_code) {
    return get_error_info(string_to_code(string_code));
}

TradierErrorCode ErrorCodeMapper::string_to_code(const std::string& string_code) {
    const auto& map = get_string_map();
    auto it = map.find(string_code);
    return (it != map.end()) ? it->second : TradierErrorCode::Unknown;
}

std::string ErrorCodeMapper::code_to_string(TradierErrorCode code) {
    return get_error_info(code).string_code;
}

std::string ErrorCodeMapper::get_description(TradierErrorCode code) {
    return get_error_info(code).description;
}

std::vector<std::string> ErrorCodeMapper::get_recovery_suggestions(TradierErrorCode code) {
    return get_error_info(code).recovery_suggestions;
}

bool ErrorCodeMapper::is_retryable(TradierErrorCode code) {
    return get_error_info(code).is_retryable;
}

std::chrono::milliseconds ErrorCodeMapper::get_retry_delay(TradierErrorCode code) {
    return get_error_info(code).retry_delay;
}

ErrorCategory ErrorCodeMapper::get_category(TradierErrorCode code) {
    return get_error_info(code).category;
}

ErrorSeverity ErrorCodeMapper::get_severity(TradierErrorCode code) {
    return get_error_info(code).severity;
}

RetryBehavior ErrorCodeMapper::get_retry_behavior(TradierErrorCode code) {
    return get_error_info(code).retry_behavior;
}

std::vector<TradierErrorCode> ErrorCodeMapper::get_codes_by_category(ErrorCategory category) {
    std::vector<TradierErrorCode> codes;
    const auto& map = get_code_map();
    for (const auto& [code, info] : map) {
        if (info.category == category) {
            codes.push_back(code);
        }
    }
    return codes;
}

std::vector<TradierErrorCode> ErrorCodeMapper::get_retryable_codes() {
    std::vector<TradierErrorCode> codes;
    const auto& map = get_code_map();
    for (const auto& [code, info] : map) {
        if (info.is_retryable) {
            codes.push_back(code);
        }
    }
    return codes;
}

std::string TradierApiException::get_detailed_message() const {
    std::string msg = ApiException::what();
    
    if (!server_message_.empty()) {
        msg += "\nServer Message: " + server_message_;
    }
    
    if (order_id_.has_value()) {
        msg += "\nOrder ID: " + order_id_.value();
    }
    
    if (account_id_.has_value()) {
        msg += "\nAccount ID: " + account_id_.value();
    }
    
    msg += "\nError Code: " + ErrorCodeMapper::code_to_string(error_code_);
    msg += "\nCategory: " + to_string(category_);
    msg += "\nSeverity: " + to_string(severity_);
    
    return msg;
}

std::string TradierApiException::get_recovery_guidance() const {
    std::string guidance = "Recovery Suggestions:\n";
    
    for (size_t i = 0; i < recovery_suggestions_.size(); ++i) {
        guidance += std::to_string(i + 1) + ". " + recovery_suggestions_[i] + "\n";
    }
    
    const auto& info = ErrorCodeMapper::get_error_info(error_code_);
    if (!info.contact_info.empty()) {
        guidance += "\nFor additional assistance, contact: " + info.contact_info;
    }
    
    return guidance;
}

std::string to_string(TradierErrorCode code) {
    return ErrorCodeMapper::code_to_string(code);
}

std::string to_string(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::Account: return "Account";
        case ErrorCategory::Trading: return "Trading";
        case ErrorCategory::Validation: return "Validation";
        case ErrorCategory::System: return "System";
        case ErrorCategory::MarketData: return "MarketData";
        case ErrorCategory::Position: return "Position";
        case ErrorCategory::Margin: return "Margin";
        case ErrorCategory::Options: return "Options";
        case ErrorCategory::ComplexOrders: return "ComplexOrders";
        default: return "Unknown";
    }
}

std::string to_string(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Info: return "Info";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error: return "Error";
        case ErrorSeverity::Critical: return "Critical";
        default: return "Unknown";
    }
}

std::string to_string(RetryBehavior behavior) {
    switch (behavior) {
        case RetryBehavior::NoRetry: return "NoRetry";
        case RetryBehavior::RetryWithDelay: return "RetryWithDelay";
        case RetryBehavior::RetryWithBackoff: return "RetryWithBackoff";
        case RetryBehavior::RetryAfterRateLimit: return "RetryAfterRateLimit";
        case RetryBehavior::RetryAfterMarketOpen: return "RetryAfterMarketOpen";
        default: return "Unknown";
    }
}

std::once_flag ErrorCodeMapper::maps_initialized_;

} // namespace oqd