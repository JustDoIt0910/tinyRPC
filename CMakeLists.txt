cmake_minimum_required(VERSION 3.16)
project(tinyRPC)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

include_directories(${CMAKE_SOURCE_DIR}/include
        ${THIRD_PARTY_DIR}/asio)

add_library(${PROJECT_NAME} src/server/server.cc
        src/server/session.cc
        src/protocol/codec.cc)
add_subdirectory(test)

