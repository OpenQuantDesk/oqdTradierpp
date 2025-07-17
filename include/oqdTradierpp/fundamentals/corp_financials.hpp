/*
/        oqdTradierpp - Full featured Tradier API library 
/       
/        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
/        Version:           v1.1
/        Release Date:  06/30/2025
/        License: Apache 2.0
/        Disclaimer: This software is provided "as-is" without warranties of any kind. 
/                    Use at your own risk. The authors are not liable for any trading losses.
/                    Not financial advice. By using this software, you accept all risks.
/
*/

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <unordered_map>
#include <simdjson.h>

namespace oqd {

struct CorporateFinancials {
    std::string symbol;
    std::string period;
    double revenue;
    double net_income;
    double eps;
    double assets;
    double liabilities;
    double equity;
    double cash_flow;
    
    static CorporateFinancials from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct FinancialRatios {
    std::string symbol;
    double price_to_earnings;
    double price_to_book;
    double price_to_sales;
    double debt_to_equity;
    double return_on_equity;
    double return_on_assets;
    double current_ratio;
    double quick_ratio;
    
    static FinancialRatios from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

} // namespace oqd