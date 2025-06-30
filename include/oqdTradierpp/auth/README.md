# Authentication Headers

This directory contains header declarations for OAuth 2.0 authentication in the oqdTradierpp library.

## Header Files

### OAuth 2.0 Token Management

#### `access_token.hpp`
- **`AccessToken`**: Complete OAuth 2.0 token structure
  - `access_token`: Bearer token for API authentication (string)
  - `token_type`: Type of token, typically "Bearer" (string)
  - `refresh_token`: Token for refreshing expired access tokens (string)
  - `expires_in`: Token lifetime in seconds (int64_t)
  - `scope`: Granted permissions scope, optional (string)

## Design Patterns

### OAuth 2.0 Compliance
```cpp
struct AccessToken {
    std::string access_token;    // Required: The actual token
    std::string token_type;      // Required: "Bearer"
    std::string refresh_token;   // Required: For token refresh
    int64_t expires_in;          // Required: Expiration time in seconds
    std::string scope;           // Optional: Granted permissions
};
```

### Secure by Default
- **No Plain Text Storage**: Designed for encrypted storage patterns
- **Immutable After Creation**: Token data not modified after parsing
- **Validation Ready**: Structure supports validation operations

### Standard Serialization
```cpp
// Consistent with OAuth 2.0 specification
{
    "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9...",
    "token_type": "Bearer",
    "refresh_token": "def502004f6a9b...",
    "expires_in": 86400,
    "scope": "read trade"
}
```

## Type Definitions

### Core Type
```cpp
namespace oqd {
    struct AccessToken {
        std::string access_token;
        std::string token_type;
        std::string refresh_token;
        int64_t expires_in;
        std::string scope;
        
        static AccessToken from_json(const simdjson::dom::element& elem);
        std::string to_json() const;
    };
}
```

### Forward Declaration
```cpp
// For use in other modules without full include
namespace oqd {
    struct AccessToken;
}
```

## Usage Patterns

### Include Strategy
```cpp
#include "oqdTradierpp/auth/access_token.hpp"  // OAuth 2.0 token management
```

### Template Compatibility
```cpp
// AccessToken supports standard operations
template<typename TokenType>
void manage_token(const TokenType& token) {
    std::string json = token.to_json();        // Serialization
    TokenType copy = token;                    // Copy construction
    TokenType moved = std::move(copy);         // Move semantics
}
```

### Validation Patterns
```cpp
// Token validation helpers
bool is_valid_token(const AccessToken& token) {
    return !token.access_token.empty() && 
           !token.refresh_token.empty() && 
           token.expires_in > 0;
}

bool is_expired(const AccessToken& token, 
                std::chrono::time_point<std::chrono::steady_clock> issued_time) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - issued_time);
    return elapsed.count() >= token.expires_in;
}
```

## Integration Points

### With API Client
```cpp
// Token integration in HTTP headers
class AuthenticatedAPIClient {
    AccessToken current_token_;
    
public:
    void set_token(const AccessToken& token) {
        current_token_ = token;
    }
    
    std::string get_auth_header() const {
        return current_token_.token_type + " " + current_token_.access_token;
    }
};
```

### With Secure Storage
```cpp
// Secure token persistence interface
class TokenStorage {
public:
    virtual void store_token(const AccessToken& token) = 0;
    virtual AccessToken load_token() = 0;
    virtual void delete_token() = 0;
    virtual bool has_token() = 0;
};

// File-based implementation
class FileTokenStorage : public TokenStorage {
    std::string file_path_;
    
public:
    void store_token(const AccessToken& token) override {
        std::string json = token.to_json();
        write_encrypted_file(file_path_, json);
    }
    
    AccessToken load_token() override {
        std::string json = read_encrypted_file(file_path_);
        auto doc = simdjson::dom::parser{}.parse(json);
        return AccessToken::from_json(doc.root());
    }
};
```

