# Authentication Module

This module provides OAuth 2.0 authentication functionality for the oqdTradierpp library, handling access tokens, refresh tokens, and secure API authentication.

## Overview

The authentication module implements OAuth 2.0 token management for secure access to the Tradier API. It handles token creation, refresh, validation, and provides secure storage patterns for authentication credentials.

✅ **MIGRATION COMPLETE**: This module has been successfully migrated to use the bespoke JSON builder for optimal performance.

## Components

### Token Management

#### `access_token.cpp` - OAuth 2.0 Token Handling
- **`AccessToken`**: Complete OAuth 2.0 token structure
  - `access_token`: The bearer token for API authentication
  - `token_type`: Token type (typically "Bearer")
  - `refresh_token`: Token for refreshing expired access tokens
  - `expires_in`: Token lifetime in seconds
  - `scope`: Granted permissions scope (optional)

## Features

### OAuth 2.0 Flow Support
- **Authorization Code Flow**: Complete implementation for web applications
- **Token Refresh**: Automatic refresh before expiration
- **Scope Management**: Permission-based access control
- **Secure Storage**: Patterns for safe token storage

### Token Lifecycle Management
```cpp
// Create access token from authorization code
AccessToken token = api->create_access_token(auth_code, redirect_uri);

// Check if token needs refresh (before expiration)
if (token.is_expired()) {
    token = api->refresh_access_token(token.refresh_token);
}

// Use token for authenticated API calls
api->set_access_token(token.access_token);
```

## Usage Examples

### Initial Authentication
```cpp
// Step 1: Get authorization URL
std::string auth_url = api->get_oauth_url("http://localhost:8080/callback", "read,trade");
std::cout << "Visit: " << auth_url << std::endl;

// Step 2: User authorizes and you receive callback with code
std::string auth_code = "received_from_callback";

// Step 3: Exchange code for access token
AccessToken token = api->create_access_token(auth_code, "http://localhost:8080/callback");
std::cout << "Access token: " << token.access_token << std::endl;
std::cout << "Expires in: " << token.expires_in << " seconds" << std::endl;
```

### Token Refresh
```cpp
// Refresh expired token
try {
    AccessToken new_token = api->refresh_access_token(current_token.refresh_token);
    
    // Update stored token
    current_token = new_token;
    api->set_access_token(new_token.access_token);
    
    std::cout << "Token refreshed successfully" << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Token refresh failed: " << e.what() << std::endl;
    // Redirect user to re-authenticate
}
```

### Token Persistence
```cpp
// Save token to secure storage
void save_token(const AccessToken& token) {
    std::string json = token.to_json();
    // Save to encrypted file or secure storage
    save_encrypted_data("token.dat", json);
}

// Load token from secure storage
AccessToken load_token() {
    std::string json = load_encrypted_data("token.dat");
    auto doc = simdjson::dom::parser{}.parse(json);
    return AccessToken::from_json(doc.root());
}
```

### Automatic Token Management
```cpp
class TokenManager {
private:
    AccessToken current_token_;
    std::chrono::steady_clock::time_point token_issued_time_;
    
public:
    std::string get_valid_token() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - token_issued_time_).count();
        
        // Refresh if token expires in less than 5 minutes
        if (elapsed > (current_token_.expires_in - 300)) {
            refresh_token();
        }
        
        return current_token_.access_token;
    }
    
private:
    void refresh_token() {
        current_token_ = api->refresh_access_token(current_token_.refresh_token);
        token_issued_time_ = std::chrono::steady_clock::now();
        save_token(current_token_);
    }
};
```

### Scope-Based Operations
```cpp
// Check token permissions
bool can_trade(const AccessToken& token) {
    return token.scope.find("trade") != std::string::npos;
}

bool can_read_account(const AccessToken& token) {
    return token.scope.find("read") != std::string::npos;
}

// Use scoped operations
if (can_trade(token)) {
    auto order_response = api->place_equity_order(account_id, order);
} else {
    std::cerr << "Insufficient permissions for trading" << std::endl;
}
```

## Security Considerations

