cmake_minimum_required(VERSION 3.22)
project(oqdTradierpp VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED CONFIG COMPONENTS system thread url)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SIMDJSON REQUIRED simdjson)

find_path(WEBSOCKETPP_INCLUDE_DIR websocketpp/config/asio_client.hpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(MAKEOPTS="-j$(nproc)")

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIMDJSON_INCLUDE_DIRS})
if(WEBSOCKETPP_INCLUDE_DIR)
    include_directories(${WEBSOCKETPP_INCLUDE_DIR})
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_library(oqdTradierpp SHARED ${SOURCES} ${HEADERS})

target_link_libraries(oqdTradierpp 
    ${Boost_LIBRARIES}
    ${SIMDJSON_LIBRARIES}
    pthread
    ssl
    crypto
)

set_target_properties(oqdTradierpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${HEADERS}"
)

install(TARGETS oqdTradierpp
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/oqdTradierpp
)

add_subdirectory(examples)

find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Tests enabled")
else()
    message(STATUS "GTest not found, tests disabled")
endif()