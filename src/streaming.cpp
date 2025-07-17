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

#include "oqdTradierpp/streaming.hpp"
#include "oqdTradierpp/utils.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <sstream>
#include <iomanip>
#include <random>
#include <atomic>
#include <mutex>
#include <future>

namespace oqd {

StreamingSession::StreamingSession(std::shared_ptr<TradierClient> client) 
    : client_(std::move(client)) {
    setup_websocket_handlers();
    tls_context_ = create_tls_context();
}

StreamingSession::~StreamingSession() {
    stop_stream();
}

void StreamingSession::stop_stream() {
    update_connection_state(ConnectionState::Closed);
    should_reconnect_ = false;
    
    reconnect_cv_.notify_all();
    
    if (ws_connection_ && ws_connection_->get_state() == websocketpp::session::state::open) {
        try {
            ws_connection_->close(websocketpp::close::status::normal, "User requested close");
        } catch (...) {
            // Ignore close errors
        }
    }
    
    if (ws_client_ && ws_client_->is_listening()) {
        ws_client_->stop();
    }
    
    if (streaming_thread_.joinable()) {
        streaming_thread_.join();
    }
    
    update_connection_state(ConnectionState::Disconnected);
}

void StreamingSession::update_connection_state(ConnectionState state) {
    ConnectionState prev_state = connection_state_.exchange(state);
    if (prev_state != state && error_callback_) {
        std::string state_str;
        switch (state) {
            case ConnectionState::Disconnected: state_str = "Disconnected"; break;
            case ConnectionState::Connecting: state_str = "Connecting"; break;
            case ConnectionState::Connected: state_str = "Connected"; break;
            case ConnectionState::Reconnecting: state_str = "Reconnecting"; break;
            case ConnectionState::Error: state_str = "Error"; break;
            case ConnectionState::Closed: state_str = "Closed"; break;
        }
        error_callback_("Connection state changed to: " + state_str);
    }
}

std::shared_ptr<boost::asio::ssl::context> StreamingSession::create_tls_context() {
    auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client);
    ctx->set_default_verify_paths();
    ctx->set_verify_mode(boost::asio::ssl::verify_peer);
    return ctx;
}

void StreamingSession::setup_websocket_handlers() {
    ws_client_ = std::make_unique<WebSocketClient>();
    ws_client_->set_access_channels(websocketpp::log::alevel::all);
    ws_client_->clear_access_channels(websocketpp::log::alevel::frame_payload);
    ws_client_->set_error_channels(websocketpp::log::elevel::all);
    ws_client_->init_asio();

    ws_client_->set_tls_init_handler([this](websocketpp::connection_hdl) {
        return tls_context_;
    });
    
    ws_client_->set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg) {
        if (data_callback_) {
            process_streaming_data(msg->get_payload());
        }
    });
    
    ws_client_->set_open_handler([this](websocketpp::connection_hdl hdl) {
        update_connection_state(ConnectionState::Connected);
        reconnect_attempts_ = 0; 
        
        // Send initial subscription message
        try {
            std::ostringstream message;
            message << R"({"sessionid":")" << session_id_ << R"(")";
            
            {
                std::lock_guard<std::mutex> lock(symbols_mutex_);
                if (!current_symbols_.empty()) {
                    message << R"(,"symbols":[)";
                    for (size_t i = 0; i < current_symbols_.size(); ++i) {
                        if (i > 0) message << ",";
                        message << "\"" << current_symbols_[i] << "\"";
                    }
                    message << "]";
                }
            }
            
            message << "}";
            
            ws_client_->send(hdl, message.str(), websocketpp::frame::opcode::text);
        } catch (const std::exception& e) {
            if (error_callback_) {
                error_callback_("Failed to send initial subscription: " + std::string(e.what()));
            }
        }
    });
    
    ws_client_->set_close_handler([this](websocketpp::connection_hdl) {
        if (connection_state_ != ConnectionState::Closed && should_reconnect_) {
            update_connection_state(ConnectionState::Reconnecting);
            handle_reconnection();
        } else {
            update_connection_state(ConnectionState::Disconnected);
        }
    });
    
    ws_client_->set_fail_handler([this](websocketpp::connection_hdl) {
        if (connection_state_ != ConnectionState::Closed && should_reconnect_) {
            update_connection_state(ConnectionState::Reconnecting);
            handle_reconnection();
        } else {
            update_connection_state(ConnectionState::Error);
        }
    });
}

