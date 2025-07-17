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

#include "oqdTradierpp/api.hpp"
#include <algorithm>
#include <regex>

namespace oqd {

ApiMethods::ApiMethods(std::shared_ptr<TradierClient> client) 
    : client_(std::move(client)) {
}

std::string ApiMethods::get_oauth_url(const std::string& redirect_uri, const std::string& scope) const {
    std::string url = client_->get_base_url() + "/oauth/authorize";
    url += "?response_type=code";
    url += "&redirect_uri=" + redirect_uri;
    if (!scope.empty()) {
        url += "&scope=" + scope;
    }
    return url;
}

std::future<AccessToken> ApiMethods::create_access_token_async(const std::string& code, const std::string& redirect_uri) {
    std::unordered_map<std::string, std::string> params = {
        {"grant_type", "authorization_code"},
        {"code", code},
        {"redirect_uri", redirect_uri}
    };
    
    return parse_response_async<AccessToken>(
        client_->post_endpoint_async(endpoints::authentication::oauth_accesstoken, params)
    );
}

AccessToken ApiMethods::create_access_token(const std::string& code, const std::string& redirect_uri) {
    return create_access_token_async(code, redirect_uri).get();
}

std::future<AccessToken> ApiMethods::refresh_access_token_async(const std::string& refresh_token) {
    std::unordered_map<std::string, std::string> params = {
        {"grant_type", "refresh_token"},
        {"refresh_token", refresh_token}
    };
    
    return parse_response_async<AccessToken>(
        client_->post_endpoint_async(endpoints::authentication::oauth_accesstoken, params)
    );
}

AccessToken ApiMethods::refresh_access_token(const std::string& refresh_token) {
    return refresh_access_token_async(refresh_token).get();
}

std::future<UserProfile> ApiMethods::get_user_profile_async() {
    return parse_response_async<UserProfile>(
        client_->get_endpoint_async(endpoints::user::profile)
    );
}

UserProfile ApiMethods::get_user_profile() {
    return get_user_profile_async().get();
}

std::future<AccountBalances> ApiMethods::get_account_balances_async(const std::string& account_id) {
    std::string endpoint = endpoints::accounts::balances::path(account_id);
    return parse_response_async<AccountBalances>(
        client_->get_async(endpoint)
    );
}

AccountBalances ApiMethods::get_account_balances(const std::string& account_id) {
    return get_account_balances_async(account_id).get();
}

std::future<std::vector<Position>> ApiMethods::get_account_positions_async(const std::string& account_id) {
    std::string endpoint = endpoints::accounts::positions::path(account_id);
    return std::async(std::launch::async, [this, endpoint]() {
        auto response = client_->get_async(endpoint).get();
        std::vector<Position> positions;
        
        auto positions_elem = response["positions"];
        if (positions_elem.is_object()) {
            auto position_result = positions_elem["position"];
            if (position_result.error() == simdjson::SUCCESS) {
                auto position_array = position_result.value();
                if (position_array.is_array()) {
                    for (const auto& pos : position_array.get_array()) {
                        positions.push_back(Position::from_json(pos));
                    }
                } else {
                    positions.push_back(Position::from_json(position_array));
                }
            }
        }
        
        return positions;
    });
}

std::vector<Position> ApiMethods::get_account_positions(const std::string& account_id) {
    return get_account_positions_async(account_id).get();
}

std::future<std::vector<Quote>> ApiMethods::get_quotes_async(const std::vector<std::string>& symbols, bool include_greeks) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    if (include_greeks) {
        params["greeks"] = "true";
    }
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::markets::quotes, params).get();
        std::vector<Quote> quotes;
        
        auto quotes_elem = response["quotes"];
        if (quotes_elem.is_object()) {
            auto quote_result = quotes_elem["quote"];
            if (quote_result.error() == simdjson::SUCCESS) {
                auto quote_array = quote_result.value();
                if (quote_array.is_array()) {
                    for (const auto& quote : quote_array.get_array()) {
                        quotes.push_back(Quote::from_json(quote));
                    }
                } else {
                    quotes.push_back(Quote::from_json(quote_array));
                }
            }
        }
        
        return quotes;
    });
}

