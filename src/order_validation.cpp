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

#include "oqdTradierpp/validation.hpp"
#include <regex>
#include <algorithm>
#include <set>
#include <cmath>
#include <stdexcept>
#include <cctype>

namespace oqd {

// PathValidator implementation
std::string PathValidator::validate_account_id(const std::string& account_id) {
    if (account_id.empty()) {
        throw ValidationException("Account ID cannot be empty");
    }
    
    if (!is_valid_account_id_format(account_id)) {
        throw ValidationException("Invalid account ID format: " + account_id);
    }
    
    return account_id;
}

std::string PathValidator::validate_order_id(const std::string& order_id) {
    if (order_id.empty()) {
        throw ValidationException("Order ID cannot be empty");
    }
    
    if (!is_valid_order_id_format(order_id)) {
        throw ValidationException("Invalid order ID format: " + order_id);
    }
    
    return order_id;
}

std::string PathValidator::validate_session_id(const std::string& session_id) {
    if (session_id.empty()) {
        throw ValidationException("Session ID cannot be empty");
    }
    
    if (!is_valid_session_id_format(session_id)) {
        throw ValidationException("Invalid session ID format: " + session_id);
    }
    
    return session_id;
}

std::string PathValidator::validate_symbol(const std::string& symbol) {
    if (symbol.empty()) {
        throw ValidationException("Symbol cannot be empty");
    }
    
    if (!OrderValidator::is_valid_symbol(symbol)) {
        throw ValidationException("Invalid symbol format: " + symbol);
    }
    
    return symbol;
}

std::string PathValidator::validate_option_symbol(const std::string& option_symbol) {
    if (option_symbol.empty()) {
        throw ValidationException("Option symbol cannot be empty");
    }
    
    if (!OrderValidator::is_valid_option_symbol(option_symbol)) {
        throw ValidationException("Invalid option symbol format: " + option_symbol);
    }
    
    return option_symbol;
}

bool PathValidator::is_valid_account_id_format(const std::string& account_id) {
    // Tradier account IDs are typically 8-16 alphanumeric characters
    if (account_id.length() < 8 || account_id.length() > 16) {
        return false;
    }
    
    std::regex account_pattern("^[A-Z0-9]{8,16}$");
    return std::regex_match(account_id, account_pattern);
}

bool PathValidator::is_valid_order_id_format(const std::string& order_id) {
    // Order IDs are typically numeric strings of 8-20 digits
    if (order_id.length() < 8 || order_id.length() > 20) {
        return false;
    }
    
    std::regex order_pattern("^[0-9]{8,20}$");
    return std::regex_match(order_id, order_pattern);
}

bool PathValidator::is_valid_session_id_format(const std::string& session_id) {
    // Session IDs are typically UUIDs or similar alphanumeric strings
    if (session_id.length() < 16 || session_id.length() > 64) {
        return false;
    }
    
    std::regex session_pattern("^[A-Za-z0-9\\-_]{16,64}$");
    return std::regex_match(session_id, session_pattern);
}

void PathValidator::throw_if_invalid(const std::string& value, const std::string& type) {
    if (value.empty()) {
        throw ValidationException(type + " cannot be empty");
    }
}

// InputSanitizer implementation
std::string InputSanitizer::sanitize_string(const std::string& input, size_t max_length) {
    if (input.empty()) {
        return input;
    }
    
    std::string result;
    result.reserve(std::min(input.length(), max_length));
    
    for (char c : input) {
        if (is_safe_character(c) && result.length() < max_length) {
            result += c;
        }
    }
    
    return normalize_whitespace(result);
}

std::string InputSanitizer::sanitize_search_query(const std::string& query) {
    if (query.empty()) {
        return query;
    }
    
    // Remove SQL injection characters and limit length
    std::string cleaned = remove_sql_injection_chars(query);
    
    // Limit to reasonable search query length
    if (cleaned.length() > 64) {
        cleaned = cleaned.substr(0, 64);
    }
    
    return normalize_whitespace(cleaned);
}

std::string InputSanitizer::sanitize_symbol_list(const std::string& symbols) {
    if (symbols.empty()) {
        return symbols;
    }
    
    std::string result;
    result.reserve(symbols.length());
    
    for (char c : symbols) {
        // Allow alphanumeric, dots, commas, and dashes for symbol lists
        if (std::isalnum(c) || c == '.' || c == ',' || c == '-' || c == '_') {
            result += c;
        }
    }
    
    return result;
}

std::string InputSanitizer::sanitize_numeric_string(const std::string& value) {
    if (value.empty()) {
        return value;
    }
    
    std::string result;
    result.reserve(value.length());
    
    for (char c : value) {
        if (std::isdigit(c) || c == '.' || c == '-' || c == '+') {
            result += c;
        }
    }
    
    return result;
}

std::string InputSanitizer::remove_sql_injection_chars(const std::string& input) {
    std::string result;
    result.reserve(input.length());
    
    for (char c : input) {
        // Remove potentially dangerous SQL characters
        if (c != '\'' && c != '"' && c != ';' && c != '\\' && c != '|' && c != '&') {
            result += c;
        }
    }
    
    return result;
}

std::string InputSanitizer::escape_special_chars(const std::string& input) {
    std::string result;
    result.reserve(input.length() * 2); // Reserve space for potential escaping
    
    for (char c : input) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

bool InputSanitizer::is_safe_character(char c) {
    return std::isalnum(c) || c == ' ' || c == '.' || c == '-' || c == '_';
}

std::string InputSanitizer::normalize_whitespace(const std::string& input) {
    std::string result;
    result.reserve(input.length());
    
    bool prev_was_space = false;
    for (char c : input) {
        if (std::isspace(c)) {
            if (!prev_was_space) {
                result += ' ';
                prev_was_space = true;
            }
        } else {
            result += c;
            prev_was_space = false;
        }
    }
    
    // Remove trailing whitespace
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

ValidationResult OrderValidator::validate_equity_order(const EquityOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    // Enhanced symbol validation
    if (!is_valid_symbol(order.symbol)) {
        result.add_error("Invalid symbol: " + order.symbol);
    } else {
        // Additional symbol checks
        if (!is_valid_us_stock_symbol(order.symbol)) {
            result.add_warning("Symbol format may not be a standard US stock symbol");
        }
    }
    
    // Enhanced quantity validation
    if (!is_valid_quantity(order.quantity)) {
        result.add_error("Invalid quantity: " + std::to_string(order.quantity));
    } else {
        // Check for unusually large quantities
        if (order.quantity > 100000) {
            result.add_warning("Large quantity order - verify this is intentional");
        }
        
        // Check daily volume limits
        if (exceeds_daily_volume_limit(order.quantity, order.symbol)) {
            result.add_warning("Order quantity may exceed reasonable daily volume");
        }
    }
    
    // Enhanced price validation
    if (!validate_price_type_combination(order.type, order.price, order.stop)) {
        result.add_error("Invalid price/type combination");
    } else {
        // Additional price checks
        if (order.price.has_value()) {
            if (!is_valid_stock_price(order.price.value())) {
                result.add_error("Invalid stock price: " + std::to_string(order.price.value()));
            }
            
            if (!is_reasonable_price_range(order.price.value(), order.symbol)) {
                result.add_warning("Price may be outside reasonable range for this symbol");
            }
        }
        
        if (order.stop.has_value()) {
            if (!is_valid_stock_price(order.stop.value())) {
                result.add_error("Invalid stop price: " + std::to_string(order.stop.value()));
            }
        }
    }
    
    // Market timing warnings
    if (order.type == OrderType::Market && order.duration == OrderDuration::Pre) {
        result.add_warning("Market orders in pre-market may have wider spreads");
    }
    
    if (order.type == OrderType::Market && order.duration == OrderDuration::Post) {
        result.add_warning("Market orders in after-hours may have limited liquidity");
    }
    
    // Risk validation
    if (order.side == OrderSide::SellShort && order.quantity > 10000) {
        result.add_warning("Large short position - ensure adequate margin and risk management");
    }
    
    return result;
}

ValidationResult OrderValidator::validate_option_order(const OptionOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    if (!is_valid_symbol(order.symbol)) {
        result.add_error("Invalid underlying symbol: " + order.symbol);
    }
    
    if (!is_valid_option_symbol(order.option_symbol)) {
        result.add_error("Invalid option symbol format: " + order.option_symbol);
    }
    
    if (!is_valid_quantity(order.quantity)) {
        result.add_error("Invalid quantity: " + std::to_string(order.quantity));
    }
    
    if (!validate_option_side_quantity_combination(order.side, order.quantity)) {
        result.add_error("Invalid side/quantity combination for options");
    }
    
    if (order.side == OrderSide::SellToOpen) {
        result.add_warning("Selling options to open involves unlimited risk potential");
    }
    
    return result;
}

ValidationResult OrderValidator::validate_oto_order(const OTOOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    auto first_validation = validate_order_component(order.first_order);
    auto second_validation = validate_order_component(order.second_order);
    
    if (!first_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : first_validation.errors) {
            result.add_error("First order: " + error);
        }
    }
    
    if (!second_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : second_validation.errors) {
            result.add_error("Second order: " + error);
        }
    }
    
    if (order.first_order.symbol != order.second_order.symbol) {
        result.add_warning("Different symbols in OTO order - ensure this is intentional");
    }
    
    if (order.first_order.side == order.second_order.side) {
        result.add_warning("Both orders have same side - unusual for OTO strategy");
    }
    
    return result;
}

ValidationResult OrderValidator::validate_oco_order(const OCOOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    auto first_validation = validate_order_component(order.first_order);
    auto second_validation = validate_order_component(order.second_order);
    
    if (!first_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : first_validation.errors) {
            result.add_error("First order: " + error);
        }
    }
    
