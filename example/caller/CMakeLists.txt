aux_source_directory(. SRC_LIST)

set(SRC_LIST ../user.pb.cc ${SRC_LIST})

add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer mprpc protobuf)