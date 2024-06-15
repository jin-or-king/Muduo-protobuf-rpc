#include <iostream>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"


int main(int argc, char **argv)
{
    // 整个程序启动后，想使用mprpc的框架，要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannnel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456"); 
    // rpc方法的响应
    fixbug::LoginResponse response;
    // RpcChannel->RpcChannel::callMethod集中做所有rpc方法调用的参数序列化和网络发送
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc方法调用的完成，读调用结果
    if (1 == response.result().errcode())
    {
        cout << "rpc login response success: " << response.success() << endl;
    }
    else
    {
        cout << "rpc login response error: " << response.result().errmsg() << endl;
    }
    
    return 0;
}