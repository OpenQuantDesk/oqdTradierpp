# Watchlist Module

This module provides watchlist management functionality for the oqdTradierpp library, allowing users to create, manage, and organize collections of symbols for monitoring.

## Overview

The watchlist module implements data structures and operations for managing symbol watchlists. Users can create multiple watchlists, add/remove symbols, and retrieve watchlist details for portfolio organization and market monitoring.


## Components

### Basic Watchlist Management

#### `watchlist.cpp` - Watchlist Structure
- **`Watchlist`**: Basic watchlist information
  - `id`: Unique watchlist identifier
  - `name`: User-defined watchlist name

### Detailed Watchlist Information

#### `watchlist_detail.cpp` - Watchlist with Symbols
- **`WatchlistDetail`**: Complete watchlist with symbol list
  - `id`: Unique watchlist identifier
  - `name`: User-defined watchlist name
  - `symbols`: Vector of symbol strings in the watchlist
  - `item_count`: Number of symbols in the watchlist

## Usage Examples

### Creating Watchlists
```cpp
// Create a new watchlist
auto watchlist = api->create_watchlist("Tech Stocks", {"AAPL", "MSFT", "GOOGL"});
std::cout << "Created watchlist: " << watchlist.name 
          << " (ID: " << watchlist.id << ")" << std::endl;

// Create empty watchlist and add symbols later
auto empty_list = api->create_watchlist("Energy Sector");
auto detailed = api->add_symbols_to_watchlist(empty_list.id, {"XOM", "CVX", "COP"});
```

### Managing Watchlist Contents
```cpp
// Get all user watchlists
auto all_watchlists = api->get_all_watchlists();
for (const auto& wl : all_watchlists) {
    std::cout << "Watchlist: " << wl.name << " (ID: " << wl.id << ")" << std::endl;
}

// Get detailed view of specific watchlist
auto details = api->get_specific_watchlist("12345");
std::cout << "Watchlist '" << details.name << "' contains:" << std::endl;
for (const auto& symbol : details.symbols) {
    std::cout << "  - " << symbol << std::endl;
}
```

### Modifying Watchlists
```cpp
// Add symbols to existing watchlist
auto updated = api->add_symbols_to_watchlist("12345", {"TSLA", "NFLX"});
std::cout << "Added symbols. New count: " << updated.item_count << std::endl;

// Remove symbol from watchlist
auto after_removal = api->remove_symbol_from_watchlist("12345", "NFLX");

// Update watchlist name
auto renamed = api->update_watchlist("12345", "Mega Cap Tech", std::nullopt);

// Update both name and symbols
std::vector<std::string> new_symbols = {"AAPL", "MSFT", "AMZN", "GOOGL"};
auto fully_updated = api->update_watchlist("12345", "FAANG Stocks", new_symbols);
```

### Watchlist-Based Market Monitoring
```cpp
// Get quotes for all symbols in a watchlist
auto watchlist_detail = api->get_specific_watchlist("12345");
auto quotes = api->get_quotes(watchlist_detail.symbols);

std::cout << "Portfolio Summary for '" << watchlist_detail.name << "':" << std::endl;
double total_change = 0.0;
for (const auto& quote : quotes) {
    double change_pct = (quote.change / quote.prevclose) * 100;
    total_change += change_pct;
    
    std::cout << quote.symbol << ": $" << quote.last 
              << " (" << std::fixed << std::setprecision(2) 
              << change_pct << "%)" << std::endl;
}

double avg_change = total_change / quotes.size();
std::cout << "Average change: " << std::setprecision(2) << avg_change << "%" << std::endl;
```

### Watchlist-Based Trading
```cpp
// Place orders for multiple symbols in a watchlist
auto tech_watchlist = api->get_specific_watchlist("tech_stocks");
auto quotes = api->get_quotes(tech_watchlist.symbols);

for (const auto& quote : quotes) {
    if (quote.change < -0.05) {  // Stock down more than 5%
        EquityOrderRequest order;
        order.symbol = quote.symbol;
        order.side = OrderSide::Buy;
        order.quantity = 10;
        order.type = OrderType::Market;
        order.duration = OrderDuration::Day;
        
        auto response = api->place_equity_order(account_id, order);
        std::cout << "Bought " << quote.symbol << " on dip" << std::endl;
    }
}
```

