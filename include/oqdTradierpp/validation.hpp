/*
/        oqdTradierpp - Full featured Tradier API library 
/       
/        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
/        Version:           v1.1
/        Release Date:  06/30/2025
/        License: Apache 2.0
/        Disclaimer: This software is provided "as-is" without warranties of any kind. 
/                    Use at your own risk. The authors are not liable for any trading losses.
/                    Not financial advice. By using this software, you accept all risks.
/
*/

#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <optional>

namespace oqd {

// Forward declarations
class ValidationException;

// Path parameter validation utilities  
class PathValidator {
public:
    static std::string validate_account_id(const std::string& account_id);
    static std::string validate_order_id(const std::string& order_id);
    static std::string validate_session_id(const std::string& session_id);
    static std::string validate_symbol(const std::string& symbol);
    static std::string validate_option_symbol(const std::string& option_symbol);
    
private:
    static bool is_valid_account_id_format(const std::string& account_id);
    static bool is_valid_order_id_format(const std::string& order_id);
    static bool is_valid_session_id_format(const std::string& session_id);
    static void throw_if_invalid(const std::string& value, const std::string& type);
};

// Input sanitization utilities
class InputSanitizer {
public:
    static std::string sanitize_string(const std::string& input, size_t max_length = 256);
    static std::string sanitize_search_query(const std::string& query);
    static std::string sanitize_symbol_list(const std::string& symbols);
    static std::string sanitize_numeric_string(const std::string& value);
    static std::string remove_sql_injection_chars(const std::string& input);
    static std::string escape_special_chars(const std::string& input);
    
private:
    static bool is_safe_character(char c);
    static std::string normalize_whitespace(const std::string& input);
};

// Enhanced validation exception
class ValidationException : public std::exception {
public:
    explicit ValidationException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
    
private:
    std::string message_;
};

// Order validation results
struct ValidationResult {
    bool is_valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void add_error(const std::string& error) {
        is_valid = false;
        errors.push_back(error);
    }
    
    void add_warning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool has_errors() const { return !errors.empty(); }
    bool has_warnings() const { return !warnings.empty(); }
};

// Order validation utilities
class OrderValidator {
public:
    // Basic order validation
    static ValidationResult validate_equity_order(const EquityOrderRequest& order);
    static ValidationResult validate_option_order(const OptionOrderRequest& order);
    static ValidationResult validate_multileg_order(const MultilegOrderRequest& order);
    static ValidationResult validate_combo_order(const ComboOrderRequest& order);
    
    // Advanced order validation
    static ValidationResult validate_oto_order(const OTOOrderRequest& order);
    static ValidationResult validate_oco_order(const OCOOrderRequest& order);
    static ValidationResult validate_otoco_order(const OTOCOOrderRequest& order);
    static ValidationResult validate_spread_order(const SpreadOrderRequest& order);
    
    // Component validation
    static ValidationResult validate_order_component(const OrderComponent& component);
    static ValidationResult validate_spread_leg(const SpreadLeg& leg);
    
    // Enhanced utility functions
    static bool is_valid_symbol(const std::string& symbol);
    static bool is_valid_option_symbol(const std::string& option_symbol);
    static bool is_valid_price(double price);
    static bool is_valid_quantity(int quantity);
    static bool is_spread_type_supported(const std::string& spread_type);
    
    // Enhanced price validation
    static bool is_valid_stock_price(double price);
    static bool is_valid_option_price(double price);
    static bool is_valid_price_increment(double price, const std::string& symbol);
    static bool is_reasonable_price_range(double price, const std::string& symbol);
    
    // Enhanced quantity validation
    static bool is_valid_stock_quantity(int quantity);
    static bool is_valid_option_quantity(int quantity);
    static bool is_valid_quantity_for_order_type(int quantity, OrderType type);
    static bool exceeds_daily_volume_limit(int quantity, const std::string& symbol);
    
    // Symbol validation enhancements
    static bool is_valid_us_stock_symbol(const std::string& symbol);
    static bool is_valid_etf_symbol(const std::string& symbol);
    static bool is_valid_index_symbol(const std::string& symbol);
    static bool is_valid_forex_symbol(const std::string& symbol);
    
    // Option symbol validation enhancements
    static bool is_valid_occ_option_symbol(const std::string& option_symbol);
    static bool is_valid_option_expiration_date(const std::string& expiration);
    static bool is_valid_option_strike_price(double strike);
    static bool is_option_symbol_consistent(const std::string& underlying, const std::string& option_symbol);
    
    // Risk validation
    static bool exceeds_risk_limits(const OrderRequest& order, double max_risk_per_trade);
    static bool violates_position_limits(const OrderRequest& order, double max_position_size);
    static double calculate_order_risk(const OrderRequest& order);
    
    // Risk assessment helpers
    static double calculate_max_loss_otoco(const OTOCOOrderRequest& order);
    static double calculate_max_profit_otoco(const OTOCOOrderRequest& order);
    static double calculate_risk_reward_ratio(const OTOCOOrderRequest& order);
    
