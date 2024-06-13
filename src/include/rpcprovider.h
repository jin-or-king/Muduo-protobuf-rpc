#pragma once
#include "google/protobuf/service.h"
// 智能指针
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
using namespace std;

// 框架提供的专门发布rpc服务的网络对象类
class RpcProvider
{
public:

    // 框架提供给外部使用的可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    // 组合EventLoop
    muduo::net::EventLoop m_eventLoop;

    // service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service* m_service; // 保存服务对象
        unordered_map<string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    unordered_map<string, ServiceInfo> m_serviceMap;

    // 新的socket连接回调cb函数，放入到setConnectionCallback中
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    
    // 调用完rpc方法后运行closure回调操作，用于序列化rpc的响应和网络发送(将方法和done绑定)
    void sendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};

