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

#include "oqdTradierpp/oqdTradierpp.hpp"
#include <iostream>
#include <iomanip>

int main() {
    try {
        oqd::initialize_library();

        // Create client and API methods
        auto client = oqd::create_client(oqd::Environment::Sandbox);
        client->set_access_token("YOUR_ACCESS_TOKEN_HERE");
        auto api = oqd::create_api_methods(client);

        std::cout << "Tradier Trading Example" << std::endl;
        std::cout << "======================" << std::endl;

        std::string account_id = "YOUR_ACCOUNT_ID"; // Replace with your account ID

        // Example 1: Get account balances
        std::cout << "\n=== Account Balances ===" << std::endl;
        try {
            auto balances = api->get_account_balances(account_id);
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Total Equity: $" << balances.total_equity << std::endl;
            std::cout << "Buying Power: $" << balances.cash << std::endl;
            std::cout << "Market Value: $" << balances.market_value << std::endl;
            std::cout << "Cash: $" << balances.cash << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error getting balances: " << e.what() << std::endl;
        }

        // Example 2: Get current positions
        std::cout << "\n=== Current Positions ===" << std::endl;
        try {
            auto positions = api->get_account_positions(account_id);
            if (positions.empty()) {
                std::cout << "No current positions" << std::endl;
            } else {
                for (const auto& position : positions) {
                    std::cout << "Symbol: " << position.symbol << std::endl;
                    std::cout << "  Quantity: " << position.quantity << std::endl;
                    std::cout << "  Cost Basis: $" << position.cost_basis << std::endl;
                    std::cout << "  Date Acquired: " << position.date_acquired << std::endl;
                    std::cout << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting positions: " << e.what() << std::endl;
        }

        // Example 3: Get account orders
        std::cout << "\n=== Recent Orders ===" << std::endl;
        try {
            auto orders = api->get_account_orders(account_id);
            if (orders.empty()) {
                std::cout << "No recent orders" << std::endl;
            } else {
                for (const auto& order : orders) {
                    std::cout << "Order ID: " << order.id << std::endl;
                    std::cout << "  Symbol: " << order.symbol << std::endl;
                    std::cout << "  Side: " << oqd::to_string(order.side) << std::endl;
                    std::cout << "  Quantity: " << order.quantity << std::endl;
                    std::cout << "  Type: " << oqd::to_string(order.type) << std::endl;
                    std::cout << "  Status: " << oqd::to_string(order.status) << std::endl;
                    if (order.price.has_value()) {
                        std::cout << "  Price: $" << order.price.value() << std::endl;
                    }
                    std::cout << "  Created: " << order.create_date << std::endl;
                    std::cout << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error getting orders: " << e.what() << std::endl;
        }

        // Example 4: Preview an equity order
        std::cout << "\n=== Order Preview ===" << std::endl;
        try {
            oqd::EquityOrderRequest preview_order;
            preview_order.symbol = "AAPL";
            preview_order.side = oqd::OrderSide::Buy;
            preview_order.quantity = 100;
            preview_order.type = oqd::OrderType::Limit;
            preview_order.duration = oqd::OrderDuration::Day;
            preview_order.price = 150.00;

            auto preview = api->preview_order(account_id, preview_order);
            std::cout << "Order Preview for " << preview.symbol << ":" << std::endl;
            std::cout << "  Commission: $" << preview.commission << std::endl;
            std::cout << "  Cost: $" << preview.cost << std::endl;
            std::cout << "  Fees: $" << preview.fees << std::endl;
            std::cout << "  Total Amount: $" << preview.amount << std::endl;
            std::cout << "  Buying Power Change: $" << preview.buying_power << std::endl;

        } catch (const std::exception& e) {
            std::cout << "Error previewing order: " << e.what() << std::endl;
        }

        // Example 5: Place an equity order (commented out for safety)
        /*
        std::cout << "\n=== Placing Equity Order ===" << std::endl;
        try {
            oqd::EquityOrderRequest order;
            order.symbol = "AAPL";
            order.side = oqd::OrderSide::Buy;
            order.quantity = 1; // Small quantity for testing
            order.type = oqd::OrderType::Limit;
            order.duration = oqd::OrderDuration::Day;
            order.price = 100.00; 

            auto response = api->place_equity_order(account_id, order);
            std::cout << "Order placed successfully!" << std::endl;
            std::cout << "Order ID: " << response.id << std::endl;
            std::cout << "Status: " << response.status << std::endl;

        } catch (const std::exception& e) {
            std::cout << "Error placing order: " << e.what() << std::endl;
        }
        */


        // Example 6: Place an option order (commented out for safety)
        /*
        std::cout << "\n=== Placing Option Order ===" << std::endl;
        try {
            oqd::OptionOrderRequest option_order;
            option_order.option_symbol = "AAPL230120C00150000";
            option_order.side = oqd::OrderSide::BuyToOpen;
            option_order.quantity = 1;
            option_order.type = oqd::OrderType::Limit;
            option_order.duration = oqd::OrderDuration::Day;
            option_order.price = 0.01; 

            auto response = api->place_option_order(account_id, option_order);
            std::cout << "Option order placed successfully!" << std::endl;
            std::cout << "Order ID: " << response.id << std::endl;
            std::cout << "Status: " << response.status << std::endl;

        } catch (const std::exception& e) {
            std::cout << "Error placing option order: " << e.what() << std::endl;
        }
        */


        // Example 7: Cancel an order (if you have an order ID)
        /*
        std::cout << "\n=== Canceling Order ===" << std::endl;
        try {
            std::string order_id = "YOUR_ORDER_ID"; // Replace with actual order ID
            auto response = api->cancel_order(account_id, order_id);
            std::cout << "Order cancellation request sent" << std::endl;
            std::cout << "Status: " << response.status << std::endl;

        } catch (const std::exception& e) {
            std::cout << "Error canceling order: " << e.what() << std::endl;
        }
        */


        // Example 8: Asynchronous trading operations
        std::cout << "\n=== Asynchronous Trading Operations ===" << std::endl;
        try {
            std::cout << "Starting async requests..." << std::endl;
            
            auto balances_future = api->get_account_balances_async(account_id);
            auto positions_future = api->get_account_positions_async(account_id);
            auto orders_future = api->get_account_orders_async(account_id);

            std::cout << "Waiting for results..." << std::endl;

            auto balances = balances_future.get();
            auto positions = positions_future.get();
            auto orders = orders_future.get();

            std::cout << "Account equity: $" << balances.total_equity << std::endl;
            std::cout << "Number of positions: " << positions.size() << std::endl;
            std::cout << "Number of orders: " << orders.size() << std::endl;

        } catch (const std::exception& e) {
            std::cout << "Error with async trading operations: " << e.what() << std::endl;
        }

        oqd::cleanup_library();
        std::cout << "\nTrading example completed!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/*
Trading Example Usage Notes:

1. Replace YOUR_ACCESS_TOKEN_HERE with your actual Tradier access token
2. Replace YOUR_ACCOUNT_ID with your actual account ID
3. Order placement examples are commented out for safety
4. Always test in sandbox mode first
5. Be careful with real money in production mode!

Risk Warning:
- This is example code for educational purposes
- Always preview orders before placing them
- Start with small quantities when testing
- Understand the risks involved in trading
- Consider the tax implications of your trades

To enable order placement:
1. Uncomment the order placement examples
2. Adjust symbols, quantities, and prices as needed
3. Test thoroughly in sandbox mode first
4. Never leave order placement code uncommented in production without proper safeguards
*/
