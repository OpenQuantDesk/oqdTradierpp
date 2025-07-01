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

#pragma once

#include <string>
#include <simdjson.h>
#include "oqdTradierpp/response_validator.hpp"

namespace oqd {

struct Position {
    double cost_basis;
    std::string date_acquired;
    std::string id;
    double quantity;
    std::string symbol;
    
    static Position from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
    
    // Validation methods
    ValidationResult validate(ValidationLevel level = ValidationLevel::Basic) const;
    bool is_valid() const;
    std::vector<ValidationIssue> get_validation_issues() const;
    
    // Specific validation methods
    std::vector<ValidationIssue> validate_position_constraints() const;
    std::vector<ValidationIssue> validate_cost_basis_rules() const;
    
    // Helper methods
    bool is_long_position() const;
    bool is_short_position() const;
    double get_current_value(double current_price) const;
    double get_unrealized_pnl(double current_price) const;
    double get_average_cost() const;
    
    // Validation summary and reporting
    std::string get_validation_summary() const;
    std::string get_validation_report() const;
    void print_validation_issues() const;
};

}