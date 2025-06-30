cmake_minimum_required(VERSION 3.22)

find_package(GTest REQUIRED)

# Performance test sources
set(PERFORMANCE_TEST_SOURCES
    benchmark_json_builder.cpp
)

# Create performance test executable
add_executable(oqdTradierpp_performance_tests ${PERFORMANCE_TEST_SOURCES})

target_link_libraries(oqdTradierpp_performance_tests
    oqdTradierpp
    GTest::gtest
    GTest::gtest_main
    ${Boost_LIBRARIES}
    ${SIMDJSON_LIBRARIES}
    pthread
    ssl
    crypto
)

set_property(TARGET oqdTradierpp_performance_tests PROPERTY CXX_STANDARD 20)

# Add CPU profiling target
find_program(GPROF_PROGRAM gprof)
if(GPROF_PROGRAM)
    add_custom_target(profile_performance
        COMMAND ${CMAKE_COMMAND} -E env GMON_OUT_PREFIX=performance_profile
        $<TARGET_FILE:oqdTradierpp_performance_tests>
        COMMAND ${GPROF_PROGRAM} $<TARGET_FILE:oqdTradierpp_performance_tests> performance_profile.* > performance_profile.txt
        DEPENDS oqdTradierpp_performance_tests
        COMMENT "Running performance tests with gprof profiling"
    )
    
    # Add profiling flags for performance tests
    target_compile_options(oqdTradierpp_performance_tests PRIVATE -pg)
    target_link_options(oqdTradierpp_performance_tests PRIVATE -pg)
endif()

# Add perf target (Linux perf tool)
find_program(PERF_PROGRAM perf)
if(PERF_PROGRAM)
    add_custom_target(perf_performance
        COMMAND ${PERF_PROGRAM} record -g $<TARGET_FILE:oqdTradierpp_performance_tests>
        COMMAND ${PERF_PROGRAM} report > perf_report.txt
        DEPENDS oqdTradierpp_performance_tests
        COMMENT "Running performance tests with perf profiling"
    )
endif()