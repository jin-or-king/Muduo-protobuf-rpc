aux_source_directory(. SRC_LIST)

set(SRC_LIST ../user.pb.cc ${SRC_LIST})

add_executable(provider ${SRC_LIST})

target_link_libraries(provider mprpc protobuf)