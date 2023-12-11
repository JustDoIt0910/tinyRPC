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

add_custom_command(OUTPUT
        ${CMAKE_SOURCE_DIR}/api/http_rule.pb.h
        ${CMAKE_SOURCE_DIR}/api/http_rule.pb.cc
        COMMAND protoc --cpp_out=. tinyRPC/api/http_rule.proto
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        COMMENT "compile http_rule.proto")

add_library(${PROJECT_NAME}
        src/server/server.cc
        src/server/session.cc
        src/server/gw.cc
        src/router/base_router.cc
        src/router/protobuf_router.cc
        src/router/http_router.cc
        src/codec/abstract_codec.cc
        src/codec/protobuf_codec.cc
        src/codec/http_codec.cc
        src/rpc/error.cc
        src/rpc/controller.cc
        src/rpc/message.cc
        src/client/client.cc
        src/client/channel.cc
        ${CMAKE_SOURCE_DIR}/api/http_rule.pb.cc)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${THIRD_PARTY_DIR}/asio
        ${THIRD_PARTY_DIR}
        ${CMAKE_SOURCE_DIR}/..)

set(LIBS ${PROJECT_NAME} ${Protobuf_LIBRARY} pthread uuid)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

install(DIRECTORY api DESTINATION include/${PROJECT_NAME}
        FILES_MATCHING
        PATTERN "*.proto"
        PATTERN "*.h"
        PATTERN "*.cc" EXCLUDE)

add_subdirectory(plugin)
add_subdirectory(test)
add_subdirectory(example/calculate)
#add_subdirectory(example/http)
add_subdirectory(benchmark)

install(TARGETS protoc-gen-http-gw DESTINATION bin)
