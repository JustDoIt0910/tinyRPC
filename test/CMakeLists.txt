link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(test_codec test_codec.cc)
add_executable(test_threadpool test_threadpool.cc)

