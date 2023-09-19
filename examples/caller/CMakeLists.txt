# set(SRC_LIST call_user_service.cc ../user.pb.cc)
set(SRC_LIST call_friend_service.cc ../friend.pb.cc)

add_executable(consumer ${SRC_LIST})
target_link_libraries(consumer crpc protobuf)