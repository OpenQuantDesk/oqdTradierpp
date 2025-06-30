# Fundamentals Headers

This directory contains header declarations for fundamental analysis data structures in the oqdTradierpp library.

## Header Files

### Corporate Information

#### `corp_info.hpp`
- **`CompanyInfo`**: Complete company profile and basic metrics
- **Business Data**: Sector, industry, description, leadership
- **Key Metrics**: Market cap, P/E ratio, dividend yield
- **Corporate Details**: Website, exchange, executive information

### Financial Analysis

#### `corp_financials.hpp`
- **`CorporateFinancials`**: Financial statement data
  - Revenue, net income, EPS
  - Balance sheet items (assets, liabilities, equity)
  - Cash flow information
  - Period-specific data (quarterly/annual)

- **`FinancialRatios`**: Key financial ratios for analysis
  - Valuation ratios (P/E, P/B, P/S)
  - Profitability ratios (ROE, ROA)
  - Liquidity ratios (current, quick)
  - Leverage ratios (debt-to-equity)

#### `corp_pricestats.hpp`
- **`PriceStatistics`**: Historical price performance data
- **Technical Metrics**: Volatility, momentum indicators
- **Volume Analysis**: Trading activity patterns
- **Performance Attribution**: Return decomposition

### Corporate Events

#### `corp_actions.hpp`
- **`CorporateActions`**: Corporate events and actions
- **Stock Events**: Splits, dividends, rights offerings
- **M&A Activity**: Mergers, acquisitions, spin-offs
- **Special Situations**: Special distributions, tender offers

#### `corp_dividends.hpp`
- **`DividendInfo`**: Comprehensive dividend analysis
- **Payment History**: Historical dividend payments
- **Yield Calculations**: Current and trailing yields
- **Growth Analysis**: Dividend growth rates and sustainability
- **Calendar Data**: Ex-dividend and payment dates

#### `corp_calendar.hpp`
- **`CorporateCalendar`**: Scheduled corporate events
- **Earnings Events**: Announcement dates, conference calls
- **Shareholder Events**: Annual meetings, proxy filings
- **Dividend Events**: Declaration and payment schedules
- **Other Events**: Product launches, regulatory filings

## Design Patterns

### Data Structure Consistency
```cpp
// All fundamentals structures follow consistent pattern
struct CompanyInfo {
    std::string symbol;          // Always present
    // ... other required fields
    
    static CompanyInfo from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};
```

### Symbol-Centric Design
- **Primary Key**: All structures keyed by symbol
- **Batch Operations**: Support for multiple symbols
- **Consistency**: Same symbol representation across all types

### Optional vs Required Fields
```cpp
struct FinancialRatios {
    std::string symbol;              // Required
    double price_to_earnings;        // Required
    std::optional<double> beta;      // Optional (not always available)
};
```

## Type Definitions

### Core Types
```cpp
namespace oqd {
    // Company information
    struct CompanyInfo;
    
    // Financial data
    struct CorporateFinancials;
    struct FinancialRatios;
    struct PriceStatistics;
    
    // Corporate events
    struct CorporateActions;
    struct DividendInfo;
    struct CorporateCalendar;
}
```

### Forward Declarations
```cpp
// For use in other modules without full includes
namespace oqd {
    struct CompanyInfo;
    struct FinancialRatios;
    // ... other forward declarations
}
```

## Usage Patterns

### Include Strategy
```cpp
// Selective includes for specific functionality
#include "oqdTradierpp/fundamentals/corp_info.hpp"      // Company data
#include "oqdTradierpp/fundamentals/corp_financials.hpp" // Financial analysis
#include "oqdTradierpp/fundamentals/corp_dividends.hpp"  // Dividend analysis
```

### Template Compatibility
```cpp
// All types support standard operations
template<typename T>
void process_fundamental_data(const T& data) {
    std::string json = data.to_json();        // Serialization
    T copy = data;                            // Copy construction
    T moved = std::move(copy);                // Move semantics
}
```

### Collection Operations
```cpp
// Multi-symbol operations
std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOGL"};
auto company_data = api->get_company_info(symbols);
auto financial_data = api->get_financial_ratios(symbols);
auto dividend_data = api->get_dividend_info(symbols);
```

## Integration Points

### With API Layer
```cpp
// Fundamentals API methods use these types
std::vector<CompanyInfo> companies = api->get_company_info(symbols);
std::vector<FinancialRatios> ratios = api->get_financial_ratios(symbols);
std::vector<DividendInfo> dividends = api->get_dividend_info(symbols);
```

### With Market Data
```cpp
// Combine fundamentals with real-time quotes
Quote quote = api->get_quote("AAPL");
CompanyInfo company = api->get_company_info({"AAPL"})[0];

double market_cap = quote.last * company.shares_outstanding;
```

### With Trading Module
```cpp
// Use fundamentals for investment decisions
auto ratios = api->get_financial_ratios({"AAPL"});
if (!ratios.empty() && ratios[0].price_to_earnings < 20.0) {
    // Consider for value investment
    EquityOrderRequest order;
    order.symbol = ratios[0].symbol;
    // ... configure order
}
```

## Performance Considerations

### Memory Layout
```cpp
// Structures optimized for cache efficiency
struct CompanyInfo {
    std::string symbol;      // Hot data (frequently accessed)
    std::string name;
    double market_cap;       // Hot numeric data
    
    std::string description; // Cold data (less frequently accessed)
    std::string website;
};
```

### Batch Processing
```cpp
// Efficient multi-symbol requests
std::vector<std::string> large_universe = get_sp500_symbols();
auto batch_size = 100;

for (size_t i = 0; i < large_universe.size(); i += batch_size) {
    auto batch = std::vector<std::string>(
        large_universe.begin() + i,
        large_universe.begin() + std::min(i + batch_size, large_universe.size())
    );
    auto batch_data = api->get_company_info(batch);
    process_batch(batch_data);
}
```

### Caching Strategy
```cpp
// Fundamentals data changes infrequently - good for caching
class FundamentalsCache {
    std::unordered_map<std::string, CompanyInfo> company_cache_;
    std::chrono::time_point<std::chrono::steady_clock> last_update_;
    
public:
    CompanyInfo get_company_info(const std::string& symbol) {
        if (needs_refresh()) refresh_cache();
        return company_cache_[symbol];
    }
};
```

## Data Quality Considerations

### Missing Data Handling
```cpp
// Robust handling of incomplete fundamental data
void analyze_company(const CompanyInfo& info) {
    if (!info.symbol.empty()) {
        if (info.market_cap > 0) {
            // Use market cap in analysis
        } else {
            log_warning("Missing market cap for " + info.symbol);
        }
    }
}
```

### Data Validation
```cpp
// Validate financial ratios for reasonableness
bool is_valid_pe_ratio(double pe) {
    return pe > 0 && pe < 1000;  // Reasonable bounds
}

bool is_valid_roe(double roe) {
    return roe > -100 && roe < 100;  // Percentage bounds
}
```

This header collection provides the foundation for fundamental analysis in the oqdTradierpp library, enabling comprehensive company research and investment analysis capabilities.