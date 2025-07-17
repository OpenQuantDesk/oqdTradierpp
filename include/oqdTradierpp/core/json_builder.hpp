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
#include <optional>
#include <vector>
#include <type_traits>
#include <array>  
#include <cstdio> 
#include <charconv>
#include <sstream>
#include <iomanip>

namespace oqd {
namespace json {

template<typename T>
std::string to_string(const T& value); 

class JsonBuilder {
private:
    std::string buffer_;
    bool first_field_ = true;
    int precision_ = -1;
    bool fixed_notation_ = false;

    static constexpr std::array<char, 16> hex_chars = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
    
public:
    JsonBuilder() {
        buffer_.reserve(1024); 
    }

    // Allow copy and move for flexibility (can be optimized later)
    JsonBuilder(const JsonBuilder&) = default;
    JsonBuilder& operator=(const JsonBuilder&) = default;
    
    // Move semantics
    JsonBuilder(JsonBuilder&&) = default;            
    JsonBuilder& operator=(JsonBuilder&&) = default; 

    JsonBuilder& set_precision(int p) { precision_ = p; return *this; }
    JsonBuilder& set_fixed(bool f = true) { fixed_notation_ = f; return *this; }

    JsonBuilder& start_object() {
        buffer_ += '{';
        first_field_ = true;
        return *this;
    }

    JsonBuilder& end_object() {
        buffer_ += '}';
        return *this;
    }

    JsonBuilder& start_array() {
        buffer_ += '[';
        first_field_ = true;
        return *this;
    }

    JsonBuilder& end_array() {
        buffer_ += ']';
        return *this;
    }

    template<typename T>
    JsonBuilder& field(const std::string& key, const T& value) {
        add_comma();
        add_key(key);
        add_value(value);
        return *this;
    }

    template<typename T>
    JsonBuilder& field_optional(const std::string& key, const std::optional<T>& value) {
        if (value.has_value()) {
            field(key, value.value());
        }
        return *this;
    }

    template<typename T>
    JsonBuilder& element(const T& value) {
        add_comma();
        add_value(value);
        return *this;
    }

    template<typename T>
    JsonBuilder& array_field(const std::string& key, const std::vector<T>& values) {
        add_comma();
        add_key(key);
        start_array();
        for (const auto& value : values) {
            element(value);
        }
        end_array();
        return *this;
    }

    const std::string& str() const & { return buffer_; } 
    std::string str() && { return std::move(buffer_); }  

    void clear() {
        buffer_.clear();
        first_field_ = true;
        precision_ = -1;
        fixed_notation_ = false;
    }

private:
    void add_comma() {
        if (!first_field_) buffer_ += ',';
        first_field_ = false;
    }

    void add_key(const std::string& key) {
        buffer_ += '"';
        buffer_ += key; 
        buffer_ += "\":";
    }

    void add_value(const std::string& value) {
        buffer_ += '"';
        escape_and_append(value);
        buffer_ += '"';
    }

    void add_value(std::string_view value) {
        buffer_ += '"';
        escape_and_append(value);
        buffer_ += '"';
    }

    template<typename IntegerType>
    void add_integer_value(IntegerType value) {
        std::array<char, 24> temp_buf;
        auto [ptr, ec] = std::to_chars(temp_buf.data(), temp_buf.data() + temp_buf.size(), value);
        if (ec == std::errc()) {
            buffer_.append(temp_buf.data(), ptr - temp_buf.data());
        } else {
            buffer_ += std::to_string(value);
        }
    }

    void add_value(int value) { add_integer_value(value); }
    void add_value(long value) { add_integer_value(value); }
    void add_value(long long value) { add_integer_value(value); } 
    void add_value(unsigned int value) { add_integer_value(value); } 
    void add_value(unsigned long value) { add_integer_value(value); }
    void add_value(unsigned long long value) { add_integer_value(value); }


    void add_value(double value) {
        if (precision_ >= 0 && fixed_notation_) {
            std::array<char, 64> temp_buf;
            int len = std::snprintf(temp_buf.data(), temp_buf.size(), "%.*f", precision_, value);
            if (len > 0 && len < static_cast<int>(temp_buf.size())) {
                buffer_.append(temp_buf.data(), len);
            } else {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(precision_) << value;
                buffer_ += oss.str();
            }
        } else {
            std::array<char, 64> temp_buf;
            auto [ptr, ec] = std::to_chars(temp_buf.data(), temp_buf.data() + temp_buf.size(), value);
            if (ec == std::errc()) {
                buffer_.append(temp_buf.data(), ptr - temp_buf.data());
            } else {
                buffer_ += std::to_string(value);
            }
        }
    }

    void add_value(bool value) {
        buffer_ += value ? "true" : "false";
    }

    void add_value(const char* value) {
        add_value(std::string_view(value));
    }

    // Handle enums with to_string() function first
    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, void>
    add_value(const T& value) {
        buffer_ += '"';
        buffer_ += to_string(value);
        buffer_ += '"';
    }

    // Handle objects with to_json() method (fallback)
    template<typename T>
    auto add_value(const T& value)
        -> std::enable_if_t<!std::is_enum_v<T>, decltype(value.to_json(), void())>
    {
        buffer_ += value.to_json();
    }

    template<typename StringType>
    void escape_and_append(const StringType& str) {
        buffer_.reserve(buffer_.length() + str.length() + 16); 

        size_t last_pos = 0;
        for (size_t i = 0; i < str.length(); ++i) {
            char c = str[i];
            const char* escape_seq = nullptr;
            size_t escape_len = 0;

            switch (c) {
                case '"':  escape_seq = "\\\""; escape_len = 2; break;
                case '\\': escape_seq = "\\\\"; escape_len = 2; break;
                case '\b': escape_seq = "\\b"; escape_len = 2; break;
                case '\f': escape_seq = "\\f"; escape_len = 2; break;
                case '\n': escape_seq = "\\n"; escape_len = 2; break;
                case '\r': escape_seq = "\\r"; escape_len = 2; break;
                case '\t': escape_seq = "\\t"; escape_len = 2; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        if (i > last_pos) {
                            buffer_.append(str.data() + last_pos, i - last_pos);
                        }
                        buffer_ += "\\u00";
                        buffer_ += hex_chars[(static_cast<unsigned char>(c) >> 4) & 0xF];
                        buffer_ += hex_chars[static_cast<unsigned char>(c) & 0xF];
                        last_pos = i + 1; 
                        continue; 
                    }
                    break;
            }

            if (escape_seq) {
                if (i > last_pos) {
                    buffer_.append(str.data() + last_pos, i - last_pos);
                }
                buffer_.append(escape_seq, escape_len);
                last_pos = i + 1;
            }
        }
        if (last_pos < str.length()) {
            buffer_.append(str.data() + last_pos, str.length() - last_pos);
        }
    }
};

template<typename T>
std::enable_if_t<std::is_enum_v<T>, JsonBuilder&>
add_enum_field(JsonBuilder& builder, const std::string& key, const T& value) {
    return builder.field(key, to_string(value));
}

inline JsonBuilder create_object() {
    JsonBuilder builder;
    builder.start_object();
    return builder;
}

inline JsonBuilder create_array() {
    JsonBuilder builder;
    builder.start_array();
    return builder;
}

} // namespace json
} // namespace oqd