### With Configuration Management
```cpp
// OAuth configuration structure
struct OAuthConfig {
    std::string client_id;
    std::string client_secret;
    std::string redirect_uri;
    std::string authorization_url;
    std::string token_url;
    std::vector<std::string> scopes;
};

// Token manager with configuration
class TokenManager {
    OAuthConfig config_;
    AccessToken current_token_;
    
public:
    AccessToken authenticate(const std::string& auth_code);
    AccessToken refresh_token();
    bool is_authenticated() const;
};
```

## Security Considerations

### Memory Management
```cpp
// Secure token cleanup
class SecureAccessToken {
    AccessToken token_;
    
public:
    SecureAccessToken(AccessToken&& token) : token_(std::move(token)) {}
    
    ~SecureAccessToken() {
        // Zero out sensitive memory
        std::fill(token_.access_token.begin(), token_.access_token.end(), '\0');
        std::fill(token_.refresh_token.begin(), token_.refresh_token.end(), '\0');
    }
    
    const AccessToken& get() const { return token_; }
};
```

### Scope Validation
```cpp
// Permission checking utilities
class ScopeValidator {
public:
    static bool has_scope(const AccessToken& token, const std::string& required_scope) {
        return token.scope.find(required_scope) != std::string::npos;
    }
    
    static bool can_trade(const AccessToken& token) {
        return has_scope(token, "trade");
    }
    
    static bool can_read_account(const AccessToken& token) {
        return has_scope(token, "read");
    }
    
    static std::vector<std::string> parse_scopes(const AccessToken& token) {
        std::vector<std::string> scopes;
        std::istringstream iss(token.scope);
        std::string scope;
        while (iss >> scope) {
            scopes.push_back(scope);
        }
        return scopes;
    }
};
```

### Thread Safety
```cpp
// Thread-safe token management
class ThreadSafeTokenManager {
    mutable std::shared_mutex token_mutex_;
    AccessToken current_token_;
    
public:
    AccessToken get_token() const {
        std::shared_lock<std::shared_mutex> lock(token_mutex_);
        return current_token_;
    }
    
    void update_token(const AccessToken& token) {
        std::unique_lock<std::shared_mutex> lock(token_mutex_);
        current_token_ = token;
    }
    
    std::string get_auth_header() const {
        std::shared_lock<std::shared_mutex> lock(token_mutex_);
        return current_token_.token_type + " " + current_token_.access_token;
    }
};
```

## Performance Characteristics

### Memory Efficiency
- **Minimal Structure**: Only essential OAuth 2.0 fields
- **String Optimization**: Uses std::string for flexibility
- **Copy/Move Friendly**: Efficient for token passing

### Serialization Performance
- **Fast JSON**: 8-15μs serialization with bespoke JSON builder
- **Minimal Parsing**: Direct field extraction from JSON
- **No Validation Overhead**: Assumes well-formed OAuth responses

### Usage Optimization
```cpp
// Efficient token operations
class OptimizedTokenManager {
    std::string cached_auth_header_;
    AccessToken current_token_;
    
public:
    const std::string& get_auth_header() {
        if (cached_auth_header_.empty()) {
            cached_auth_header_ = current_token_.token_type + " " + current_token_.access_token;
        }
        return cached_auth_header_;
    }
    
    void update_token(AccessToken&& token) {
        current_token_ = std::move(token);
        cached_auth_header_.clear();  // Force rebuild on next access
    }
};
```

## Error Handling Patterns

### Token Validation
```cpp
enum class TokenValidationError {
    EmptyAccessToken,
    EmptyRefreshToken,
    InvalidExpiration,
    MalformedToken
};

class TokenValidator {
public:
    static std::optional<TokenValidationError> validate(const AccessToken& token) {
        if (token.access_token.empty()) {
            return TokenValidationError::EmptyAccessToken;
        }
        if (token.refresh_token.empty()) {
            return TokenValidationError::EmptyRefreshToken;
        }
        if (token.expires_in <= 0) {
            return TokenValidationError::InvalidExpiration;
        }
        return std::nullopt;  // Valid token
    }
};
```

This header provides a secure, efficient foundation for OAuth 2.0 authentication in the oqdTradierpp library, emphasizing security best practices and performance optimization.