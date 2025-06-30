cmake_minimum_required(VERSION 3.22)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Example files
set(EXAMPLE_SOURCES
    basic_usage.cpp
    market_data_example.cpp
    trading_example.cpp
    streaming_example.cpp
    comprehensive_example.cpp
    full_trading_demo.cpp
)

# Create example executables
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_NAME} oqdTradierpp)
endforeach()