std::vector<Quote> ApiMethods::get_quotes(const std::vector<std::string>& symbols, bool include_greeks) {
    return get_quotes_async(symbols, include_greeks).get();
}

std::future<OptionChain> ApiMethods::get_option_chain_async(const std::string& symbol, const std::string& expiration, bool include_greeks) {
    std::unordered_map<std::string, std::string> params = {
        {"symbol", symbol},
        {"expiration", expiration}
    };
    
    if (include_greeks) {
        params["greeks"] = "true";
    }
    
    return parse_response_async<OptionChain>(
        client_->get_endpoint_async(endpoints::markets::options::chains, params)
    );
}

OptionChain ApiMethods::get_option_chain(const std::string& symbol, const std::string& expiration, bool include_greeks) {
    return get_option_chain_async(symbol, expiration, include_greeks).get();
}

std::future<std::vector<std::string>> ApiMethods::get_option_expirations_async(const std::string& symbol, bool include_all_roots, bool include_strikes) {
    std::unordered_map<std::string, std::string> params = {
        {"symbol", symbol}
    };
    
    if (include_all_roots) {
        params["includeAllRoots"] = "true";
    }
    if (include_strikes) {
        params["strikes"] = "true";
    }
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::markets::options::expirations, params).get();
        std::vector<std::string> expirations;
        
        auto expirations_elem = response["expirations"];
        if (expirations_elem.is_object()) {
            auto expiration_result = expirations_elem["expiration"];
            if (expiration_result.error() == simdjson::SUCCESS) {
                auto expiration_array = expiration_result.value();
                if (expiration_array.is_array()) {
                    for (const auto& exp : expiration_array.get_array()) {
                        auto date_result = exp["date"];
                        if (exp.is_object() && date_result.error() == simdjson::SUCCESS) {
                            expirations.push_back(std::string(date_result.value().get_string().value()));
                        } else {
                            expirations.push_back(std::string(exp.get_string().value()));
                        }
                    }
                } else {
                    auto date_result = expiration_array["date"];
                    if (expiration_array.is_object() && date_result.error() == simdjson::SUCCESS) {
                        expirations.push_back(std::string(date_result.value().get_string().value()));
                    } else {
                        expirations.push_back(std::string(expiration_array.get_string().value()));
                    }
                }
            }
        }
        
        return expirations;
    });
}

std::vector<std::string> ApiMethods::get_option_expirations(const std::string& symbol, bool include_all_roots, bool include_strikes) {
    return get_option_expirations_async(symbol, include_all_roots, include_strikes).get();
}