void StreamingSession::handle_reconnection() {
    if (!should_reconnect_ || reconnect_attempts_ >= max_reconnect_attempts_) {
        update_connection_state(ConnectionState::Error);
        if (error_callback_) {
            error_callback_("Maximum reconnection attempts reached");
        }
        return;
    }
    
    reconnect_attempts_++;
    
    auto delay = base_reconnect_delay_ * (1 << (reconnect_attempts_ - 1));
    auto jitter = delay / 4;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-jitter.count(), jitter.count());
    delay += std::chrono::milliseconds(dis(gen));
    
    if (delay > std::chrono::seconds(30)) {
        delay = std::chrono::seconds(30);
    }
    
    if (error_callback_) {
        error_callback_("Reconnection attempt #" + std::to_string(reconnect_attempts_.load()) + 
                       " in " + std::to_string(delay.count()) + "ms");
    }
    
    std::unique_lock<std::mutex> lock(reconnect_mutex_);
    if (reconnect_cv_.wait_for(lock, delay, [this] { return !should_reconnect_; })) {
        // Reconnection cancelled
        return;
    }
    
    refresh_session_if_needed();

    {
        std::lock_guard<std::mutex> params_lock(connection_params_mutex_);
        if (connection_params_.is_websocket) {
            websocket_stream_worker(connection_params_.endpoint, connection_params_.params);
        } else {
            http_stream_worker(connection_params_.endpoint, connection_params_.params);
        }
    }
}

void StreamingSession::refresh_session_if_needed() {
    if (is_session_expired()) {
        try {
            std::lock_guard<std::mutex> params_lock(connection_params_mutex_);
            if (connection_params_.endpoint.find("/markets/") != std::string::npos) {
                session_id_ = create_market_session();
            } else if (connection_params_.endpoint.find("/accounts/") != std::string::npos) {
                session_id_ = create_account_session();
            }
        } catch (const std::exception& e) {
            if (error_callback_) {
                error_callback_("Failed to refresh session: " + std::string(e.what()));
            }
        }
    }
}

std::future<void> StreamingSession::start_market_websocket_stream_async(
    const std::vector<std::string>& symbols,
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    return std::async(std::launch::async, [this, symbols, on_data, on_error]() {
        start_market_websocket_stream(symbols, on_data, on_error);
    });
}

void StreamingSession::start_market_websocket_stream(
    const std::vector<std::string>& symbols,
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    data_callback_ = on_data;
    error_callback_ = on_error ? on_error : [](const std::string&) {};
    
    {
        std::lock_guard<std::mutex> lock(symbols_mutex_);
        current_symbols_ = symbols;
    }
    
    try {
        update_connection_state(ConnectionState::Connecting);
        session_id_ = create_market_session();
        std::string endpoint = "/v1/markets/events";
        std::unordered_map<std::string, std::string> params;
        if (!symbols.empty()) {
            params["symbols"] = join_symbols(symbols);
        }
        
        {
            std::lock_guard<std::mutex> lock(connection_params_mutex_);
            connection_params_ = {endpoint, params, true};
        }
        
        streaming_thread_ = std::thread([this, endpoint, params]() {
            websocket_stream_worker(endpoint, params);
        });
        
    } catch (const std::exception& e) {
        update_connection_state(ConnectionState::Error);
        error_callback_("Failed to start WebSocket stream: " + std::string(e.what()));
    }
}

std::future<void> StreamingSession::start_account_websocket_stream_async(
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    return std::async(std::launch::async, [this, on_data, on_error]() {
        start_account_websocket_stream(on_data, on_error);
    });
}

