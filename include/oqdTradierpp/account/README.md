# Account Module Headers

This directory contains the header files for account management functionality, defining all data structures for account balances, positions, transaction history, and gain/loss tracking.

## Header Files Overview

### Core Account Information

#### `user_profile.hpp`
**User account and profile information**
```cpp
struct UserProfile {
    std::string id;          // Unique user identifier
    std::string name;        // Account holder name
    std::string account;     // Primary account number
    
    static UserProfile from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

#### `account_balances.hpp`
**Real-time account financial data**
```cpp
struct AccountBalances {
    // Cash balances
    double total_cash;           // Total cash available
    double cash;                 // Available cash for trading
    double uncleared_funds;      // Funds pending clearance
    double unsettled_funds;      // Unsettled trade proceeds
    
    // Equity values
    double total_equity;         // Total account equity
    double account_value;        // Total account value
    double market_value;         // Market value of positions
    
    // Position values
    double long_market_value;    // Long position market value
    double short_market_value;   // Short position market value
    double long_liquid_value;    // Long position buying power
    double short_liquid_value;   // Short position value
    
    // P&L tracking
    double close_pl;             // Realized P&L
    double dividend;             // Dividend income
    
    // Requirements and constraints
    double current_requirement;   // Margin requirement
    double pending_orders_count; // Number of pending orders
    
    // Additional tracking fields
    double account_number;       // Account identifier
    double equity;               // Account equity
    double long_market_value_bp; // Long position buying power
    double short_market_value_bp;// Short position buying power
    double option_short_value;   // Option short position value
    
    static AccountBalances from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

### Position Management

#### `position.hpp`
**Individual security positions**
```cpp
struct Position {
    std::string symbol;          // Security symbol
    int quantity;                // Position size (+ long, - short)
    double cost_basis;           // Total cost basis
    std::string date_acquired;   // Position opening date
    
    // Additional position details
    std::string id;              // Position ID
    
    static Position from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

### Transaction History

#### `history_item.hpp`
**Individual transaction records**
```cpp
struct HistoryItem {
    // Transaction identification
    std::string type;            // Transaction type
    std::string description;     // Transaction description
    std::string trade_date;      // Trade execution date
    
    // Financial details
    double amount;               // Transaction amount
    double commission;           // Commission charged
    double fees;                 // Additional fees
    
    // Security information
    std::string symbol;          // Security symbol
    double quantity;             // Shares/contracts
    double price;                // Execution price
    
    static HistoryItem from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

#### `account_history.hpp`
**Transaction history collections**
```cpp
struct AccountHistory {
    std::vector<HistoryItem> events;  // Collection of transactions
    
    static AccountHistory from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

### Gain/Loss Analysis

#### `gain_loss_item.hpp`
**Realized P&L records**
```cpp
struct GainLossItem {
    // Date tracking (ISO 8601 format)
    std::string close_date;      // Position close date
    std::string open_date;       // Position open date
    
    // Financial metrics
    double cost;                 // Cost basis
    double proceeds;             // Sale proceeds
    double gain_loss;            // Realized gain/loss
    double gain_loss_percent;    // Percentage gain/loss
    
    // Position details
    std::string symbol;          // Security symbol
    double quantity;             // Position size
    int term;                    // Holding period (days)
    
