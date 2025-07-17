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
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl.hpp>

namespace oqd {

using StreamingCallback = std::function<void(const simdjson::dom::element&)>;
using ErrorCallback = std::function<void(const std::string&)>;

enum class StreamingDataType {
    Quote,
    Trade,
    Summary,
    TimeSale,
    TradEx,
    OrderStatus,
    AccountActivity
};

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
    Error,
    Closed
};

// Custom WebSocket config with TLS support
struct websocket_tls_config : public websocketpp::config::asio_tls_client {
    typedef websocketpp::config::asio_tls_client base;
    
    typedef base::concurrency_type concurrency_type;
    typedef base::request_type request_type;
    typedef base::response_type response_type;
    typedef base::message_type message_type;
    typedef base::con_msg_manager_type con_msg_manager_type;
    typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;
    typedef base::alog_type alog_type;
    typedef base::elog_type elog_type;
    typedef base::rng_type rng_type;
    
    struct transport_config : public base::transport_config {
        typedef base::transport_config::concurrency_type concurrency_type;
        typedef base::transport_config::elog_type elog_type;
        typedef base::transport_config::alog_type alog_type;
        typedef base::transport_config::request_type request_type;
        typedef base::transport_config::response_type response_type;
        typedef websocketpp::transport::asio::tls_socket::endpoint socket_type;
    };
    
    typedef websocketpp::transport::asio::endpoint<transport_config> transport_type;
};

class StreamingSession {
public:
    explicit StreamingSession(std::shared_ptr<TradierClient> client);
    ~StreamingSession();

    StreamingSession(const StreamingSession&) = delete;
    StreamingSession& operator=(const StreamingSession&) = delete;
    StreamingSession(StreamingSession&&) = default;
    StreamingSession& operator=(StreamingSession&&) = default;

    // HTTP Streaming (Server-Sent Events)
    std::future<void> start_market_http_stream_async(const std::vector<std::string>& symbols,
                                                    StreamingCallback on_data,
                                                    ErrorCallback on_error = nullptr);
    
    std::future<void> start_account_http_stream_async(StreamingCallback on_data,
                                                     ErrorCallback on_error = nullptr);

    void start_market_http_stream(const std::vector<std::string>& symbols,
                                 StreamingCallback on_data,
                                 ErrorCallback on_error = nullptr);
    
    void start_account_http_stream(StreamingCallback on_data,
                                  ErrorCallback on_error = nullptr);

    // WebSocket Streaming
    std::future<void> start_market_websocket_stream_async(const std::vector<std::string>& symbols,
                                                         StreamingCallback on_data,
                                                         ErrorCallback on_error = nullptr);
    
    std::future<void> start_account_websocket_stream_async(StreamingCallback on_data,
                                                          ErrorCallback on_error = nullptr);

    void start_market_websocket_stream(const std::vector<std::string>& symbols,
                                      StreamingCallback on_data,
                                      ErrorCallback on_error = nullptr);
    
    void start_account_websocket_stream(StreamingCallback on_data,
                                       ErrorCallback on_error = nullptr);

    // Control methods
    void stop_stream();
    bool is_streaming() const { return connection_state_ != ConnectionState::Disconnected; }
    ConnectionState get_connection_state() const { return connection_state_.load(); }
    void add_symbols(const std::vector<std::string>& symbols);
    void remove_symbols(const std::vector<std::string>& symbols);

    // Filter methods
    void set_data_filter(const std::vector<StreamingDataType>& types);
    void clear_data_filter();
    
    // Configuration
    void set_reconnect_enabled(bool enabled) { should_reconnect_ = enabled; }
    void set_max_reconnect_attempts(int attempts) { max_reconnect_attempts_ = attempts; }
    void set_reconnect_delay(std::chrono::milliseconds delay) { base_reconnect_delay_ = delay; }

    // Static utility method for external access
    static StreamingDataType determine_data_type_static(const simdjson::dom::element& data);

private:
    std::shared_ptr<TradierClient> client_;
    std::atomic<ConnectionState> connection_state_{ConnectionState::Disconnected};
    std::string session_id_;
    std::thread streaming_thread_;
    