std::future<OrderResponse> ApiMethods::place_equity_order_async(const std::string& account_id, const EquityOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params = {
        {"class", to_string(order.order_class)},
        {"symbol", order.symbol},
        {"side", to_string(order.side)},
        {"quantity", std::to_string(order.quantity)},
        {"type", to_string(order.type)},
        {"duration", to_string(order.duration)}
    };
    
    if (order.price.has_value()) {
        params["price"] = std::to_string(order.price.value());
    }
    if (order.stop.has_value()) {
        params["stop"] = std::to_string(order.stop.value());
    }
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_equity_order(const std::string& account_id, const EquityOrderRequest& order) {
    return place_equity_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::place_option_order_async(const std::string& account_id, const OptionOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params = {
        {"class", to_string(order.order_class)},
        {"option_symbol", order.option_symbol},
        {"side", to_string(order.side)},
        {"quantity", std::to_string(order.quantity)},
        {"type", to_string(order.type)},
        {"duration", to_string(order.duration)}
    };
    
    if (order.price.has_value()) {
        params["price"] = std::to_string(order.price.value());
    }
    if (order.stop.has_value()) {
        params["stop"] = std::to_string(order.stop.value());
    }
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_option_order(const std::string& account_id, const OptionOrderRequest& order) {
    return place_option_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::cancel_order_async(const std::string& account_id, const std::string& order_id) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders/" + order_id;
    return parse_response_async<OrderResponse>(
        client_->delete_async(endpoint)
    );
}

OrderResponse ApiMethods::cancel_order(const std::string& account_id, const std::string& order_id) {
    return cancel_order_async(account_id, order_id).get();
}

std::future<OrderResponse> ApiMethods::place_multileg_order_async(const std::string& account_id, const MultilegOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params = {
        {"class", "multileg"},
        {"type", to_string(order.type)},
        {"duration", to_string(order.duration)}
    };
    
    for (size_t i = 0; i < order.legs.size(); ++i) {
        std::string leg_prefix = "leg" + std::to_string(i + 1) + "_";
        params[leg_prefix + "option_symbol"] = order.legs[i].option_symbol;
        params[leg_prefix + "side"] = to_string(order.legs[i].side);
        params[leg_prefix + "quantity"] = std::to_string(order.legs[i].quantity);
    }
    
    if (order.price.has_value()) {
        params["price"] = std::to_string(order.price.value());
    }
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_multileg_order(const std::string& account_id, const MultilegOrderRequest& order) {
    return place_multileg_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::place_combo_order_async(const std::string& account_id, const ComboOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params = {
        {"class", "combo"},
        {"type", to_string(order.type)},
        {"duration", to_string(order.duration)}
    };
    
    for (size_t i = 0; i < order.legs.size(); ++i) {
        std::string leg_prefix = "leg" + std::to_string(i + 1) + "_";
        params[leg_prefix + "option_symbol"] = order.legs[i].option_symbol;
        params[leg_prefix + "side"] = to_string(order.legs[i].side);
        params[leg_prefix + "quantity"] = std::to_string(order.legs[i].quantity);
    }
    
    if (order.price.has_value()) {
        params["price"] = std::to_string(order.price.value());
    }
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_combo_order(const std::string& account_id, const ComboOrderRequest& order) {
    return place_combo_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::modify_order_async(const std::string& account_id, const std::string& order_id, const OrderModification& modification) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders/" + order_id;
    
    std::unordered_map<std::string, std::string> params;
    
    if (modification.type.has_value()) {
        params["type"] = to_string(modification.type.value());
    }
    if (modification.duration.has_value()) {
        params["duration"] = to_string(modification.duration.value());
    }
    if (modification.price.has_value()) {
        params["price"] = std::to_string(modification.price.value());
    }
    if (modification.stop.has_value()) {
        params["stop"] = std::to_string(modification.stop.value());
    }
    if (modification.quantity.has_value()) {
        params["quantity"] = std::to_string(modification.quantity.value());
    }
    
    return parse_response_async<OrderResponse>(
        client_->put_async(endpoint, params)
    );
}

OrderResponse ApiMethods::modify_order(const std::string& account_id, const std::string& order_id, const OrderModification& modification) {
    return modify_order_async(account_id, order_id, modification).get();
}

std::future<OrderResponse> ApiMethods::place_oto_order_async(const std::string& account_id, const OTOOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params;
    params["class"] = to_string(order.order_class);
    
    // First order (primary)
    params["symbol[0]"] = order.first_order.symbol;
    params["side[0]"] = to_string(order.first_order.side);
    params["quantity[0]"] = std::to_string(order.first_order.quantity);
    params["type[0]"] = to_string(order.first_order.type);
    params["duration[0]"] = to_string(order.first_order.duration);
    
    if (order.first_order.price.has_value()) {
        params["price[0]"] = std::to_string(order.first_order.price.value());
    }
    if (order.first_order.stop.has_value()) {
        params["stop[0]"] = std::to_string(order.first_order.stop.value());
    }
    if (order.first_order.option_symbol.has_value()) {
        params["option_symbol[0]"] = order.first_order.option_symbol.value();
    }
    
    // Second order (triggered)
    params["symbol[1]"] = order.second_order.symbol;
    params["side[1]"] = to_string(order.second_order.side);
    params["quantity[1]"] = std::to_string(order.second_order.quantity);
    params["type[1]"] = to_string(order.second_order.type);
    params["duration[1]"] = to_string(order.second_order.duration);
    
    if (order.second_order.price.has_value()) {
        params["price[1]"] = std::to_string(order.second_order.price.value());
    }
    if (order.second_order.stop.has_value()) {
        params["stop[1]"] = std::to_string(order.second_order.stop.value());
    }
    if (order.second_order.option_symbol.has_value()) {
        params["option_symbol[1]"] = order.second_order.option_symbol.value();
    }
    
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_oto_order(const std::string& account_id, const OTOOrderRequest& order) {
    return place_oto_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::place_oco_order_async(const std::string& account_id, const OCOOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params;
    params["class"] = to_string(order.order_class);
    
    // First order
    params["symbol[0]"] = order.first_order.symbol;
    params["side[0]"] = to_string(order.first_order.side);
    params["quantity[0]"] = std::to_string(order.first_order.quantity);
    params["type[0]"] = to_string(order.first_order.type);
    params["duration[0]"] = to_string(order.first_order.duration);
    
    if (order.first_order.price.has_value()) {
        params["price[0]"] = std::to_string(order.first_order.price.value());
    }
    if (order.first_order.stop.has_value()) {
        params["stop[0]"] = std::to_string(order.first_order.stop.value());
    }
    if (order.first_order.option_symbol.has_value()) {
        params["option_symbol[0]"] = order.first_order.option_symbol.value();
    }
    
    // Second order (alternate)
    params["symbol[1]"] = order.second_order.symbol;
    params["side[1]"] = to_string(order.second_order.side);
    params["quantity[1]"] = std::to_string(order.second_order.quantity);
    params["type[1]"] = to_string(order.second_order.type);
    params["duration[1]"] = to_string(order.second_order.duration);
    
    if (order.second_order.price.has_value()) {
        params["price[1]"] = std::to_string(order.second_order.price.value());
    }
    if (order.second_order.stop.has_value()) {
        params["stop[1]"] = std::to_string(order.second_order.stop.value());
    }
    if (order.second_order.option_symbol.has_value()) {
        params["option_symbol[1]"] = order.second_order.option_symbol.value();
    }
    
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_oco_order(const std::string& account_id, const OCOOrderRequest& order) {
    return place_oco_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::place_otoco_order_async(const std::string& account_id, const OTOCOOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params;
    params["class"] = to_string(order.order_class);
    
    // Primary order (triggers the bracket)
    params["symbol[0]"] = order.primary_order.symbol;
    params["side[0]"] = to_string(order.primary_order.side);
    params["quantity[0]"] = std::to_string(order.primary_order.quantity);
    params["type[0]"] = to_string(order.primary_order.type);
    params["duration[0]"] = to_string(order.primary_order.duration);
    
    if (order.primary_order.price.has_value()) {
        params["price[0]"] = std::to_string(order.primary_order.price.value());
    }
    if (order.primary_order.stop.has_value()) {
        params["stop[0]"] = std::to_string(order.primary_order.stop.value());
    }
    if (order.primary_order.option_symbol.has_value()) {
        params["option_symbol[0]"] = order.primary_order.option_symbol.value();
    }
    
    // Profit order
    params["symbol[1]"] = order.profit_order.symbol;
    params["side[1]"] = to_string(order.profit_order.side);
    params["quantity[1]"] = std::to_string(order.profit_order.quantity);
    params["type[1]"] = to_string(order.profit_order.type);
    params["duration[1]"] = to_string(order.profit_order.duration);
    
    if (order.profit_order.price.has_value()) {
        params["price[1]"] = std::to_string(order.profit_order.price.value());
    }
    if (order.profit_order.stop.has_value()) {
        params["stop[1]"] = std::to_string(order.profit_order.stop.value());
    }
    if (order.profit_order.option_symbol.has_value()) {
        params["option_symbol[1]"] = order.profit_order.option_symbol.value();
    }
    
    // Stop order
    params["symbol[2]"] = order.stop_order.symbol;
    params["side[2]"] = to_string(order.stop_order.side);
    params["quantity[2]"] = std::to_string(order.stop_order.quantity);
    params["type[2]"] = to_string(order.stop_order.type);
    params["duration[2]"] = to_string(order.stop_order.duration);
    
    if (order.stop_order.price.has_value()) {
        params["price[2]"] = std::to_string(order.stop_order.price.value());
    }
    if (order.stop_order.stop.has_value()) {
        params["stop[2]"] = std::to_string(order.stop_order.stop.value());
    }
    if (order.stop_order.option_symbol.has_value()) {
        params["option_symbol[2]"] = order.stop_order.option_symbol.value();
    }
    
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_otoco_order(const std::string& account_id, const OTOCOOrderRequest& order) {
    return place_otoco_order_async(account_id, order).get();
}

std::future<OrderResponse> ApiMethods::place_spread_order_async(const std::string& account_id, const SpreadOrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params;
    params["class"] = to_string(order.order_class);
    params["type"] = to_string(order.type);
    params["duration"] = to_string(order.duration);
    
    if (order.price.has_value()) {
        params["price"] = std::to_string(order.price.value());
    }
    
    // Add spread-specific metadata
    params["spread_type"] = order.spread_type;
    
    for (size_t i = 0; i < order.legs.size(); ++i) {
        const auto& leg = order.legs[i];
        std::string idx = std::to_string(i);
        
        params["option_symbol[" + idx + "]"] = leg.option_symbol;
        params["side[" + idx + "]"] = to_string(leg.side);
        params["quantity[" + idx + "]"] = std::to_string(leg.quantity);
        
        if (leg.ratio.has_value()) {
            params["ratio[" + idx + "]"] = std::to_string(leg.ratio.value());
        }
    }
    
    if (order.tag.has_value()) {
        params["tag"] = order.tag.value();
    }
    
    return parse_response_async<OrderResponse>(
        client_->post_async(endpoint, params)
    );
}

OrderResponse ApiMethods::place_spread_order(const std::string& account_id, const SpreadOrderRequest& order) {
    return place_spread_order_async(account_id, order).get();
}

std::future<MarketClock> ApiMethods::get_market_clock_async() {
    return parse_response_async<MarketClock>(
        client_->get_endpoint_async(endpoints::markets::clock)
    );
}

MarketClock ApiMethods::get_market_clock() {
    return get_market_clock_async().get();
}

std::future<std::vector<CompanySearch>> ApiMethods::search_companies_async(const std::string& query, bool include_indexes) {
    std::unordered_map<std::string, std::string> params = {
        {"q", query}
    };
    
    if (include_indexes) {
        params["indexes"] = "true";
    }
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::markets::search, params).get();
        std::vector<CompanySearch> results;
        
        auto securities_elem = response["securities"];
        if (securities_elem.is_object()) {
            auto security_result = securities_elem["security"];
            if (security_result.error() == simdjson::SUCCESS) {
                auto security_array = security_result.value();
                if (security_array.is_array()) {
                    for (const auto& security : security_array.get_array()) {
                        results.push_back(CompanySearch::from_json(security));
                    }
                } else {
                    results.push_back(CompanySearch::from_json(security_array));
                }
            }
        }
        
        return results;
    });
}

std::vector<CompanySearch> ApiMethods::search_companies(const std::string& query, bool include_indexes) {
    return search_companies_async(query, include_indexes).get();
}

std::future<std::vector<CompanyInfo>> ApiMethods::get_company_info_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_async("/beta/markets/fundamentals/company", params).get();
        std::vector<CompanyInfo> results;
        
        // Placeholder implementation for beta endpoints
        // In production, this would parse the actual API response structure
        auto companies_elem = response["companies"];
        if (companies_elem.is_array()) {
            for (const auto& company : companies_elem.get_array()) {
                results.push_back(CompanyInfo::from_json(company));
            }
        }
        
        return results;
    });
}

std::vector<CompanyInfo> ApiMethods::get_company_info(const std::vector<std::string>& symbols) {
    return get_company_info_async(symbols).get();
}

std::future<std::vector<FinancialRatios>> ApiMethods::get_financial_ratios_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_async("/beta/markets/fundamentals/ratios", params).get();
        std::vector<FinancialRatios> results;
        
        // Placeholder implementation for beta endpoints
        auto ratios_elem = response["ratios"];
        if (ratios_elem.is_array()) {
            for (const auto& ratio : ratios_elem.get_array()) {
                results.push_back(FinancialRatios::from_json(ratio));
            }
        }
        
        return results;
    });
}

std::vector<FinancialRatios> ApiMethods::get_financial_ratios(const std::vector<std::string>& symbols) {
    return get_financial_ratios_async(symbols).get();
}

std::string ApiMethods::join_symbols(const std::vector<std::string>& symbols) const {
    std::string result;
    for (size_t i = 0; i < symbols.size(); ++i) {
        if (i > 0) result += ",";
        result += symbols[i];
    }
    return result;
}

template<typename T>
std::future<T> ApiMethods::parse_response_async(std::future<simdjson::dom::element> response_future) {
    return std::async(std::launch::async, [response_future = std::move(response_future)]() mutable {
        auto response = response_future.get();
        return T::from_json(response);
    });
}

template<typename T>
T ApiMethods::parse_response(const simdjson::dom::element& response) {
    return T::from_json(response);
}

std::unordered_map<std::string, std::string> ApiMethods::build_params(const std::unordered_map<std::string, std::string>& base_params) const {
    return base_params;
}

std::vector<Order> ApiMethods::get_account_orders(const std::string& account_id, bool include_tags) {
    return get_account_orders_async(account_id, include_tags).get();
}

std::future<std::vector<Order>> ApiMethods::get_account_orders_async(const std::string& account_id, bool include_tags) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params;
    if (include_tags) {
        params["includeTags"] = "true";
    }
    
    return std::async(std::launch::async, [this, endpoint, params]() {
        auto response = client_->get_async(endpoint, params).get();
        std::vector<Order> orders;
        
        auto orders_elem = response["orders"];
        if (orders_elem.is_object()) {
            auto order_result = orders_elem["order"];
            if (order_result.error() == simdjson::SUCCESS) {
                auto order_array = order_result.value();
                if (order_array.is_array()) {
                    for (const auto& order : order_array.get_array()) {
                        orders.push_back(Order::from_json(order));
                    }
                } else {
                    orders.push_back(Order::from_json(order_array));
                }
            }
        } else if (orders_elem.is_array()) {
            for (const auto& order : orders_elem.get_array()) {
                orders.push_back(Order::from_json(order));
            }
        }
        
        return orders;
    });
}

OrderPreview ApiMethods::preview_order(const std::string& account_id, const OrderRequest& order) {
    return preview_order_async(account_id, order).get();
}

std::future<OrderPreview> ApiMethods::preview_order_async(const std::string& account_id, const OrderRequest& order) {
    std::string endpoint = "/v1/accounts/" + account_id + "/orders";
    
    std::unordered_map<std::string, std::string> params = {
        {"preview", "true"},
        {"class", to_string(order.order_class)},
        {"symbol", order.symbol},
        {"side", to_string(order.side)},
        {"quantity", std::to_string(order.quantity)},
        {"type", to_string(order.type)},
        {"duration", to_string(order.duration)}
    };
    
    if (order.price.has_value()) {
        params["price"] = std::to_string(order.price.value());
    }
    if (order.stop.has_value()) {
        params["stop"] = std::to_string(order.stop.value());
    }
    
    return parse_response_async<OrderPreview>(
        client_->post_async(endpoint, params)
    );
}

std::vector<HistoricalData> ApiMethods::get_historical_data(const std::string& symbol, 
                                                           const std::string& interval,
                                                           std::optional<std::string> start,
                                                           std::optional<std::string> end) {
    return get_historical_data_async(symbol, interval, start, end).get();
}

std::future<std::vector<HistoricalData>> ApiMethods::get_historical_data_async(const std::string& symbol,
                                                                              const std::string& interval,
                                                                              std::optional<std::string> start,
                                                                              std::optional<std::string> end) {
    std::unordered_map<std::string, std::string> params = {
        {"symbol", symbol},
        {"interval", interval}
    };
    
    if (start.has_value()) {
        params["start"] = start.value();
    }
    if (end.has_value()) {
        params["end"] = end.value();
    }
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::markets::history, params).get();
        std::vector<HistoricalData> data;
        
        auto history_elem = response["history"];
        if (history_elem.is_object()) {
            auto day_result = history_elem["day"];
            if (day_result.error() == simdjson::SUCCESS) {
                auto day_array = day_result.value();
                if (day_array.is_array()) {
                    for (const auto& day : day_array.get_array()) {
                        data.push_back(HistoricalData::from_json(day));
                    }
                } else {
                    data.push_back(HistoricalData::from_json(day_array));
                }
            }
        } else if (history_elem.is_array()) {
            for (const auto& day : history_elem.get_array()) {
                data.push_back(HistoricalData::from_json(day));
            }
        }
        
        return data;
    });
}

