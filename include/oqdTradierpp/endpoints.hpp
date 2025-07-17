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

#include <string_view>
#include <array>
#include <string>
#include <unordered_map>
#include "validation.hpp"

namespace oqd::endpoints {

struct EndpointConfig {
    std::string_view path;
    std::string_view method;
    std::string_view auth_type;
    int rate_limit_per_second = 60;
    
    constexpr EndpointConfig(std::string_view p, std::string_view m, std::string_view a, int r = 60)
        : path(p), method(m), auth_type(a), rate_limit_per_second(r) {}
};

namespace base_urls {
    constexpr std::string_view production = "https://api.tradier.com";
    constexpr std::string_view sandbox = "https://sandbox.tradier.com";
}

namespace authentication {
    constexpr EndpointConfig oauth_authorize{"/oauth/authorize", "GET", "none"};
    constexpr EndpointConfig oauth_accesstoken{"/oauth/accesstoken", "POST", "basic"};
}

namespace user {
    constexpr EndpointConfig profile{"/v1/user/profile", "GET", "bearer"};
}

namespace accounts {
    constexpr std::string_view balances_template = "/v1/accounts/{account_id}/balances";
    constexpr std::string_view positions_template = "/v1/accounts/{account_id}/positions";
    constexpr std::string_view orders_template = "/v1/accounts/{account_id}/orders";
    constexpr std::string_view order_template = "/v1/accounts/{account_id}/orders/{order_id}";
    
    struct balances {
        static constexpr std::string_view method = "GET";
        static constexpr std::string_view auth_type = "bearer";
        static constexpr int rate_limit = 120;
        static std::string path(const std::string& account_id) {
            std::string validated_id = PathValidator::validate_account_id(account_id);
            return "/v1/accounts/" + validated_id + "/balances";
        }
    };
    
    struct positions {
        static constexpr std::string_view method = "GET";
        static constexpr std::string_view auth_type = "bearer";
        static constexpr int rate_limit = 120;
        static std::string path(const std::string& account_id) {
            std::string validated_id = PathValidator::validate_account_id(account_id);
            return "/v1/accounts/" + validated_id + "/positions";
        }
    };
    
    struct orders {
        static constexpr std::string_view method = "GET";
        static constexpr std::string_view auth_type = "bearer";
        static constexpr int rate_limit = 180;
        static std::string path(const std::string& account_id) {
            std::string validated_id = PathValidator::validate_account_id(account_id);
            return "/v1/accounts/" + validated_id + "/orders";
        }
        
        struct create {
            static constexpr std::string_view method = "POST";
            static constexpr std::string_view auth_type = "bearer";
            static constexpr int rate_limit = 180;
            static std::string path(const std::string& account_id) {
                std::string validated_id = PathValidator::validate_account_id(account_id);
                return "/v1/accounts/" + validated_id + "/orders";
            }
        };
        
        struct cancel {
            static constexpr std::string_view method = "DELETE";
            static constexpr std::string_view auth_type = "bearer";
            static constexpr int rate_limit = 180;
            static std::string path(const std::string& account_id, const std::string& order_id) {
                std::string validated_account_id = PathValidator::validate_account_id(account_id);
                std::string validated_order_id = PathValidator::validate_order_id(order_id);
                return "/v1/accounts/" + validated_account_id + "/orders/" + validated_order_id;
            }
        };
    };
}

namespace markets {
    constexpr EndpointConfig quotes{"/v1/markets/quotes", "GET", "bearer", 120};
    constexpr EndpointConfig clock{"/v1/markets/clock", "GET", "bearer", 60};
    constexpr EndpointConfig search{"/v1/markets/search", "GET", "bearer", 60};
    constexpr EndpointConfig lookup{"/v1/markets/lookup", "GET", "bearer", 60};
    constexpr EndpointConfig history{"/v1/markets/history", "GET", "bearer", 120};
    
    namespace options {
        constexpr EndpointConfig chains{"/v1/markets/options/chains", "GET", "bearer", 60};
        constexpr EndpointConfig expirations{"/v1/markets/options/expirations", "GET", "bearer", 60};
        constexpr EndpointConfig strikes{"/v1/markets/options/strikes", "GET", "bearer", 60};
    };
    
    namespace events {
        constexpr EndpointConfig session{"/v1/markets/events/session", "POST", "bearer", 5};
        constexpr std::string_view stream_template = "/v1/markets/events/{session_id}";
        
        struct stream {
            static constexpr std::string_view method = "GET";
            static constexpr std::string_view auth_type = "bearer";
            static constexpr int rate_limit = 1;
            static std::string path(const std::string& session_id) {
                std::string validated_session_id = PathValidator::validate_session_id(session_id);
                return "/v1/markets/events/" + validated_session_id;
            }
        };
    };
}

namespace beta {
    namespace fundamentals {
        constexpr EndpointConfig company{"/beta/markets/fundamentals/company", "GET", "bearer", 30};
        constexpr EndpointConfig ratios{"/beta/markets/fundamentals/ratios", "GET", "bearer", 30};
        constexpr EndpointConfig financials{"/beta/markets/fundamentals/financials", "GET", "bearer", 30};
        constexpr EndpointConfig price_stats{"/beta/markets/fundamentals/price_stats", "GET", "bearer", 30};
        constexpr EndpointConfig corporate_calendar{"/beta/markets/fundamentals/corporate_calendar", "GET", "bearer", 30};
        constexpr EndpointConfig dividend{"/beta/markets/fundamentals/dividend", "GET", "bearer", 30};
    };
}

namespace websocket {
    namespace base_urls {
        constexpr std::string_view production = "wss://ws.tradier.com";
        constexpr std::string_view sandbox = "wss://sandbox.tradier.com";
    }
    
    constexpr std::string_view markets = "/v1/markets/events";
    constexpr std::string_view accounts = "/v1/accounts/events";
}

class EndpointBuilder {
public:
    template<typename T>
    static constexpr auto get_config() -> decltype(T{}) {
        return T{};
    }
    
    static std::string build_path(std::string_view template_path, 
                                 const std::unordered_map<std::string, std::string>& params) {
        std::string result{template_path};
        for (const auto& [key, value] : params) {
            std::string placeholder = "{" + key + "}";
            size_t pos = result.find(placeholder);
            if (pos != std::string::npos) {
                result.replace(pos, placeholder.length(), value);
            }
        }
        return result;
    }
};

namespace registry {
    constexpr std::array all_endpoints = {
        &authentication::oauth_authorize,
        &authentication::oauth_accesstoken,
        &user::profile,
        &markets::quotes,
        &markets::clock,
        &markets::search,
        &markets::lookup,
        &markets::history,
        &markets::options::chains,
        &markets::options::expirations,
        &markets::options::strikes,
        &markets::events::session,
        &beta::fundamentals::company,
        &beta::fundamentals::ratios,
        &beta::fundamentals::financials,
        &beta::fundamentals::price_stats,
        &beta::fundamentals::corporate_calendar,
        &beta::fundamentals::dividend
    };
}

} // namespace oqd::endpoints