## Issues Identified

### JSON Serialization Inconsistency
**Problem**: All watchlist classes use std::ostringstream instead of the bespoke JSON builder.

**Impact**: 
- Performance degradation compared to other modules
- Inconsistent serialization approach across library
- Missing the 8-35μs performance benefits of the custom JSON builder

**Required Fix**: Migrate all `to_json()` methods to use `oqd::json::create_object()`.

### Missing JSON Builder Include
**Problem**: Files still include `<sstream>` instead of the JSON builder.

**Required Change**: Replace `#include <sstream>` with `#include "oqdTradierpp/core/json_builder.hpp"`.

## Integration Points

### With Market Data
```cpp
// Monitor watchlist performance
auto watchlist = api->get_specific_watchlist("growth_stocks");
auto quotes = api->get_quotes(watchlist.symbols);

// Calculate watchlist metrics
double total_market_value = 0.0;
for (const auto& quote : quotes) {
    total_market_value += quote.last * 100;  // Assume 100 shares each
}
```

### With Account Management
```cpp
// Compare watchlist against current positions
auto positions = api->get_account_positions(account_id);
auto watchlist = api->get_specific_watchlist("target_stocks");

std::vector<std::string> not_owned;
for (const auto& symbol : watchlist.symbols) {
    bool found = false;
    for (const auto& pos : positions) {
        if (pos.symbol == symbol) {
            found = true;
            break;
        }
    }
    if (!found) {
        not_owned.push_back(symbol);
    }
}

std::cout << "Watchlist symbols not currently owned:" << std::endl;
for (const auto& symbol : not_owned) {
    std::cout << "  - " << symbol << std::endl;
}
```

### With Streaming
```cpp
// Stream real-time data for watchlist symbols
auto watchlist = api->get_specific_watchlist("active_trades");
for (const auto& symbol : watchlist.symbols) {
    streaming->subscribe_quotes({symbol});
}

streaming->on_quote([](const Quote& quote) {
    // Process real-time updates for watchlist symbols
    std::cout << "Update: " << quote.symbol << " = $" << quote.last << std::endl;
});
```

## Performance Characteristics

### Current Performance (Before Migration)
- **JSON Serialization**: ~50-100μs using std::ostringstream
- **Memory Usage**: Higher due to temporary string objects
- **Simple Structure**: Minimal overhead due to basic data types

### Target Performance (After Migration)
- **JSON Serialization**: 8-15μs using bespoke JSON builder
- **Memory Usage**: Minimal with direct buffer manipulation
- **Batch Operations**: Efficient for multiple watchlist operations

## Required Migration Tasks

1. **Update includes**: Replace `<sstream>` with JSON builder header
2. **Migrate to_json()**: Convert all serialization to use `json::create_object()`
3. **Performance testing**: Benchmark before/after migration
4. **Integration testing**: Ensure API compatibility maintained

## Workflow Patterns

### Sector Rotation Strategy
```cpp
// Create sector-based watchlists
auto tech_wl = api->create_watchlist("Technology", {"AAPL", "MSFT", "GOOGL"});
auto health_wl = api->create_watchlist("Healthcare", {"JNJ", "PFE", "MRK"});
auto finance_wl = api->create_watchlist("Finance", {"JPM", "BAC", "WFC"});

// Monitor sector performance
std::vector<std::string> watchlist_ids = {tech_wl.id, health_wl.id, finance_wl.id};
for (const auto& wl_id : watchlist_ids) {
    auto detail = api->get_specific_watchlist(wl_id);
    auto quotes = api->get_quotes(detail.symbols);
    
    double sector_change = 0.0;
    for (const auto& quote : quotes) {
        sector_change += (quote.change / quote.prevclose) * 100;
    }
    double avg_sector_change = sector_change / quotes.size();
    
    std::cout << detail.name << " sector average: " 
              << std::fixed << std::setprecision(2) 
              << avg_sector_change << "%" << std::endl;
}
```
