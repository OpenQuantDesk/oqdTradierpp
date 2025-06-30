# Market Data Module

This module provides comprehensive market data functionality for the oqdTradierpp library, including real-time quotes, historical data, option chains, market status, symbol search, and time & sales data.

## Overview

The market data module implements all market data structures and operations required for professional trading applications. All classes use the high-performance bespoke JSON builder and include robust error handling for API response parsing.

## Components

### Real-Time Market Data

#### `quote.hpp/cpp` - Market Quotes
- **`Quote`**: Comprehensive market quote data structure
  - **Basic Fields**: `symbol`, `last`, `change`, `volume`, `open`, `high`, `low`, `close`
  - **Bid/Ask Data**: `bid`, `ask`, `bidsize`, `asksize`, `bidexch`, `askexch`
  - **Additional Metrics**: `prevclose`, `week_52_high`, `week_52_low`, `biddate`, `askdate`
  - **Options Data**: `strike`, `open_interest`, `contract_size`, `expiration_date`
  - **Greeks**: `delta`, `gamma`, `theta`, `vega`, `rho`, `phi`
  - **Volatility**: `bid_iv`, `mid_iv`, `ask_iv`, `smv_vol`
  - **Metadata**: `updated_at`, `option_type`, `expiration_type`, `root_symbol`

#### Key Features:
- **Comprehensive Coverage**: Supports both equity and option quotes
- **Options Greeks**: Complete derivatives risk metrics
- **Real-time Updates**: Timestamp tracking for data freshness
- **Exchange Information**: Bid/ask exchange identifiers

### Historical Data

#### `historical_data.hpp/cpp` - OHLCV Data
- **`HistoricalData`**: Historical price and volume data
  - `date`: Trading date (YYYY-MM-DD format)
  - `open`: Opening price
  - `high`: Daily high price
  - `low`: Daily low price  
  - `close`: Closing price
  - `volume`: Trading volume

#### Applications:
- **Technical Analysis**: OHLCV data for indicators and patterns
- **Backtesting**: Historical price data for strategy validation
- **Charting**: Data for price charts and visualization

### Options Market Data

#### `option_chain.hpp/cpp` - Options Chains
- **`OptionChain`**: Complete option chain data
  - `underlying`: Underlying security symbol
  - `options`: Vector of option quotes with all strikes and expirations
  - Complete Greeks and volatility data for each option

#### Features:
- **Complete Chains**: All strikes and expirations
- **Greeks Included**: Delta, gamma, theta, vega for risk analysis
- **Implied Volatility**: Bid, mid, and ask IV for each option
- **Open Interest**: Contract open interest data

### Market Infrastructure

#### `market_status.hpp/cpp` - Trading Sessions
- **`MarketClock`**: Real-time market status
  - `date`: Current trading date
  - `state`: Current market state (open/closed/premarket/postmarket)
  - `timestamp`: Current server timestamp
  - `next_state_change`: When the next state change occurs
  - `next_state`: What the next state will be

- **`MarketDay`**: Daily trading schedule
  - `date`: Trading date
  - `status`: Market status for the day
  - `description`: Human-readable status description
  - `premarket`: Pre-market session details (start/end times)
  - `open`: Regular session details (start/end times)
  - `close`: Regular session closing information
  - `postmarket`: Post-market session details (start/end times)

#### Market States:
- **`open`**: Regular trading session active
- **`closed`**: Market closed
- **`premarket`**: Pre-market trading session
- **`postmarket`**: After-hours trading session

### Symbol Management

#### `symbol_search.hpp/cpp` - Symbol Discovery
- **`CompanySearch`**: Company name-based search results
  - `symbol`: Stock symbol
  - `company`: Company name
  - `exchange`: Primary exchange

- **`SymbolLookup`**: Symbol lookup results
  - `symbol`: Security symbol
  - `exchange`: Primary exchange
  - `type`: Security type (stock, etf, etc.)
  - `description`: Security description

#### Use Cases:
- **Symbol Discovery**: Find symbols by company name
- **Validation**: Verify symbol existence and details
- **Exchange Information**: Determine primary listing exchange

### Trade Data

#### `time_sales.hpp/cpp` - Tick Data
- **`TimeSales`**: Individual trade records
  - `timestamp`: Trade execution time
  - `price`: Trade price
  - `open`: Session opening price
  - `high`: Session high price
  - `low`: Session low price
  - `close`: Most recent trade price
  - `volume`: Cumulative volume
  - `vwap`: Volume-weighted average price

#### Applications:
- **Execution Analysis**: Trade timing and price analysis
- **Market Microstructure**: Order flow and execution patterns
- **VWAP Calculations**: Volume-weighted pricing strategies

## Usage Examples

### Getting Market Quotes
```cpp
// Single symbol quote
auto quote = api->get_quote("AAPL");
std::cout << quote.symbol << ": $" << quote.last 
          << " (Vol: " << quote.volume << ")" << std::endl;

// Multiple symbols
std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOGL"};
auto quotes = api->get_quotes(symbols);
for (const auto& quote : quotes) {
    double change_pct = (quote.change / quote.prevclose) * 100;
    std::cout << quote.symbol << ": $" << quote.last 
              << " (" << std::fixed << std::setprecision(2) 
              << change_pct << "%)" << std::endl;
}
```

