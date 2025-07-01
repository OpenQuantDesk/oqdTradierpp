#include <iostream>
#include <simdjson.h>
#include "include/oqdTradierpp/trading/order.hpp"
#include "include/oqdTradierpp/account/position.hpp"

using namespace oqd;

int main() {
    simdjson::dom::parser parser;
    
    // Test Order validation
    std::string order_json = R"({
        "id": "12345",
        "class": "equity",
        "symbol": "AAPL",
        "side": "buy",
        "quantity": "100",
        "type": "limit",
        "duration": "day",
        "price": "150.50",
        "avg_fill_price": "0.00",
        "exec_quantity": "0",
        "last_fill_price": "0.00",
        "last_fill_quantity": "0",
        "remaining_quantity": "100",
        "status": "pending",
        "tag": "user_order",
        "create_date": "2023-01-01T09:30:00.000Z",
        "transaction_date": "2023-01-01T09:30:00.000Z"
    })";
    
    auto order_doc = parser.parse(order_json);
    auto order = Order::from_json(order_doc.value());
    
    std::cout << "Order validation:\n";
    std::cout << "ID: " << order.id << "\n";
    std::cout << "Price: " << order.price << "\n";
    std::cout << "Symbol: " << order.symbol << "\n";
    
    auto result = order.validate(ValidationLevel::Basic);
    std::cout << "Basic validation: " << (result.is_valid ? "PASS" : "FAIL") << "\n";
    
    if (!result.is_valid) {
        std::cout << "Issues:\n";
        for (const auto& issue : result.issues) {
            std::cout << "  - " << issue.field_name << ": " << issue.message << "\n";
        }
    }
    
    // Test Position validation
    std::string position_json = R"({
        "cost_basis": "15000.00",
        "date_acquired": "2023-01-01T00:00:00.000Z",
        "id": "pos123",
        "quantity": "100.0",
        "symbol": "AAPL"
    })";
    
    auto pos_doc = parser.parse(position_json);
    auto position = Position::from_json(pos_doc.value());
    
    std::cout << "\nPosition validation:\n";
    std::cout << "Cost basis: " << position.cost_basis << "\n";
    std::cout << "Quantity: " << position.quantity << "\n";
    std::cout << "Average cost: " << position.get_average_cost() << "\n";
    
    auto pos_result = position.validate(ValidationLevel::Basic);
    std::cout << "Basic validation: " << (pos_result.is_valid ? "PASS" : "FAIL") << "\n";
    
    if (!pos_result.is_valid) {
        std::cout << "Issues:\n";
        for (const auto& issue : pos_result.issues) {
            std::cout << "  - " << issue.field_name << ": " << issue.message << "\n";
        }
    }
    
    return 0;
}