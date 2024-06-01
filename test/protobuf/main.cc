#include "test.pb.h"
#include <iostream>
#include <string>
using namespace std;
using namespace fixbug;

int main1(){
    LoginRequest req;
    // 将数据序列化放入req中
    req.set_name("zhang san");
    req.set_pwd("123456");

    string send_str;
    // 将数据序列化放入send_str字符串中
    if (req.SerializeToString(&send_str)){
        cout << send_str << endl;
    }

    // 从send_str反序列化一个login请求对象
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str)){
        cout << reqB.name() << endl;
        cout << reqB.pwd() << endl;
    }

    return 0;
}

int main(){
    LoginResponse rsp;

    // 消息类型是其他的消息类如何设置其中的值
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("登录处理失败");

    GetFriendListsResponse grsp;
    ResultCode *rc1 = grsp.mutable_result();
    rc1->set_errcode(0);

    // 向friendsList中添加数据，同时返回一个user指针来设置user
    User* user1 = grsp.add_friendslist();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User* user2 = grsp.add_friendslist();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::WOMAN);

    cout << "好友个数：" << grsp.friendslist_size() << endl;

    // 只能输出字符类型
    string send_str;
    if (grsp.friendslist(0).SerializeToString(&send_str)){
        cout << send_str << endl;
    }

    User userB;
    if (userB.ParseFromString(send_str)){
        cout << userB.name() << endl;
        cout << userB.sex() << endl;
        cout << userB.age() << endl;
    }

    return 0;
}