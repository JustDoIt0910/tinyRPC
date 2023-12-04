link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(test_accept test_accept.cc)
add_executable(test_crc test_crc.cc)
add_executable(test_connect test_connect.cc)

