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

namespace oqd {

ValidationResult OrderValidator::validate_equity_order(const EquityOrderRequest& order) {
    ValidationResult result;
    result.is_valid = true;
    
    if (!is_valid_symbol(order.symbol)) {
        result.add_error("Invalid symbol: " + order.symbol);
    }
    
    if (!is_valid_quantity(order.quantity)) {
        result.add_error("Invalid quantity: " + std::to_string(order.quantity));
    }
    
    if (!validate_price_type_combination(order.type, order.price, order.stop)) {
        result.add_error("Invalid price/type combination");
    }
    
    if (order.type == OrderType::Market && order.duration == OrderDuration::Pre) {
        result.add_warning("Market orders in pre-market may have wider spreads");
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

bool OrderValidator::is_valid_quantity(int quantity) {
    return quantity > 0 && quantity <= 1000000;
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