#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "crpc_channel.h"
#include "crpc_header.pb.h"
#include "application.h"
#include "zk_client.h"
#include "crpc_controller.h"
#include "load_balance.h"

void CRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response, 
                    google::protobuf::Closure* done) {
    const google::protobuf::ServiceDescriptor* serviceDescriptor = method->service();
    std::string service_name = serviceDescriptor->name();
    std::string method_name = method->name();

    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        controller->SetFailed("serialize request error!");
        return;
    }

    crpc::CRpcHeader crpc_header;
    crpc_header.set_service_name(service_name);
    crpc_header.set_method_name(method_name);
    crpc_header.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string header;
    if (crpc_header.SerializeToString(&header)) {
        header_size = header.size();
    } else {
        controller->SetFailed("serialize header error!");
        return;
    }

    std::string send_str;
    send_str.insert(0, std::string((char*)&header_size, 4));
    send_str += header;
    send_str += args_str;

    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "============================================" << std::endl;

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd  ==  -1) {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    ZkClient zkCli;
    zkCli.start();
    //  /UserServiceRpc/Login
    std::string method_path = "/" + service_name + "/" + method_name;
    // 127.0.0.1:8000
    // std::string host_data = zkCli.getData(method_path.c_str());

    // addrs每个元素都是 "ip:port"
    LoadBalance* loadBalance = new ConsistentHashLoadBalance();
    std::string request_name = service_name + method_name;
    // std::string path = method_path + "/";
    std::vector<std::string> addrs = zkCli.getChildrenNodes(method_path.c_str());
    std::string host_data = loadBalance->doSelect(addrs, request_name, args_str);

    std::cout << "============================================" << std::endl;
    std::cout << "server_addr: " << host_data << std::endl; 
    std::cout << "============================================" << std::endl;

    if (host_data == "") {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1) {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); 

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }


    if (send(clientfd, send_str.c_str(), send_str.size(), 0) == -1) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    char recv_buf[1024] = {0};
    int recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if (!response->ParseFromArray(recv_buf, recv_size)) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! response_str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);
}