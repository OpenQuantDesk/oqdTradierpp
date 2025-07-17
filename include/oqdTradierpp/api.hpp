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

#include "client.hpp"
#include "types.hpp"
#include <vector>
#include <string>
#include <optional>
#include <future>

namespace oqd {

class ApiMethods {
public:
    explicit ApiMethods(std::shared_ptr<TradierClient> client);
    
    // Authentication
    std::string get_oauth_url(const std::string& redirect_uri, const std::string& scope = "") const;
    std::future<AccessToken> create_access_token_async(const std::string& code, const std::string& redirect_uri);
    std::future<AccessToken> refresh_access_token_async(const std::string& refresh_token);
    
    AccessToken create_access_token(const std::string& code, const std::string& redirect_uri);
    AccessToken refresh_access_token(const std::string& refresh_token);

    // Account Management
    std::future<UserProfile> get_user_profile_async();
    std::future<AccountBalances> get_account_balances_async(const std::string& account_id);
    std::future<std::vector<Position>> get_account_positions_async(const std::string& account_id);
    std::future<AccountHistory> get_account_history_async(const std::string& account_id, 
                                                         std::optional<int> page = std::nullopt,
                                                         std::optional<int> limit = std::nullopt);
    std::future<GainLoss> get_account_gainloss_async(const std::string& account_id,
                                                     std::optional<int> page = std::nullopt,
                                                     std::optional<int> limit = std::nullopt);
    std::future<std::vector<Order>> get_account_orders_async(const std::string& account_id,
                                                            bool include_tags = false);
    std::future<Order> get_individual_order_async(const std::string& account_id, const std::string& order_id);

    UserProfile get_user_profile();
    AccountBalances get_account_balances(const std::string& account_id);
    std::vector<Position> get_account_positions(const std::string& account_id);
    AccountHistory get_account_history(const std::string& account_id, 
                                      std::optional<int> page = std::nullopt,
                                      std::optional<int> limit = std::nullopt);
    GainLoss get_account_gainloss(const std::string& account_id,
                                 std::optional<int> page = std::nullopt,
                                 std::optional<int> limit = std::nullopt);
    std::vector<Order> get_account_orders(const std::string& account_id, bool include_tags = false);
    Order get_individual_order(const std::string& account_id, const std::string& order_id);

    // Trading
    std::future<OrderPreview> preview_order_async(const std::string& account_id, const OrderRequest& order);
    std::future<OrderResponse> place_equity_order_async(const std::string& account_id, const EquityOrderRequest& order);
    std::future<OrderResponse> place_option_order_async(const std::string& account_id, const OptionOrderRequest& order);
    std::future<OrderResponse> place_multileg_order_async(const std::string& account_id, const MultilegOrderRequest& order);
    std::future<OrderResponse> place_combo_order_async(const std::string& account_id, const ComboOrderRequest& order);
    std::future<OrderResponse> modify_order_async(const std::string& account_id, const std::string& order_id, const OrderModification& modification);
    std::future<OrderResponse> cancel_order_async(const std::string& account_id, const std::string& order_id);
    
    // Advanced Order Types
    std::future<OrderResponse> place_oto_order_async(const std::string& account_id, const OTOOrderRequest& order);
    std::future<OrderResponse> place_oco_order_async(const std::string& account_id, const OCOOrderRequest& order);
    std::future<OrderResponse> place_otoco_order_async(const std::string& account_id, const OTOCOOrderRequest& order);
    std::future<OrderResponse> place_spread_order_async(const std::string& account_id, const SpreadOrderRequest& order);

    OrderPreview preview_order(const std::string& account_id, const OrderRequest& order);
    OrderResponse place_equity_order(const std::string& account_id, const EquityOrderRequest& order);
    OrderResponse place_option_order(const std::string& account_id, const OptionOrderRequest& order);
    OrderResponse place_multileg_order(const std::string& account_id, const MultilegOrderRequest& order);
    OrderResponse place_combo_order(const std::string& account_id, const ComboOrderRequest& order);
    
    // Advanced Order Types
    OrderResponse place_oto_order(const std::string& account_id, const OTOOrderRequest& order);
    OrderResponse place_oco_order(const std::string& account_id, const OCOOrderRequest& order);
    OrderResponse place_otoco_order(const std::string& account_id, const OTOCOOrderRequest& order);
    OrderResponse place_spread_order(const std::string& account_id, const SpreadOrderRequest& order);
    
    OrderResponse modify_order(const std::string& account_id, const std::string& order_id, const OrderModification& modification);
    OrderResponse cancel_order(const std::string& account_id, const std::string& order_id);

    // Market Data
    std::future<std::vector<Quote>> get_quotes_async(const std::vector<std::string>& symbols, bool include_greeks = false);
    std::future<OptionChain> get_option_chain_async(const std::string& symbol, const std::string& expiration, bool include_greeks = false);
    std::future<std::vector<std::string>> get_option_expirations_async(const std::string& symbol, bool include_all_roots = false, bool include_strikes = false);
    std::future<std::vector<double>> get_option_strikes_async(const std::string& symbol, const std::string& expiration);
    std::future<std::vector<HistoricalData>> get_historical_data_async(const std::string& symbol, 
                                                                      const std::string& interval = "daily",
                                                                      std::optional<std::string> start = std::nullopt,
                                                                      std::optional<std::string> end = std::nullopt);
    std::future<std::vector<TimeSales>> get_time_and_sales_async(const std::string& symbol,
                                                                const std::string& interval = "tick",
                                                                std::optional<std::string> start = std::nullopt,
                                                                std::optional<std::string> end = std::nullopt);
    std::future<MarketClock> get_market_clock_async();
    std::future<std::vector<MarketDay>> get_market_calendar_async(std::optional<int> month = std::nullopt, std::optional<int> year = std::nullopt);
    std::future<std::vector<CompanySearch>> search_companies_async(const std::string& query, bool include_indexes = false);
    std::future<std::vector<SymbolLookup>> lookup_symbols_async(const std::string& query, const std::vector<std::string>& types = {});
    std::future<std::vector<std::string>> get_etb_list_async();