    // Spread analysis
    static std::string analyze_spread_strategy(const SpreadOrderRequest& order);
    static double calculate_spread_max_profit(const SpreadOrderRequest& order);
    static double calculate_spread_max_loss(const SpreadOrderRequest& order);
    
private:
    static bool validate_price_type_combination(OrderType type, std::optional<double> price, std::optional<double> stop);
    static bool validate_option_side_quantity_combination(OrderSide side, int quantity);
    static ValidationResult validate_bracket_order_logic(const OrderComponent& entry, const OrderComponent& profit, const OrderComponent& stop);
};

// Helper class for building complex orders
class OrderBuilder {
public:
    // OTO Order Builder
    class OTOBuilder {
    public:
        OTOBuilder& set_tag(const std::string& tag);
        OTOBuilder& set_first_order(const OrderComponent& order);
        OTOBuilder& set_second_order(const OrderComponent& order);
        
        // Convenience methods for common patterns
        OTOBuilder& buy_then_sell_at_profit(const std::string& symbol, int quantity, double entry_price, double profit_price);
        OTOBuilder& sell_then_buy_to_cover(const std::string& symbol, int quantity, double entry_price, double cover_price);
        
        OTOOrderRequest build() const;
        ValidationResult validate() const;
        
    private:
        OTOOrderRequest order_;
    };
    
    // OCO Order Builder  
    class OCOBuilder {
    public:
        OCOBuilder& set_tag(const std::string& tag);
        OCOBuilder& set_first_order(const OrderComponent& order);
        OCOBuilder& set_second_order(const OrderComponent& order);
        
        // Convenience methods
        OCOBuilder& profit_or_stop_loss(const std::string& symbol, int quantity, double profit_price, double stop_price);
        
        OCOOrderRequest build() const;
        ValidationResult validate() const;
        
    private:
        OCOOrderRequest order_;
    };
    
    // OTOCO (Bracket) Order Builder
    class OTOCOBuilder {
    public:
        OTOCOBuilder& set_tag(const std::string& tag);
        OTOCOBuilder& set_primary_order(const OrderComponent& order);
        OTOCOBuilder& set_profit_order(const OrderComponent& order);
        OTOCOBuilder& set_stop_order(const OrderComponent& order);
        
        // Convenience methods for bracket orders
        OTOCOBuilder& buy_with_bracket(const std::string& symbol, int quantity, 
                                      double entry_price, double profit_price, double stop_price);
        OTOCOBuilder& sell_with_bracket(const std::string& symbol, int quantity,
                                       double entry_price, double profit_price, double stop_price);
        
        // Risk management helpers
        OTOCOBuilder& set_risk_reward_ratio(double ratio); // Automatically calculate profit target
        OTOCOBuilder& set_stop_loss_percentage(double percentage); // Set stop as % below entry
        OTOCOBuilder& set_profit_target_percentage(double percentage); // Set profit as % above entry
        
        OTOCOOrderRequest build() const;
        ValidationResult validate() const;
        
    private:
        OTOCOOrderRequest order_;
        std::optional<double> entry_price_;
        std::optional<double> risk_reward_ratio_;
        std::optional<double> stop_loss_percentage_;
        std::optional<double> profit_target_percentage_;
    };
    
    // Spread Order Builder
    class SpreadBuilder {
    public:
        SpreadBuilder& set_type(OrderType type);
        SpreadBuilder& set_duration(OrderDuration duration);
        SpreadBuilder& set_price(double price);
        SpreadBuilder& set_spread_type(const std::string& spread_type);
        SpreadBuilder& set_tag(const std::string& tag);
        SpreadBuilder& add_leg(const SpreadLeg& leg);
        
        // Pre-built spread strategies
        SpreadBuilder& bull_call_spread(const std::string& underlying, const std::string& expiration,
                                       double lower_strike, double higher_strike, double net_debit);
        SpreadBuilder& bear_put_spread(const std::string& underlying, const std::string& expiration,
                                      double higher_strike, double lower_strike, double net_debit);
        SpreadBuilder& iron_condor(const std::string& underlying, const std::string& expiration,
                                  double put_strike_sell, double put_strike_buy,
                                  double call_strike_sell, double call_strike_buy, double net_credit);
        SpreadBuilder& butterfly_spread(const std::string& underlying, const std::string& expiration,
                                       double lower_strike, double middle_strike, double higher_strike, double net_debit);
        
        SpreadOrderRequest build() const;
        ValidationResult validate() const;
        
    private:
        SpreadOrderRequest order_;
        std::string build_option_symbol(const std::string& underlying, const std::string& expiration, 
                                       double strike, bool is_call) const;
    };
    
    // Factory methods
    static OTOBuilder create_oto_order();
    static OCOBuilder create_oco_order();
    static OTOCOBuilder create_otoco_order();
    static SpreadBuilder create_spread_order();
};

// Risk management utilities
struct RiskAnalysis {
    double max_loss;
    double max_profit;
    double risk_reward_ratio;
    double breakeven_price;
    std::string strategy_description;
    std::vector<std::string> risk_warnings;
};

class RiskAnalyzer {
public:
    static RiskAnalysis analyze_otoco_order(const OTOCOOrderRequest& order);
    static RiskAnalysis analyze_spread_order(const SpreadOrderRequest& order);
    static std::vector<std::string> identify_risk_factors(const OrderRequest& order);
    
    // Portfolio risk assessment
    static double calculate_position_size_by_risk(double account_value, double risk_percentage, 
                                                 double entry_price, double stop_price);
    static bool exceeds_position_limit(const OrderRequest& order, double max_position_percentage, 
                                      double account_value, double current_position_value);
};

} // namespace oqd