std::vector<SymbolLookup> ApiMethods::lookup_symbols(const std::string& query, const std::vector<std::string>& types) {
    return lookup_symbols_async(query, types).get();
}

std::vector<Watchlist> ApiMethods::get_all_watchlists() {
    return get_all_watchlists_async().get();
}

std::future<std::vector<Watchlist>> ApiMethods::get_all_watchlists_async() {
    return std::async(std::launch::async, [this]() {
        auto response = client_->get_async("/v1/watchlists").get();
        std::vector<Watchlist> watchlists;
        
        auto watchlists_elem = response["watchlists"];
        if (watchlists_elem.is_object()) {
            auto watchlist_result = watchlists_elem["watchlist"];
            if (watchlist_result.error() == simdjson::SUCCESS) {
                auto watchlist_array = watchlist_result.value();
                if (watchlist_array.is_array()) {
                    for (const auto& wl : watchlist_array.get_array()) {
                        watchlists.push_back(Watchlist::from_json(wl));
                    }
                } else {
                    watchlists.push_back(Watchlist::from_json(watchlist_array));
                }
            }
        }
        
        return watchlists;
    });
}

Watchlist ApiMethods::create_watchlist(const std::string& name, const std::vector<std::string>& symbols) {
    return create_watchlist_async(name, symbols).get();
}