    std::vector<Quote> get_quotes(const std::vector<std::string>& symbols, bool include_greeks = false);
    OptionChain get_option_chain(const std::string& symbol, const std::string& expiration, bool include_greeks = false);
    std::vector<std::string> get_option_expirations(const std::string& symbol, bool include_all_roots = false, bool include_strikes = false);
    std::vector<double> get_option_strikes(const std::string& symbol, const std::string& expiration);
    std::vector<HistoricalData> get_historical_data(const std::string& symbol, 
                                                   const std::string& interval = "daily",
                                                   std::optional<std::string> start = std::nullopt,
                                                   std::optional<std::string> end = std::nullopt);
    std::vector<TimeSales> get_time_and_sales(const std::string& symbol,
                                             const std::string& interval = "tick",
                                             std::optional<std::string> start = std::nullopt,
                                             std::optional<std::string> end = std::nullopt);
    MarketClock get_market_clock();
    std::vector<MarketDay> get_market_calendar(std::optional<int> month = std::nullopt, std::optional<int> year = std::nullopt);
    std::vector<CompanySearch> search_companies(const std::string& query, bool include_indexes = false);
    std::vector<SymbolLookup> lookup_symbols(const std::string& query, const std::vector<std::string>& types = {});
    std::vector<std::string> get_etb_list();

    // Fundamentals (Beta)
    std::future<std::vector<CompanyInfo>> get_company_info_async(const std::vector<std::string>& symbols);
    std::future<std::vector<FinancialRatios>> get_financial_ratios_async(const std::vector<std::string>& symbols);
    std::future<std::vector<CorporateActions>> get_corporate_actions_async(const std::vector<std::string>& symbols);
    std::future<std::vector<CorporateFinancials>> get_corporate_financials_async(const std::vector<std::string>& symbols);
    std::future<std::vector<PriceStatistics>> get_price_statistics_async(const std::vector<std::string>& symbols);
    std::future<std::vector<DividendInfo>> get_dividend_info_async(const std::vector<std::string>& symbols);
    std::future<std::vector<CorporateCalendar>> get_corporate_calendar_async(const std::vector<std::string>& symbols);

    std::vector<CompanyInfo> get_company_info(const std::vector<std::string>& symbols);
    std::vector<FinancialRatios> get_financial_ratios(const std::vector<std::string>& symbols);
    std::vector<CorporateActions> get_corporate_actions(const std::vector<std::string>& symbols);
    std::vector<CorporateFinancials> get_corporate_financials(const std::vector<std::string>& symbols);
    std::vector<PriceStatistics> get_price_statistics(const std::vector<std::string>& symbols);
    std::vector<DividendInfo> get_dividend_info(const std::vector<std::string>& symbols);
    std::vector<CorporateCalendar> get_corporate_calendar(const std::vector<std::string>& symbols);

    // Watchlists
    std::future<std::vector<Watchlist>> get_all_watchlists_async();
    std::future<WatchlistDetail> get_specific_watchlist_async(const std::string& watchlist_id);
    std::future<Watchlist> create_watchlist_async(const std::string& name, const std::vector<std::string>& symbols = {});
    std::future<Watchlist> update_watchlist_async(const std::string& watchlist_id, 
                                                  std::optional<std::string> name = std::nullopt,
                                                  std::optional<std::vector<std::string>> symbols = std::nullopt);
    std::future<void> delete_watchlist_async(const std::string& watchlist_id);
    std::future<WatchlistDetail> add_symbols_to_watchlist_async(const std::string& watchlist_id, const std::vector<std::string>& symbols);
    std::future<WatchlistDetail> remove_symbol_from_watchlist_async(const std::string& watchlist_id, const std::string& symbol);

    std::vector<Watchlist> get_all_watchlists();
    WatchlistDetail get_specific_watchlist(const std::string& watchlist_id);
    Watchlist create_watchlist(const std::string& name, const std::vector<std::string>& symbols = {});
    Watchlist update_watchlist(const std::string& watchlist_id, 
                              std::optional<std::string> name = std::nullopt,
                              std::optional<std::vector<std::string>> symbols = std::nullopt);
    void delete_watchlist(const std::string& watchlist_id);
    WatchlistDetail add_symbols_to_watchlist(const std::string& watchlist_id, const std::vector<std::string>& symbols);
    WatchlistDetail remove_symbol_from_watchlist(const std::string& watchlist_id, const std::string& symbol);

private:
    std::shared_ptr<TradierClient> client_;
    
    template<typename T>
    std::future<T> parse_response_async(std::future<simdjson::dom::element> response_future);
    
    template<typename T>
    T parse_response(const simdjson::dom::element& response);
    
    std::string join_symbols(const std::vector<std::string>& symbols) const;
    std::unordered_map<std::string, std::string> build_params(const std::unordered_map<std::string, std::string>& base_params) const;
};

} // namespace oqd