    static GainLossItem from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

#### `gain_loss.hpp`
**P&L collections and analysis**
```cpp
struct GainLoss {
    std::vector<GainLossItem> items;  // Collection of P&L records
    
    static GainLoss from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

## Design Principles

### Financial Data Integrity
- **Precise Types**: Double precision for all monetary values
- **ISO 8601 Dates**: Standard date format for consistency
- **Integer Terms**: Holding periods as day counts

### Error Resilience
- **Safe Parsing**: All JSON operations include error checking
- **Optional Fields**: Graceful handling of missing data
- **Default Values**: Sensible defaults for optional fields

### Performance Optimization
- **Move Semantics**: Efficient data transfer
- **Reserve Strategies**: Pre-allocation for collections
- **Minimal Copying**: Reference semantics where possible

## Common Usage Patterns

### Balance Checking
```cpp
// Check available buying power
auto balances = api->account_balances(account_id);
if (balances.long_liquid_value >= required_buying_power) {
    // Proceed with order placement
}

// Monitor margin requirements
if (balances.current_requirement > 0) {
    // Handle margin call situation
}
```

### Position Analysis
```cpp
// Analyze current holdings
auto positions = api->account_positions(account_id);
double total_position_value = 0.0;
for (const auto& position : positions) {
    if (position.quantity > 0) {  // Long position
        total_position_value += position.cost_basis;
    }
}
```

### Risk Management
```cpp
// Calculate unrealized P&L
auto balances = api->account_balances(account_id);
double unrealized_pl = balances.market_value - balances.cost_basis;

// Check for large losses
if (unrealized_pl < -5000.0) {
    // Trigger risk management protocols
}
```

### Tax Reporting
```cpp
// Categorize gains for tax purposes
auto gainloss = api->account_gainloss(account_id);
double short_term_gains = 0.0;
double long_term_gains = 0.0;

for (const auto& item : gainloss.items) {
    if (item.term <= 365) {
        short_term_gains += item.gain_loss;
    } else {
        long_term_gains += item.gain_loss;
    }
}
```

### Performance Tracking
```cpp
// Calculate account performance
auto history = api->account_history(account_id);
double total_commissions = 0.0;
int trade_count = 0;

for (const auto& item : history.events) {
    if (item.type == "trade") {
        total_commissions += item.commission;
        trade_count++;
    }
}

double avg_commission = total_commissions / trade_count;
```

## Integration Points

### With Trading Module
```cpp
// Pre-trade validation
bool can_place_order(const EquityOrderRequest& order) {
    auto balances = api->account_balances(account_id);
    double required_bp = order.quantity * order.price.value_or(0);
    return balances.long_liquid_value >= required_bp;
}
```

### With Market Data
```cpp
// Real-time position valuation
auto positions = api->account_positions(account_id);
std::vector<std::string> symbols;
for (const auto& pos : positions) {
    symbols.push_back(pos.symbol);
}
auto quotes = api->get_quotes(symbols);
// Calculate current position values
```

### With Streaming
```cpp
// Real-time account updates
streaming->on_account_update([](const AccountBalances& balances) {
    // Update risk monitoring systems
    if (balances.current_requirement > 0) {
        alert_margin_call();
    }
});
```

## Thread Safety Considerations

### Read Operations
- **Thread Safe**: All const methods are thread-safe
- **Immutable Data**: Structures are immutable after construction
- **Copy Semantics**: Safe to copy across threads

### Write Operations
- **Single Writer**: Account updates should be serialized
- **Atomic Updates**: Use std::atomic for shared counters
- **Lock Protection**: Protect shared account state

## Memory Management

### Efficient Storage
- **Vector-based Collections**: Efficient storage for transaction lists
- **String Interning**: Consider for repeated symbol names
- **Move Semantics**: Avoid unnecessary copies

### Cache Considerations
- **Temporal Locality**: Recent transactions accessed frequently
- **Size Management**: Limit history collection sizes
- **Lazy Loading**: Load detailed history on demand

## Validation and Constraints

### Data Validation
```cpp
// Validate balance consistency
bool validate_balances(const AccountBalances& balances) {
    return balances.total_equity >= 0 &&
           balances.total_cash >= 0 &&
           balances.market_value >= 0;
}
```

### Range Checks
```cpp
// Validate position data
bool validate_position(const Position& position) {
    return !position.symbol.empty() &&
           position.cost_basis >= 0 &&
           position.quantity != 0;  // Non-zero position
}
```

This module provides the foundation for all account management operations with emphasis on data integrity, performance, and thread safety.