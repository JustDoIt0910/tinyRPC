cmake_minimum_required(VERSION 3.16)
project(tinyRPC)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/third_party/asio)

add_library(${PROJECT_NAME} src/server.cc src/session.cc)
add_subdirectory(test)