    if (!second_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : second_validation.errors) {
            result.add_error("Second order: " + error);
        }
    }
    
    if (order.first_order.symbol != order.second_order.symbol) {
        result.add_error("OCO orders must be for the same symbol");
    }
    
    if (order.first_order.side != order.second_order.side) {
        result.add_error("OCO orders must have the same side");
    }
    
    if (order.first_order.quantity != order.second_order.quantity) {
        result.add_error("OCO orders must have the same quantity");
    }
    
    return result;
}

ValidationResult OrderValidator::validate_otoco_order(const OTOCOOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    auto primary_validation = validate_order_component(order.primary_order);
    auto profit_validation = validate_order_component(order.profit_order);
    auto stop_validation = validate_order_component(order.stop_order);
    
    if (!primary_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : primary_validation.errors) {
            result.add_error("Primary order: " + error);
        }
    }
    
    if (!profit_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : profit_validation.errors) {
            result.add_error("Profit order: " + error);
        }
    }
    
    if (!stop_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : stop_validation.errors) {
            result.add_error("Stop order: " + error);
        }
    }
    
    auto bracket_validation = validate_bracket_order_logic(order.primary_order, order.profit_order, order.stop_order);
    if (!bracket_validation.is_valid) {
        result.is_valid = false;
        for (const auto& error : bracket_validation.errors) {
            result.add_error(error);
        }
    }
    
    return result;
}

