# set(SRC_LIST user_service.cc ../user.pb.cc)
set(SRC_LIST friend_service.cc ../friend.pb.cc)


add_executable(provider ${SRC_LIST})
target_link_libraries(provider crpc protobuf)