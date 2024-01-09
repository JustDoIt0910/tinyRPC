link_directories(${CMAKE_BINARY_DIR})
link_libraries(${LIBS})

add_executable(test_codec test_codec.cc)
add_executable(test_threadpool test_threadpool.cc)
add_executable(test_etcd_watch test_etcd_watch.cc)
add_executable(test_etcd_put test_etcd_put.cc)

add_executable(test_consistant_hash test_consistant_hash.cc)