void StreamingSession::start_account_websocket_stream(
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    data_callback_ = on_data;
    error_callback_ = on_error ? on_error : [](const std::string&) {};
    
    try {
        update_connection_state(ConnectionState::Connecting);
        session_id_ = create_account_session();
        std::string endpoint = "/v1/accounts/events";
        std::unordered_map<std::string, std::string> params;
        
        {
            std::lock_guard<std::mutex> lock(connection_params_mutex_);
            connection_params_ = {endpoint, params, true};
        }
        
        streaming_thread_ = std::thread([this, endpoint, params]() {
            websocket_stream_worker(endpoint, params);
        });
        
    } catch (const std::exception& e) {
        update_connection_state(ConnectionState::Error);
        error_callback_("Failed to start account WebSocket stream: " + std::string(e.what()));
    }
}

void StreamingSession::websocket_stream_worker(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& /*params*/
) {
    
    try {
        std::string uri = build_websocket_url(endpoint);
        
        websocketpp::lib::error_code ec;
        auto con = ws_client_->get_connection(uri, ec);
        
        if (ec) {
            throw std::runtime_error("Failed to create connection: " + ec.message());
        }
        
        con->append_header("Authorization", "Bearer " + client_->get_access_token());
        ws_connection_ = con;
        ws_client_->connect(con);
        ws_client_->run();
        
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("WebSocket error: " + std::string(e.what()));
        }
        
        if (should_reconnect_ && connection_state_ != ConnectionState::Closed) {
            update_connection_state(ConnectionState::Reconnecting);
            handle_reconnection();
        }
    }
}

void StreamingSession::add_symbols(const std::vector<std::string>& symbols) {
    if (!ws_connection_ || symbols.empty()) {
        return;
    }
    
    try {
        {
            std::lock_guard<std::mutex> lock(symbols_mutex_);
            for (const auto& symbol : symbols) {
                if (std::find(current_symbols_.begin(), current_symbols_.end(), symbol) == current_symbols_.end()) {
                    current_symbols_.push_back(symbol);
                }
            }
        }
        
        std::ostringstream message;
        message << R"({"action":"subscribe","symbols":[)";
        
        for (size_t i = 0; i < symbols.size(); ++i) {
            if (i > 0) message << ",";
            message << "\"" << symbols[i] << "\"";
        }
        
        message << "]}";
        ws_client_->send(ws_connection_->get_handle(), message.str(), websocketpp::frame::opcode::text);
        
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("Error adding symbols: " + std::string(e.what()));
        }
    }
}

void StreamingSession::remove_symbols(const std::vector<std::string>& symbols) {
    if (!ws_connection_ || symbols.empty()) {
        return;
    }
    
    try {
        {
            std::lock_guard<std::mutex> lock(symbols_mutex_);
            for (const auto& symbol : symbols) {
                auto it = std::find(current_symbols_.begin(), current_symbols_.end(), symbol);
                if (it != current_symbols_.end()) {
                    current_symbols_.erase(it);
                }
            }
        }
        
        std::ostringstream message;
        message << R"({"action":"unsubscribe","symbols":[)";
        
        for (size_t i = 0; i < symbols.size(); ++i) {
            if (i > 0) message << ",";
            message << "\"" << symbols[i] << "\"";
        }
        
        message << "]}";
        ws_client_->send(ws_connection_->get_handle(), message.str(), websocketpp::frame::opcode::text);
        
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("Error removing symbols: " + std::string(e.what()));
        }
    }
}

void StreamingSession::set_data_filter(const std::vector<StreamingDataType>& types) {
    std::lock_guard<std::mutex> lock(filter_mutex_);
    data_filter_ = types;
    has_filter_ = true;
}

void StreamingSession::clear_data_filter() {
    std::lock_guard<std::mutex> lock(filter_mutex_);
    data_filter_.clear();
    has_filter_ = false;
}

bool StreamingSession::should_process_data(StreamingDataType type) const {
    if (!has_filter_) {
        return true;
    }
    
    std::lock_guard<std::mutex> lock(filter_mutex_);
    return std::find(data_filter_.begin(), data_filter_.end(), type) != data_filter_.end();
}

