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
#include <memory>
#include <optional>
#include <future>
#include <concepts>
#include <ranges>
#include <unordered_map>
#include <chrono>
#include <functional>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/url.hpp>
#include <simdjson.h>
#include "endpoints.hpp"
#include "utils.hpp"

namespace oqd {

enum class Environment {
    Production,
    Sandbox
};

enum class AuthType {
    None,
    Basic,
    Bearer
};

class RateLimitException : public std::runtime_error {
public:
    explicit RateLimitException(const std::string& msg) : std::runtime_error(msg) {}
};

class ApiException : public std::runtime_error {
public:
    explicit ApiException(const std::string& msg) : std::runtime_error(msg) {}
};

template<typename T>
concept Serializable = requires(T t) {
    { t.to_json() } -> std::convertible_to<std::string>;
};

template<typename T>
concept Deserializable = requires(T t, const simdjson::dom::element& elem) {
    { T::from_json(elem) } -> std::same_as<T>;
};

struct RequestOptions {
    std::optional<std::chrono::milliseconds> timeout = std::chrono::milliseconds(30000);
    std::unordered_map<std::string, std::string> headers;
    bool follow_redirects = true;
    int max_redirects = 5;
};

struct RateLimit {
    int available;
    int used;
    std::chrono::time_point<std::chrono::steady_clock> expiry;
};

class TradierClient {
public:
    explicit TradierClient(Environment env = Environment::Production);
    ~TradierClient() = default;

    TradierClient(const TradierClient&) = delete;
    TradierClient& operator=(const TradierClient&) = delete;
    TradierClient(TradierClient&&) = default;
    TradierClient& operator=(TradierClient&&) = default;

    void set_access_token(const std::string& token);
    void set_client_credentials(const std::string& client_id, const std::string& client_secret);
    void set_environment(Environment env);
    
    const std::string& get_access_token() const { return access_token_; }

    std::future<simdjson::dom::element> get_async(const std::string& endpoint, 
                                                  const std::unordered_map<std::string, std::string>& params = {},
                                                  const RequestOptions& options = {});

    std::future<simdjson::dom::element> post_async(const std::string& endpoint,
                                                   const std::unordered_map<std::string, std::string>& params = {},
                                                   const RequestOptions& options = {});

    std::future<simdjson::dom::element> put_async(const std::string& endpoint,
                                                  const std::unordered_map<std::string, std::string>& params = {},
                                                  const RequestOptions& options = {});

    std::future<simdjson::dom::element> delete_async(const std::string& endpoint,
                                                     const std::unordered_map<std::string, std::string>& params = {},
                                                     const RequestOptions& options = {});

    simdjson::dom::element get(const std::string& endpoint,
                               const std::unordered_map<std::string, std::string>& params = {},
                               const RequestOptions& options = {});

    simdjson::dom::element post(const std::string& endpoint,
                                const std::unordered_map<std::string, std::string>& params = {},
                                const RequestOptions& options = {});

    simdjson::dom::element put(const std::string& endpoint,
                               const std::unordered_map<std::string, std::string>& params = {},
                               const RequestOptions& options = {});

    simdjson::dom::element delete_request(const std::string& endpoint,
                                          const std::unordered_map<std::string, std::string>& params = {},
                                          const RequestOptions& options = {});

    std::optional<RateLimit> get_rate_limit(const std::string& endpoint_group) const;
    
    bool is_rate_limited(const std::string& endpoint_group) const;

    const std::string& get_base_url() const { return base_url_; }
    
    template<typename Endpoint>
    std::future<simdjson::dom::element> get_endpoint_async(const Endpoint& endpoint,
                                                          const std::unordered_map<std::string, std::string>& params = {},
                                                          const RequestOptions& options = {}) {
        static_assert(std::is_same_v<std::string_view, decltype(endpoint.path)>,
                      "Endpoint must have constexpr path field");
        std::string endpoint_group = std::string(endpoint.path);
        check_rate_limit(endpoint_group);
        return get_async(std::string(endpoint.path), params, options);
    }
    
    template<typename Endpoint>
    std::future<simdjson::dom::element> post_endpoint_async(const Endpoint& endpoint,
                                                           const std::unordered_map<std::string, std::string>& params = {},
                                                           const RequestOptions& options = {}) {
        static_assert(std::is_same_v<std::string_view, decltype(endpoint.path)>,
                      "Endpoint must have constexpr path field");
        std::string endpoint_group = std::string(endpoint.path);
        check_rate_limit(endpoint_group);
        return post_async(std::string(endpoint.path), params, options);
    }

private:
    Environment environment_;
    std::string base_url_;
    std::string websocket_url_;
    std::string access_token_;
    std::string client_id_;
    std::string client_secret_;
    
    mutable std::unordered_map<std::string, RateLimit> rate_limits_;
    
    std::unique_ptr<boost::asio::io_context> io_context_;
    std::unique_ptr<boost::asio::ssl::context> ssl_context_;
    simdjson::dom::parser json_parser_;

    void initialize_ssl_context();
    void update_base_url();
    void check_rate_limit(const std::string& endpoint_group) const;
    void update_rate_limit(const std::string& endpoint_group, 
                          const boost::beast::http::response<boost::beast::http::string_body>& response);
    
    std::string build_url(const std::string& endpoint, 
                         const std::unordered_map<std::string, std::string>& params) const;
    
    std::string build_form_data(const std::unordered_map<std::string, std::string>& params) const;
    
    boost::beast::http::request<boost::beast::http::string_body> 
    create_request(boost::beast::http::verb method,
                   const std::string& target,
                   const std::string& body,
                   AuthType auth_type,
                   const RequestOptions& options) const;

    std::future<boost::beast::http::response<boost::beast::http::string_body>>
    perform_request_async(boost::beast::http::request<boost::beast::http::string_body> request);

    boost::beast::http::response<boost::beast::http::string_body>
    perform_request(boost::beast::http::request<boost::beast::http::string_body> request);
};

} // namespace oqd