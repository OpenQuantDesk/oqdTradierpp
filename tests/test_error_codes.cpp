/*
/        oqdTradierpp - Full featured Tradier API library 
/       
/        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
/        Version:           v1.0.0
/        Release Date:  06/30/2025
/        License: Apache 2.0
/        Disclaimer: This software is provided "as-is" without warranties of any kind. 
/                    Use at your own risk. The authors are not liable for any trading losses.
/                    Not financial advice. By using this software, you accept all risks.
/
*/

#include <iostream>
#include <cassert>
#include "../include/oqdTradierpp/error_codes.hpp"

using namespace oqd;

void test_error_code_mapping() {
    std::cout << "Testing error code mapping..." << std::endl;
    
    // Test string to code mapping
    auto code = ErrorCodeMapper::string_to_code("AccountDisabled");
    assert(code == TradierErrorCode::AccountDisabled);
    
    // Test code to string mapping
    auto str = ErrorCodeMapper::code_to_string(TradierErrorCode::AccountDisabled);
    assert(str == "AccountDisabled");
    
    // Test error info retrieval
    auto info = ErrorCodeMapper::get_error_info(TradierErrorCode::AccountDisabled);
    assert(info.category == ErrorCategory::Account);
    assert(info.severity == ErrorSeverity::Critical);
    assert(!info.is_retryable);
    
    std::cout << "✓ Error code mapping tests passed" << std::endl;
}

void test_exception_creation() {
    std::cout << "Testing exception creation..." << std::endl;
    
    // Test exception with error code
    try {
        throw TradierApiException(TradierErrorCode::AccountDisabled);
    } catch (const TradierApiException& e) {
        assert(e.get_error_code() == TradierErrorCode::AccountDisabled);
        assert(e.get_category() == ErrorCategory::Account);
        assert(e.get_severity() == ErrorSeverity::Critical);
        assert(!e.is_retryable());
        
        std::string detailed = e.get_detailed_message();
        assert(!detailed.empty());
        
        std::string guidance = e.get_recovery_guidance();
        assert(!guidance.empty());
    }
    
    // Test exception with string code
    try {
        throw TradierApiException("DayTradingBuyingPowerExceeded", "Not enough buying power");
    } catch (const TradierApiException& e) {
        assert(e.get_error_code() == TradierErrorCode::DayTradingBuyingPowerExceeded);
        assert(e.get_category() == ErrorCategory::Margin);
        assert(!e.get_server_message().empty());
    }
    
    std::cout << "✓ Exception creation tests passed" << std::endl;
}

void test_error_categories() {
    std::cout << "Testing error categories..." << std::endl;
    
    // Test category filtering
    auto account_errors = ErrorCodeMapper::get_codes_by_category(ErrorCategory::Account);
    assert(!account_errors.empty());
    
    auto retryable_errors = ErrorCodeMapper::get_retryable_codes();
    assert(!retryable_errors.empty());
    
    // Test retryable error
    auto retryable_code = TradierErrorCode::DayTraderPatternRestriction;
    assert(ErrorCodeMapper::is_retryable(retryable_code));
    assert(ErrorCodeMapper::get_retry_delay(retryable_code) > std::chrono::milliseconds{0});
    
    std::cout << "✓ Error category tests passed" << std::endl;
}

void test_string_conversions() {
    std::cout << "Testing string conversions..." << std::endl;
    
    // Test enum to string conversions
    assert(to_string(ErrorCategory::Account) == "Account");
    assert(to_string(ErrorSeverity::Critical) == "Critical");
    assert(to_string(RetryBehavior::NoRetry) == "NoRetry");
    
    std::cout << "✓ String conversion tests passed" << std::endl;
}

void test_specialized_exceptions() {
    std::cout << "Testing specialized exceptions..." << std::endl;
    
    // Test AccountException
    try {
        throw AccountException(TradierErrorCode::AccountDisabled);
    } catch (const AccountException& e) {
        assert(e.get_error_code() == TradierErrorCode::AccountDisabled);
        assert(e.get_category() == ErrorCategory::Account);
    } catch (const TradierApiException& e) {
        assert(false && "Should catch AccountException");
    }
    
    // Test OrderException
    try {
        throw OrderException(TradierErrorCode::BuyStopOrderStopPriceLessAsk);
    } catch (const OrderException& e) {
        assert(e.get_error_code() == TradierErrorCode::BuyStopOrderStopPriceLessAsk);
        assert(e.get_category() == ErrorCategory::Validation);
    }
    
    std::cout << "✓ Specialized exception tests passed" << std::endl;
}

void test_error_info_completeness() {
    std::cout << "Testing error info completeness..." << std::endl;
    
    // Test that all error codes have proper info
    std::vector<TradierErrorCode> test_codes = {
        TradierErrorCode::AccountDisabled,
        TradierErrorCode::AccountIsNotApproved,
        TradierErrorCode::DayTradingBuyingPowerExceeded,
        TradierErrorCode::BuyStopOrderStopPriceLessAsk,
        TradierErrorCode::InitialMargin,
        TradierErrorCode::OmsInternalError,
        TradierErrorCode::Unknown
    };
    
    for (auto code : test_codes) {
        auto info = ErrorCodeMapper::get_error_info(code);
        assert(!info.string_code.empty());
        assert(!info.description.empty());
        assert(!info.recovery_suggestions.empty());
        
        // Test round-trip conversion
        auto converted_code = ErrorCodeMapper::string_to_code(info.string_code);
        assert(converted_code == code);
    }
    
    std::cout << "✓ Error info completeness tests passed" << std::endl;
}

int main() {
    std::cout << "Running error codes tests..." << std::endl;
    
    try {
        test_error_code_mapping();
        test_exception_creation();
        test_error_categories();
        test_string_conversions();
        test_specialized_exceptions();
        test_error_info_completeness();
        
        std::cout << "\n✓ All error codes tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}