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

#include "oqdTradierpp/core/enums.hpp"

namespace oqd {

std::string to_string(OrderClass order_class) {
    switch (order_class) {
        case OrderClass::Equity: return "equity";
        case OrderClass::Option: return "option";
        case OrderClass::Multileg: return "multileg";
        case OrderClass::Combo: return "combo";
        case OrderClass::OTO: return "oto";
        case OrderClass::OCO: return "oco";
        case OrderClass::OTOCO: return "otoco";
        default: return "unknown";
    }
}

std::string to_string(OrderType order_type) {
    switch (order_type) {
        case OrderType::Market: return "market";
        case OrderType::Limit: return "limit";
        case OrderType::Stop: return "stop";
        case OrderType::StopLimit: return "stop_limit";
        default: return "unknown";
    }
}

std::string to_string(OrderDuration duration) {
    switch (duration) {
        case OrderDuration::Day: return "day";
        case OrderDuration::GTC: return "gtc";
        case OrderDuration::Pre: return "pre";
        case OrderDuration::Post: return "post";
        default: return "unknown";
    }
}

std::string to_string(OrderSide side) {
    switch (side) {
        case OrderSide::Buy: return "buy";
        case OrderSide::Sell: return "sell";
        case OrderSide::SellShort: return "sell_short";
        case OrderSide::BuyToOpen: return "buy_to_open";
        case OrderSide::BuyToClose: return "buy_to_close";
        case OrderSide::SellToOpen: return "sell_to_open";
        case OrderSide::SellToClose: return "sell_to_close";
        default: return "unknown";
    }
}

std::string to_string(OrderStatus status) {
    switch (status) {
        case OrderStatus::Open: return "open";
        case OrderStatus::PartiallyFilled: return "partially_filled";
        case OrderStatus::Filled: return "filled";
        case OrderStatus::Expired: return "expired";
        case OrderStatus::Canceled: return "canceled";
        case OrderStatus::Pending: return "pending";
        case OrderStatus::Rejected: return "rejected";
        default: return "unknown";
    }
}

OrderClass order_class_from_string(const std::string& str) {
    if (str == "equity") return OrderClass::Equity;
    if (str == "option") return OrderClass::Option;
    if (str == "multileg") return OrderClass::Multileg;
    if (str == "combo") return OrderClass::Combo;
    if (str == "oto") return OrderClass::OTO;
    if (str == "oco") return OrderClass::OCO;
    if (str == "otoco") return OrderClass::OTOCO;
    return OrderClass::Equity;
}

OrderType order_type_from_string(const std::string& str) {
    if (str == "market") return OrderType::Market;
    if (str == "limit") return OrderType::Limit;
    if (str == "stop") return OrderType::Stop;
    if (str == "stop_limit") return OrderType::StopLimit;
    return OrderType::Market;
}

OrderDuration order_duration_from_string(const std::string& str) {
    if (str == "day") return OrderDuration::Day;
    if (str == "gtc") return OrderDuration::GTC;
    if (str == "pre") return OrderDuration::Pre;
    if (str == "post") return OrderDuration::Post;
    return OrderDuration::Day;
}

OrderSide order_side_from_string(const std::string& str) {
    if (str == "buy") return OrderSide::Buy;
    if (str == "sell") return OrderSide::Sell;
    if (str == "sell_short") return OrderSide::SellShort;
    if (str == "buy_to_open") return OrderSide::BuyToOpen;
    if (str == "buy_to_close") return OrderSide::BuyToClose;
    if (str == "sell_to_open") return OrderSide::SellToOpen;
    if (str == "sell_to_close") return OrderSide::SellToClose;
    return OrderSide::Buy;
}

OrderStatus order_status_from_string(const std::string& str) {
    if (str == "open") return OrderStatus::Open;
    if (str == "partially_filled") return OrderStatus::PartiallyFilled;
    if (str == "filled") return OrderStatus::Filled;
    if (str == "expired") return OrderStatus::Expired;
    if (str == "canceled") return OrderStatus::Canceled;
    if (str == "pending") return OrderStatus::Pending;
    if (str == "rejected") return OrderStatus::Rejected;
    return OrderStatus::Open;
}

} // namespace oqd