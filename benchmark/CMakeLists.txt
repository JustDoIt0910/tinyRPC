link_libraries(${LIBS})

add_executable(bench bench.cc calculate_service.pb.cc)

target_include_directories(bench PRIVATE ${THIRD_PARTY_DIR}/asio)