    // Connection parameters storage for reconnection
    struct ConnectionParams {
        std::string endpoint;
        std::unordered_map<std::string, std::string> params;
        bool is_websocket;
    };
    ConnectionParams connection_params_;
    std::mutex connection_params_mutex_;
    
    // WebSocket client with TLS
    using WebSocketClient = websocketpp::client<websocket_tls_config>;
    std::unique_ptr<WebSocketClient> ws_client_;
    WebSocketClient::connection_ptr ws_connection_;
    std::shared_ptr<boost::asio::ssl::context> tls_context_;
    
    // Callbacks
    StreamingCallback data_callback_;
    ErrorCallback error_callback_;
    
    // Filtering
    std::vector<StreamingDataType> data_filter_;
    std::atomic<bool> has_filter_{false};
    mutable std::mutex filter_mutex_;
    
    // Symbol tracking
    std::vector<std::string> current_symbols_;
    std::mutex symbols_mutex_;
    
    // Reconnection state
    std::atomic<bool> should_reconnect_{true};
    std::atomic<int> reconnect_attempts_{0};
    std::atomic<int> max_reconnect_attempts_{10};
    std::chrono::milliseconds base_reconnect_delay_{1000};
    std::chrono::time_point<std::chrono::steady_clock> session_create_time_;
    std::condition_variable reconnect_cv_;
    std::mutex reconnect_mutex_;

    // Session management
    std::string create_market_session();
    std::string create_account_session();
    bool is_session_expired() const;
    void refresh_session_if_needed();
    
    // Connection management
    void connect_websocket(const std::string& endpoint, const std::unordered_map<std::string, std::string>& params);
    void disconnect_websocket();
    void handle_reconnection();
    
    // HTTP streaming implementation
    void http_stream_worker(const std::string& endpoint, const std::unordered_map<std::string, std::string>& params);
    
    // WebSocket streaming implementation
    void websocket_stream_worker(const std::string& endpoint, const std::unordered_map<std::string, std::string>& params);
    void setup_websocket_handlers();
    std::shared_ptr<boost::asio::ssl::context> create_tls_context();
    
    // Data processing
    void process_streaming_data(const std::string& data);
    void process_sse_event(const std::string& event_type, const std::string& event_data);
    bool should_process_data(StreamingDataType type) const;
    StreamingDataType determine_data_type(const simdjson::dom::element& data) const;
    
    // Utility methods
    std::string join_symbols(const std::vector<std::string>& symbols) const;
    std::string build_websocket_url(const std::string& endpoint) const;
    void update_connection_state(ConnectionState state);
};

// Enhanced streaming data structures with more fields
struct StreamingQuote {
    std::string symbol;
    double bid;
    double ask;
    double last;
    int bid_size;
    int ask_size;
    int last_size;
    std::string bid_exch;
    std::string ask_exch;
    std::chrono::system_clock::time_point timestamp;
    
    static StreamingQuote from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct StreamingTrade {
    std::string symbol;
    double price;
    int size;
    std::string exch;
    std::string condition;
    std::chrono::system_clock::time_point timestamp;
    
    static StreamingTrade from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct StreamingSummary {
    std::string symbol;
    double open;
    double high;
    double low;
    double close;
    double prev_close;
    long volume;
    std::chrono::system_clock::time_point timestamp;
    
    static StreamingSummary from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

struct StreamingOrderStatus {
    std::string order_id;
    std::string status;
    std::string symbol;
    OrderType order_type;
    OrderSide side;
    double quantity;
    double filled_quantity;
    double avg_fill_price;
    double remaining_quantity;
    std::chrono::system_clock::time_point timestamp;
    
    static StreamingOrderStatus from_json(const simdjson::dom::element& elem);
    std::string to_json() const;
};

// Factory function for creating streaming sessions
std::unique_ptr<StreamingSession> create_streaming_session(std::shared_ptr<TradierClient> client);

// Utility function to parse streaming message type
StreamingDataType parse_message_type(const simdjson::dom::element& elem);

} // namespace oqd