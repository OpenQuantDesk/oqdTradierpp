# Watchlist Headers

This directory contains header declarations for watchlist management in the oqdTradierpp library.

## Header Files

### Basic Watchlist Types

#### `watchlist.hpp`
- **`Watchlist`**: Basic watchlist information structure
  - `id`: Unique watchlist identifier (string)
  - `name`: User-defined watchlist name (string)
- **Operations**: Creation, basic management, listing
- **Lightweight**: Minimal structure for watchlist enumeration

#### `watchlist_detail.hpp`
- **`WatchlistDetail`**: Complete watchlist with symbol contents
  - `id`: Unique watchlist identifier
  - `name`: User-defined watchlist name  
  - `symbols`: Vector of symbol strings in the watchlist
  - `item_count`: Number of symbols in the watchlist
- **Full Context**: Complete watchlist information including contents
- **Management**: Symbol addition, removal, bulk operations

## Design Patterns

### Hierarchical Information Design
```cpp
// Basic info for listing operations
struct Watchlist {
    std::string id;
    std::string name;
};

// Detailed info when full context needed
struct WatchlistDetail : public Watchlist {
    std::vector<std::string> symbols;
    int item_count;
};
```

### Separation of Concerns
- **`Watchlist`**: Lightweight for listing and basic operations
- **`WatchlistDetail`**: Complete information for content management
- **API Efficiency**: Fetch only needed data level

### Consistent API Pattern
```cpp
// All watchlist types follow same pattern
struct WatchlistType {
    static WatchlistType from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

## Type Definitions

### Core Types
```cpp
namespace oqd {
    struct Watchlist {
        std::string id;
        std::string name;
    };
    
    struct WatchlistDetail {
        std::string id;
        std::string name;
        std::vector<std::string> symbols;
        int item_count;
    };
}
```

### Forward Declarations
```cpp
// For use in other modules
namespace oqd {
    struct Watchlist;
    struct WatchlistDetail;
}
```

## Usage Patterns

### Include Strategy
```cpp
// Include based on needed functionality level
#include "oqdTradierpp/watchlist/watchlist.hpp"        // Basic operations
#include "oqdTradierpp/watchlist/watchlist_detail.hpp" // Full management

// Or include both for complete functionality
#include "oqdTradierpp/watchlist/watchlist.hpp"
#include "oqdTradierpp/watchlist/watchlist_detail.hpp"
```

### Template Compatibility
```cpp
// Both types support standard operations
template<typename WatchlistType>
void process_watchlist(const WatchlistType& wl) {
    std::string json = wl.to_json();        // Serialization
    std::cout << "Watchlist: " << wl.name << std::endl;
}

// Works with both Watchlist and WatchlistDetail
process_watchlist(basic_watchlist);
process_watchlist(detailed_watchlist);
```

### Collection Operations
```cpp
// Efficient bulk operations
std::vector<Watchlist> all_watchlists = api->get_all_watchlists();
std::vector<WatchlistDetail> detailed_watchlists;

for (const auto& wl : all_watchlists) {
    detailed_watchlists.push_back(api->get_specific_watchlist(wl.id));
}
```

## Integration Points

### With API Layer
```cpp
// API methods use appropriate types
std::vector<Watchlist> get_all_watchlists();           // Lightweight listing
WatchlistDetail get_specific_watchlist(const std::string& id); // Full details
Watchlist create_watchlist(const std::string& name);   // Returns basic info
```

### With Market Data Module
```cpp
// Combine watchlists with market data
WatchlistDetail tech_stocks = api->get_specific_watchlist("tech_id");
std::vector<Quote> quotes = api->get_quotes(tech_stocks.symbols);

// Process watchlist quotes
for (const auto& quote : quotes) {
    std::cout << quote.symbol << ": $" << quote.last << std::endl;
}
```

### With Trading Module
```cpp
// Use watchlists for batch trading operations
WatchlistDetail targets = api->get_specific_watchlist("buy_list");
for (const auto& symbol : targets.symbols) {
    EquityOrderRequest order;
    order.symbol = symbol;
    order.side = OrderSide::Buy;
    order.quantity = 100;
    order.type = OrderType::Market;
    
    api->place_equity_order(account_id, order);
}
```

### With Account Module
```cpp
// Compare watchlists against positions
auto positions = api->get_account_positions(account_id);
auto watchlist = api->get_specific_watchlist("target_stocks");

