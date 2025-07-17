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
#include <string_view>
#include <sstream>
#include <iomanip>
#include <array>
#include <unordered_map>

namespace oqd::utils {

/**
 * @brief URL encode a string according to RFC 3986
 * @param str The string to encode
 * @return URL encoded string
 */

inline std::string url_encode(std::string_view str) {
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (unsigned char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
            continue;
        }
        encoded << std::uppercase;
        encoded << '%' << std::setw(2) << static_cast<int>(c);
        encoded << std::nouppercase;
    }

    return encoded.str();
}

/**
 * @brief URL decode a percent-encoded string
 * @param str The string to decode
 * @return URL decoded string
 */

inline std::string url_decode(std::string_view str) {
    std::string decoded;
    decoded.reserve(str.length());

    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%') {
            if (i + 2 < str.length()) {
                int hex_value;
                std::istringstream is(std::string(str.substr(i + 1, 2)));
                if (is >> std::hex >> hex_value) {
                    decoded += static_cast<char>(hex_value);
                    i += 2;
                } else {
                    decoded += str[i];
                }
            } else {
                decoded += str[i];
            }
        } else if (str[i] == '+') {
            decoded += ' ';
        } else {
            decoded += str[i];
        }
    }

    return decoded;
}

/**
 * @brief Base64 encode a string
 * @param input The string to encode
 * @return Base64 encoded string
 */

inline std::string base64_encode(std::string_view input) {
    static constexpr std::array<char, 64> chars = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };

    std::string result;
    result.reserve(((input.length() + 2) / 3) * 4);

    int val = 0;
    int valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (result.size() % 4) {
        result.push_back('=');
    }
    return result;
}

/**
 * @brief Base64 decode a string
 * @param input The Base64 encoded string to decode
 * @return Decoded string
 */

inline std::string base64_decode(std::string_view input) {
    static constexpr std::array<int, 128> T = {
        -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
        52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
        -1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
        15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
        -1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
        41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1
    };

    std::string result;
    result.reserve(input.length() * 3 / 4);

    int val = 0;
    int valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

/**
 * @brief Create HTTP Basic Authentication header value
 * @param username The username
 * @param password The password
 * @return Basic auth header value (without "Basic " prefix)
 */

inline std::string create_basic_auth(std::string_view username, std::string_view password) {
    std::string credentials = std::string(username) + ":" + std::string(password);
    return base64_encode(credentials);
}

/**
 * @brief Build query string from parameters with proper URL encoding
 * @param params Key-value pairs to encode
 * @return URL-encoded query string (without leading '?')
 */

inline std::string build_query_string(const std::unordered_map<std::string, std::string>& params) {
    if (params.empty()) {
        return "";
    }

    std::ostringstream query;
    bool first = true;
    
    for (const auto& [key, value] : params) {
        if (!first) {
            query << "&";
        }
        query << url_encode(key) << "=" << url_encode(value);
        first = false;
    }
    
    return query.str();
}

/**
 * @brief Build form data string from parameters with proper URL encoding
 * @param params Key-value pairs to encode
 * @return URL-encoded form data string
 */

inline std::string build_form_data(const std::unordered_map<std::string, std::string>& params) {
    return build_query_string(params); // Same format for form data
}

} // namespace oqd::utils