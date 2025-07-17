cmake_minimum_required(VERSION 3.22)
project(oqdTradierpp VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED CONFIG COMPONENTS system thread url)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SIMDJSON REQUIRED simdjson)
if(NOT SIMDJSON_FOUND)
    message(FATAL_ERROR "simdjson not found. Please install simdjson library.")
endif()

find_path(WEBSOCKETPP_INCLUDE_DIR websocketpp/config/asio_client.hpp)
if(NOT WEBSOCKETPP_INCLUDE_DIR)
    message(FATAL_ERROR "websocketpp not found. Please install websocketpp library.")
endif()

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

# Source files
set(SOURCES
    src/account/account_balances.cpp
    src/account/account_history.cpp
    src/account/gain_loss.cpp
    src/account/gain_loss_item.cpp
    src/account/history_item.cpp
    src/account/position.cpp
    src/account/user_profile.cpp
    src/api_methods.cpp
    src/auth/access_token.cpp
    src/core/enums.cpp
    src/factory.cpp
    src/fundamentals/corp_actions.cpp
    src/fundamentals/corp_calendar.cpp
    src/fundamentals/corp_dividends.cpp
    src/fundamentals/corp_financials.cpp
    src/fundamentals/corp_info.cpp
    src/fundamentals/corp_pricestats.cpp
    src/market/historical_data.cpp
    src/market/market_status.cpp
    src/market/option_chain.cpp
    src/market/quote.cpp
    src/market/symbol_search.cpp
    src/market/time_sales.cpp
    src/oqdTradierpp.cpp
    src/order_validation.cpp
    src/streaming.cpp
    src/trading/advanced_orders.cpp
    src/trading/multileg_orders.cpp
    src/trading/order.cpp
    src/trading/order_management.cpp
    src/trading/order_requests.cpp
    src/trading/spread_orders.cpp
    src/watchlist/watchlist.cpp
    src/watchlist/watchlist_detail.cpp
)

# Header files
set(HEADERS
    include/oqdTradierpp/account/account_balances.hpp
    include/oqdTradierpp/account/account_history.hpp
    include/oqdTradierpp/account/gain_loss.hpp
    include/oqdTradierpp/account/gain_loss_item.hpp
    include/oqdTradierpp/account/history_item.hpp
    include/oqdTradierpp/account/position.hpp
    include/oqdTradierpp/account/user_profile.hpp
    include/oqdTradierpp/api.hpp
    include/oqdTradierpp/auth/access_token.hpp
    include/oqdTradierpp/client.hpp
    include/oqdTradierpp/core/enums.hpp
    include/oqdTradierpp/core/json_builder.hpp
    include/oqdTradierpp/endpoints.hpp
    include/oqdTradierpp/fundamentals/corp_actions.hpp
    include/oqdTradierpp/fundamentals/corp_calendar.hpp
    include/oqdTradierpp/fundamentals/corp_dividends.hpp
    include/oqdTradierpp/fundamentals/corp_financials.hpp
    include/oqdTradierpp/fundamentals/corp_info.hpp
    include/oqdTradierpp/fundamentals/corp_pricestats.hpp
    include/oqdTradierpp/market/historical_data.hpp
    include/oqdTradierpp/market/market_status.hpp
    include/oqdTradierpp/market/option_chain.hpp
    include/oqdTradierpp/market/quote.hpp
    include/oqdTradierpp/market/symbol_search.hpp
    include/oqdTradierpp/market/time_sales.hpp
    include/oqdTradierpp/oqdTradierpp.hpp
    include/oqdTradierpp/streaming.hpp
    include/oqdTradierpp/trading/advanced_orders.hpp
    include/oqdTradierpp/trading/multileg_orders.hpp
    include/oqdTradierpp/trading/order.hpp
    include/oqdTradierpp/trading/order_management.hpp
    include/oqdTradierpp/trading/order_requests.hpp
    include/oqdTradierpp/trading/spread_orders.hpp
    include/oqdTradierpp/types.hpp
    include/oqdTradierpp/utils.hpp
    include/oqdTradierpp/validation.hpp
    include/oqdTradierpp/watchlist/watchlist_detail.hpp
    include/oqdTradierpp/watchlist/watchlist.hpp
)

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