std::future<Watchlist> ApiMethods::create_watchlist_async(const std::string& name, const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"name", name}
    };
    
    if (!symbols.empty()) {
        params["symbols"] = join_symbols(symbols);
    }
    
    return parse_response_async<Watchlist>(
        client_->post_async("/v1/watchlists", params)
    );
}

WatchlistDetail ApiMethods::add_symbols_to_watchlist(const std::string& watchlist_id, const std::vector<std::string>& symbols) {
    return add_symbols_to_watchlist_async(watchlist_id, symbols).get();
}

std::future<WatchlistDetail> ApiMethods::add_symbols_to_watchlist_async(const std::string& watchlist_id, const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return parse_response_async<WatchlistDetail>(
        client_->post_async("/v1/watchlists/" + watchlist_id + "/symbols", params)
    );
}

void ApiMethods::delete_watchlist(const std::string& watchlist_id) {
    return delete_watchlist_async(watchlist_id).get();
}

std::future<void> ApiMethods::delete_watchlist_async(const std::string& watchlist_id) {
    return std::async(std::launch::async, [this, watchlist_id]() {
        client_->delete_async("/v1/watchlists/" + watchlist_id).get();
    });
}

std::future<std::vector<SymbolLookup>> ApiMethods::lookup_symbols_async(const std::string& query, const std::vector<std::string>& types) {
    std::unordered_map<std::string, std::string> params = {
        {"q", query}
    };
    
    if (!types.empty()) {
        params["types"] = join_symbols(types);
    }
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::markets::lookup, params).get();
        std::vector<SymbolLookup> results;
        
        // Similar to company search implementation
        auto securities_elem = response["securities"];
        if (securities_elem.is_object()) {
            auto security_result = securities_elem["security"];
            if (security_result.error() == simdjson::SUCCESS) {
                auto security_array = security_result.value();
                if (security_array.is_array()) {
                    for (const auto& security : security_array.get_array()) {
                        results.push_back(SymbolLookup::from_json(security));
                    }
                } else {
                    results.push_back(SymbolLookup::from_json(security_array));
                }
            }
        }
        
        return results;
    });
}

