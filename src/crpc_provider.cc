#include "log.h"
#include "crpc_provider.h"
#include "application.h"
#include "crpc_header.pb.h"
#include "zk_client.h"

using namespace std::placeholders;

using sendHandler = std::function<void(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*)>;

void CRpcProvider::publishService(google::protobuf::Service* service) {
    ServiceInfo serviceInfo;

    const google::protobuf::ServiceDescriptor* serviceDescriptor = service->GetDescriptor();
    std::string service_name = serviceDescriptor->name();

    LOG_INFO("service_name: %s", service_name.c_str());

    int method_count = serviceDescriptor->method_count();
    for (int i = 0; i < method_count; i++) {
        const google::protobuf::MethodDescriptor* methodDescriptor = serviceDescriptor->method(i);
        std::string method_name = methodDescriptor->name();
        serviceInfo.m_methodMap.insert({method_name, methodDescriptor});

        LOG_INFO("method_name: %s", method_name.c_str());
    }

    serviceInfo.m_service = service;
    m_serviceMap.insert({service_name, serviceInfo});
}

void CRpcProvider::run() {
    std::string ip = Application::getInstance().getConfig().getConfigItem("serverip");
    uint16_t port = atoi(Application::getInstance().getConfig().getConfigItem("serverport").c_str());
    muduo::net::InetAddress address(ip, port);

    muduo::net::TcpServer server(&m_loop, address, "CRpcProvider");
    server.setConnectionCallback(std::bind(&CRpcProvider::onConnection, this, _1));
    server.setMessageCallback(std::bind(&CRpcProvider::onMessage, this, _1, _2, _3));
    server.setThreadNum(4);

    ZkClient zkClient;
    zkClient.start();

    for (auto& s: m_serviceMap) {
        std::string service_path = "/" + s.first;
        zkClient.create(service_path.c_str(), nullptr, 0);
        for (auto& m: s.second.m_methodMap) {
            std::string method_path = service_path + "/" + m.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            zkClient.create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);

            std::cout << "CRpcProvider start service at ip: " << ip << " port: " << port << std::endl; 
        }
    }

    server.start();
    m_loop.loop();
}

// 处理新连接事件的函数回调
void CRpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        conn->shutdown();
    }
}

// 处理读写事件的函数回调
void CRpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,
                             muduo::net::Buffer* buffer,
                             muduo::Timestamp timestamp) {
    std::string recv_buf = buffer->retrieveAllAsString();

    // 约定前四个字节为消息头的长度
    // uint32_t header_size = atoi(recv_buf.substr(0, 4).c_str());
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    std::string header = recv_buf.substr(4, header_size);
    crpc::CRpcHeader crpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if (crpc_header.ParseFromString(header)) {
        service_name = crpc_header.service_name();
        method_name = crpc_header.method_name();
        args_size = crpc_header.args_size();
    } else {
        std::cout << "crpc_header: " << header << " parse error!" << std::endl;
    }

    // rpc方法的参数
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header: " << header << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

    auto s_it = m_serviceMap.find(service_name);
    if (s_it == m_serviceMap.end()) {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto m_it = s_it->second.m_methodMap.find(method_name);
    if (m_it == s_it->second.m_methodMap.end()) {
        std::cout << service_name << ": " << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service* service = s_it->second.m_service;
    const google::protobuf::MethodDescriptor* methodDescriptor = m_it->second;

    google::protobuf::Message* request = service->GetRequestPrototype(methodDescriptor).New();
    if (!request->ParseFromString(args_str)) {
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }

    google::protobuf::Message* response = service->GetResponsePrototype(methodDescriptor).New();

    // 绑定回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback<CRpcProvider, 
                                                                       const muduo::net::TcpConnectionPtr&, 
                                                                       google::protobuf::Message*>
                                                                      (this, &CRpcProvider::sendRpcResponse, conn, response);

    service->CallMethod(methodDescriptor, nullptr, request, response, done);
}

// 序列化rpc响应消息和发送
void CRpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr& conn,
                                   google::protobuf::Message* message) {
    std::string response_msg;
    if (message->SerializeToString(&response_msg)) {
        // 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方
        conn->send(response_msg);
    } else {
        std::cout << "serialize response_msg error!" << std::endl; 
    }
    
    conn->shutdown();
}