std::string StreamingSession::create_market_session() {
    try {
        auto response = client_->post_async("/v1/markets/events/session", {}).get();
        
        auto stream_result = response["stream"];
        if (stream_result.error() == simdjson::SUCCESS) {
            auto stream_obj = stream_result.value();
            auto sessionid_result = stream_obj["sessionid"];
            if (sessionid_result.error() == simdjson::SUCCESS) {
                session_create_time_ = std::chrono::steady_clock::now();
                return std::string(sessionid_result.value().get_string().value());
            }
        }
        
        throw std::runtime_error("Failed to extract session ID from market session response");
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create market session: " + std::string(e.what()));
    }
}

std::string StreamingSession::create_account_session() {
    try {
        auto response = client_->post_async("/v1/accounts/events/session", {}).get();
        
        auto stream_result = response["stream"];
        if (stream_result.error() == simdjson::SUCCESS) {
            auto stream_obj = stream_result.value();
            auto sessionid_result = stream_obj["sessionid"];
            if (sessionid_result.error() == simdjson::SUCCESS) {
                session_create_time_ = std::chrono::steady_clock::now();
                return std::string(sessionid_result.value().get_string().value());
            }
        }
        
        throw std::runtime_error("Failed to extract session ID from account session response");
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create account session: " + std::string(e.what()));
    }
}

bool StreamingSession::is_session_expired() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - session_create_time_);
    return elapsed.count() >= 5;
}

std::future<void> StreamingSession::start_market_http_stream_async(
    const std::vector<std::string>& symbols,
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    return std::async(std::launch::async, [this, symbols, on_data, on_error]() {
        start_market_http_stream(symbols, on_data, on_error);
    });
}

void StreamingSession::start_market_http_stream(
    const std::vector<std::string>& symbols,
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    data_callback_ = on_data;
    error_callback_ = on_error ? on_error : [](const std::string&) {};
    
    try {
        update_connection_state(ConnectionState::Connecting);
        
        session_id_ = create_market_session();
        std::string endpoint = "/v1/markets/events/" + session_id_;
        
        std::unordered_map<std::string, std::string> params;
        if (!symbols.empty()) {
            params["symbols"] = join_symbols(symbols);
        }
        
        // Store connection params
        {
            std::lock_guard<std::mutex> lock(connection_params_mutex_);
            connection_params_ = {endpoint, params, false};
        }
        
        streaming_thread_ = std::thread([this, endpoint, params]() {
            http_stream_worker(endpoint, params);
        });
        
    } catch (const std::exception& e) {
        update_connection_state(ConnectionState::Error);
        error_callback_("Failed to start HTTP stream: " + std::string(e.what()));
    }
}

std::future<void> StreamingSession::start_account_http_stream_async(
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    return std::async(std::launch::async, [this, on_data, on_error]() {
        start_account_http_stream(on_data, on_error);
    });
}

void StreamingSession::start_account_http_stream(
    StreamingCallback on_data,
    ErrorCallback on_error) {
    
    data_callback_ = on_data;
    error_callback_ = on_error ? on_error : [](const std::string&) {};
    
    try {
        update_connection_state(ConnectionState::Connecting);
        
        session_id_ = create_account_session();
        std::string endpoint = "/v1/accounts/events/" + session_id_;
        
        std::unordered_map<std::string, std::string> params;
        
        {
            std::lock_guard<std::mutex> lock(connection_params_mutex_);
            connection_params_ = {endpoint, params, false};
        }
        
        streaming_thread_ = std::thread([this, endpoint, params]() {
            http_stream_worker(endpoint, params);
        });
        
    } catch (const std::exception& e) {
        update_connection_state(ConnectionState::Error);
        error_callback_("Failed to start account HTTP stream: " + std::string(e.what()));
    }
}