std::future<std::vector<CorporateActions>> ApiMethods::get_corporate_actions_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::beta::fundamentals::corporate_calendar, params).get();
        std::vector<CorporateActions> results;
        
        auto actions_result = response["corporate_actions"];
        if (actions_result.error() == simdjson::SUCCESS) {
            auto actions_elem = actions_result.value();
            if (actions_elem.is_array()) {
                for (const auto& action : actions_elem.get_array()) {
                    results.push_back(CorporateActions::from_json(action));
                }
            } else if (actions_elem.is_object()) {
                results.push_back(CorporateActions::from_json(actions_elem));
            }
        }
        
        return results;
    });
}

std::vector<CorporateActions> ApiMethods::get_corporate_actions(const std::vector<std::string>& symbols) {
    return get_corporate_actions_async(symbols).get();
}

std::future<std::vector<CorporateFinancials>> ApiMethods::get_corporate_financials_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::beta::fundamentals::financials, params).get();
        std::vector<CorporateFinancials> results;
        
        auto financials_result = response["financials"];
        if (financials_result.error() == simdjson::SUCCESS) {
            auto financials_elem = financials_result.value();
            if (financials_elem.is_array()) {
                for (const auto& financial : financials_elem.get_array()) {
                    results.push_back(CorporateFinancials::from_json(financial));
                }
            } else if (financials_elem.is_object()) {
                results.push_back(CorporateFinancials::from_json(financials_elem));
            }
        }
        
        return results;
    });
}

