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

#include "oqdTradierpp/client.hpp"
#include <sstream>
#include <regex>
#include <boost/url/url.hpp>
#include <boost/url/params_encoded_view.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace oqd {

TradierClient::TradierClient(Environment env) 
    : environment_(env)
    , io_context_(std::make_unique<boost::asio::io_context>())
    , ssl_context_(std::make_unique<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client))
{
    update_base_url();
    initialize_ssl_context();
}

void TradierClient::set_access_token(const std::string& token) {
    access_token_ = token;
}

void TradierClient::set_client_credentials(const std::string& client_id, const std::string& client_secret) {
    client_id_ = client_id;
    client_secret_ = client_secret;
}

void TradierClient::set_environment(Environment env) {
    environment_ = env;
    update_base_url();
}

void TradierClient::update_base_url() {
    switch (environment_) {
        case Environment::Production:
            base_url_ = std::string(endpoints::base_urls::production);
            websocket_url_ = std::string(endpoints::websocket::base_urls::production);
            break;
        case Environment::Sandbox:
            base_url_ = std::string(endpoints::base_urls::sandbox);
            websocket_url_ = std::string(endpoints::websocket::base_urls::sandbox);
            break;
    }
}

void TradierClient::initialize_ssl_context() {
    ssl_context_->set_default_verify_paths();
    ssl_context_->set_verify_mode(boost::asio::ssl::verify_peer);
    ssl_context_->set_verify_callback(
        [](bool /*preverified*/
, boost::asio::ssl::verify_context&) {
            return true;
        });
}

std::future<simdjson::dom::element> TradierClient::get_async(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    auto url = build_url(endpoint, params);
    auto request = create_request(boost::beast::http::verb::get, url, "", AuthType::Bearer, options);
    
    return std::async(std::launch::async, [this, request = std::move(request)]() mutable {
        auto response = perform_request(std::move(request));
        auto json_doc = json_parser_.parse(response.body());
        if (json_doc.error() != simdjson::SUCCESS) {
            throw ApiException("Failed to parse JSON response");
        }
        return json_doc.value();
    });
}

std::future<simdjson::dom::element> TradierClient::post_async(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    auto url = base_url_ + endpoint;
    auto body = build_form_data(params);
    auto request = create_request(boost::beast::http::verb::post, url, body, AuthType::Bearer, options);
    
    return std::async(std::launch::async, [this, request = std::move(request)]() mutable {
        auto response = perform_request(std::move(request));
        auto json_doc = json_parser_.parse(response.body());
        if (json_doc.error() != simdjson::SUCCESS) {
            throw ApiException("Failed to parse JSON response");
        }
        return json_doc.value();
    });
}

std::future<simdjson::dom::element> TradierClient::put_async(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    auto url = base_url_ + endpoint;
    auto body = build_form_data(params);
    auto request = create_request(boost::beast::http::verb::put, url, body, AuthType::Bearer, options);
    
    return std::async(std::launch::async, [this, request = std::move(request)]() mutable {
        auto response = perform_request(std::move(request));
        auto json_doc = json_parser_.parse(response.body());
        if (json_doc.error() != simdjson::SUCCESS) {
            throw ApiException("Failed to parse JSON response");
        }
        return json_doc.value();
    });
}

std::future<simdjson::dom::element> TradierClient::delete_async(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    auto url = build_url(endpoint, params);
    auto request = create_request(boost::beast::http::verb::delete_, url, "", AuthType::Bearer, options);
    
    return std::async(std::launch::async, [this, request = std::move(request)]() mutable {
        auto response = perform_request(std::move(request));
        auto json_doc = json_parser_.parse(response.body());
        if (json_doc.error() != simdjson::SUCCESS) {
            throw ApiException("Failed to parse JSON response");
        }
        return json_doc.value();
    });
}

simdjson::dom::element TradierClient::get(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    return get_async(endpoint, params, options).get();
}

simdjson::dom::element TradierClient::post(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    return post_async(endpoint, params, options).get();
}

simdjson::dom::element TradierClient::put(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    return put_async(endpoint, params, options).get();
}

simdjson::dom::element TradierClient::delete_request(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params,
    const RequestOptions& options) {
    
    return delete_async(endpoint, params, options).get();
}

std::string TradierClient::build_url(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params) const {
    
    std::string full_url = base_url_ + endpoint;
    
    if (!params.empty()) {
        std::string query = utils::build_query_string(params);
        full_url += "?" + query;
    }
    
    return full_url;
}

std::string TradierClient::build_form_data(
    const std::unordered_map<std::string, std::string>& params) const {
    
    return utils::build_form_data(params);
}