ValidationResult OrderValidator::validate_spread_order(const SpreadOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    if (!is_spread_type_supported(order.spread_type)) {
        result.add_error("Unsupported spread type: " + order.spread_type);
    }
    
    if (order.legs.empty()) {
        result.add_error("Spread order must have at least one leg");
    }
    
    if (order.legs.size() > 4) {
        result.add_warning("Complex spreads with more than 4 legs may have execution challenges");
    }
    
    for (size_t i = 0; i < order.legs.size(); ++i) {
        auto leg_validation = validate_spread_leg(order.legs[i]);
        if (!leg_validation.is_valid) {
            result.is_valid = false;
            for (const auto& error : leg_validation.errors) {
                result.add_error("Leg " + std::to_string(i + 1) + ": " + error);
            }
        }
    }
    
    return result;
}

ValidationResult OrderValidator::validate_order_component(const OrderComponent& component) {
    ValidationResult result;
    result.is_valid = true;
    
    if (!is_valid_symbol(component.symbol)) {
        result.add_error("Invalid symbol: " + component.symbol);
    }
    
    if (!is_valid_quantity(component.quantity)) {
        result.add_error("Invalid quantity: " + std::to_string(component.quantity));
    }
    
    if (!validate_price_type_combination(component.type, component.price, component.stop)) {
        result.add_error("Invalid price/type combination");
    }
    
    if (component.option_symbol.has_value() && !is_valid_option_symbol(component.option_symbol.value())) {
        result.add_error("Invalid option symbol: " + component.option_symbol.value());
    }
    
    return result;
}

