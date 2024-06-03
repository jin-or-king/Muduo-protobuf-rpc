aux_source_directory(. SRC_LIST)
# STATIC(静态库)/SHARED(动态库)/MODULE(模块库)
add_library(mprpc SHARED ${SRC_LIST})