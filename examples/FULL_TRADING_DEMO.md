# 🚀 Full Trading Demo - Comprehensive Tradier C++ Library Example

This comprehensive demo showcases **ALL** major features of the Tradier C++ library in a safe sandbox environment. No real money is involved!

## 🛡️ Safety First - Sandbox Mode

**This demo runs exclusively in Tradier's SANDBOX environment:**
- ✅ **No real money** is used or at risk
- ✅ **No real trades** are executed on live markets  
- ✅ **Safe for testing** and learning
- ✅ **Unlimited paper trading** with virtual funds

## 🎯 Features Demonstrated

### 1. **Authentication & Account Management**
- Secure connection to Tradier Sandbox
- User profile retrieval
- Account balance checking
- Position and order status

### 2. **Market Data**
- Real-time stock quotes (SPY, AAPL, MSFT, GOOGL, TSLA)
- Market clock and trading hours
- Historical price data
- Options chains and expirations

### 3. **Watchlist Management**
- Create custom watchlists
- Add/remove symbols dynamically
- List all watchlists
- Clean up when done

### 4. **Paper Trading (Sandbox)**
- **Equity Orders**: Market and limit orders
- **Option Orders**: Buy-to-open calls and puts
- **Order Management**: Track, modify, and cancel
- **Multiple Order Types**: Market, limit, stop orders

### 5. **Real-Time Streaming**
- Live market data feeds
- Dynamic symbol subscription
- HTTP streaming implementation
- Error handling and reconnection

### 6. **Portfolio Analysis**
- Position tracking
- Account history
- Balance updates after trading
- Performance monitoring

## 🚀 Quick Start

### Prerequisites

