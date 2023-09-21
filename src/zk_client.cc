#include "zk_client.h"
#include "application.h"
#include <iostream>

void global_watcher(zhandle_t* zh, int type, int state, 
                    const char* path, void* watcherCtx) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr) {
}

ZkClient::~ZkClient() {
    if (m_zhandle == nullptr) {
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::start() {
    std::string address = Application::getInstance().getConfig().getConfigItem("zk");

    m_zhandle = zookeeper_init(address.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (m_zhandle == nullptr) {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    std::cout << "zookeeper_init sucess!" << std::endl;
}

void ZkClient::create(const char* path, const char* data, int len, int state) {
    char path_buf[128];
    int buf_len = sizeof(path_buf);
    int flag;

    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (flag == ZNONODE) {
        flag = zoo_create(m_zhandle, path, data, len, &ZOO_OPEN_ACL_UNSAFE, state, path_buf, buf_len);
        if (flag == ZOK) {
            std::cout << "znode create sucess... path: " << path << std::endl;
        } else {
            std::cout << "flag: " << flag << std::endl;
            std::cout << "znode create error... path: : " << path << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::getData(const char* path) {
    char buf[64];
    int buf_len = sizeof(buf);
    int flag = zoo_get(m_zhandle, path, 0, buf, &buf_len, nullptr);
    if (flag == ZOK) {
        return buf;
    } else {
        std::cout << "get znode error... path: " << path << std::endl;
        return "";
    }
}


// std::string = "ip:port"
std::vector<std::string> ZkClient::getChildrenNodes(const char* path) {
    String_vector strs;
    std::vector<std::string> res;
    int flag = zoo_get_children(m_zhandle, path, 0, &strs);
    if (flag == ZOK) {
        int n = strs.count;
        for (int i = 0; i < n; i++) {
            res.push_back(strs.data[i]);
        }
    } else {
        std::cout << "get children znodes error... path: " << path << std::endl;
        return {};
    }

    return res;
}