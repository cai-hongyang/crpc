aux_source_directory(. SRC_LIST)

add_library(crpc ${SRC_LIST})

target_link_libraries(crpc muduo_net muduo_base pthread zookeeper_mt -lssl -lcrypto)