boost::beast::http::request<boost::beast::http::string_body> 
TradierClient::create_request(boost::beast::http::verb method,
                             const std::string& target,
                             const std::string& body,
                             AuthType auth_type,
                             const RequestOptions& options) const {
    
    boost::beast::http::request<boost::beast::http::string_body> req{method, target, 11};
    
    std::string request_target = target;
    if (target.find(base_url_) == 0) {
        boost::url full_url(target);
        request_target = std::string(full_url.path());
        if (!full_url.query().empty()) {
            request_target += "?" + std::string(full_url.query());
        }
    }
    
    req.target(request_target);
    
    boost::url base_url_obj(base_url_);
    req.set(boost::beast::http::field::host, base_url_obj.host());
    req.set(boost::beast::http::field::user_agent, "liboqdTradierpp/2.0.0");
    req.set(boost::beast::http::field::accept, "application/json");
    
    switch (auth_type) {
        case AuthType::Bearer:
            if (!access_token_.empty()) {
                req.set(boost::beast::http::field::authorization, "Bearer " + access_token_);
            }
            break;
        case AuthType::Basic:
            if (!client_id_.empty() && !client_secret_.empty()) {
                std::string credentials = utils::create_basic_auth(client_id_, client_secret_);
                req.set(boost::beast::http::field::authorization, "Basic " + credentials);
            }
            break;
        case AuthType::None:
            break;
    }
    
    if (method == boost::beast::http::verb::post || method == boost::beast::http::verb::put) {
        req.set(boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
    }
    
    for (const auto& [key, value] : options.headers) {
        req.set(key, value);
    }
    
    if (!body.empty()) {
        req.body() = body;
    }
    
    req.prepare_payload();
    return req;
}

boost::beast::http::response<boost::beast::http::string_body>
TradierClient::perform_request(boost::beast::http::request<boost::beast::http::string_body> request) {
    
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;
    using ssl_stream = net::ssl::stream<tcp::socket>;
    
    boost::url base_url(base_url_);
    std::string host = std::string(base_url.host());
    std::string port = base_url.port().empty() ? "443" : std::string(base_url.port());
    
    try {
        tcp::resolver resolver(*io_context_);
        beast::error_code ec;
        auto const results = resolver.resolve(host, port, ec);
        if (ec) {
            throw ApiException("DNS resolution failed for " + host + ":" + port + " - " + ec.message());
        }
        
        ssl_stream stream(*io_context_, *ssl_context_);
        
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            beast::error_code ssl_ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw ApiException("SSL SNI setup failed: " + ssl_ec.message());
        }
        
        net::connect(beast::get_lowest_layer(stream), results, ec);
        if (ec) {
            throw ApiException("TCP connection failed to " + host + ":" + port + " - " + ec.message());
        }
        
        stream.handshake(ssl_stream::client, ec);
        if (ec) {
            throw ApiException("SSL handshake failed: " + ec.message());
        }
        
        http::write(stream, request, ec);
        if (ec) {
            throw ApiException("HTTP write failed: " + ec.message());
        }
        
        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(stream, buffer, response, ec);
        if (ec) {
            throw ApiException("HTTP read failed: " + ec.message());
        }
        
        if (response.result_int() >= 400) {
            throw ApiException("HTTP error: " + std::to_string(response.result_int()) + " " + response.body());
        }
        
        update_rate_limit("default", response);
        
        beast::error_code close_ec;
        stream.shutdown(close_ec);
        
        return response;
        
    } catch (const ApiException&) {
        throw;
    } catch (const std::exception& e) {
        throw ApiException("Request failed: " + std::string(e.what()));
    }
}

void TradierClient::check_rate_limit(const std::string& endpoint_group) const {
    auto it = rate_limits_.find(endpoint_group);
    if (it != rate_limits_.end()) {
        const auto& limit = it->second;
        auto now = std::chrono::steady_clock::now();
        
        if (now < limit.expiry && limit.available <= 0) {
            throw RateLimitException("Rate limit exceeded for " + endpoint_group);
        }
    }
}

bool TradierClient::is_rate_limited(const std::string& endpoint_group) const {
    auto it = rate_limits_.find(endpoint_group);
    if (it != rate_limits_.end()) {
        const auto& limit = it->second;
        auto now = std::chrono::steady_clock::now();
        return now < limit.expiry && limit.available <= 0;
    }
    return false;
}

std::optional<RateLimit> TradierClient::get_rate_limit(const std::string& endpoint_group) const {
    auto it = rate_limits_.find(endpoint_group);
    if (it != rate_limits_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void TradierClient::update_rate_limit(
    const std::string& endpoint_group,
    const boost::beast::http::response<boost::beast::http::string_body>& response) {
    
    auto available_header = response.find("X-Ratelimit-Available");
    auto used_header = response.find("X-Ratelimit-Used");
    auto expiry_header = response.find("X-Ratelimit-Expiry");
    
    if (available_header != response.end() && 
        used_header != response.end() && 
        expiry_header != response.end()) {
        
        RateLimit limit;
        limit.available = std::stoi(std::string(available_header->value()));
        limit.used = std::stoi(std::string(used_header->value()));
        
        auto expiry_timestamp = std::stoll(std::string(expiry_header->value()));
        limit.expiry = std::chrono::steady_clock::now() + 
                      std::chrono::seconds(expiry_timestamp - std::time(nullptr));
        
        rate_limits_[endpoint_group] = limit;
    }
}

} // namespace oqd