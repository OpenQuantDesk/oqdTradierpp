# Trading Module Headers

This directory contains the header files for the trading module, defining all order types, strategies, and management structures used in professional trading applications.

## Header Files Overview

### Core Order Infrastructure

#### `order.hpp`
**Core order data structures**
```cpp
struct Leg {
    std::string option_symbol;
    OrderSide side;
    int quantity;
    static Leg from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct Order {
    // Complete order representation with 25+ fields
    // Full order lifecycle tracking
    // Comprehensive JSON parsing
};
```

#### `order_requests.hpp`
**Order placement requests**
```cpp
struct OrderRequest {
    OrderClass order_class;
    std::string symbol;
    OrderSide side;
    int quantity;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::optional<double> stop;
    std::optional<std::string> tag;
};

struct EquityOrderRequest : public OrderRequest;
struct OptionOrderRequest : public OrderRequest {
    std::string option_symbol;
};
```

### Advanced Order Types

#### `advanced_orders.hpp`
**Complex trading strategies**
```cpp
struct OrderComponent {
    // Individual order within complex strategies
    std::string symbol;
    OrderSide side;
    int quantity;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::optional<double> stop;
    std::optional<std::string> option_symbol;
    std::optional<std::string> tag;
};

struct OTOOrderRequest {
    // One-Triggers-Other: Entry followed by target/stop
    OrderClass order_class = OrderClass::OTO;
    OrderComponent first_order;
    OrderComponent second_order;
    std::optional<std::string> tag;
};

struct OCOOrderRequest {
    // One-Cancels-Other: Alternative orders
    OrderClass order_class = OrderClass::OCO;
    OrderComponent first_order;
    OrderComponent second_order;
    std::optional<std::string> tag;
};

struct OTOCOOrderRequest {
    // One-Triggers-OCO: Complete bracket order
    OrderClass order_class = OrderClass::OTOCO;
    OrderComponent primary_order;  // Entry
    OrderComponent profit_order;   // Target
    OrderComponent stop_order;     // Stop loss
    std::optional<std::string> tag;
};
```

### Multi-Leg Strategies

#### `multileg_orders.hpp`
**Options spread orders**
```cpp
struct MultilegOrderRequest {
    OrderClass order_class = OrderClass::Multileg;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::vector<Leg> legs;
    std::optional<std::string> tag;
};

struct ComboOrderRequest {
    // Mixed equity/option strategies
    OrderClass order_class = OrderClass::Combo;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::vector<Leg> legs;
    std::optional<std::string> equity_symbol;
    std::optional<OrderSide> equity_side;
    std::optional<int> equity_quantity;
    std::optional<std::string> tag;
};
```

#### `spread_orders.hpp`
**Specialized spread structures**
```cpp
struct SpreadLeg {
    std::string option_symbol;
    OrderSide side;
    int quantity;
    std::optional<double> ratio;  // For uneven spreads
    
    static SpreadLeg from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct SpreadOrderRequest {
    OrderClass order_class = OrderClass::Multileg;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::string spread_type;
    std::vector<SpreadLeg> legs;
    std::optional<std::string> tag;
};
```

### Order Management

#### `order_management.hpp`
**Order lifecycle management**
```cpp
struct OrderModification {
    // Modify existing orders
    std::optional<OrderType> type;
    std::optional<OrderDuration> duration;
    std::optional<double> price;
    std::optional<double> stop;
    std::optional<int> quantity;
};

struct OrderPreview {
    // Pre-execution analysis
    double commission;
    double cost;
    double fees;
    std::string symbol;
    int quantity;
    OrderSide side;
    OrderType type;
    OrderDuration duration;
    std::optional<double> price;
    std::string result;
    std::string strategy_type;
    std::string last_day;
    double day_trades;
    double buying_power;
    double change;
    double amount;
};

struct OrderResponse {
    // Order placement confirmation
    std::string id;
    std::string status;
};
```

## Design Principles

### Type Safety
- **Strong typing** for all order parameters
- **Enum classes** for order types, sides, and durations
- **Optional fields** for parameters that may not apply

### Performance
- **Header-only where possible** for compile-time optimization
- **Move semantics** for efficient object transfer
- **Minimal memory footprint** with careful field selection

### Extensibility
- **Inheritance hierarchy** for order types
- **Template compatibility** for generic algorithms
- **JSON serialization** for all structures

## Common Patterns

### Order Creation
```cpp
// Basic equity order
EquityOrderRequest equity_order;
equity_order.symbol = "AAPL";
equity_order.side = OrderSide::Buy;
equity_order.quantity = 100;
equity_order.type = OrderType::Limit;
equity_order.price = 150.00;
equity_order.duration = OrderDuration::Day;

// Option order
OptionOrderRequest option_order;
option_order.option_symbol = "AAPL240315C00150000";
option_order.side = OrderSide::BuyToOpen;
option_order.quantity = 5;
option_order.type = OrderType::Market;
option_order.duration = OrderDuration::Day;
```

### Spread Construction
```cpp
// Call spread
Leg long_call;
long_call.option_symbol = "SPY240315C00400000";
long_call.side = OrderSide::BuyToOpen;
long_call.quantity = 1;

Leg short_call;
short_call.option_symbol = "SPY240315C00410000";
short_call.side = OrderSide::SellToOpen;
short_call.quantity = 1;

MultilegOrderRequest call_spread;
call_spread.type = OrderType::Market;
call_spread.duration = OrderDuration::Day;
call_spread.legs = {long_call, short_call};
```

### Bracket Orders
```cpp
// Complete risk management
OTOCOOrderRequest bracket;
bracket.primary_order = entry_order;
bracket.profit_order = target_order;
bracket.stop_order = stop_loss_order;
```

## Integration Points

### With API Methods
```cpp
auto api = create_api_methods(client);
auto response = api->place_equity_order(account_id, equity_order);
auto preview = api->preview_order(account_id, option_order);
auto modification = api->modify_order(account_id, order_id, changes);
```

### With Streaming
```cpp
streaming->on_order_update([](const Order& order) {
    // Real-time order status changes
    if (order.status == "filled") {
        // Handle fill
    }
});
```

### With Account Management
```cpp
auto positions = api->account_positions(account_id);
auto balances = api->account_balances(account_id);
// Use for order sizing and risk management
```

## Thread Safety

All structures are **thread-safe for read operations** and **move-safe for transfer between threads**. JSON serialization is **stateless and thread-safe**.

## Memory Management

- **RAII compliance** for all dynamic allocations
- **Smart pointer usage** where appropriate
- **Move semantics** to avoid unnecessary copies
- **Stack allocation** preferred for order structures