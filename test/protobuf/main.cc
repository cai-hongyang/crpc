#include "test.pb.h"
#include <iostream>

int main() {
    /*
    chat::LoginResponse response;
    response.set_success(1);
    chat::ResultCode* res = response.mutable_result(); // 可修改
    res->set_errcode(1);
    res->set_errmsg("登录成功!");
    */

    chat::GetFriendListsResponse response;
    chat::ResultCode* res = response.mutable_result();
    res->set_errcode(1);
    res->set_errmsg("成功!");

    chat::User* user = response.add_friendlist();
    user->set_age(24);
    user->set_name("zhang san");
    user->set_sex(chat::User::MAN);
    return 0;
}