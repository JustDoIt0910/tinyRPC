add_executable(protoc-gen-http-gw http_gw_generator.cc)

target_include_directories(protoc-gen-http-gw PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(protoc-gen-http-gw ${PROJECT_NAME} protoc ${Protobuf_LIBRARY})