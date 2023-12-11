link_libraries(${LIBS})

add_executable(server_with_http_gateway server.cc test_service.pb.cc)
add_executable(client client.cc test_service.pb.cc)