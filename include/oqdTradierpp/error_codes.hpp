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
#include <chrono>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <stdexcept>

namespace oqd {

// Forward declarations
class ApiException : public std::runtime_error {
public:
    explicit ApiException(const std::string& msg) : std::runtime_error(msg) {}
};

enum class TradierErrorCode {
    // Account Related Errors
    AccountDisabled,
    AccountIsNotApproved,
    AccountMarginRuleViolation,
    AssetTradingNotConfiguredForAccount,
    DayTraderPatternRestriction,
    DayTradingBuyingPowerExceeded,
    LongOptionTradingDeniedForAccount,
    ShortOptionTradingDeniedForAccount,
    ShortStockTradingDeniedForAccount,
    SpreadTradingDeniedForAccount,
    TradingDeniedForAccount,
    TradingDeniedForSecurity,
    UserDisabled,
    TooSmallEquityForDayTrading,
    
    // Order Price and Validation Errors
    BuyStopOrderStopPriceLessAsk,
    SellStopOrderStopPriceGreaterBid,
    IncorrectOrderQuantity,
    IncorrectTimeInForce,
    LimitPriceUndefined,
    StopPriceUndefined,
    OrderQuantity,
    OrderPriceIsInvalid,
    QuotePriceIsInvalid,
    
    // Position and Trading Errors
    LongPositionCrossZero,
    ShortPositionCrossZero,
    UnexpectedBuyOrder,
    UnexpectedBuyOrderOption,
    UnexpectedBuyToCoverOrder,
    UnexpectedBuyToCoverOrderOption,
    UnexpectedSellOrder,
    UnexpectedSellOrderOption,
    UnexpectedSellShortOrder,
    UnexpectedSellShortOrderOption,
    WashTradeAttempt,
    
    // Margin and Buying Power Errors
    InitialMargin,
    MaintenanceMargin,
    TotalInitialMargin,
    
    // Order Type Specific Errors
    MarketOrderIsGtc,
    ShortOrderIsGtc,
    SellShortOrderLastPriceBelow5,
    
    // Option Related Errors
    ExpirationDateUndefined,
    InvalidOrderExpiration,
    OptionTypeUndefined,
    StrikePriceUndefined,
    OptionLevelRestriction,
    IndexOptionsOneExparyDate,
    TradeNonStandartOptions,
    
    // Complex Order Errors
    ContingentOrderExecution,
    OrderContingentChangeNotAllowed,
    OcoExpirationTypeNotTheSame,
    OcoOrderWithOppositeLegs,
    OcoPriceDifferenceIsLessThanDelta,
    OrderWithDifferentSide,
    OtoFirstLesIsMarketNotAllowed,
    OtoOcoMarketNotAllowed,
    OtoOcoTrailingNotAllowed,
    
    // System and Service Errors
    OmsInternalError,
    OmsUnavailable,
    SecurityUndefined,
    OrderIsNotAllowedForAccount,
    
    // Market Hours and Trading Session Errors
    PreMarketTradingUnavailable,
    OtcTradingRestricted,
    VolatilityLimitOrderRequired,
    MarketHoursLimitOrderRequired,
    OrderFailedPriceRangeAggressive,
    
    // Unknown or unmapped error
    Unknown
};

enum class ErrorCategory {
    Account,
    Trading,
    Validation,
    System,
    MarketData,
    Position,
    Margin,
    Options,
    ComplexOrders
};

enum class ErrorSeverity {
    Info,
    Warning,
    Error,
    Critical
};

enum class RetryBehavior {
    NoRetry,
    RetryWithDelay,
    RetryWithBackoff,
    RetryAfterRateLimit,
    RetryAfterMarketOpen
};

struct ErrorCodeInfo {
    TradierErrorCode code;
    std::string string_code;
    std::string description;
    ErrorCategory category;
    ErrorSeverity severity;
    RetryBehavior retry_behavior;
    std::chrono::milliseconds retry_delay{0};
    std::vector<std::string> recovery_suggestions;
    std::string contact_info;
    bool is_retryable;
    
    ErrorCodeInfo() = default;
    
    ErrorCodeInfo(TradierErrorCode c, std::string sc, std::string desc, 
                  ErrorCategory cat, ErrorSeverity sev, RetryBehavior retry,
                  std::chrono::milliseconds delay = std::chrono::milliseconds{0},
                  std::vector<std::string> suggestions = {},
                  std::string contact = "980-272-3880",
                  bool retryable = false)
        : code(c), string_code(std::move(sc)), description(std::move(desc)), 
          category(cat), severity(sev), retry_behavior(retry), retry_delay(delay),
          recovery_suggestions(std::move(suggestions)), contact_info(std::move(contact)),
          is_retryable(retryable) {}
};

class ErrorCodeMapper {
public:
    static const ErrorCodeInfo& get_error_info(TradierErrorCode code);
    
    static const ErrorCodeInfo& get_error_info(const std::string& string_code);
    
    static TradierErrorCode string_to_code(const std::string& string_code);
    
