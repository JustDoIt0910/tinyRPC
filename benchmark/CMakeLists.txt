link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(bench bench.cc calculate_service.pb.cc)