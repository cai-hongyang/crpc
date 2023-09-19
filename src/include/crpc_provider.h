#ifndef CRPC_SRC_INCLUDE_CRPC_PROVIDER_H
#define CRPC_SRC_INCLUDE_CRPC_PROVIDER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <string>
#include <unordered_map>
#include <functional>

class CRpcProvider {
public:
    void publishService(google::protobuf::Service* service);

    void run();
private:
    // 处理新连接事件的函数回调
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    // 处理读写事件的函数回调
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buffer,
                   muduo::Timestamp timestamp);

    // 序列化rpc响应消息和发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr& conn,
                         google::protobuf::Message* message);

    muduo::net::EventLoop m_loop;

    struct ServiceInfo {
        google::protobuf::Service* m_service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };

    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
};

#endif