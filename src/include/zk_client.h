#ifndef CRPC_SRC_INCLUDE_ZK_CLIENT_H
#define CRPC_SRC_INCLUDE_ZK_CLIENT_H

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>
#include <vector>

class ZkClient {
public:
    ZkClient();
    ~ZkClient();

    void start();

    void create(const char* path, const char* data, int len, int state = 0);

    std::string getData(const char* path);

    std::vector<std::string> getChildrenNodes(const char* path);
private:
    zhandle_t* m_zhandle;
};

#endif