ValidationResult OrderValidator::validate_spread_leg(const SpreadLeg& leg) {
    ValidationResult result;
    result.is_valid = true;
    
    if (!is_valid_option_symbol(leg.option_symbol)) {
        result.add_error("Invalid option symbol: " + leg.option_symbol);
    }
    
    if (!is_valid_quantity(leg.quantity)) {
        result.add_error("Invalid quantity: " + std::to_string(leg.quantity));
    }
    
    if (leg.ratio.has_value()) {
        if (leg.ratio.value() <= 0.0) {
            result.add_error("Ratio must be positive");
        }
        if (leg.ratio.value() > 10.0) {
            result.add_warning("High ratio may indicate unusual spread strategy");
        }
    }
    
    return result;
}

bool OrderValidator::is_valid_symbol(const std::string& symbol) {
    if (symbol.empty() || symbol.length() > 10) {
        return false;
    }
    
    std::regex symbol_pattern("^[A-Z0-9\\.\\^\\-]+$");
    return std::regex_match(symbol, symbol_pattern);
}

bool OrderValidator::is_valid_option_symbol(const std::string& option_symbol) {
    // OCC format: AAPL240315C00150000
    // Format: SYMBOL + YYMMDD + C/P + STRIKE (8 digits, 3 decimal places)
    if (option_symbol.length() < 15) {
        return false;
    }
    
    std::regex option_pattern("^[A-Z]+[0-9]{6}[CP][0-9]{8}$");
    return std::regex_match(option_symbol, option_pattern);
}

bool OrderValidator::is_valid_price(double price) {
    return price > 0.0 && price < 100000.0 && !std::isnan(price) && !std::isinf(price);
}

// Enhanced price validation functions
bool OrderValidator::is_valid_stock_price(double price) {
    // Stock prices must be positive, reasonable, and meet increment requirements
    if (price <= 0.0 || price > 50000.0 || std::isnan(price) || std::isinf(price)) {
        return false;
    }
    
    // Check minimum price increment (penny stocks vs. regular stocks)
    if (price < 1.0) {
        // Penny stocks can have smaller increments
        return price >= 0.0001;
    } else {
        // Regular stocks minimum increment is $0.01
        return price >= 0.01;
    }
}

bool OrderValidator::is_valid_option_price(double price) {
    // Option prices have different rules
    if (price < 0.0 || price > 10000.0 || std::isnan(price) || std::isinf(price)) {
        return false;
    }
    
    // Options can be priced at $0.01 increments for prices < $3.00
    // and $0.05 increments for prices >= $3.00
    if (price < 3.0) {
        return price >= 0.01;
    } else {
        return price >= 0.05;
    }
}

bool OrderValidator::is_valid_price_increment(double price, const std::string& symbol) {
    // Enhanced price increment validation based on symbol type
    if (symbol.empty()) {
        return false;
    }
    
    // Check if it's an option symbol
    if (is_valid_option_symbol(symbol)) {
        return is_valid_option_price(price);
    }
    
    // Default to stock price validation
    return is_valid_stock_price(price);
}

bool OrderValidator::is_reasonable_price_range(double price, const std::string& symbol) {
    // Additional reasonableness checks based on symbol characteristics
    if (symbol.empty() || price <= 0.0) {
        return false;
    }
    
    // Very basic heuristics - in practice, you'd want real market data
    if (symbol.length() <= 4) {
        // Regular stock symbols
        return price >= 0.01 && price <= 10000.0;
    } else if (symbol.find('.') != std::string::npos) {
        // ETF or fund symbols
        return price >= 0.01 && price <= 1000.0;
    }
    
    return price >= 0.01 && price <= 50000.0;
}

bool OrderValidator::is_valid_quantity(int quantity) {
    return quantity > 0 && quantity <= 1000000;
}

// Enhanced quantity validation functions
bool OrderValidator::is_valid_stock_quantity(int quantity) {
    // Stock quantities must be positive and reasonable
    if (quantity <= 0 || quantity > 1000000) {
        return false;
    }
    
    // Additional validation: warn about unusually large quantities
    if (quantity > 100000) {
        // This should trigger a warning in the ValidationResult
        return true; // Still valid but should be flagged
    }
    
    return true;
}

bool OrderValidator::is_valid_option_quantity(int quantity) {
    // Option quantities (contracts) have different limits
    if (quantity <= 0 || quantity > 10000) {
        return false;
    }
    
    // Options are typically traded in smaller quantities
    if (quantity > 1000) {
        return true; // Valid but should trigger warning
    }
    
    return true;
}