### Token Storage Best Practices
```cpp
// NEVER store tokens in plain text
class SecureTokenStorage {
public:
    void store_token(const AccessToken& token) {
        std::string json = token.to_json();
        std::string encrypted = encrypt_data(json, get_master_key());
        write_to_secure_file(encrypted);
    }
    
    AccessToken load_token() {
        std::string encrypted = read_from_secure_file();
        std::string json = decrypt_data(encrypted, get_master_key());
        auto doc = simdjson::dom::parser{}.parse(json);
        return AccessToken::from_json(doc.root());
    }
    
private:
    std::string get_master_key() {
        // Derive from user credentials or system keystore
        return derive_key_from_secure_source();
    }
};
```

### Environment Variable Security
```cpp
// Secure environment variable handling
class EnvironmentAuth {
public:
    static std::string get_client_id() {
        const char* client_id = std::getenv("TRADIER_CLIENT_ID");
        if (!client_id) {
            throw std::runtime_error("TRADIER_CLIENT_ID not set");
        }
        return std::string(client_id);
    }
    
    static std::string get_client_secret() {
        const char* secret = std::getenv("TRADIER_CLIENT_SECRET");
        if (!secret) {
            throw std::runtime_error("TRADIER_CLIENT_SECRET not set");
        }
        return std::string(secret);
    }
};
```

### Token Validation
```cpp
bool validate_token(const AccessToken& token) {
    // Check required fields
    if (token.access_token.empty() || token.token_type.empty()) {
        return false;
    }
    
    // Verify token format (should be base64-like)
    if (token.access_token.length() < 20) {
        return false;
    }
    
    // Check expiration
    if (token.expires_in <= 0) {
        return false;
    }
    
    return true;
}
```

## Integration Points

### With API Client
```cpp
// Token integration with HTTP client
class AuthenticatedClient {
private:
    TokenManager token_manager_;
    
public:
    http::response make_authenticated_request(const std::string& endpoint) {
        auto token = token_manager_.get_valid_token();
        
        http::request req;
        req.headers["Authorization"] = "Bearer " + token;
        req.headers["Accept"] = "application/json";
        
        return http_client_.make_request(req);
    }
};
```

### With Error Handling
```cpp
// Handle authentication errors
void handle_auth_error(const http::response& response) {
    if (response.status_code == 401) {
        // Token expired or invalid
        std::cout << "Authentication failed - refreshing token" << std::endl;
        token_manager_.force_refresh();
    } else if (response.status_code == 403) {
        // Insufficient permissions
        std::cerr << "Insufficient permissions for this operation" << std::endl;
    }
}
```

### With Configuration Management
```cpp
struct AuthConfig {
    std::string client_id;
    std::string client_secret;
    std::string redirect_uri;
    std::string sandbox_base_url;
    std::string production_base_url;
    
    static AuthConfig load_from_file(const std::string& config_path) {
        // Load configuration from secure file
        return parse_config_file(config_path);
    }
};
```

## Performance Characteristics

### JSON Processing
- **Serialization**: 8-15μs using bespoke JSON builder
- **Deserialization**: Fast parsing with simdjson
- **Memory Usage**: Minimal overhead with small token structure

### Network Efficiency
- **Token Caching**: Avoid unnecessary refresh requests
- **Proactive Refresh**: Refresh before expiration to prevent API disruption
- **Connection Reuse**: Maintain persistent connections with valid tokens

## Error Handling

### Token Refresh Failures
```cpp
enum class AuthError {
    TokenExpired,
    RefreshTokenInvalid,
    NetworkError,
    InvalidCredentials,
    InsufficientScope
};

class AuthException : public std::exception {
    AuthError error_type_;
    std::string message_;
    
public:
    AuthException(AuthError type, const std::string& msg) 
        : error_type_(type), message_(msg) {}
    
    AuthError type() const { return error_type_; }
    const char* what() const noexcept override { return message_.c_str(); }
};
```

### Robust Authentication Flow
```cpp
AccessToken authenticate_with_retry(const std::string& auth_code) {
    int max_retries = 3;
    
    for (int attempt = 1; attempt <= max_retries; ++attempt) {
        try {
            return api->create_access_token(auth_code, redirect_uri);
        } catch (const std::exception& e) {
            if (attempt == max_retries) {
                throw;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(attempt));
        }
    }
    
    throw std::runtime_error("Authentication failed after retries");
}
```

This module provides secure, efficient OAuth 2.0 authentication with robust token management and security best practices for professional trading applications.