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

#include "oqdTradierpp/fundamentals/corp_calendar.hpp"
#include "oqdTradierpp/core/json_builder.hpp"

namespace oqd {

CorporateCalendar CorporateCalendar::from_json(const simdjson::dom::element& elem) {
    CorporateCalendar calendar;
    calendar.symbol = std::string(elem["symbol"].get_string().value_unsafe());
    calendar.event_type = std::string(elem["event_type"].get_string().value_unsafe());
    calendar.date = std::string(elem["date"].get_string().value_unsafe());
    calendar.description = std::string(elem["description"].get_string().value_unsafe());
    return calendar;
}

std::string CorporateCalendar::to_json() const {
    return json::create_object()
        .field("symbol", symbol)
        .field("event_type", event_type)
        .field("date", date)
        .field("description", description)
        .end_object()
        .str();
}

} // namespace oqd