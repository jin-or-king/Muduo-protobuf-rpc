#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <string>
#include <functional>
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
using namespace std;

/*
*service 记录服务对象 -> GetDescriptor() -> service_name -> service描述
                                        -> method_count -> method方法描述
*/

// 框架提供给外部使用的可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service* service)
{
    ServiceInfo service_info;

    // 获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    string service_name = pserviceDesc->name();
    // 获取服务对象service的方法的数量
    int methodCnt = pserviceDesc->method_count();

    cout << "service name" << service_name << endl;

    for (int i = 0; i < methodCnt; ++i)
    {
        // 获取了服务对象指定的下标服务方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});
        cout << "method name" << method_name << endl;
    }

    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
    
}

// 启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{

    string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // port是无符号int类型，Load传回来的是string类型，atoi需要char*。使用c_str转换为char*
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip, port);
    // 创建tcpserver对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调的方法
    server.setConnectionCallback(bind(&RpcProvider::OnConnection, this, placeholders::_1));
    server.setMessageCallback(bind(&RpcProvider::OnMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    // 启动网络
    server.start();
    m_eventLoop.loop(); 
}

// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        // 和rpc client的连接断开了
        conn->shutdown();
    }
    
}

/*
框架内RpcProvider和RpcConsumer协商通信用的数据格式类型service_name,method_name,args_size
header_size(4个字节) + header_str + args_str
存储二进制的大小insert和copy
*/
// 建立连接用户的读写事件回调，如果远程有一个rpc服务的调用请求，那么OnMessage就会响应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流 service_name,method_name args等等
    string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前四个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    string service_name;
    string methond_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        service_name = rpcHeader.service_name();
        methond_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        cout << "rpc_header_str" << rpc_header_str << "parse error" << endl;
        return ;
    }

    string args_str = recv_buf.substr(4 + header_size, args_size);
    
    cout << "===================================" << endl;
    cout << "header_size" << header_size << endl;
    cout << "rpc_header_str" << rpc_header_str << endl;
    cout << "service_name" << service_name << endl;
    cout << "methond_name" << methond_name << endl;
    cout << "args_size" << args_size << endl;
    cout << "===================================" << endl;

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        cout << service_name << "is not exist" << endl;
        return ;
    }

    auto mit = it->second.m_methodMap.find(methond_name);
    if (mit == it->second.m_methodMap.end())    
    {
        cout << service_name << ":" << methond_name << "is not exist" << endl;
        return ;
    }

    // 获取service对象，new UserService
    google::protobuf::Service* service = it->second.m_service;
    // 获取method对象
    const google::protobuf::MethodDescriptor* method = mit->second;

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))    
    {
        cout << "request parse error, content:" << args_str << endl;
        return ;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 给调用的方法绑定一个closure回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::sendRpcResponse, conn, response);

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法，使用基类调用
    // new UserService().Login(controller, request, response, done)，done就是closure函数
    service->CallMethod(method, nullptr, request, response, done);
}

// closure的回调操作，用于rpc的响应和网络发送
void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    string response_str;
    if (response->SerializeToString(&response_str))
    {
        // 序列化成功后将rpc的返回结果发送给调用方
        conn->send(response_str);
    }
    else
    {
        cout << "serialize response_str error" << endl;
    }
    
    // 主动断开连接，模拟http短链接服务
    conn->shutdown();
    
}