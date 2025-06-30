# oqdTradierpp Headers

This directory contains the complete header interface for the oqdTradierpp C++20 library, providing professional trading API access to the Tradier brokerage platform.

## Overview

The oqdTradierpp header collection offers a comprehensive, type-safe interface for algorithmic trading, portfolio management, and market data analysis. All headers are designed for high-performance applications with emphasis on low latency and memory efficiency.

## Module Headers

### Core Infrastructure

#### `core/`
- **`json_builder.hpp`**: High-performance JSON serialization (8-35μs)
- **`enums.hpp`**: Type-safe trading enumerations
- **Performance**: Zero-dependency JSON processing optimized for trading

#### `auth/`
- **`access_token.hpp`**: OAuth 2.0 token management
- **Security**: Production-ready authentication patterns

### Data Access Headers

#### `account/`
- **`account_balances.hpp`**: Account balance and buying power
- **`position.hpp`**: Portfolio positions and P&L tracking
- **`account_history.hpp`**: Transaction history and reporting
- **`gain_loss.hpp`**: Realized gain/loss analysis
- **`user_profile.hpp`**: Account configuration and preferences

#### `market/`
- **`quote.hpp`**: Real-time market quotes with options Greeks
- **`option_chain.hpp`**: Complete option chains with risk metrics
- **`historical_data.hpp`**: OHLCV data for backtesting
- **`market_status.hpp`**: Trading hours and market state
- **`symbol_search.hpp`**: Symbol discovery and validation
- **`time_sales.hpp`**: Tick-by-tick trade data

#### `fundamentals/`
- **`corp_info.hpp`**: Company profiles and business metrics
- **`corp_financials.hpp`**: Financial statements and ratios
- **`corp_actions.hpp`**: Corporate events and dividends
- **`corp_calendar.hpp`**: Earnings and event calendars
- **Analysis**: Comprehensive fundamental analysis data structures

#### `watchlist/`
- **`watchlist.hpp`**: Symbol list management
- **`watchlist_detail.hpp`**: Detailed watchlist with contents
- **Organization**: Portfolio monitoring and batch operations

### Trading Headers

#### `trading/`
- **`order.hpp`**: Order status and execution tracking
- **`order_requests.hpp`**: Order placement structures
- **`order_management.hpp`**: Order lifecycle management
- **`advanced_orders.hpp`**: OTO, OCO, OTOCO strategies
- **`multileg_orders.hpp`**: Complex options strategies
- **`spread_orders.hpp`**: Spread trading support

## Primary Interface Headers

### Main Library Interface

#### `api.hpp`
- **Unified API**: Complete Tradier API interface
- **Async/Sync**: Both operation modes supported
- **Type Safety**: Strong typing for all operations
- **Performance**: Optimized for trading latency requirements

#### `oqdTradierpp.hpp`
- **Library Entry Point**: Main include for library users
- **Client Management**: HTTP client configuration
- **Environment Support**: Sandbox and production modes

### Network and Communication

#### `client.hpp`
- **HTTP Client**: Boost.Asio-based HTTP implementation
- **Connection Management**: Persistent connections and pooling
- **SSL/TLS**: Secure communication with automatic certificate validation

#### `streaming.hpp`
- **WebSocket Streaming**: Real-time market data
- **Connection Management**: Automatic reconnection and failover
- **Threading**: Thread-safe real-time data processing

### Utility Headers

#### `types.hpp`
- **Core Types**: Fundamental library type definitions
- **Containers**: Specialized collections for trading data
- **Compatibility**: Standard library integration

#### `endpoints.hpp`
- **API Endpoints**: Tradier API URL management
- **Environment Switching**: Sandbox/production endpoint selection
- **Versioning**: API version management

#### `utils.hpp`
- **Utility Functions**: Common operations and helpers
- **String Processing**: Symbol validation and formatting
- **Time Handling**: Market hours and timestamp utilities

#### `validation.hpp`
- **Input Validation**: Order and parameter validation
- **Risk Controls**: Pre-trade risk checking
- **Data Integrity**: Type and range validation

## Design Principles

### Type Safety
```cpp
// Strong typing prevents errors
OrderSide side = OrderSide::Buy;        // Type-safe enum
OrderType type = OrderType::Limit;      // No magic strings
OrderDuration duration = OrderDuration::Day;  // Clear semantics
```