void StreamingSession::http_stream_worker(
    const std::string& endpoint,
    const std::unordered_map<std::string, std::string>& params) {
    
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;
    
    try {
        net::io_context ioc;
        ssl::context ctx(ssl::context::tlsv12_client);
        ctx.set_default_verify_paths();
        
        boost::url base_url(client_->get_base_url());
        std::string host = std::string(base_url.host());
        std::string port = base_url.port().empty() ? "443" : std::string(base_url.port());
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
        
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw beast::system_error{ec};
        }
        
        beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));
        
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        
        stream.handshake(ssl::stream_base::client);
        
        update_connection_state(ConnectionState::Connected);
        reconnect_attempts_ = 0;
        
        std::string request_target = endpoint;
        if (!params.empty()) {
            request_target += "?";
            bool first = true;
            for (const auto& [key, value] : params) {
                if (!first) request_target += "&";
                request_target += key + "=" + utils::url_encode(value);
                first = false;
            }
        }
        
        http::request<http::string_body> req{http::verb::get, request_target, 11};
        req.set(http::field::host, host);
        req.set(http::field::authorization, "Bearer " + client_->get_access_token());
        req.set(http::field::accept, "text/event-stream");
        req.set(http::field::cache_control, "no-cache");
        req.set(http::field::connection, "keep-alive");
        
        http::write(stream, req);
        
        beast::flat_buffer buffer;
        http::response_parser<http::string_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        
        http::read_header(stream, buffer, parser);
        
        if (parser.get().result() != http::status::ok) {
            throw std::runtime_error("HTTP streaming failed with status: " + 
                                   std::to_string(static_cast<unsigned>(parser.get().result())));
        }
        
        std::string line_buffer;
        std::string event_type = "message";
        std::string event_data;
        std::string event_id;
        
        while (connection_state_ == ConnectionState::Connected && should_reconnect_) {
            beast::error_code ec;
            http::read_some(stream, buffer, parser, ec);
            
            if (ec == http::error::end_of_stream) {
                break;
            } else if (ec) {
                throw beast::system_error{ec};
            }
            
            std::string chunk = parser.get().body();
            parser.get().body().clear();
            line_buffer += chunk;
            
            size_t pos = 0;
            while ((pos = line_buffer.find('\n')) != std::string::npos) {
                std::string line = line_buffer.substr(0, pos);
                line_buffer.erase(0, pos + 1);
                
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                
                if (line.empty()) {
                    if (!event_data.empty()) {
                        process_sse_event(event_type, event_data);
                        event_data.clear();
                        event_type = "message";
                    }
                    continue;
                }
                
                size_t colon_pos = line.find(':');
                if (colon_pos != std::string::npos) {
                    std::string field = line.substr(0, colon_pos);
                    std::string value = line.substr(colon_pos + 1);
                    
                    if (!value.empty() && value[0] == ' ') {
                        value.erase(0, 1);
                    }
                    
                    if (field == "event") {
                        event_type = value;
                    } else if (field == "data") {
                        if (!event_data.empty()) {
                            event_data += "\n";
                        }
                        event_data += value;
                    } else if (field == "id") {
                        event_id = value;
                    } else if (field == "retry") {
                        // Retry interval is parsed but not currently used
                        // Could be used in future for retry logic
                    }
                } else if (!line.empty() && line[0] == ':') {
                    // Comment line, ignore
                }
            }
        }
        
        beast::error_code ec;
        stream.shutdown(ec);
        
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("HTTP stream error: " + std::string(e.what()));
        }
        
        if (should_reconnect_ && connection_state_ != ConnectionState::Closed) {
            update_connection_state(ConnectionState::Reconnecting);
            handle_reconnection();
        }
    }
}

void StreamingSession::process_streaming_data(const std::string& data) {
    try {
        simdjson::dom::parser parser;
        auto doc = parser.parse(data);
        
        if (doc.error() == simdjson::SUCCESS) {
            auto element = doc.value();
            
            // Determine data type and apply filtering
            StreamingDataType data_type = determine_data_type(element);
            if (!should_process_data(data_type)) {
                return;
            }
            
            if (data_callback_) {
                data_callback_(element);
            }
        }
    } catch (const std::exception& e) {
        if (error_callback_) {
            error_callback_("Error processing streaming data: " + std::string(e.what()));
        }
    }
}

