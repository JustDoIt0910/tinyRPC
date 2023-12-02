link_directories(${CMAKE_BINARY_DIR})
link_libraries(tinyRPC pthread)

add_executable(test_accept test_accept.cc)

