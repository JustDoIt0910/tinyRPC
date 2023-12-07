link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(test_accept test_accept.cc)
add_executable(test_connect test_connect.cc)
add_executable(test_codec test_codec.cc)
add_executable(protoc-gen-myplg test_plugin.cc)
target_link_libraries(protoc-gen-myplg protoc)