void StreamingSession::process_sse_event(const std::string& event_type, const std::string& event_data) {
    if (event_data.empty()) {
        return;
    }
    
    if (event_type == "heartbeat") {
        return;
    } else if (event_type == "session") {
        try {
            simdjson::dom::parser parser;
            auto doc = parser.parse(event_data);
            if (doc.error() == simdjson::SUCCESS) {
                auto elem = doc.value();
                auto session_result = elem["sessionid"];
                if (session_result.error() == simdjson::SUCCESS) {
                    session_id_ = std::string(session_result.value().get_string().value());
                    session_create_time_ = std::chrono::steady_clock::now();
                }
            }
        } catch (...) {
            // Ignore parsing errors for session events
        }
        return;
    }
    
    process_streaming_data(event_data);
}

StreamingDataType StreamingSession::determine_data_type(const simdjson::dom::element& data) const {
    return StreamingSession::determine_data_type_static(data);
}

StreamingDataType StreamingSession::determine_data_type_static(const simdjson::dom::element& data) {
    auto type_result = data["type"];
    if (type_result.error() == simdjson::SUCCESS) {
        std::string type = std::string(type_result.value().get_string().value());
        
        if (type == "quote") return StreamingDataType::Quote;
        if (type == "trade") return StreamingDataType::Trade;
        if (type == "summary") return StreamingDataType::Summary;
        if (type == "timesale") return StreamingDataType::TimeSale;
        if (type == "tradex") return StreamingDataType::TradEx;
        if (type == "order") return StreamingDataType::OrderStatus;
        if (type == "journal" || type == "fill") return StreamingDataType::AccountActivity;
    }
    
    if (data["bid"].error() == simdjson::SUCCESS) return StreamingDataType::Quote;
    if (data["price"].error() == simdjson::SUCCESS && data["size"].error() == simdjson::SUCCESS) return StreamingDataType::Trade;
    if (data["order_id"].error() == simdjson::SUCCESS) return StreamingDataType::OrderStatus;
    
    return StreamingDataType::Quote; 
}

std::string StreamingSession::join_symbols(const std::vector<std::string>& symbols) const {
    std::string result;
    for (size_t i = 0; i < symbols.size(); ++i) {
        if (i > 0) result += ",";
        result += symbols[i];
    }
    return result;
}

std::string StreamingSession::build_websocket_url(const std::string& endpoint) const {
    std::string base_url = client_->get_base_url();
    
    std::string ws_host;
    if (base_url.find("api.tradier.com") != std::string::npos) {
        ws_host = "wss://ws.tradier.com";
    } else if (base_url.find("sandbox.tradier.com") != std::string::npos) {
        ws_host = "wss://sandbox-ws.tradier.com";
    } else {
        boost::url url(base_url);
        std::string host = std::string(url.host());
        if (host.find("api.") == 0) {
            host = "ws." + host.substr(4);
        }
        ws_host = "wss://" + host;
    }
    
    return ws_host + endpoint;
}


StreamingQuote StreamingQuote::from_json(const simdjson::dom::element& elem) {
    StreamingQuote quote;
    
    auto get_string = [](const simdjson::dom::element& e, const char* key) -> std::string {
        auto result = e[key];
        if (result.error() == simdjson::SUCCESS) {
            return std::string(result.value().get_string().value());
        }
        return "";
    };
    
    auto get_double = [](const simdjson::dom::element& e, const char* key, double default_val = 0.0) -> double {
        auto result = e[key];
        if (result.error() == simdjson::SUCCESS) {
            return result.value().get_double().value();
        }
        return default_val;
    };
    
    auto get_int = [](const simdjson::dom::element& e, const char* key, int default_val = 0) -> int {
        auto result = e[key];
        if (result.error() == simdjson::SUCCESS) {
            return static_cast<int>(result.value().get_int64().value());
        }
        return default_val;
    };
    
    quote.symbol = get_string(elem, "symbol");
    quote.bid = get_double(elem, "bid");
    quote.ask = get_double(elem, "ask");
    quote.last = get_double(elem, "last");
    quote.bid_size = get_int(elem, "bidsize");
    quote.ask_size = get_int(elem, "asksize");
    quote.last_size = get_int(elem, "last_volume");
    quote.bid_exch = get_string(elem, "bidexch");
    quote.ask_exch = get_string(elem, "askexch");
    
    auto timestamp_result = elem["timestamp"];
    if (timestamp_result.error() == simdjson::SUCCESS) {
        auto ts = timestamp_result.value().get_int64().value();
        quote.timestamp = std::chrono::system_clock::from_time_t(ts);
    } else {
        quote.timestamp = std::chrono::system_clock::now();
    }
    
    return quote;
}

