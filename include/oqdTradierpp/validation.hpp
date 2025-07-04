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

#include "types.hpp"
#include <string>
#include <vector>
#include <optional>

namespace oqd {

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
    
    // Utility functions
    static bool is_valid_symbol(const std::string& symbol);
    static bool is_valid_option_symbol(const std::string& option_symbol);
    static bool is_valid_price(double price);
    static bool is_valid_quantity(int quantity);
    static bool is_spread_type_supported(const std::string& spread_type);
    
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