    static std::string code_to_string(TradierErrorCode code);
    
    static std::string get_description(TradierErrorCode code);
    
    static std::vector<std::string> get_recovery_suggestions(TradierErrorCode code);
    
    static bool is_retryable(TradierErrorCode code);
    
    static std::chrono::milliseconds get_retry_delay(TradierErrorCode code);
    
    static ErrorCategory get_category(TradierErrorCode code);
    
    static ErrorSeverity get_severity(TradierErrorCode code);
    
    static RetryBehavior get_retry_behavior(TradierErrorCode code);
    
    static std::vector<TradierErrorCode> get_codes_by_category(ErrorCategory category);
    
    static std::vector<TradierErrorCode> get_retryable_codes();
    
private:
    static const std::unordered_map<TradierErrorCode, ErrorCodeInfo>& get_code_map();
    static const std::unordered_map<std::string, TradierErrorCode>& get_string_map();
    
    static void initialize_maps();
    static std::once_flag maps_initialized_;
};

class TradierApiException : public ApiException {
private:
    TradierErrorCode error_code_;
    std::string server_message_;
    std::vector<std::string> recovery_suggestions_;
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::optional<std::string> order_id_;
    std::optional<std::string> account_id_;
    
public:
    explicit TradierApiException(const std::string& message)
        : ApiException(message), error_code_(TradierErrorCode::Unknown),
          category_(ErrorCategory::System), severity_(ErrorSeverity::Error) {}
    
    TradierApiException(TradierErrorCode code, const std::string& server_message = "")
        : ApiException(ErrorCodeMapper::get_description(code)),
          error_code_(code), server_message_(server_message),
          recovery_suggestions_(ErrorCodeMapper::get_recovery_suggestions(code)),
          category_(ErrorCodeMapper::get_category(code)),
          severity_(ErrorCodeMapper::get_severity(code)) {}
    
    TradierApiException(const std::string& string_code, const std::string& server_message = "")
        : TradierApiException(ErrorCodeMapper::string_to_code(string_code), server_message) {}
    
    TradierErrorCode get_error_code() const noexcept { return error_code_; }
    
    const std::string& get_server_message() const noexcept { return server_message_; }
    
    const std::vector<std::string>& get_recovery_suggestions() const noexcept { 
        return recovery_suggestions_; 
    }
    
    ErrorCategory get_category() const noexcept { return category_; }
    
    ErrorSeverity get_severity() const noexcept { return severity_; }
    
    bool is_retryable() const noexcept { 
        return ErrorCodeMapper::is_retryable(error_code_); 
    }
    
    std::chrono::milliseconds get_retry_delay() const noexcept {
        return ErrorCodeMapper::get_retry_delay(error_code_);
    }
    
    RetryBehavior get_retry_behavior() const noexcept {
        return ErrorCodeMapper::get_retry_behavior(error_code_);
    }
    
    void set_order_id(const std::string& order_id) { order_id_ = order_id; }
    
    void set_account_id(const std::string& account_id) { account_id_ = account_id; }
    
    const std::optional<std::string>& get_order_id() const noexcept { return order_id_; }
    
    const std::optional<std::string>& get_account_id() const noexcept { return account_id_; }
    
    std::string get_detailed_message() const;
    
    std::string get_recovery_guidance() const;
};

class AccountException : public TradierApiException {
public:
    explicit AccountException(TradierErrorCode code, const std::string& server_message = "")
        : TradierApiException(code, server_message) {}
        
    explicit AccountException(const std::string& string_code, const std::string& server_message = "")
        : TradierApiException(string_code, server_message) {}
};

class OrderException : public TradierApiException {
public:
    explicit OrderException(TradierErrorCode code, const std::string& server_message = "")
        : TradierApiException(code, server_message) {}
        
    explicit OrderException(const std::string& string_code, const std::string& server_message = "")
        : TradierApiException(string_code, server_message) {}
};

class PositionException : public TradierApiException {
public:
    explicit PositionException(TradierErrorCode code, const std::string& server_message = "")
        : TradierApiException(code, server_message) {}
        
    explicit PositionException(const std::string& string_code, const std::string& server_message = "")
        : TradierApiException(string_code, server_message) {}
};

class MarginException : public TradierApiException {
public:
    explicit MarginException(TradierErrorCode code, const std::string& server_message = "")
        : TradierApiException(code, server_message) {}
        
    explicit MarginException(const std::string& string_code, const std::string& server_message = "")
        : TradierApiException(string_code, server_message) {}
};

class OptionException : public TradierApiException {
public:
    explicit OptionException(TradierErrorCode code, const std::string& server_message = "")
        : TradierApiException(code, server_message) {}
        
    explicit OptionException(const std::string& string_code, const std::string& server_message = "")
        : TradierApiException(string_code, server_message) {}
};

std::string to_string(TradierErrorCode code);
std::string to_string(ErrorCategory category);
std::string to_string(ErrorSeverity severity);
std::string to_string(RetryBehavior behavior);

} // namespace oqd