std::string StreamingQuote::to_json() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << R"({"symbol":")" << symbol 
        << R"(","bid":)" << bid
        << R"(,"ask":)" << ask
        << R"(,"last":)" << last
        << R"(,"bid_size":)" << bid_size
        << R"(,"ask_size":)" << ask_size
        << R"(,"last_size":)" << last_size
        << R"(,"bid_exch":")" << bid_exch
        << R"(","ask_exch":")" << ask_exch
        << R"(","timestamp":)" << std::chrono::duration_cast<std::chrono::seconds>(
               timestamp.time_since_epoch()).count()
        << "}";
    return oss.str();
}

StreamingTrade StreamingTrade::from_json(const simdjson::dom::element& elem) {
    StreamingTrade trade;
    
    auto symbol_result = elem["symbol"];
    if (symbol_result.error() == simdjson::SUCCESS) {
        trade.symbol = std::string(symbol_result.value().get_string().value());
    }
    
    auto price_result = elem["price"];
    if (price_result.error() == simdjson::SUCCESS) {
        trade.price = price_result.value().get_double().value();
    }
    
    auto size_result = elem["size"];
    if (size_result.error() == simdjson::SUCCESS) {
        trade.size = static_cast<int>(size_result.value().get_int64().value());
    }
    
    auto exch_result = elem["exch"];
    if (exch_result.error() == simdjson::SUCCESS) {
        trade.exch = std::string(exch_result.value().get_string().value());
    }
    
    auto condition_result = elem["condition"];
    if (condition_result.error() == simdjson::SUCCESS) {
        trade.condition = std::string(condition_result.value().get_string().value());
    }
    
    auto timestamp_result = elem["timestamp"];
    if (timestamp_result.error() == simdjson::SUCCESS) {
        auto ts = timestamp_result.value().get_int64().value();
        trade.timestamp = std::chrono::system_clock::from_time_t(ts);
    } else {
        trade.timestamp = std::chrono::system_clock::now();
    }
    
    return trade;
}

std::string StreamingTrade::to_json() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << R"({"symbol":")" << symbol 
        << R"(","price":)" << price
        << R"(,"size":)" << size
        << R"(,"exch":")" << exch
        << R"(","condition":")" << condition
        << R"(","timestamp":)" << std::chrono::duration_cast<std::chrono::seconds>(
               timestamp.time_since_epoch()).count()
        << "}";
    return oss.str();
}

StreamingSummary StreamingSummary::from_json(const simdjson::dom::element& elem) {
    StreamingSummary summary;
    
    auto symbol_result = elem["symbol"];
    if (symbol_result.error() == simdjson::SUCCESS) {
        summary.symbol = std::string(symbol_result.value().get_string().value());
    }
    
    auto open_result = elem["open"];
    if (open_result.error() == simdjson::SUCCESS) {
        summary.open = open_result.value().get_double().value();
    }
    
    auto high_result = elem["high"];
    if (high_result.error() == simdjson::SUCCESS) {
        summary.high = high_result.value().get_double().value();
    }
    
    auto low_result = elem["low"];
    if (low_result.error() == simdjson::SUCCESS) {
        summary.low = low_result.value().get_double().value();
    }
    
    auto close_result = elem["close"];
    if (close_result.error() == simdjson::SUCCESS) {
        summary.close = close_result.value().get_double().value();
    }
    
    auto prev_close_result = elem["prevclose"];
    if (prev_close_result.error() == simdjson::SUCCESS) {
        summary.prev_close = prev_close_result.value().get_double().value();
    }
    
    auto volume_result = elem["volume"];
    if (volume_result.error() == simdjson::SUCCESS) {
        summary.volume = volume_result.value().get_int64().value();
    }
    
    summary.timestamp = std::chrono::system_clock::now();
    return summary;
}

