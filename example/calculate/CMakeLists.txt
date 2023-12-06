link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(client calculate_client.cc calculate_service.pb.cc)
add_executable(server calculate_server calculate_service.pb.cc)