std::vector<CorporateFinancials> ApiMethods::get_corporate_financials(const std::vector<std::string>& symbols) {
    return get_corporate_financials_async(symbols).get();
}

std::future<std::vector<PriceStatistics>> ApiMethods::get_price_statistics_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::beta::fundamentals::price_stats, params).get();
        std::vector<PriceStatistics> results;
        
        auto stats_result = response["price_statistics"];
        if (stats_result.error() == simdjson::SUCCESS) {
            auto stats_elem = stats_result.value();
            if (stats_elem.is_array()) {
                for (const auto& stat : stats_elem.get_array()) {
                    results.push_back(PriceStatistics::from_json(stat));
                }
            } else if (stats_elem.is_object()) {
                results.push_back(PriceStatistics::from_json(stats_elem));
            }
        }
        
        return results;
    });
}

std::vector<PriceStatistics> ApiMethods::get_price_statistics(const std::vector<std::string>& symbols) {
    return get_price_statistics_async(symbols).get();
}

std::future<std::vector<DividendInfo>> ApiMethods::get_dividend_info_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::beta::fundamentals::dividend, params).get();
        std::vector<DividendInfo> results;
        
        auto dividends_result = response["dividends"];
        if (dividends_result.error() == simdjson::SUCCESS) {
            auto dividends_elem = dividends_result.value();
            if (dividends_elem.is_array()) {
                for (const auto& dividend : dividends_elem.get_array()) {
                    results.push_back(DividendInfo::from_json(dividend));
                }
            } else if (dividends_elem.is_object()) {
                results.push_back(DividendInfo::from_json(dividends_elem));
            }
        }
        
        return results;
    });
}

