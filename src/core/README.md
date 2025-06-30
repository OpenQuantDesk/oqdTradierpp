# Core Module

This module contains the fundamental building blocks of the oqdTradierpp library, including essential enumerations and the high-performance bespoke JSON builder that powers all serialization operations.

## Overview

The core module provides the foundational types and utilities used throughout the library. It emphasizes performance, type safety, and modern C++ practices while maintaining simplicity and reliability.

## Components

### Enumerations (`enums.hpp/cpp`)

#### Trading Enumerations
The core module defines all essential trading enumerations with complete string conversion support:

**`OrderClass`** - Order categorization
- `Equity`: Stock orders
- `Option`: Single-leg option orders  
- `Multileg`: Multi-leg option strategies
- `Combo`: Combined equity/option orders
- `OTO`: One-Triggers-Other orders
- `OCO`: One-Cancels-Other orders
- `OTOCO`: One-Triggers-OCO orders

**`OrderType`** - Execution methods
- `Market`: Immediate execution at best price
- `Limit`: Execution at specified price or better
- `Stop`: Market order triggered at stop price
- `StopLimit`: Limit order triggered at stop price
- `Debit`: Net debit spread order
- `Credit`: Net credit spread order
- `Even`: Even money spread order

**`OrderDuration`** - Time in force
- `Day`: Valid until market close
- `GTC`: Good Till Canceled
- `Pre`: Pre-market session
- `Post`: Post-market session
- `IOC`: Immediate Or Cancel
- `FOK`: Fill Or Kill

**`OrderSide`** - Transaction direction
- `Buy`: Buy equity
- `Sell`: Sell equity
- `SellShort`: Short sell equity
- `BuyToCover`: Cover short position
- `BuyToOpen`: Open long option position
- `BuyToClose`: Close short option position
- `SellToOpen`: Open short option position
- `SellToClose`: Close long option position

**`OrderStatus`** - Order lifecycle states
- `Pending`: Order submitted, awaiting processing
- `Open`: Order active in market
- `Filled`: Order completely executed
- `Canceled`: Order canceled
- `Rejected`: Order rejected by exchange/broker
- `Expired`: Order expired (GTC/GTD orders)

#### String Conversion Functions
All enumerations include bidirectional string conversion:

```cpp
// Convert enum to string
std::string to_string(OrderType type);
std::string to_string(OrderSide side);
std::string to_string(OrderDuration duration);
std::string to_string(OrderClass order_class);
std::string to_string(OrderStatus status);

// Convert string to enum (with default fallback)
OrderType order_type_from_string(const std::string& str);
OrderSide order_side_from_string(const std::string& str);
OrderDuration order_duration_from_string(const std::string& str);
OrderClass order_class_from_string(const std::string& str);
OrderStatus order_status_from_string(const std::string& str);
```

### JSON Builder (`json_builder.hpp`)

#### High-Performance Serialization
The bespoke JSON builder achieves exceptional performance while maintaining type safety and ease of use:

**Performance Characteristics**:
- **8-35μs per operation** (3-5x faster than external libraries)
- **Memory efficient** with pre-allocated buffers
- **Zero external dependencies** 
- **Template-based** for compile-time optimization

**Key Features**:
- **Method chaining** for fluent API
- **Type safety** with template specialization
- **Automatic escaping** for JSON special characters
- **Precision control** for floating-point numbers
- **Optional field support** 
- **Array serialization**
- **Move semantics** for efficient transfers

#### Core API

**Object Creation**:
```cpp
auto builder = json::create_object()
    .field("symbol", "AAPL")
    .field("price", 150.25)
    .field("quantity", 100)
    .end_object();
```

**Array Creation**:
```cpp
auto builder = json::create_array()
    .element("AAPL")
    .element("MSFT") 
    .element("GOOGL")
    .end_array();
```

**Type Support**:
- **Strings**: Automatic escaping of quotes, backslashes, control characters
- **Integers**: All standard integer types with `std::to_chars` optimization
- **Doubles**: Configurable precision with fixed/dynamic notation
- **Booleans**: Native true/false representation
- **Enums**: Automatic string conversion using `to_string()`
- **Objects**: Nested objects via `to_json()` methods
- **Arrays**: Vector serialization with `array_field()`

**Precision Control**:
```cpp
auto builder = json::create_object()
    .set_fixed().set_precision(2)
    .field("price", 123.456789);  // Output: "price":123.46
```

**Optional Fields**:
```cpp
std::optional<double> price;
auto builder = json::create_object()
    .field_optional("price", price);  // Omitted if empty
```

