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

#include <gtest/gtest.h>
#include "oqdTradierpp/auth/access_token.hpp"
#include <simdjson.h>

using namespace oqd;

class AccessTokenTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AccessTokenTest, DefaultConstruction) {
    AccessToken token;
    EXPECT_TRUE(token.access_token.empty());
    EXPECT_TRUE(token.token_type.empty());
    EXPECT_TRUE(token.refresh_token.empty());
    EXPECT_EQ(token.expires_in, 0);
    EXPECT_TRUE(token.scope.empty());
}

TEST_F(AccessTokenTest, FieldAssignment) {
    AccessToken token;
    token.access_token = "test_access_token";
    token.token_type = "bearer";
    token.refresh_token = "test_refresh_token";
    token.expires_in = 3600;
    token.scope = "read";
    
    EXPECT_EQ(token.access_token, "test_access_token");
    EXPECT_EQ(token.token_type, "bearer");
    EXPECT_EQ(token.refresh_token, "test_refresh_token");
    EXPECT_EQ(token.expires_in, 3600);
    EXPECT_EQ(token.scope, "read");
}

TEST_F(AccessTokenTest, ToJsonSerialization) {
    AccessToken token;
    token.access_token = "abc123";
    token.token_type = "bearer";
    token.refresh_token = "refresh123";
    token.expires_in = 7200;
    token.scope = "read write";
    
    std::string json = token.to_json();
    
    EXPECT_TRUE(json.find("\"access_token\":\"abc123\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"token_type\":\"bearer\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"refresh_token\":\"refresh123\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"expires_in\":7200") != std::string::npos);
    EXPECT_TRUE(json.find("\"scope\":\"read write\"") != std::string::npos);
}

TEST_F(AccessTokenTest, FromJsonDeserialization) {
    std::string json_str = R"({
        "access_token": "test_token_123",
        "token_type": "bearer",
        "refresh_token": "refresh_456",
        "expires_in": 3600,
        "scope": "read write"
    })";
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json_str);
    AccessToken token = AccessToken::from_json(doc.value());
    
    EXPECT_EQ(token.access_token, "test_token_123");
    EXPECT_EQ(token.token_type, "bearer");
    EXPECT_EQ(token.refresh_token, "refresh_456");
    EXPECT_EQ(token.expires_in, 3600);
    EXPECT_EQ(token.scope, "read write");
}

TEST_F(AccessTokenTest, FromJsonMissingScope) {
    std::string json_str = R"({
        "access_token": "test_token",
        "token_type": "bearer",
        "refresh_token": "refresh_token",
        "expires_in": 1800
    })";
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json_str);
    AccessToken token = AccessToken::from_json(doc.value());
    
    EXPECT_EQ(token.access_token, "test_token");
    EXPECT_EQ(token.token_type, "bearer");
    EXPECT_EQ(token.refresh_token, "refresh_token");
    EXPECT_EQ(token.expires_in, 1800);
    EXPECT_TRUE(token.scope.empty());
}

TEST_F(AccessTokenTest, FromJsonEmptyScope) {
    std::string json_str = R"({
        "access_token": "test_token",
        "token_type": "bearer", 
        "refresh_token": "refresh_token",
        "expires_in": 1800,
        "scope": ""
    })";
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json_str);
    AccessToken token = AccessToken::from_json(doc.value());
    
    EXPECT_EQ(token.scope, "");
}

TEST_F(AccessTokenTest, RoundTripSerialization) {
    AccessToken original;
    original.access_token = "round_trip_token";
    original.token_type = "bearer";
    original.refresh_token = "round_trip_refresh";
    original.expires_in = 5400;
    original.scope = "read write admin";
    
    std::string json = original.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccessToken deserialized = AccessToken::from_json(doc.value());
    
    EXPECT_EQ(deserialized.access_token, original.access_token);
    EXPECT_EQ(deserialized.token_type, original.token_type);
    EXPECT_EQ(deserialized.refresh_token, original.refresh_token);
    EXPECT_EQ(deserialized.expires_in, original.expires_in);
    EXPECT_EQ(deserialized.scope, original.scope);
}

TEST_F(AccessTokenTest, EmptyFieldsSerialization) {
    AccessToken token;
    
    std::string json = token.to_json();
    
    EXPECT_TRUE(json.find("\"access_token\":\"\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"token_type\":\"\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"refresh_token\":\"\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"expires_in\":0") != std::string::npos);
    EXPECT_TRUE(json.find("\"scope\":\"\"") != std::string::npos);
}

TEST_F(AccessTokenTest, SpecialCharacterHandling) {
    AccessToken token;
    token.access_token = "token_with\"quotes'and\\backslashes";
    token.scope = "scope with\nnewlines\tand\ttabs";
    
    std::string json = token.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccessToken deserialized = AccessToken::from_json(doc.value());
    
    EXPECT_EQ(deserialized.access_token, token.access_token);
    EXPECT_EQ(deserialized.scope, token.scope);
}

TEST_F(AccessTokenTest, LargeExpiresIn) {
    AccessToken token;
    token.access_token = "long_lived_token";
    token.expires_in = 2147483647;
    
    std::string json = token.to_json();
    
    simdjson::dom::parser parser;
    auto doc = parser.parse(json);
    AccessToken deserialized = AccessToken::from_json(doc.value());
    
    EXPECT_EQ(deserialized.expires_in, 2147483647);
}