std::vector<DividendInfo> ApiMethods::get_dividend_info(const std::vector<std::string>& symbols) {
    return get_dividend_info_async(symbols).get();
}

std::future<std::vector<CorporateCalendar>> ApiMethods::get_corporate_calendar_async(const std::vector<std::string>& symbols) {
    std::unordered_map<std::string, std::string> params = {
        {"symbols", join_symbols(symbols)}
    };
    
    return std::async(std::launch::async, [this, params]() {
        auto response = client_->get_endpoint_async(endpoints::beta::fundamentals::corporate_calendar, params).get();
        std::vector<CorporateCalendar> results;
        
        auto calendar_result = response["corporate_calendar"];
        if (calendar_result.error() == simdjson::SUCCESS) {
            auto calendar_elem = calendar_result.value();
            if (calendar_elem.is_array()) {
                for (const auto& event : calendar_elem.get_array()) {
                    results.push_back(CorporateCalendar::from_json(event));
                }
            } else if (calendar_elem.is_object()) {
                results.push_back(CorporateCalendar::from_json(calendar_elem));
            }
        }
        
        return results;
    });
}

std::vector<CorporateCalendar> ApiMethods::get_corporate_calendar(const std::vector<std::string>& symbols) {
    return get_corporate_calendar_async(symbols).get();
}

} // namespace oqd