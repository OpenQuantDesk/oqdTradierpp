#!/bin/bash

# Tradier API Environment Setup Script
# This script helps you set up environment variables for the Tradier C++ library

echo "=== Tradier C++ Library Environment Setup ==="
echo ""

# Check if environment variables are already set
if [[ -n "$TRADIER_SANDBOX_KEY" ]]; then
    echo "✓ TRADIER_SANDBOX_KEY is already set"
else
    echo "✗ TRADIER_SANDBOX_KEY is not set"
fi

if [[ -n "$TRADIER_SANDBOX_ACCT" ]]; then
    echo "✓ TRADIER_SANDBOX_ACCT is already set"
else
    echo "✗ TRADIER_SANDBOX_ACCT is not set"
fi

if [[ -n "$TRADIER_PRODUCTION_KEY" ]]; then
    echo "✓ TRADIER_PRODUCTION_KEY is already set"
else
    echo "✗ TRADIER_PRODUCTION_KEY is not set"
fi

echo ""
echo "To set up your Tradier API credentials:"
echo ""
echo "1. For Sandbox (recommended for testing):"
echo "   export TRADIER_SANDBOX_KEY=\"your_sandbox_access_token\""
echo "   export TRADIER_SANDBOX_ACCT=\"your_sandbox_account_id\""
echo ""
echo "2. For Production (use with extreme caution!):"
echo "   export TRADIER_PRODUCTION_KEY=\"your_production_access_token\""
echo "   export TRADIER_PRODUCTION_ACCT=\"your_production_account_id\""
echo ""
echo "3. You can add these to your ~/.bashrc or ~/.zshrc for persistence:"
echo "   echo 'export TRADIER_SANDBOX_KEY=\"your_token\"' >> ~/.bashrc"
echo "   echo 'export TRADIER_SANDBOX_ACCT=\"your_account\"' >> ~/.bashrc"
echo ""

# Interactive setup option
read -p "Would you like to set up sandbox credentials interactively? (y/n): " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    read -p "Enter your Tradier sandbox access token: " sandbox_key
    read -p "Enter your Tradier sandbox account ID: " sandbox_account
    
    echo ""
    echo "Setting environment variables for this session..."
    export TRADIER_SANDBOX_KEY="$sandbox_key"
    export TRADIER_SANDBOX_ACCT="$sandbox_account"
    
    echo "✓ TRADIER_SANDBOX_KEY set"
    echo "✓ TRADIER_SANDBOX_ACCT set"
    echo ""
    echo "These variables are only set for the current session."
    echo "To make them permanent, add the following to your shell profile:"
    echo ""
    echo "export TRADIER_SANDBOX_KEY=\"$sandbox_key\""
    echo "export TRADIER_SANDBOX_ACCT=\"$sandbox_account\""
    echo ""
fi

echo "Setup complete! You can now run the Tradier C++ library examples."
echo ""
echo "Example usage:"
echo "  ./examples/basic_usage"
echo "  ./examples/market_data_example"
echo "  ./examples/trading_example"
echo "  ./examples/streaming_example"
echo ""
echo "For more information, see the README.md file."