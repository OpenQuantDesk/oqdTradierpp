# Account Module

This module provides comprehensive account management functionality for the oqdTradierpp library, including balance tracking, position management, transaction history, and gain/loss analysis.

## Overview

The account module implements all account-related data structures and operations required for professional trading applications. All classes use the high-performance bespoke JSON builder and include robust error handling for API response parsing.

## Components

### Core Account Information

#### `user_profile.hpp/cpp` - User Account Details
- **`UserProfile`**: Complete user account information
  - `id`: Unique user identifier
  - `name`: Account holder name
  - `account`: Primary account number
  - Additional profile fields for account verification

#### `account_balances.hpp/cpp` - Financial Balances
- **`AccountBalances`**: Real-time account financial data
  - **Cash Balances**: `total_cash`, `cash`, `uncleared_funds`, `unsettled_funds`
  - **Equity Values**: `total_equity`, `account_value`, `market_value`
  - **Position Values**: `long_market_value`, `short_market_value`
  - **Buying Power**: `long_liquid_value`, `short_liquid_value`
  - **P&L Tracking**: `close_pl`, `dividend`
  - **Requirements**: `current_requirement`
  - **Order Count**: `pending_orders_count`

### Position Management

#### `position.hpp/cpp` - Current Holdings
- **`Position`**: Individual position details
  - `symbol`: Security identifier
  - `quantity`: Shares/contracts held (positive for long, negative for short)
  - `cost_basis`: Total cost basis for the position
  - `date_acquired`: Position opening date
  - Position valuation and P&L calculations
  - Complete JSON serialization for API integration

### Transaction History

#### `history_item.hpp/cpp` - Individual Transactions
- **`HistoryItem`**: Single transaction record
  - **Transaction Details**: `type`, `description`, `trade_date`
  - **Financial Impact**: `amount`, `commission`, `fees`
  - **Security Info**: `symbol`, `quantity`, `price`
  - **Comprehensive Tracking**: All transaction types supported

#### `account_history.hpp/cpp` - Transaction Collections
- **`AccountHistory`**: Complete transaction history management
  - Collection of `HistoryItem` objects
  - Date range filtering support
  - Transaction type categorization
  - Bulk processing capabilities

### Gain/Loss Analysis

#### `gain_loss_item.hpp/cpp` - Realized P&L Records
- **`GainLossItem`**: Individual realized gain/loss record
  - **Date Tracking**: `open_date`, `close_date` (ISO 8601 format)
  - **Financial Metrics**: `cost`, `proceeds`, `gain_loss`, `gain_loss_percent`
  - **Position Details**: `symbol`, `quantity`
  - **Tax Classification**: `term` (holding period in days)
  - **Improved Data Types**: Proper string dates and integer term

#### `gain_loss.hpp/cpp` - P&L Collections
- **`GainLoss`**: Comprehensive gain/loss analysis
  - Collection of realized P&L records
  - Tax reporting support (short-term vs long-term)
  - Performance analytics
  - Bulk data processing

## Key Features

### Robust Error Handling
- **Safe JSON Parsing**: All classes use proper error checking
- **Missing Field Tolerance**: Graceful handling of optional fields
- **Data Validation**: Type-safe conversions with fallbacks

### High-Performance Serialization
- **Bespoke JSON Builder**: 8-35μs serialization performance
- **Fixed Precision**: Proper financial data formatting
- **Memory Efficient**: Minimal allocations with move semantics

### Complete API Coverage
- **Real-time Data**: Live balance and position updates
- **Historical Analysis**: Complete transaction and P&L history
- **User Management**: Profile and account information

## Usage Examples

### Account Balances
```cpp
auto balances = api->account_balances(account_id);
std::cout << "Total Equity: $" << balances.total_equity << std::endl;
std::cout << "Cash Available: $" << balances.total_cash << std::endl;
std::cout << "Buying Power: $" << balances.long_liquid_value << std::endl;
std::cout << "Open P&L: $" << balances.close_pl << std::endl;
```

### Position Management
```cpp
auto positions = api->account_positions(account_id);
for (const auto& position : positions) {
    double avg_cost = position.cost_basis / position.quantity;
    std::cout << position.symbol << ": " 
              << position.quantity << " shares @ $" 
              << avg_cost << " avg cost" << std::endl;
}
```

### Transaction History
```cpp
auto history = api->account_history(account_id);
for (const auto& item : history) {
    std::cout << item.trade_date << " - " 
              << item.type << ": " 
              << item.symbol << " " 
              << item.quantity << " @ $" 
              << item.price << std::endl;
}
```

### Gain/Loss Analysis
```cpp
auto gainloss = api->account_gainloss(account_id);
double total_realized = 0.0;
int short_term = 0, long_term = 0;

for (const auto& item : gainloss) {
    total_realized += item.gain_loss;
    if (item.term <= 365) {
        short_term++;
    } else {
        long_term++;
    }
}

std::cout << "Total Realized P&L: $" << total_realized << std::endl;
std::cout << "Short-term trades: " << short_term << std::endl;
std::cout << "Long-term trades: " << long_term << std::endl;
```

### User Profile
```cpp
auto profile = api->user_profile();
std::cout << "Account Holder: " << profile.name << std::endl;
std::cout << "User ID: " << profile.id << std::endl;
std::cout << "Primary Account: " << profile.account << std::endl;
```

## Data Integrity

### Type Safety
- **Proper Data Types**: String dates, integer terms, double financials
- **Optional Fields**: Safe handling of missing API data
- **Range Validation**: Sensible defaults and bounds checking

### JSON Parsing Safety
- **Error Checking**: All field access includes error validation
- **Graceful Degradation**: Missing fields don't crash the application
- **Default Values**: Reasonable defaults for optional fields

### Memory Management
- **RAII Compliance**: Automatic resource management
- **Move Semantics**: Efficient data transfer
- **Minimal Copies**: Reference semantics where appropriate

## Performance Characteristics

### JSON Operations
- **Serialization**: 8-35μs per object
- **Deserialization**: Safe parsing with error checking
- **Memory Usage**: Minimal overhead with efficient string handling

### Collection Processing
- **Bulk Operations**: Efficient processing of transaction lists
- **Streaming Support**: Compatible with large datasets
- **Memory Efficient**: Vector-based storage with reserve optimization

## Integration Points

### With Trading Module
```cpp
// Check buying power before placing orders
auto balances = api->account_balances(account_id);
if (balances.long_liquid_value >= order_value) {
    auto response = api->place_equity_order(account_id, order);
}
```

### With Market Data
```cpp
// Calculate position values with current quotes
auto positions = api->account_positions(account_id);
auto quotes = api->get_quotes({position.symbol for position in positions});
// Update position values with current market prices
```

### With Streaming
```cpp
// Real-time balance updates
streaming->on_account_update([](const AccountBalances& balances) {
    // Update UI with new balance information
});
```

## Thread Safety

All account data structures are **thread-safe for read operations** and support **concurrent JSON serialization**. Account data should be **synchronized at the application level** when multiple threads are updating the same account information.

## Error Recovery

### API Failures
- **Graceful Degradation**: Cached data when API is unavailable
- **Retry Logic**: Built-in support for transient failures
- **Fallback Values**: Safe defaults for critical calculations

### Data Validation
- **Range Checks**: Validate financial values are reasonable
- **Format Validation**: Ensure dates and identifiers are well-formed
- **Consistency Checks**: Verify related fields are coherent

This module provides the foundation for all account management operations in professional trading applications, with emphasis on data integrity, performance, and reliability.