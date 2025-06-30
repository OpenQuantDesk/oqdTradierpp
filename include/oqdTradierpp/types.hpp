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
#include <vector>
#include <optional>
#include <chrono>
#include <unordered_map>
#include <simdjson.h>
#include "auth/access_token.hpp"
#include "account/user_profile.hpp"
#include "account/account_balances.hpp"
#include "account/position.hpp"
#include "account/history_item.hpp"
#include "account/account_history.hpp"
#include "account/gain_loss_item.hpp"
#include "account/gain_loss.hpp"
#include "trading/order.hpp"
#include "trading/order_requests.hpp"
#include "trading/multileg_orders.hpp"
#include "trading/advanced_orders.hpp"
#include "trading/spread_orders.hpp"
#include "trading/order_management.hpp"
#include "market/quote.hpp"
#include "market/option_chain.hpp"
#include "market/historical_data.hpp"
#include "market/time_sales.hpp"
#include "market/market_status.hpp"
#include "market/symbol_search.hpp"
#include "fundamentals/corp_actions.hpp"
#include "fundamentals/corp_calendar.hpp"
#include "fundamentals/corp_dividends.hpp"
#include "fundamentals/corp_financials.hpp"
#include "fundamentals/corp_pricestats.hpp"
#include "fundamentals/corp_info.hpp"
#include "watchlist/watchlist.hpp"
#include "watchlist/watchlist_detail.hpp"


namespace oqd {

// All types are defined in their respective headers.

}