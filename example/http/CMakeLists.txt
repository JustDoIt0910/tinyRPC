link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(server_with_http_gateway server.cc services.pb.cc)
add_executable(client client.cc services.pb.cc)