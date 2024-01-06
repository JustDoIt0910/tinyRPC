cmake_minimum_required(VERSION 3.16)
project(tinyRPC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

find_package(Protobuf REQUIRED)
if (${PROTOBUF_FOUND})
    message("protobuf found")
else ()
    message(FATAL_ERROR "Cannot find Protobuf")
endif ()

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

add_subdirectory(${THIRD_PARTY_DIR}/json-3.11.3)
add_subdirectory(${THIRD_PARTY_DIR}/b64.c)

add_custom_command(OUTPUT
        ${CMAKE_SOURCE_DIR}/tinyRPC/api/http_rule.pb.h
        ${CMAKE_SOURCE_DIR}/tinyRPC/api/http_rule.pb.cc
        COMMAND protoc --cpp_out=. tinyRPC/api/http_rule.proto
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "compile http_rule.proto")

add_library(${PROJECT_NAME}
#        src/server/server.cc
#        src/server/session.cc
#        src/server/gw.cc
        src/server_v2/server.cc
        src/server_v2/session.cc
        src/server_v2/gw.cc
        src/exec/task_queue.cc
        src/exec/executor.cc
        src/router/base_router.cc
        src/router/protobuf_router.cc
        src/router/http_router.cc
        src/codec/abstract_codec.cc
        src/codec/protobuf_codec.cc
        src/codec/http_codec.cc
        src/rpc/error.cc
        src/rpc/controller.cc
        src/rpc/message.cc
        src/registry/etcd_def.cc
        src/registry/etcd.cc
        src/registry/registry.cc
        src/client/client.cc
        src/client/channel.cc
        ${CMAKE_SOURCE_DIR}/tinyRPC/api/http_rule.pb.cc)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${THIRD_PARTY_DIR}
        ${THIRD_PARTY_DIR}/asio)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json b64c)

set(LIBS ${PROJECT_NAME} ${Protobuf_LIBRARY} pthread uuid)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/tinyRPC/api
        DESTINATION include/${PROJECT_NAME}
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
