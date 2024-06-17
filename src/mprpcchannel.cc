#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include "mprpcapplication.h"
#include <unistd.h>
#include "socketdeleter.h"
#include "mprpccontroller.h"

/*
和框架读取消息流程相似，约定好调用格式
header_size + service_name method_name args_size + args
*/
void MprpcChannnel::CallMethod(const google::protobuf::MethodDescriptor* method,
                               google::protobuf::RpcController* controller, 
                               const google::protobuf::Message* request,
                               google::protobuf::Message* response, 
                               google::protobuf::Closure* done)
{
    // 获取服务描述，再获取服务name以及方法的描述
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name(); // service_name
    std::string method_name = method->name();

    std::cout << service_name << " " << method_name << " " << "CallMethod" << std::endl;

    uint32_t args_size = 0;
    std::string args_str;

    // method中包含service_name和method_name, 调用框架前先解析request命令(调用的参数)
    if (request->SerializeToString(&args_str))  
    {
        args_size = args_str.size();
    }
    else
    {
        // std::cout << "serialize request error" << std::endl;
        // 保存状态信息
        controller->SetFailed("serialize request error");
        return;
    }
    
    // 将解析出来的参数放入统一的proto中，通过protobuf来统一格式(自定义的头文件格式)
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;

    std::string rpc_header_str;
    // 通过protobuf生成统一的string格式
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();

    }
    else
    {
        // 保存状态信息
        controller->SetFailed("serialize rpc_header_str error");
        return;
    }

    // 将头头文件长度，头文件格式str，参数args_str放入发送字符串（双方约定的格式）
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;
    
    std::cout << "===================================" << std::endl;
    std::cout << "header_size" << header_size << std::endl;
    std::cout << "rpc_header_str" << rpc_header_str << std::endl;
    std::cout << "service_name" << service_name << std::endl;
    std::cout << "methond_name" << method_name << std::endl;
    std::cout << "args_size" << args_size << std::endl;
    std::cout << "===================================" << std::endl;
    
    // ====使用tcp编程，完成rpc方法的远程调用====

    // 设置socket通道
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        char errtext[512] = {0};
        sprintf(errtext, "create socket error:%d", errno);
        // 保存状态信息
        controller->SetFailed(errtext);
        return;
    }
    
    // 使用std::unique_ptr来管理socket文件描述符
    std::unique_ptr<int, SocketDeleter> clientfd_unique_ptr(new int(clientfd));

    // 读取IP和port（无论发布方还是使用方都需要连接rpc服务器）
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // port是无符号int类型，Load传回来的是string类型，atoi需要char*。使用c_str转换为char*
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // client和server进行连接
    if (-1 == connect(clientfd, (sockaddr *)&server_addr, sizeof(sockaddr_in)))
    {
        // 保存状态信息
        controller->SetFailed("connect server error");
        return;
    }

    // 发送rpc请求
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        // 保存状态信息
        controller->SetFailed("send rpc resquest error");
        return;
    }
    

    // 接受网络传输回来的数据流
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = (recv(clientfd, recv_buf, 1024, 0))))
    {
        // 保存状态信息
        controller->SetFailed("send rpc recv error");
        return;
    }

    // 数据流通过protobuf处理后为response格式，解析response
    // std::string response_str(recv_buf, 0, recv_size); // recv_buf中数据为\n \000 \020 \001, recv_size为4，但是string构造函数遇到\0停止了，response_str为\n
    // if (response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        // 保存状态信息
        controller->SetFailed("send rpc recv parse error");
        return;
    }
    
    
}