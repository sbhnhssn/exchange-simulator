cmake_minimum_required(VERSION 3.20)
project(jane_street_exchange VERSION 2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
add_executable(exchange
    src/main.cpp
    src/order_book.cpp
    src/event_log.cpp
    src/protocol.cpp
    src/server.cpp
)
target_include_directories(exchange PRIVATE include)
if (UNIX)
  target_link_libraries(exchange PRIVATE pthread)
endif()
enable_testing()
add_executable(tests tests/test_exchange.cpp src/order_book.cpp src/event_log.cpp src/protocol.cpp)
target_include_directories(tests PRIVATE include)
add_test(NAME exchange_tests COMMAND tests)
