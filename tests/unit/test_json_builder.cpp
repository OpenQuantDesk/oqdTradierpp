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
#include "oqdTradierpp/core/json_builder.hpp"
#include "oqdTradierpp/core/enums.hpp"

using namespace oqd::json;

class JsonBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(JsonBuilderTest, EmptyObject) {
    auto result = create_object().end_object().str();
    EXPECT_EQ(result, "{}");
}

TEST_F(JsonBuilderTest, SingleStringField) {
    auto result = create_object()
        .field("name", "value")
        .end_object().str();
    EXPECT_EQ(result, "{\"name\":\"value\"}");
}

TEST_F(JsonBuilderTest, MultipleFields) {
    auto result = create_object()
        .field("string", "test")
        .field("integer", 42)
        .field("boolean", true)
        .end_object().str();
    EXPECT_EQ(result, "{\"string\":\"test\",\"integer\":42,\"boolean\":true}");
}

TEST_F(JsonBuilderTest, IntegerTypes) {
    auto result = create_object()
        .field("int", 123)
        .field("long", 123L)
        .field("long_long", 123LL)
        .field("unsigned", 123U)
        .field("unsigned_long", 123UL)
        .end_object().str();
    EXPECT_EQ(result, "{\"int\":123,\"long\":123,\"long_long\":123,\"unsigned\":123,\"unsigned_long\":123}");
}

TEST_F(JsonBuilderTest, DoubleWithPrecision) {
    auto result = create_object()
        .set_fixed().set_precision(2)
        .field("price", 123.456)
        .end_object().str();
    EXPECT_EQ(result, "{\"price\":123.46}");
}

TEST_F(JsonBuilderTest, DoubleWithoutPrecision) {
    auto result = create_object()
        .field("value", 123.456)
        .end_object().str();
    EXPECT_EQ(result, "{\"value\":123.456}");
}

TEST_F(JsonBuilderTest, BooleanValues) {
    auto result = create_object()
        .field("true_val", true)
        .field("false_val", false)
        .end_object().str();
    EXPECT_EQ(result, "{\"true_val\":true,\"false_val\":false}");
}

TEST_F(JsonBuilderTest, StringEscaping) {
    auto result = create_object()
        .field("quotes", "text with \"quotes\"")
        .field("backslash", "path\\to\\file")
        .field("newline", "line1\nline2")
        .field("tab", "col1\tcol2")
        .end_object().str();
    EXPECT_EQ(result, "{\"quotes\":\"text with \\\"quotes\\\"\",\"backslash\":\"path\\\\to\\\\file\",\"newline\":\"line1\\nline2\",\"tab\":\"col1\\tcol2\"}");
}

TEST_F(JsonBuilderTest, ControlCharacterEscaping) {
    std::string control_char;
    control_char.push_back(0x1F);
    auto result = create_object()
        .field("control", control_char)
        .end_object().str();
    EXPECT_EQ(result, "{\"control\":\"\\u001f\"}");
}

TEST_F(JsonBuilderTest, EmptyArray) {
    auto result = create_array().end_array().str();
    EXPECT_EQ(result, "[]");
}

TEST_F(JsonBuilderTest, ArrayWithElements) {
    auto result = create_array()
        .element("first")
        .element(42)
        .element(true)
        .end_array().str();
    EXPECT_EQ(result, "[\"first\",42,true]");
}

TEST_F(JsonBuilderTest, ArrayField) {
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    auto result = create_object()
        .array_field("numbers", numbers)
        .end_object().str();
    EXPECT_EQ(result, "{\"numbers\":[1,2,3,4,5]}");
}

TEST_F(JsonBuilderTest, ArrayFieldWithStrings) {
    std::vector<std::string> strings = {"apple", "banana", "cherry"};
    auto result = create_object()
        .array_field("fruits", strings)
        .end_object().str();
    EXPECT_EQ(result, "{\"fruits\":[\"apple\",\"banana\",\"cherry\"]}");
}

TEST_F(JsonBuilderTest, OptionalFieldPresent) {
    std::optional<int> value = 42;
    auto result = create_object()
        .field_optional("optional", value)
        .end_object().str();
    EXPECT_EQ(result, "{\"optional\":42}");
}

TEST_F(JsonBuilderTest, OptionalFieldAbsent) {
    std::optional<int> value;
    auto result = create_object()
        .field_optional("optional", value)
        .end_object().str();
    EXPECT_EQ(result, "{}");
}

TEST_F(JsonBuilderTest, NestedObject) {
    struct TestObject {
        std::string to_json() const {
            return create_object()
                .field("inner", "value")
                .end_object().str();
        }
    };
    
    TestObject obj;
    auto result = create_object()
        .field("outer", obj)
        .end_object().str();
    EXPECT_EQ(result, "{\"outer\":{\"inner\":\"value\"}}");
}

TEST_F(JsonBuilderTest, EnumField) {
    auto result = create_object()
        .field("side", oqd::OrderSide::Buy)
        .end_object().str();
    EXPECT_EQ(result, "{\"side\":\"buy\"}");
}

TEST_F(JsonBuilderTest, ClearAndReuse) {
    JsonBuilder builder;
    builder.start_object().field("first", "value").end_object();
    auto first_result = builder.str();
    
    builder.clear();
    builder.start_object().field("second", "value").end_object();
    auto second_result = builder.str();
    
    EXPECT_EQ(first_result, "{\"first\":\"value\"}");
    EXPECT_EQ(second_result, "{\"second\":\"value\"}");
}

TEST_F(JsonBuilderTest, MoveSemantics) {
    auto result = create_object()
        .field("test", "value")
        .end_object().str();
    EXPECT_EQ(result, "{\"test\":\"value\"}");
}

TEST_F(JsonBuilderTest, ComplexNestedStructure) {
    std::vector<int> array1 = {1, 2, 3};
    std::vector<std::string> array2 = {"a", "b", "c"};
    
    auto result = create_object()
        .field("name", "complex")
        .field("count", 100)
        .field("active", true)
        .array_field("numbers", array1)
        .array_field("letters", array2)
        .set_fixed().set_precision(2)
        .field("price", 99.995)
        .end_object().str();
    
    EXPECT_EQ(result, "{\"name\":\"complex\",\"count\":100,\"active\":true,\"numbers\":[1,2,3],\"letters\":[\"a\",\"b\",\"c\"],\"price\":100.00}");
}