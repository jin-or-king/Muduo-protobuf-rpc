#include <iostream>
#include "mprpcapplication.h"
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
    // rpc方法的异常情况控制器
    MprpcController controller;
    // RpcChannel->RpcChannel::callMethod集中做所有rpc方法调用的参数序列化和网络发送
    stub.Login(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        // 一次rpc方法调用的完成，读调用结果
        if (1 == response.result().errcode())
        {
            cout << "rpc login response success: " << response.success() << endl;
        }
        else
        {
            cout << "rpc login response error: " << response.result().errmsg() << endl;
        }
    }
    

    // rpc方法的请求参数
    fixbug::RegisterRequest register_request;
    register_request.set_name("li si");
    register_request.set_pwd("66666"); 
    // rpc方法的响应
    fixbug::RegisterResponse register_response;
    MprpcController register_controller;
    // RpcChannel->RpcChannel::callMethod集中做所有rpc方法调用的参数序列化和网络发送
    stub.Register(&register_controller, &register_request, &register_response, nullptr);

    if (register_controller.Failed())
    {
        std::cout << register_controller.ErrorText() << std::endl;
    }
    else
    {    // 一次rpc方法调用的完成，读调用结果
        if (1 == register_response.result().errcode())
        {
            cout << "rpc register response success: " << register_response.success() << endl;
        }
        else
        {
            cout << "rpc register response error: " << register_response.result().errmsg() << endl;
        }
    }
    
    return 0;
}