### Performance First
```cpp
// Optimized for trading latency
struct Quote {
    double last;           // Hot data first (cache-friendly)
    double bid, ask;       // Frequently accessed together
    std::string symbol;    // Warm data
    std::optional<double> delta;  // Cold data last
};
```

### RAII and Move Semantics
```cpp
// Efficient resource management
auto quotes = api->get_quotes(symbols);        // Move-friendly
auto quote = std::move(quotes[0]);             // Zero-copy transfer
std::string json = std::move(quote).to_json(); // Move-optimized
```

## Usage Patterns

### Minimal Includes
```cpp
// Include only what you need
#include "oqdTradierpp/market/quote.hpp"       // For market data
#include "oqdTradierpp/trading/order.hpp"      // For order management
#include "oqdTradierpp/account/position.hpp"   // For portfolio tracking
```

### Complete Interface
```cpp
// Single include for full functionality
#include "oqdTradierpp/oqdTradierpp.hpp"       // Everything included
```

### Module-Specific Usage
```cpp
// Trading-focused application
#include "oqdTradierpp/api.hpp"
#include "oqdTradierpp/trading/order_requests.hpp"
#include "oqdTradierpp/trading/advanced_orders.hpp"
#include "oqdTradierpp/market/quote.hpp"

// Market data application
#include "oqdTradierpp/api.hpp"
#include "oqdTradierpp/market/quote.hpp"
#include "oqdTradierpp/market/option_chain.hpp"
#include "oqdTradierpp/streaming.hpp"
```

## Integration Guidelines

### CMake Integration
```cmake
target_link_libraries(your_app oqdTradierpp)
target_include_directories(your_app PRIVATE ${oqdTradierpp_INCLUDE_DIRS})
```

### Namespace Usage
```cpp
using namespace oqd;  // Optional: Use library namespace

// Or explicit qualification
oqd::Quote quote = api->get_quote("AAPL");
oqd::EquityOrderRequest order;
```

### Error Handling
```cpp
#include "oqdTradierpp/api.hpp"

try {
    auto quotes = api->get_quotes({"AAPL", "MSFT"});
    // Process quotes
} catch (const std::exception& e) {
    std::cerr << "API error: " << e.what() << std::endl;
}
```

## Performance Characteristics

### Compilation Performance
- **Minimal Dependencies**: Headers include only necessary dependencies
- **Forward Declarations**: Reduce compilation time
- **Template Optimization**: Efficient compile-time code generation

### Runtime Performance
- **Zero-Copy Operations**: Move semantics throughout
- **Cache-Friendly Layout**: Hot data grouped for optimal access
- **Minimal Allocations**: Stack-based operations where possible

### Memory Efficiency
- **Optional Fields**: std::optional for missing data
- **String Optimization**: Efficient string handling
- **Container Optimization**: Vector reserve for known sizes

## Thread Safety Guarantees

### Immutable Types
```cpp
// Data structures are immutable after construction
const Quote quote = api->get_quote("AAPL");
// quote can be safely shared across threads
```

### Thread-Safe Operations
```cpp
// API client operations are thread-safe
std::thread t1([&]() { api->get_quotes(symbols1); });
std::thread t2([&]() { api->get_quotes(symbols2); });
```

### Streaming Safety
```cpp
// Streaming callbacks are thread-safe
streaming->on_quote([](const Quote& quote) {
    // This callback is thread-safe
    process_quote(quote);
});
```

## Migration Status

### Fully Migrated Modules
- ✅ **Core**: Complete bespoke JSON implementation
- ✅ **Auth**: OAuth 2.0 with high-performance serialization
- ✅ **Account**: All account operations optimized
- ✅ **Trading**: Complete trading infrastructure
- ✅ **Market**: Real-time data with performance optimization

### Pending Migrations
- ⚠️ **Fundamentals**: Needs bespoke JSON migration for consistency
- ⚠️ **Watchlist**: Needs bespoke JSON migration for performance

## Quality Assurance

### Header Validation
- **Include Guards**: All headers protected against multiple inclusion
- **Dependencies**: Minimal and explicit dependency declarations
- **Forward Declarations**: Used where possible to reduce coupling

### Documentation Standards
- **Doxygen Ready**: Headers documented for API generation
- **Usage Examples**: Practical examples in header comments
- **Performance Notes**: Latency and memory characteristics documented

This header collection provides a complete, high-performance interface for professional trading applications with emphasis on type safety, performance, and maintainability.