// Find symbols in watchlist but not in positions
std::vector<std::string> missing_positions;
for (const auto& symbol : watchlist.symbols) {
    bool found = std::any_of(positions.begin(), positions.end(),
                           [&symbol](const Position& pos) {
                               return pos.symbol == symbol;
                           });
    if (!found) {
        missing_positions.push_back(symbol);
    }
}
```

## Performance Considerations

### Memory Efficiency
```cpp
// Use appropriate type for use case
std::vector<Watchlist> lightweight_list = api->get_all_watchlists();  // Minimal memory
WatchlistDetail full_info = api->get_specific_watchlist(id);          // When symbols needed
```

### Batch Operations
```cpp
// Efficient multi-watchlist processing
auto all_watchlists = api->get_all_watchlists();
std::vector<std::string> all_symbols;

// Collect all symbols from all watchlists
for (const auto& wl : all_watchlists) {
    auto detail = api->get_specific_watchlist(wl.id);
    all_symbols.insert(all_symbols.end(), 
                      detail.symbols.begin(), 
                      detail.symbols.end());
}

// Remove duplicates
std::sort(all_symbols.begin(), all_symbols.end());
all_symbols.erase(std::unique(all_symbols.begin(), all_symbols.end()), 
                  all_symbols.end());

// Single bulk quote request
auto all_quotes = api->get_quotes(all_symbols);
```

### Caching Strategy
```cpp
// Cache watchlist metadata for frequent access
class WatchlistCache {
    std::unordered_map<std::string, Watchlist> basic_cache_;
    std::unordered_map<std::string, WatchlistDetail> detail_cache_;
    
public:
    const Watchlist& get_basic(const std::string& id) {
        if (basic_cache_.find(id) == basic_cache_.end()) {
            // Fetch and cache
        }
        return basic_cache_[id];
    }
    
    const WatchlistDetail& get_detail(const std::string& id) {
        if (detail_cache_.find(id) == detail_cache_.end()) {
            detail_cache_[id] = api->get_specific_watchlist(id);
        }
        return detail_cache_[id];
    }
};
```

## Validation Patterns

### Input Validation
```cpp
bool is_valid_watchlist_name(const std::string& name) {
    return !name.empty() && name.length() <= 50;
}

bool is_valid_symbol(const std::string& symbol) {
    return !symbol.empty() && 
           symbol.length() <= 10 && 
           std::all_of(symbol.begin(), symbol.end(), 
                      [](char c) { return std::isalnum(c) || c == '.'; });
}
```

### Data Consistency
```cpp
void validate_watchlist_detail(const WatchlistDetail& detail) {
    assert(!detail.id.empty());
    assert(!detail.name.empty());
    assert(detail.item_count == static_cast<int>(detail.symbols.size()));
    
    for (const auto& symbol : detail.symbols) {
        assert(is_valid_symbol(symbol));
    }
}
```

## Thread Safety Considerations

### Concurrent Access
```cpp
// Thread-safe watchlist operations
class ThreadSafeWatchlistManager {
    mutable std::shared_mutex watchlist_mutex_;
    std::unordered_map<std::string, WatchlistDetail> watchlists_;
    
public:
    WatchlistDetail get_watchlist(const std::string& id) const {
        std::shared_lock<std::shared_mutex> lock(watchlist_mutex_);
        return watchlists_.at(id);
    }
    
    void update_watchlist(const std::string& id, const WatchlistDetail& detail) {
        std::unique_lock<std::shared_mutex> lock(watchlist_mutex_);
        watchlists_[id] = detail;
    }
};
```

This header collection provides a clean, efficient foundation for watchlist management in the oqdTradierpp library, supporting both lightweight operations and detailed symbol management.