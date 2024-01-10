link_libraries(${LIBS})

add_executable(calculate_client client.cc calculate_service.pb.cc)
add_executable(calculate_server server.cc calculate_service.pb.cc)

file(COPY ${CMAKE_SOURCE_DIR}/config.yaml
        DESTINATION ${CMAKE_BINARY_DIR}/example/calculate)