## Usage Examples

### Enum Conversions
```cpp
// String to enum
OrderType type = order_type_from_string("limit");
OrderSide side = order_side_from_string("buy_to_open");

// Enum to string
std::string type_str = to_string(OrderType::Limit);     // "limit"
std::string side_str = to_string(OrderSide::BuyToOpen); // "buy_to_open"
```

### JSON Serialization
```cpp
// Simple object
auto json = json::create_object()
    .field("symbol", "AAPL")
    .field("side", OrderSide::Buy)
    .field("quantity", 100)
    .field("type", OrderType::Market)
    .end_object()
    .str();
// Result: {"symbol":"AAPL","side":"buy","quantity":100,"type":"market"}

// Complex nested structure
auto json = json::create_object()
    .field("order_class", OrderClass::Equity)
    .set_fixed().set_precision(2)
    .field("price", 150.256)
    .array_field("symbols", std::vector<std::string>{"AAPL", "MSFT"})
    .end_object()
    .str();
```

### Error Handling
```cpp
// Enum conversion with validation
OrderType parse_order_type(const std::string& input) {
    OrderType type = order_type_from_string(input);
    
    // Verify the conversion was successful
    if (to_string(type) != input) {
        throw std::invalid_argument("Invalid order type: " + input);
    }
    
    return type;
}
```

## Performance Optimization

### JSON Builder Optimizations
1. **Buffer Pre-allocation**: Initial 1024-byte buffer to minimize reallocations
2. **std::to_chars**: Modern C++17 integer conversion for optimal performance
3. **Batch Operations**: Efficient string concatenation with append operations
4. **Template Specialization**: Compile-time type dispatch
5. **Move Semantics**: Efficient object transfers

### Memory Management
```cpp
// Efficient object creation with move semantics
JsonBuilder create_order_json(const EquityOrderRequest& order) {
    return json::create_object()
        .field("symbol", order.symbol)
        .field("side", order.side)
        .field("quantity", order.quantity)
        .end_object();  // Return value optimization applies
}

// Use with move semantics
std::string json_str = create_order_json(order).str();
```

### String Escaping Performance
The JSON builder includes optimized string escaping:
- **Batch processing** of unescaped segments
- **Minimal allocations** with efficient reserve strategies
- **Unicode support** with proper \uXXXX encoding
- **Common case optimization** for strings without special characters

## Integration Points

### With Trading Module
```cpp
// Order serialization
std::string EquityOrderRequest::to_json() const {
    auto builder = json::create_object()
        .field("class", order_class)      // Uses enum to_string()
        .field("symbol", symbol)
        .field("side", side)              // Uses enum to_string()
        .field("quantity", quantity)
        .field("type", type)              // Uses enum to_string()
        .field("duration", duration);     // Uses enum to_string()
    
    if (price.has_value()) {
        builder.set_fixed().set_precision(2)
               .field("price", price.value());
    }
    
    return builder.end_object().str();
}
```

### With API Methods
```cpp
// Response parsing
OrderStatus parse_order_status(const std::string& status_str) {
    return order_status_from_string(status_str);
}

// Request serialization uses JSON builder automatically
auto response = client->post("/orders", order.to_json());
```

### With Streaming
```cpp
// Real-time data parsing
void process_quote_update(const simdjson::dom::element& data) {
    OrderSide side = order_side_from_string(
        std::string(data["side"].get_string().value())
    );
    // Process with type-safe enum
}
```

## Thread Safety

### Enumerations
- **Fully thread-safe**: All enum operations are stateless
- **Concurrent access**: Safe for read operations from multiple threads
- **Immutable**: Enum values never change after compilation

### JSON Builder
- **Thread-safe creation**: Each builder instance is independent  
- **Local state**: No shared state between builder instances
- **Move-safe**: Safe to transfer between threads via move semantics

## Error Handling Strategy

### Current Behavior
- **Enum conversions**: Default to first enum value for invalid strings
- **JSON builder**: Graceful handling of type conversion errors
- **No exceptions**: Library is exception-neutral by design

### Recommended Usage
```cpp
// Validate enum conversions
bool is_valid_order_type(const std::string& str) {
    OrderType type = order_type_from_string(str);
    return to_string(type) == str;
}

// Use validation before conversion
if (is_valid_order_type(input)) {
    OrderType type = order_type_from_string(input);
    // Safe to use type
}
```

The core module provides the essential foundation for all library operations with emphasis on performance, type safety, and reliability.