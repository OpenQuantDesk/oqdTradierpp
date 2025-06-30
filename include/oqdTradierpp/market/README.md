# Market Data Headers

This directory contains the header declarations for all market data structures and functionality in the oqdTradierpp library.

## Header Files

### Core Market Data Types

#### `quote.hpp`
- **`Quote`**: Complete market quote structure with 50+ fields
- **Options Support**: Greeks, implied volatility, open interest
- **Real-time Data**: Bid/ask, exchange information, timestamps
- **Performance**: Optional field optimization for memory efficiency

#### `historical_data.hpp`
- **`HistoricalData`**: OHLCV price and volume data
- **Time Series**: Support for daily, weekly, monthly intervals
- **Backtesting**: Clean data structure for strategy validation

#### `option_chain.hpp`
- **`OptionChain`**: Complete options chain with all strikes/expirations
- **Greeks Integration**: Full derivatives risk metrics
- **Volatility Data**: Bid, mid, ask implied volatility

### Market Infrastructure

#### `market_status.hpp`
- **`MarketClock`**: Real-time market state tracking
- **`MarketDay`**: Daily trading schedule and session details
- **Session Management**: Pre-market, regular, post-market hours

#### `symbol_search.hpp`
- **`CompanySearch`**: Company name-based symbol discovery
- **`SymbolLookup`**: Symbol validation and metadata
- **Exchange Information**: Primary listing details

#### `time_sales.hpp`
- **`TimeSales`**: Individual trade tick data
- **Execution Analytics**: Price, volume, VWAP calculations
- **Market Microstructure**: Order flow analysis support

## Design Patterns

### Data Structures
- **POD Design**: Plain old data for optimal performance
- **Optional Fields**: std::optional for missing data handling
- **Serialization**: JSON serialization with bespoke builder
- **Parsing**: simdjson integration for fast deserialization

### Memory Layout
```cpp
struct Quote {
    // Core fields (always present)
    std::string symbol;
    double last;
    double volume;
    
    // Optional fields (equity vs options)
    std::optional<double> delta;    // Options only
    std::optional<double> strike;   // Options only
    std::optional<std::string> expiration_date; // Options only
};
```

### Type Safety
- **Strong Typing**: Distinct types for different data categories
- **Validation**: Built-in data validation in from_json methods
- **Error Handling**: Graceful handling of missing/invalid data

## Usage Patterns

### Include Strategy
```cpp
#include "oqdTradierpp/market/quote.hpp"          // For basic quotes
#include "oqdTradierpp/market/option_chain.hpp"   // For options data
#include "oqdTradierpp/market/market_status.hpp"  // For market hours
```

### Real Data Examples

#### Quote Data Structure
```cpp
// Example quote for AAPL from live API
Quote aapl_quote;
aapl_quote.symbol = "AAPL";
aapl_quote.last = 201.08;
aapl_quote.bid = 202.10;
aapl_quote.ask = 202.20;
aapl_quote.volume = 565194;
aapl_quote.change = 0.00;
aapl_quote.change_percentage = 0.00;
```

#### Market Clock Information
```cpp
// Live market status from Tradier API
MarketClock clock;
clock.date = "2025-06-30";
clock.state = "premarket";
clock.description = "Market is in premarket hours from 07:00 to 09:24";
clock.timestamp = "";  // May be empty during certain periods
```

#### Historical Data Sample
```cpp
// SPY historical data example
HistoricalData spy_daily;
spy_daily.date = "2025-01-02";
spy_daily.open = 589.39;
spy_daily.high = 591.13;
spy_daily.low = 580.50;
spy_daily.close = 584.64;
spy_daily.volume = 50203975;
```

### Forward Declarations
```cpp
// Forward declarations available in this module
namespace oqd {
    struct Quote;
    struct OptionChain;
    struct MarketClock;
    struct HistoricalData;
    struct TimeSales;
    struct CompanySearch;
    struct SymbolLookup;
    struct MarketDay;
}
```

### Template Compatibility
```cpp
// All market data types support:
template<typename T>
void process_market_data(const T& data) {
    // JSON serialization
    std::string json = data.to_json();
    
    // Copy/move semantics
    T copy = data;
    T moved = std::move(copy);
}
```

## Integration Points

### With API Layer
```cpp
// Headers provide types for API methods
std::vector<Quote> quotes = api->get_quotes(symbols);
OptionChain chain = api->get_option_chain("AAPL", "2024-12-20");
MarketClock status = api->get_market_clock();
```

### With Streaming
```cpp
// Market data callbacks use these types
streaming->on_quote([](const Quote& quote) {
    // Process real-time quote update
});
```

### With Trading
```cpp
// Market data informs trading decisions
Quote quote = api->get_quote("AAPL");
if (quote.bid > 0 && quote.ask > 0) {
    double mid_price = (quote.bid + quote.ask) / 2.0;
    // Use for limit order pricing
}
```

## Performance Considerations

### Memory Efficiency
- **Optional Fields**: Only allocate memory for present data
- **String Interning**: Consider for repeated symbols in large datasets
- **Vector Reserve**: Pre-allocate containers for bulk operations

### Serialization Performance
- **Bespoke JSON**: 8-35μs serialization using custom JSON builder
- **Minimal Copying**: Move semantics throughout
- **Buffer Reuse**: JsonBuilder supports reuse for repeated operations

### Cache Friendliness
```cpp
// Struct layout optimized for cache lines
struct Quote {
    // Hot data first (frequently accessed)
    double last, bid, ask;
    
    // Warm data (sometimes accessed)
    std::string symbol;
    double volume, change;
    
    // Cold data last (rarely accessed)
    std::optional<double> week_52_high;
    std::optional<std::string> expiration_date;
};
```

This header collection provides the foundation for all market data operations in the oqdTradierpp library, emphasizing performance, type safety, and integration with real-time trading systems.