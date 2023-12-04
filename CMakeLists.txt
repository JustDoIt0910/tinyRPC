cmake_minimum_required(VERSION 3.16)
project(tinyRPC)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Debug")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

include_directories(${CMAKE_SOURCE_DIR}/include
        ${THIRD_PARTY_DIR}/asio)

add_library(${PROJECT_NAME} src/server/server.cc
        src/server/session.cc
        src/protocol/codec.cc
        src/protocol/controller.cc
        src/client/client.cc
        src/client/channel.cc)

find_package(Protobuf REQUIRED)
if (${PROTOBUF_FOUND})
    message("protobuf found")
else ()
    message(FATAL_ERROR "Cannot find Protobuf")
endif ()

set(LIBS ${PROJECT_NAME}
        ${Protobuf_LIBRARY}
        pthread
        uuid)

add_subdirectory(test)
add_subdirectory(example/calculate)

