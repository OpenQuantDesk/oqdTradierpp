cmake_minimum_required(VERSION 3.22)

find_package(GTest REQUIRED)
include(GoogleTest)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GTest_INCLUDE_DIRS})

# Create test executable for unit tests
file(GLOB_RECURSE UNIT_TEST_SOURCES "unit/*.cpp")
add_executable(oqdTradierpp_unit_tests ${UNIT_TEST_SOURCES})

target_link_libraries(oqdTradierpp_unit_tests
    oqdTradierpp
    GTest::gtest
    GTest::gtest_main
    ${Boost_LIBRARIES}
    ${SIMDJSON_LIBRARIES}
    pthread
    ssl
    crypto
)

set_property(TARGET oqdTradierpp_unit_tests PROPERTY CXX_STANDARD 20)

# Create test executable for integration tests (only if sources exist)
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES "integration/*.cpp")
if(INTEGRATION_TEST_SOURCES)
    add_executable(oqdTradierpp_integration_tests ${INTEGRATION_TEST_SOURCES})

    target_link_libraries(oqdTradierpp_integration_tests
        oqdTradierpp
        GTest::gtest
        GTest::gtest_main
        ${Boost_LIBRARIES}
        ${SIMDJSON_LIBRARIES}
        pthread
        ssl
        crypto
    )

    set_property(TARGET oqdTradierpp_integration_tests PROPERTY CXX_STANDARD 20)
    gtest_discover_tests(oqdTradierpp_integration_tests)
endif()

# Create test executable for error codes and validation tests
add_executable(oqdTradierpp_error_tests test_error_codes.cpp)
target_link_libraries(oqdTradierpp_error_tests
    oqdTradierpp
    GTest::gtest
    GTest::gtest_main
    ${Boost_LIBRARIES}
    ${SIMDJSON_LIBRARIES}
    pthread
    ssl
    crypto
)
set_property(TARGET oqdTradierpp_error_tests PROPERTY CXX_STANDARD 20)

# Create test executable for integrated validation tests
add_executable(oqdTradierpp_validation_tests test_integrated_validation.cpp)
target_link_libraries(oqdTradierpp_validation_tests
    oqdTradierpp
    GTest::gtest
    GTest::gtest_main
    ${Boost_LIBRARIES}
    ${SIMDJSON_LIBRARIES}
    pthread
    ssl
    crypto
)
set_property(TARGET oqdTradierpp_validation_tests PROPERTY CXX_STANDARD 20)

# Add tests to CTest
gtest_discover_tests(oqdTradierpp_unit_tests)
gtest_discover_tests(oqdTradierpp_error_tests)
gtest_discover_tests(oqdTradierpp_validation_tests)

# Add valgrind target
find_program(VALGRIND_PROGRAM valgrind)
if(VALGRIND_PROGRAM)
    add_custom_target(test_valgrind
        COMMAND ${VALGRIND_PROGRAM} 
        --tool=memcheck 
        --leak-check=full 
        --show-leak-kinds=all 
        --track-origins=yes 
        --verbose 
        --error-exitcode=1
        $<TARGET_FILE:oqdTradierpp_unit_tests>
        DEPENDS oqdTradierpp_unit_tests
        COMMENT "Running unit tests with Valgrind memory check"
    )
endif()

# Add performance tests
add_subdirectory(performance)