#include "../user.pb.h"
#include <iostream>
#include <string>
using namespace std;

/*
1. caller => 远端调用Login(LoginRequest) => 传入框架中 => muduo网络层传输 => callee
2. callee => Login(LoginRequest) => 交到重写的Login方法中
*/

// 要想变成rpc服务的提供者需要继承protobu中service的类fixbug::UserServiceRpc
class UserService : public fixbug::UserServiceRpc{
public:
    bool Login(string name, string pwd){
        cout << "doing local service Login" << endl;
        cout << "name" << name << "pwd" << pwd << endl;

        return true;
    }

    // 重写父类中的纯虚函数,这个方法是提供rpc服务的，收到远端请求，给框架提供Login服务
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done){

        // 框架给业务层上报请求参数LoginRequest，应用获取相应数据做本地业务
        string name = request->name();
        string pwd = request->pwd();

        // 实现本地业务
        bool login_result = Login(name, pwd);

        // 业务实现后把响应写入response(错误码，错误消息，返回值)
        // 如果有错误返回错误消息
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作，closure是一个纯虚函数，重写run
        done->Run();
    }
};

int main(){
    return 0;
}