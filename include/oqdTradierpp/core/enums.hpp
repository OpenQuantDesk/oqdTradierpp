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

#include <string>

namespace oqd {

enum class OrderClass {
    Equity,
    Option,
    Multileg,
    Combo,
    OTO,      // One-Triggers-Other
    OCO,      // One-Cancels-Other
    OTOCO     // One-Triggers-One-Cancels-Other
};

enum class OrderType {
    Market,
    Limit,
    Stop,
    StopLimit
};

enum class OrderDuration {
    Day,
    GTC,
    Pre,
    Post
};

enum class OrderSide {
    Buy,
    Sell,
    SellShort,
    BuyToOpen,
    BuyToClose,
    SellToOpen,
    SellToClose
};

enum class OrderStatus {
    Open,
    PartiallyFilled,
    Filled,
    Expired,
    Canceled,
    Pending,
    Rejected
};
std::string to_string(OrderClass order_class);
std::string to_string(OrderType order_type);
std::string to_string(OrderDuration duration);
std::string to_string(OrderSide side);
std::string to_string(OrderStatus status);

OrderClass order_class_from_string(const std::string& str);
OrderType order_type_from_string(const std::string& str);
OrderDuration order_duration_from_string(const std::string& str);
OrderSide order_side_from_string(const std::string& str);
OrderStatus order_status_from_string(const std::string& str);

} // namespace oqd