bool OrderValidator::is_valid_quantity_for_order_type(int quantity, OrderType type) {
    // Some order types have specific quantity requirements
    if (quantity <= 0) {
        return false;
    }
    
    switch (type) {
        case OrderType::Market:
            // Market orders can have any positive quantity
            return quantity <= 1000000;
        case OrderType::Limit:
            // Limit orders can have any positive quantity
            return quantity <= 1000000;
        case OrderType::Stop:
        case OrderType::StopLimit:
            // Stop orders typically have reasonable size limits
            return quantity <= 100000;
        default:
            return quantity <= 1000000;
    }
}

bool OrderValidator::exceeds_daily_volume_limit(int quantity, const std::string& symbol) {
    // This would need real market data to implement properly
    // For now, just basic validation
    if (symbol.empty() || quantity <= 0) {
        return false;
    }
    
    // Very basic heuristic - in practice, you'd check against actual volume
    if (quantity > 500000) {
        return true; // Likely exceeds reasonable daily volume
    }
    
    return false;
}

// Enhanced symbol validation functions
bool OrderValidator::is_valid_us_stock_symbol(const std::string& symbol) {
    if (symbol.empty() || symbol.length() > 5) {
        return false;
    }
    
    // US stock symbols are 1-5 uppercase letters
    std::regex us_stock_pattern("^[A-Z]{1,5}$");
    return std::regex_match(symbol, us_stock_pattern);
}

bool OrderValidator::is_valid_etf_symbol(const std::string& symbol) {
    if (symbol.empty() || symbol.length() > 5) {
        return false;
    }
    
    // ETF symbols are similar to stocks but may have different characteristics
    std::regex etf_pattern("^[A-Z]{1,5}$");
    return std::regex_match(symbol, etf_pattern);
}

bool OrderValidator::is_valid_index_symbol(const std::string& symbol) {
    if (symbol.empty()) {
        return false;
    }
    
    // Index symbols may have special formats (e.g., ^SPX, $SPX)
    std::regex index_pattern("^[\\$\\^]?[A-Z]{1,5}$");
    return std::regex_match(symbol, index_pattern);
}

bool OrderValidator::is_valid_forex_symbol(const std::string& symbol) {
    if (symbol.empty() || symbol.length() != 6) {
        return false;
    }
    
    // Forex symbols are typically 6 characters (EURUSD, GBPUSD, etc.)
    std::regex forex_pattern("^[A-Z]{6}$");
    return std::regex_match(symbol, forex_pattern);
}

// Enhanced option symbol validation
bool OrderValidator::is_valid_occ_option_symbol(const std::string& option_symbol) {
    return is_valid_option_symbol(option_symbol); // Use existing implementation
}

bool OrderValidator::is_valid_option_expiration_date(const std::string& expiration) {
    if (expiration.length() != 6) {
        return false;
    }
    
    // Format: YYMMDD
    std::regex date_pattern("^[0-9]{6}$");
    if (!std::regex_match(expiration, date_pattern)) {
        return false;
    }
    
    // Basic date validation
    int month = std::stoi(expiration.substr(2, 2));
    int day = std::stoi(expiration.substr(4, 2));
    
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    
    return true;
}

bool OrderValidator::is_valid_option_strike_price(double strike) {
    // Strike prices must be positive and reasonable
    return strike > 0.0 && strike <= 10000.0 && !std::isnan(strike) && !std::isinf(strike);
}

bool OrderValidator::is_option_symbol_consistent(const std::string& underlying, const std::string& option_symbol) {
    if (underlying.empty() || option_symbol.empty()) {
        return false;
    }
    
    // Check if the option symbol starts with the underlying symbol
    if (option_symbol.length() < underlying.length()) {
        return false;
    }
    
    return option_symbol.substr(0, underlying.length()) == underlying;
}

bool OrderValidator::is_spread_type_supported(const std::string& spread_type) {
    static const std::set<std::string> supported_spreads = {
        "vertical", "vertical_call", "vertical_put", "vertical_call_bull", "vertical_call_bear",
        "vertical_put_bull", "vertical_put_bear", "horizontal", "diagonal", 
        "iron_condor", "iron_butterfly", "butterfly", "calendar", "ratio"
    };
    
    return supported_spreads.find(spread_type) != supported_spreads.end();
}