### Options Analysis
```cpp
// Get option chain
auto chain = api->option_chain("AAPL", "2024-12-20");
std::cout << "Options for " << chain.underlying << ":" << std::endl;

for (const auto& option : chain.options) {
    if (option.option_type == "call") {
        std::cout << "Strike $" << option.strike 
                  << " Call: $" << option.last
                  << " (Delta: " << option.delta.value_or(0) 
                  << ")" << std::endl;
    }
}
```

### Historical Data Analysis
```cpp
// Get 1-year daily data
auto history = api->historical_data("SPY", "daily", "2023-01-01", "2023-12-31");

double total_return = 0.0;
if (!history.empty()) {
    double start_price = history.front().close;
    double end_price = history.back().close;
    total_return = ((end_price - start_price) / start_price) * 100;
}

std::cout << "SPY 1-year return: " << std::fixed << std::setprecision(2) 
          << total_return << "%" << std::endl;
```

### Market Status Monitoring
```cpp
// Check market status
auto status = api->market_status();
std::cout << "Market Status: " << status.state << std::endl;
std::cout << "Description: " << status.description << std::endl;

if (status.next_state_change.has_value()) {
    std::cout << "Next change at: " << status.next_state_change.value() 
              << " to " << status.next_state.value_or("unknown") << std::endl;
}

// Get specific day's trading hours
auto market_day = api->market_calendar("2024-12-25");
if (market_day.status == "closed") {
    std::cout << "Market is closed on " << market_day.date 
              << " (" << market_day.description << ")" << std::endl;
}
```

### Symbol Search
```cpp
// Find symbols by company name
auto companies = api->company_search("Apple");
for (const auto& company : companies) {
    std::cout << company.symbol << ": " << company.company 
              << " (" << company.exchange << ")" << std::endl;
}

// Lookup specific symbol
auto lookup = api->symbol_lookup("AAPL");
for (const auto& result : lookup) {
    std::cout << result.symbol << " - " << result.description 
              << " [" << result.type << "]" << std::endl;
}
```

### Time & Sales Data
```cpp
// Get recent trades
auto trades = api->time_sales("SPY", "1min", "2024-01-15", "2024-01-15");
for (const auto& trade : trades) {
    std::cout << "Time: " << trade.timestamp 
              << " Price: $" << trade.price
              << " Volume: " << trade.volume
              << " VWAP: $" << trade.vwap << std::endl;
}
```

## Performance Characteristics

### JSON Processing
- **Serialization**: 8-35μs per object using bespoke JSON builder
- **Deserialization**: Efficient parsing with simdjson
- **Memory Usage**: Minimal overhead with optional field handling

### Data Structure Efficiency
- **Quote Objects**: ~50 fields with optional field optimization
- **Vector Storage**: Efficient bulk data handling
- **String Interning**: Consider for repeated symbols

### API Integration
- **Batch Requests**: Multiple symbols in single API call
- **Caching**: Built-in support for quote caching
- **Rate Limiting**: Automatic rate limit compliance

## Error Handling

### Robust Parsing
```cpp
// Safe quote processing
auto quotes = api->get_quotes(symbols);
for (const auto& quote : quotes) {
    if (!quote.symbol.empty() && quote.last > 0) {
        // Process valid quote
        process_quote(quote);
    } else {
        // Handle invalid/missing data
        log_warning("Invalid quote data for symbol");
    }
}
```

### Missing Data Handling
```cpp
// Optional field access
if (quote.delta.has_value()) {
    double delta = quote.delta.value();
    // Use delta for options analysis
} else {
    // Delta not available (equity quote)
    std::cout << "Delta not available for " << quote.symbol << std::endl;
}
```

## Integration Points

### With Trading Module
```cpp
// Use quotes for order pricing
auto quote = api->get_quote("AAPL");
if (quote.bid > 0 && quote.ask > 0) {
    double mid_price = (quote.bid + quote.ask) / 2.0;
    
    EquityOrderRequest order;
    order.symbol = quote.symbol;
    order.side = OrderSide::Buy;
    order.quantity = 100;
    order.type = OrderType::Limit;
    order.price = mid_price;  // Use market mid-point
}
```

### With Account Module
```cpp
// Value positions with current quotes
auto positions = api->account_positions(account_id);
std::vector<std::string> symbols;
for (const auto& pos : positions) {
    symbols.push_back(pos.symbol);
}

auto quotes = api->get_quotes(symbols);
double total_market_value = 0.0;
for (const auto& quote : quotes) {
    for (const auto& pos : positions) {
        if (pos.symbol == quote.symbol) {
            total_market_value += pos.quantity * quote.last;
        }
    }
}
```

### With Streaming
```cpp
// Real-time quote updates
streaming->on_quote([](const Quote& quote) {
    // Update local quote cache
    quote_cache[quote.symbol] = quote;
    
    // Trigger any quote-dependent calculations
    update_portfolio_values();
});
```

## Thread Safety

### Concurrent Access
- **Read Operations**: All quote data structures are thread-safe for reads
- **Collection Updates**: Use mutex protection for quote collections
- **Cache Management**: Thread-safe quote caching strategies

### Real-time Updates
```cpp
std::mutex quote_mutex;
std::unordered_map<std::string, Quote> quote_cache;

void update_quote(const Quote& quote) {
    std::lock_guard<std::mutex> lock(quote_mutex);
    quote_cache[quote.symbol] = quote;
}

Quote get_cached_quote(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(quote_mutex);
    return quote_cache[symbol];
}
```
