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
#include <chrono>
#include <vector>
#include <sstream>
#include <iomanip>
#include <numeric>
#include "oqdTradierpp/core/json_builder.hpp"

using namespace oqd::json;
using namespace std::chrono;

class JsonBuilderBenchmark : public ::testing::Test {
protected:
    static constexpr int ITERATIONS = 100000;
    static constexpr int WARMUP_ITERATIONS = 1000;
    
    template<typename Func>
    double benchmark_function(const std::string& name, Func&& func) {
        for (int i = 0; i < WARMUP_ITERATIONS; ++i) {
            func();
        }
        
        auto start = high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            func();
        }
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start).count();
        double avg_microseconds = static_cast<double>(duration) / ITERATIONS;
        
        std::cout << name << ": " << std::fixed << std::setprecision(3) 
                  << avg_microseconds << " µs/op (" << ITERATIONS << " iterations)" << std::endl;
        
        return avg_microseconds;
    }
};

TEST_F(JsonBuilderBenchmark, SimpleObjectCreation) {
    benchmark_function("Simple object creation", []() {
        auto result = create_object()
            .field("name", "test")
            .field("value", 42)
            .field("active", true)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, ComplexObjectCreation) {
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<std::string> strings = {"apple", "banana", "cherry", "date", "elderberry"};
    
    benchmark_function("Complex object creation", [&]() {
        auto result = create_object()
            .field("name", "complex_object")
            .field("count", 1000)
            .field("active", true)
            .set_fixed().set_precision(2)
            .field("price", 99.995)
            .field("discount", 15.50)
            .array_field("numbers", numbers)
            .array_field("strings", strings)
            .field("description", "This is a complex object with multiple fields and arrays")
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, StringEscaping) {
    std::string complex_string = "String with \"quotes\", \\backslashes\\, \nnewlines, \ttabs, and control chars: \x01\x02\x03";
    
    benchmark_function("String escaping", [&]() {
        auto result = create_object()
            .field("complex", complex_string)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, IntegerConversion) {
    benchmark_function("Integer conversion", []() {
        auto result = create_object()
            .field("int1", 1234567890)
            .field("int2", -987654321)
            .field("long1", 1234567890123456789LL)
            .field("ulong1", 18446744073709551615ULL)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, DoubleConversionFixed) {
    benchmark_function("Double conversion (fixed precision)", []() {
        auto result = create_object()
            .set_fixed().set_precision(6)
            .field("d1", 123.456789)
            .field("d2", -987.654321)
            .field("d3", 0.000001)
            .field("d4", 999999.999999)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, DoubleConversionDynamic) {
    benchmark_function("Double conversion (dynamic)", []() {
        auto result = create_object()
            .field("d1", 123.456789)
            .field("d2", -987.654321)
            .field("d3", 0.000001)
            .field("d4", 999999.999999)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, LargeArrayCreation) {
    std::vector<int> large_array(1000);
    std::iota(large_array.begin(), large_array.end(), 1);
    
    benchmark_function("Large array creation", [&]() {
        auto result = create_object()
            .array_field("data", large_array)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, MemoryReallocation) {
    benchmark_function("Memory reallocation stress", []() {
        JsonBuilder builder;
        builder.start_object();
        
        for (int i = 0; i < 100; ++i) {
            builder.field("field" + std::to_string(i), "value" + std::to_string(i));
        }
        
        auto result = builder.end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, NestedObjectCreation) {
    struct NestedObject {
        std::string to_json() const {
            return create_object()
                .field("inner_name", "nested")
                .field("inner_value", 123)
                .end_object().str();
        }
    };
    
    NestedObject nested;
    
    benchmark_function("Nested object creation", [&]() {
        auto result = create_object()
            .field("outer", "value")
            .field("nested1", nested)
            .field("nested2", nested)
            .field("nested3", nested)
            .end_object().str();
        (void)result;
    });
}

TEST_F(JsonBuilderBenchmark, ClearAndReuse) {
    JsonBuilder builder;
    
    benchmark_function("Clear and reuse", [&]() {
        builder.clear();
        builder.start_object()
            .field("name", "reused")
            .field("value", 42)
            .end_object();
        auto result = builder.str();
        (void)result;
    });
}