1. **Tradier Sandbox Account**: Sign up at [tradier.com](https://tradier.com)
2. **Sandbox API Credentials**: Generate in your Tradier dashboard
3. **Environment Variables**: Set your credentials

### Setup Environment Variables

```bash
# Set your Tradier sandbox credentials
export TRADIER_SANDBOX_KEY="your_sandbox_access_token"
export TRADIER_SANDBOX_ACCT="your_sandbox_account_number"
```

### Build and Run

```bash
# Build the library
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run the comprehensive demo
./examples/full_trading_demo
```

## 📖 Demo Walkthrough

### Phase 1: Connection & Authentication
```
🚀 TRADIER C++ LIBRARY - FULL TRADING DEMONSTRATION
====================================================
⚠️  SANDBOX MODE - Safe for testing, no real money involved!

===============================================================================
  1. CLIENT SETUP & AUTHENTICATION
===============================================================================
✓ Connected to Tradier Sandbox environment
  Base URL: https://sandbox.tradier.com
  Account: YOUR_SANDBOX_ACCOUNT
✓ User Profile Retrieved:
  Name: Demo User
  ID: sandbox_user_123
  Accounts: 1
```

### Phase 2: Account Information
```
===============================================================================
  2. ACCOUNT INFORMATION  
===============================================================================
💰 Account Balances:
  Total Equity: $100,000.00
  Cash Available: $100,000.00
  Market Value: $0.00
  Buying Power: $200,000.00

📊 Current Positions (0):
  No positions found

📋 Current Orders (0):
  No open orders
```

### Phase 3: Market Data
```
===============================================================================
  3. MARKET DATA
===============================================================================
🕐 Market Status:
  Date: 2024-01-15
  State: open
  Description: Market is open

📈 Current Stock Quotes:
  🟢 SPY    $   475.23 (+0.45%) Vol: 45000000
  🟢 AAPL   $   185.67 (+1.23%) Vol: 52000000
  🔴 MSFT   $   415.89 (-0.33%) Vol: 28000000
  🟢 GOOGL  $   142.45 (+0.87%) Vol: 15000000
  🟢 TSLA   $   248.12 (+2.15%) Vol: 65000000
```

### Phase 4: Watchlist Management
```
===============================================================================
  4. WATCHLIST MANAGEMENT
===============================================================================
✓ Created watchlist: Demo Portfolio (ID: wl_demo_123)

📋 All Watchlists (1):
  Demo Portfolio (ID: wl_demo_123)

✓ Added symbols to watchlist. Current symbols: SPY, AAPL, MSFT, GOOGL, TSLA
```

### Phase 5: Paper Trading
```
===============================================================================
  5. PAPER TRADING DEMONSTRATION
===============================================================================
⚠️  The following trades are executed in SANDBOX mode - no real money!

--- Equity Order - Buy SPY (Market Order) ---
📤 Placing market order: BUY 10 SPY...
✓ Order placed! ID: 12345 Status: filled

--- Equity Order - Sell SPY (Limit Order) ---
📤 Placing limit order: SELL 5 SPY @ $600.00...
✓ Limit order placed! ID: 12346 Status: open

--- Option Order - Buy AAPL Call ---
📊 Using expiration: 2024-02-16
📤 Placing option order: BUY TO OPEN 1 AAPL240216C00185000...
✓ Option order placed! ID: 12347 Status: filled
```

### Phase 6: Real-Time Streaming
```
===============================================================================
  6. REAL-TIME STREAMING DEMONSTRATION
===============================================================================
🌊 Setting up streaming data feed...
🔌 Starting HTTP streaming for SPY, AAPL...
⏳ Streaming for 10 seconds...
📡 SPY: $475.25
📡 AAPL: $185.70
📡 SPY: $475.27
✓ Successfully received streaming data!

--- Dynamic Symbol Management ---
🔄 Adding TSLA to stream...
🔄 Removing SPY from stream...
✓ Symbol subscription updated
```

### Phase 7: Portfolio Analysis
```
===============================================================================
  7. PORTFOLIO ANALYSIS
===============================================================================
💰 Updated Account Balances:
  Total Equity: $99,523.45
  Cash Available: $94,234.12
  Market Value: $5,289.33
```

### Phase 8: Cleanup
```
===============================================================================
  8. CLEANUP
===============================================================================
✓ Cancelled order 12346: cancelled
✓ Deleted demo watchlist

===============================================================================
  DEMONSTRATION COMPLETE
===============================================================================
🎉 Successfully demonstrated all major Tradier C++ library features:
   ✅ Authentication & account management
   ✅ Market data retrieval (quotes, history, options)
   ✅ Watchlist management (create, update, delete)
   ✅ Paper trading (equity & option orders)
   ✅ Real-time streaming data
   ✅ Portfolio analysis & order management
   ✅ Proper cleanup procedures
```

## 🔧 Customization

### Trading Different Symbols
```cpp
// Modify the symbols list
std::vector<std::string> symbols = {"QQQ", "IWM", "GLD", "NVDA"};
auto quotes = api.get_quotes(symbols);
```

### Different Order Types
```cpp
// Stop-loss order
EquityOrderRequest stop_order;
stop_order.symbol = "AAPL";
stop_order.side = OrderSide::Sell;
stop_order.quantity = 100;
stop_order.type = OrderType::Stop;
stop_order.stop = 180.00;  // Stop price
```

### Extended Streaming
```cpp
// Stream for longer period
std::this_thread::sleep_for(std::chrono::minutes(5));

// Add more symbols dynamically
streaming_session->add_symbols({"QQQ", "IWM", "VIX"});
```

## 🛠️ Error Handling

The demo includes comprehensive error handling:

```cpp
try {
    auto response = api.place_equity_order(account_id, order);
    std::cout << "✓ Order successful: " << response.id << std::endl;
} catch (const RateLimitException& e) {
    std::cout << "⏳ Rate limited: " << e.what() << std::endl;
} catch (const ApiException& e) {
    std::cout << "❌ API error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cout << "⚠️  General error: " << e.what() << std::endl;
}
```

## 📚 Next Steps

1. **Study the Code**: Examine `full_trading_demo.cpp` for implementation details
2. **Modify Examples**: Adapt the demo for your specific use cases
3. **Production Setup**: When ready, switch to production credentials
4. **Advanced Features**: Explore options trading, multi-leg strategies
5. **Integration**: Incorporate into your trading applications

## ⚠️ Important Notes

- **Sandbox Only**: This demo is designed for sandbox environment
- **Rate Limits**: Respect API rate limits (handled automatically)
- **Error Handling**: Always implement proper error handling
- **Security**: Never commit API keys to version control
- **Testing**: Thoroughly test before using production credentials

## 🎓 Learning Resources

- **Tradier API Docs**: [docs.tradier.com](https://documentation.tradier.com)
- **C++ Library Docs**: See `include/tradier2/` headers
- **Options Trading**: Learn options strategies before trading
- **Risk Management**: Understand position sizing and stop-losses

## 🤝 Support

For questions or issues:
1. Check the comprehensive example code
2. Review the API documentation
3. Test in sandbox environment first
4. Reach out to the library maintainers

---

**Happy Paper Trading! 📈🚀**

*Remember: This demo uses sandbox mode with virtual money. Always test thoroughly before using real funds.*