std::string StreamingSummary::to_json() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << R"({"symbol":")" << symbol 
        << R"(","open":)" << open
        << R"(,"high":)" << high
        << R"(,"low":)" << low
        << R"(,"close":)" << close
        << R"(,"prev_close":)" << prev_close
        << R"(,"volume":)" << volume
        << R"(,"timestamp":)" << std::chrono::duration_cast<std::chrono::seconds>(
               timestamp.time_since_epoch()).count()
        << "}";
    return oss.str();
}

StreamingOrderStatus StreamingOrderStatus::from_json(const simdjson::dom::element& elem) {
    StreamingOrderStatus status;
    
    auto order_id_result = elem["id"];
    if (order_id_result.error() == simdjson::SUCCESS) {
        status.order_id = std::string(order_id_result.value().get_string().value());
    }
    
    auto status_result = elem["status"];
    if (status_result.error() == simdjson::SUCCESS) {
        status.status = std::string(status_result.value().get_string().value());
    }
    
    auto symbol_result = elem["symbol"];
    if (symbol_result.error() == simdjson::SUCCESS) {
        status.symbol = std::string(symbol_result.value().get_string().value());
    }
    
    auto type_result = elem["type"];
    if (type_result.error() == simdjson::SUCCESS) {
        status.order_type = order_type_from_string(std::string(type_result.value().get_string().value()));
    }
    
    auto side_result = elem["side"];
    if (side_result.error() == simdjson::SUCCESS) {
        status.side = order_side_from_string(std::string(side_result.value().get_string().value()));
    }
    
    auto quantity_result = elem["quantity"];
    if (quantity_result.error() == simdjson::SUCCESS) {
        status.quantity = quantity_result.value().get_double().value();
    }
    
    auto filled_result = elem["filled_quantity"];
    if (filled_result.error() == simdjson::SUCCESS) {
        status.filled_quantity = filled_result.value().get_double().value();
    }
    
    auto avg_price_result = elem["avg_fill_price"];
    if (avg_price_result.error() == simdjson::SUCCESS) {
        status.avg_fill_price = avg_price_result.value().get_double().value();
    }
    
    auto remaining_result = elem["remaining_quantity"];
    if (remaining_result.error() == simdjson::SUCCESS) {
        status.remaining_quantity = remaining_result.value().get_double().value();
    }
    
    status.timestamp = std::chrono::system_clock::now();
    return status;
}

std::string StreamingOrderStatus::to_json() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << R"({"order_id":")" << order_id 
        << R"(","status":")" << status
        << R"(","symbol":")" << symbol
        << R"(","order_type":")" << to_string(order_type)
        << R"(","side":")" << to_string(side)
        << R"(","quantity":)" << quantity
        << R"(,"filled_quantity":)" << filled_quantity
        << R"(,"avg_fill_price":)" << avg_fill_price
        << R"(,"remaining_quantity":)" << remaining_quantity
        << R"(","timestamp":)" << std::chrono::duration_cast<std::chrono::seconds>(
               timestamp.time_since_epoch()).count()
        << "}";
    return oss.str();
}

std::unique_ptr<StreamingSession> create_streaming_session(std::shared_ptr<TradierClient> client) {
    return std::make_unique<StreamingSession>(std::move(client));
}

StreamingDataType parse_message_type(const simdjson::dom::element& elem) {
    return StreamingSession::determine_data_type_static(elem);
}

} // namespace oqd