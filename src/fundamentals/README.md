# Fundamentals Module

This module provides comprehensive fundamental analysis data for securities, including corporate information, financial statements, ratios, dividends, and corporate actions.

** THESE ARE NOT AVAILABLE FOR RETAIL/BROKERAGE API USERS **

## Overview

The fundamentals module implements data structures and operations for fundamental analysis of securities. This includes company information, financial metrics, corporate actions, and dividend data essential for long-term investment analysis.


## Components

### Corporate Information

#### `corp_info.cpp` - Company Details
- **`CompanyInfo`**: Basic company information and metrics
  - `symbol`: Stock ticker symbol
  - `name`: Full company name
  - `description`: Business description
  - `exchange`: Primary listing exchange
  - `sector`: Business sector classification
  - `industry`: Industry classification
  - `website`: Corporate website URL
  - `ceo`: Chief Executive Officer name
  - `market_cap`: Market capitalization
  - `pe_ratio`: Price-to-earnings ratio
  - `dividend_yield`: Current dividend yield

### Financial Statements

#### `corp_financials.cpp` - Financial Data
- **`CorporateFinancials`**: Key financial statement data
  - `symbol`: Stock ticker symbol
  - `period`: Reporting period (quarterly/annual)
  - `revenue`: Total revenue
  - `net_income`: Net income/profit
  - `eps`: Earnings per share
  - `assets`: Total assets
  - `liabilities`: Total liabilities
  - `equity`: Shareholders' equity
  - `cash_flow`: Operating cash flow

- **`FinancialRatios`**: Key financial ratios for analysis
  - `symbol`: Stock ticker symbol
  - `price_to_earnings`: P/E ratio
  - `price_to_book`: P/B ratio
  - `price_to_sales`: P/S ratio
  - `debt_to_equity`: D/E ratio
  - `return_on_equity`: ROE percentage
  - `return_on_assets`: ROA percentage
  - `current_ratio`: Current liquidity ratio
  - `quick_ratio`: Quick liquidity ratio

### Price Analytics

#### `corp_pricestats.cpp` - Price Statistics
- **`PriceStatistics`**: Historical price performance metrics
  - Technical analysis metrics
  - Historical volatility data
  - Price momentum indicators
  - Trading volume statistics

### Corporate Actions

#### `corp_actions.cpp` - Corporate Events
- **`CorporateActions`**: Stock splits, mergers, acquisitions
  - Stock split information
  - Dividend announcements
  - Merger and acquisition data
  - Rights offerings
  - Spin-offs and special distributions

### Dividend Information

#### `corp_dividends.cpp` - Dividend Data
- **`DividendInfo`**: Comprehensive dividend information
  - Dividend payment history
  - Yield calculations
  - Payment schedules
  - Ex-dividend dates
  - Dividend growth rates

### Corporate Calendar

#### `corp_calendar.cpp` - Earnings & Events
- **`CorporateCalendar`**: Scheduled corporate events
  - Earnings announcement dates
  - Ex-dividend dates
  - Annual meeting schedules
  - Conference calls
  - Important corporate events




### Company Research
```cpp
// Get company information
auto company_info = api->get_company_info({"AAPL", "MSFT", "GOOGL"});
for (const auto& info : company_info) {
    std::cout << info.name << " (" << info.symbol << ")" << std::endl;
    std::cout << "Sector: " << info.sector << std::endl;
    std::cout << "Market Cap: $" << std::fixed << std::setprecision(0) 
              << info.market_cap / 1e9 << "B" << std::endl;
    std::cout << "P/E Ratio: " << std::setprecision(2) 
              << info.pe_ratio << std::endl;
}
```

### Financial Analysis
```cpp
// Get financial ratios for analysis
auto ratios = api->get_financial_ratios({"AAPL"});
for (const auto& ratio : ratios) {
    std::cout << "Financial Ratios for " << ratio.symbol << ":" << std::endl;
    std::cout << "P/E: " << ratio.price_to_earnings << std::endl;
    std::cout << "P/B: " << ratio.price_to_book << std::endl;
    std::cout << "ROE: " << ratio.return_on_equity << "%" << std::endl;
    std::cout << "Current Ratio: " << ratio.current_ratio << std::endl;
}
```

### Dividend Analysis
```cpp
// Check dividend information
auto dividends = api->get_dividend_info({"JNJ", "KO", "PG"});
for (const auto& div : dividends) {
    std::cout << div.symbol << " Dividend Yield: " 
              << std::fixed << std::setprecision(2) 
              << div.yield << "%" << std::endl;
}
```

### Earnings Calendar
```cpp
// Check upcoming earnings
auto calendar = api->get_corporate_calendar({"AAPL", "MSFT"});
for (const auto& event : calendar) {
    std::cout << event.symbol << " earnings on " 
              << event.earnings_date << std::endl;
}
```

## Integration Points

### With Market Data
```cpp
// Combine fundamentals with market data
auto quote = api->get_quote("AAPL");
auto financials = api->get_financial_ratios({"AAPL"});

if (!financials.empty()) {
    double market_pe = quote.last / (financials[0].price_to_earnings * quote.last);
    std::cout << "Current P/E based on market price: " << market_pe << std::endl;
}
```

### With Trading Decisions
```cpp
// Use fundamentals for investment screening
auto ratios = api->get_financial_ratios(screening_symbols);
std::vector<std::string> value_stocks;

for (const auto& ratio : ratios) {
    if (ratio.price_to_book < 2.0 && ratio.return_on_equity > 15.0) {
        value_stocks.push_back(ratio.symbol);
    }
}
```

## Performance Characteristics

### Current Performance (Before Migration)
- **JSON Serialization**: ~100-300μs using std::ostringstream
- **Memory Usage**: Higher due to temporary string objects
- **Parsing**: Verbose error checking adds overhead

### Target Performance (After Migration)
- **JSON Serialization**: 8-35μs using bespoke JSON builder
- **Memory Usage**: Minimal with direct buffer manipulation
- **Parsing**: Streamlined with value_unsafe() approach

## Required Migration Tasks

1. **Update includes**: Replace `<sstream>` with JSON builder header
2. **Migrate to_json()**: Convert all serialization to use `json::create_object()`
3. **Simplify parsing**: Replace verbose error checking with `value_unsafe()`
4. **Performance testing**: Benchmark before/after migration
5. **Integration testing**: Ensure API compatibility maintained
