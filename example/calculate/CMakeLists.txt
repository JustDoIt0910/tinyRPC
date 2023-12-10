link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(calculate_client client.cc calculate_service.pb.cc)
add_executable(calculate_server server.cc calculate_service.pb.cc)