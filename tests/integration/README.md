# Integration Tests for oqdTradierpp

## Overview

These integration tests validate the library's functionality against real Tradier API endpoints. They are divided into two main categories:

1. **Sandbox Trading Tests** - Test order placement, modification, and cancellation in the sandbox environment
2. **Streaming Validation Tests** - Test real-time market data streaming in the production environment

## Prerequisites

### Environment Variables

Before running the integration tests, you must set the following environment variables:

#### For Sandbox Trading Tests:
```bash
export TRADIER_SANDBOX_TOKEN="your_sandbox_api_token"
export TRADIER_SANDBOX_ACCOUNT="your_sandbox_account_id"
```

#### For Streaming Validation Tests:
```bash
export TRADIER_PRODUCTION_TOKEN="your_production_api_token"
```

### Getting API Credentials

1. Sign up for a Tradier account at https://tradier.com
2. For sandbox access:
   - Navigate to the sandbox environment at https://sandbox.tradier.com
   - Generate an API token from your account settings
   - Note your sandbox account ID
3. For production streaming:
   - Generate a production API token from your account settings
   - Note: Streaming requires a funded account or market data subscription

## Running the Tests

### Run All Integration Tests
```bash
cd build
./tests/oqdTradierpp_integration_tests
```

### Run Specific Test Suite
```bash
# Sandbox trading tests only
./tests/oqdTradierpp_integration_tests --gtest_filter="SandboxTradingTest.*"

# Streaming validation tests only
./tests/oqdTradierpp_integration_tests --gtest_filter="StreamingValidationTest.*"
```

### Run with Verbose Output
```bash
./tests/oqdTradierpp_integration_tests --gtest_print_time=1 --gtest_color=yes
```

## Test Coverage

### Sandbox Trading Tests
- Account information retrieval
- Market quotes
- Equity order placement (market, limit, stop)
- Option order placement
- Order modification
- Order cancellation
- Position management
- Order history
- Error handling

### Streaming Validation Tests
- WebSocket connection management
- Real-time quote streaming
- Trade data streaming
- Market summary updates
- Multiple symbol subscriptions
- Reconnection handling
- High-frequency data handling
- Memory stability

## Important Notes

1. **Market Hours**: Some tests will have limited functionality outside of market hours (9:30 AM - 4:00 PM ET, Monday-Friday)

2. **Rate Limits**: The Tradier API has rate limits. If you encounter rate limit errors, wait a few minutes before running tests again.

3. **Sandbox Limitations**: The sandbox environment may have delayed or simulated market data. Orders may not fill exactly as they would in production.

4. **Cost Considerations**: 
   - Sandbox trading is free
   - Production streaming may require a market data subscription
   - Be cautious with production credentials

## Troubleshooting

### Tests Skip with "credentials not found"
- Ensure environment variables are set correctly
- Check that tokens are valid and not expired

### Connection Errors
- Verify internet connectivity
- Check if Tradier services are operational
- Ensure firewall allows WebSocket connections

### No Market Data
- Check if markets are open
- Verify market data subscription for production streaming
- Sandbox may have limited real-time data

## Example Test Output

```
[==========] Running 13 tests from 1 test suite.
[----------] 13 tests from SandboxTradingTest
[ RUN      ] SandboxTradingTest.GetAccountInfo
[       OK ] SandboxTradingTest.GetAccountInfo (245 ms)
[ RUN      ] SandboxTradingTest.PlaceAndCancelMarketOrder
[       OK ] SandboxTradingTest.PlaceAndCancelMarketOrder (523 ms)
...
[==========] 13 tests from 1 test suite ran. (8234 ms total)
[  PASSED  ] 13 tests.
```