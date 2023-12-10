cmake_minimum_required(VERSION 3.16)
project(tinyRPC)

find_package(Protobuf REQUIRED)
if (${PROTOBUF_FOUND})
    message("protobuf found")
else ()
    message(FATAL_ERROR "Cannot find Protobuf")
endif ()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "Debug")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

include_directories(${CMAKE_SOURCE_DIR}/include
        ${THIRD_PARTY_DIR}/asio
        ${THIRD_PARTY_DIR}/CRCpp-release-1.2.0.0/inc)

add_custom_command(OUTPUT http_rule.pb.h http_rule.pb.cc
        COMMAND protoc -I${CMAKE_SOURCE_DIR}/plugin/proto --cpp_out=. http_rule.proto
        COMMENT "compile http_rule.proto")

add_library(${PROJECT_NAME}
        src/server/server.cc
        src/server/session.cc
        src/codec/abstract_codec.cc
        src/router/base_router.cc
        src/router/protobuf_router.cc
        src/router/http_router.cc
        src/codec/protobuf_codec.cc
        src/codec/http_codec.cc
        src/rpc/error.cc
        src/rpc/controller.cc
        src/client/client.cc
        src/client/channel.cc
        http_rule.pb.cc)


set(LIBS ${PROJECT_NAME}
        ${Protobuf_LIBRARY}
        pthread
        uuid)

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(plugin)
add_subdirectory(test)
add_subdirectory(example/calculate)
add_subdirectory(example/http)
add_subdirectory(benchmark)