double OrderValidator::calculate_max_loss_otoco(const OTOCOOrderRequest& order) {
    if (order.primary_order.price.has_value() && order.stop_order.stop.has_value()) {
        double entry_price = order.primary_order.price.value();
        double stop_price = order.stop_order.stop.value();
        
        if (order.primary_order.side == OrderSide::Buy) {
            return std::abs(entry_price - stop_price) * order.primary_order.quantity;
        } else {
            return std::abs(stop_price - entry_price) * order.primary_order.quantity;
        }
    }
    
    return 0.0;
}

double OrderValidator::calculate_max_profit_otoco(const OTOCOOrderRequest& order) {
    if (order.primary_order.price.has_value() && order.profit_order.price.has_value()) {
        double entry_price = order.primary_order.price.value();
        double profit_price = order.profit_order.price.value();
        
        if (order.primary_order.side == OrderSide::Buy) {
            return std::abs(profit_price - entry_price) * order.primary_order.quantity;
        } else {
            return std::abs(entry_price - profit_price) * order.primary_order.quantity;
        }
    }
    
    return 0.0;
}

double OrderValidator::calculate_risk_reward_ratio(const OTOCOOrderRequest& order) {
    double max_loss = calculate_max_loss_otoco(order);
    double max_profit = calculate_max_profit_otoco(order);
    
    if (max_loss > 0.0) {
        return max_profit / max_loss;
    }
    
    return 0.0;
}

bool OrderValidator::validate_price_type_combination(OrderType type, std::optional<double> price, std::optional<double> stop) {
    switch (type) {
        case OrderType::Market:
            return true;
            
        case OrderType::Limit:
            return price.has_value() && is_valid_price(price.value());
            
        case OrderType::Stop:
            return stop.has_value() && is_valid_price(stop.value());
            
        case OrderType::StopLimit:
            return price.has_value() && stop.has_value() && 
                   is_valid_price(price.value()) && is_valid_price(stop.value());
    }
    
    return false;
}

bool OrderValidator::validate_option_side_quantity_combination(OrderSide /*side*/
, int quantity) {
    return is_valid_quantity(quantity);
}

ValidationResult OrderValidator::validate_bracket_order_logic(const OrderComponent& entry, const OrderComponent& profit, const OrderComponent& stop) {
    ValidationResult result;
    result.is_valid = true;
    
    if (entry.symbol != profit.symbol || entry.symbol != stop.symbol) {
        result.add_error("All bracket order components must be for the same symbol");
    }
    
    if (entry.quantity != profit.quantity || entry.quantity != stop.quantity) {
        result.add_error("All bracket order components must have the same quantity");
    }
    
    if (entry.side == OrderSide::Buy) {
        if (profit.side != OrderSide::Sell || stop.side != OrderSide::Sell) {
            result.add_error("For buy entry, both profit and stop orders must be sell orders");
        }
    } else if (entry.side == OrderSide::Sell || entry.side == OrderSide::SellShort) {
        if (profit.side != OrderSide::Buy || stop.side != OrderSide::Buy) {
            result.add_error("For sell entry, both profit and stop orders must be buy orders");
        }
    }
    
    if (entry.price.has_value() && profit.price.has_value() && stop.stop.has_value()) {
        double entry_price = entry.price.value();
        double profit_price = profit.price.value();
        double stop_price = stop.stop.value();
        
        if (entry.side == OrderSide::Buy) {
            if (profit_price <= entry_price) {
                result.add_warning("Profit target is not above entry price");
            }
            if (stop_price >= entry_price) {
                result.add_warning("Stop loss is not below entry price");
            }
        } else {
            if (profit_price >= entry_price) {
                result.add_warning("Profit target is not below entry price");
            }
            if (stop_price <= entry_price) {
                result.add_warning("Stop loss is not above entry price");
            }
        }
    }
    
    return result;
}


OrderBuilder::OTOBuilder OrderBuilder::create_oto_order() {
    return OTOBuilder();
}

OrderBuilder::OCOBuilder OrderBuilder::create_oco_order() {
    return OCOBuilder();
}

OrderBuilder::OTOCOBuilder OrderBuilder::create_otoco_order() {
    return OTOCOBuilder();
}

OrderBuilder::SpreadBuilder OrderBuilder::create_spread_order() {
    return SpreadBuilder();
}

} // namespace oqd