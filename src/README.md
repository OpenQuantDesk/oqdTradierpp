# oqdTradierpp Source Code

This directory contains the complete implementation of the oqdTradierpp C++20 library for professional trading applications using the Tradier API.

## Overview

The oqdTradierpp library provides a comprehensive C++ interface to the Tradier brokerage API, featuring high-performance JSON processing, WebSocket streaming, advanced order management, and robust error handling for production trading systems.

## Module Organization

### Core Modules

#### `core/`
- **JSON Processing**: Bespoke JSON builder with 8-35μs serialization performance
- **Enumerations**: Type-safe enums for order types, sides, durations, and status values
- **Performance**: Zero-dependency JSON serialization optimized for trading latency

#### `auth/`
- **OAuth 2.0**: Complete authentication flow implementation
- **Token Management**: Automatic refresh and secure storage patterns
- **Security**: Production-ready credential handling

### Data Modules

#### `account/`
- **Account Management**: Balances, positions, history, gain/loss tracking
- **User Profiles**: Account information and configuration
- **Performance Tracking**: P&L analysis and reporting

#### `market/`
- **Real-time Data**: Market quotes with options Greeks and volatility
- **Historical Data**: OHLCV data for backtesting and analysis
- **Market Infrastructure**: Trading hours, market status, symbol lookup
- **Options Chains**: Complete option chains with risk metrics

#### `fundamentals/`
- **Company Information**: Corporate profiles and business metrics
- **Financial Analysis**: Ratios, financials, and performance statistics
- **Corporate Events**: Dividends, splits, earnings calendars
- ⚠️ **Migration Needed**: Still using std::ostringstream instead of bespoke JSON builder

#### `watchlist/`
- **Portfolio Organization**: Symbol grouping and monitoring
- **Batch Operations**: Multi-symbol market data and trading
- ⚠️ **Migration Needed**: Still using std::ostringstream instead of bespoke JSON builder

### Trading Modules

#### `trading/`
- **Order Management**: Comprehensive order lifecycle handling
- **Advanced Orders**: OTO, OCO, OTOCO, and spread strategies
- **Multi-leg Strategies**: Complex options and combo orders
- **Risk Management**: Pre-trade validation and position sizing

## Core Implementation Files

### Primary API Interface

#### `api_methods.cpp`
- **Unified API**: Single interface for all Tradier operations
- **Async/Sync**: Both synchronous and asynchronous operation modes
- **Error Handling**: Comprehensive exception management
- **Performance**: Connection pooling and request optimization

#### `oqdTradierpp.cpp`
- **Library Entry Point**: Main library initialization and configuration
- **Client Management**: HTTP client setup and configuration
- **Environment Support**: Sandbox and production environments

### Legacy Files (Migration Required)

#### `tradier2.cpp`
- **Status**: Legacy implementation file
- **Action Required**: Remove after confirming all functionality migrated to modules

#### `types.cpp`
- **Status**: Legacy type definitions
- **Action Required**: Verify all types moved to appropriate modules

### Network and Streaming

#### `streaming.cpp`
- **WebSocket Implementation**: Real-time market data streaming
- **Connection Management**: Automatic reconnection and error recovery
- **Data Processing**: High-throughput quote and trade processing
- **Thread Safety**: Concurrent access patterns for real-time data

### Validation and Utilities

#### `order_validation.cpp`
- **Pre-trade Checks**: Order validation before submission
- **Risk Controls**: Position limits and buying power validation
- **Data Integrity**: Parameter validation and sanitization

## Build Integration

### CMake Integration
```cmake
# Core library
add_library(oqdTradierpp
    src/oqdTradierpp.cpp
    src/api_methods.cpp
    src/streaming.cpp
    src/order_validation.cpp
)

# Module libraries
add_subdirectory(src/core)
add_subdirectory(src/auth)
add_subdirectory(src/account)
add_subdirectory(src/market)
add_subdirectory(src/trading)
add_subdirectory(src/fundamentals)
add_subdirectory(src/watchlist)
```

## Performance Characteristics

### JSON Processing Performance

**Current Performance (Post-Migration):**
- **All Modules**: 5.7-24.6μs using bespoke JSON builder (✅ Migration Complete)
- **String Escaping**: 9.4μs for complex strings
- **Large Arrays**: 503.3μs for 1000+ elements
- **Memory Reuse**: 3.9μs with clear() and reuse patterns

**Live Benchmark Results:**
```
Simple object creation: 5.696 µs/op (100000 iterations)
Complex object creation: 24.593 µs/op (100000 iterations)
String escaping: 9.400 µs/op (100000 iterations)
Large array creation: 503.318 µs/op (100000 iterations)
```

### Memory Efficiency
- **Stack Allocation**: Minimal heap usage for hot path operations
- **RAII Patterns**: Automatic resource management
- **Move Semantics**: Efficient data transfer throughout

### Network Performance
- **Connection Pooling**: Persistent HTTP connections
- **Compression**: Gzip compression for large responses
- **Streaming**: WebSocket connections for real-time data

## Completed Improvements

### JSON Migration Success ✅

#### All Modules Migrated
- **Fundamentals Module**: ✅ Migrated to bespoke JSON builder
- **Watchlist Module**: ✅ Migrated to bespoke JSON builder  
- **Market Module**: ✅ Migrated to bespoke JSON builder
- **Performance Gain**: 3-10x improvement across all modules

### Error Handling Standardization ✅
- **All Modules**: ✅ Standardized on value_unsafe() for performance
- **Market Module**: ✅ Migrated from verbose error checking
- **Fundamentals**: ✅ Migrated from verbose simdjson error checking
- **Consistency**: All modules now use consistent error handling patterns

### Test Infrastructure Enhancements ✅
- **Unit Tests**: ✅ 53+ tests passing (20 JSON builder, 17 streaming, 8 account, 5 order, 3 auth)
- **Integration Tests**: ✅ Streaming validation with environment variables
- **Performance Tests**: ✅ Comprehensive benchmarking suite

## Testing Infrastructure

### Unit Tests
- **Coverage**: Core, auth, account, trading modules complete
- **Missing**: Market data, fundamentals, watchlist modules
- **Framework**: GoogleTest with comprehensive assertions

### Integration Tests
- **Sandbox**: Complete trading test suite with live API
- **Streaming**: Production WebSocket validation
- **Performance**: JSON serialization benchmarks

### Quality Assurance
- **Memory Validation**: Valgrind integration
- **Performance Profiling**: gprof/perf integration
- **CI/CD**: GitHub Actions automation

## Development Guidelines

### Code Style
- **C++20**: Modern C++ features throughout
- **RAII**: Resource management patterns
- **const-correctness**: Immutable by default
- **Move Semantics**: Efficient data transfer

### Performance Requirements
- **JSON Serialization**: <50μs for all operations
- **Memory Usage**: Minimal heap allocation in hot paths
- **Thread Safety**: Lock-free where possible

### Security Standards
- **No Secrets**: Never log or expose authentication tokens
- **Input Validation**: All user inputs validated
- **Secure Storage**: Encrypted credential storage patterns
