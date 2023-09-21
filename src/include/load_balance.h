#ifndef CRPC_SRC_INCLUDE_LOAD_BALANCE_H
#define CRPC_SRC_INCLUDE_LOAD_BALANCE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <cmath>
#include <cstdint>
#include <openssl/md5.h>

// 负载均衡类
class LoadBalance {
public:
    virtual std::string doSelect(const std::vector<std::string>& serviceAddresses, const std::string& rpcServiceName, const std::string& args) = 0;
};

class ConsistentHashSelector {
public:
    // ConsistentHashSelector() = default;
    // ConsistentHashSelector(const ConsistentHashSelector&) = default;
    // ~ConsistentHashSelector() = default;
    ConsistentHashSelector(const std::vector<std::string>& invokers, int replicaNumber);

    std::string md5(const std::string& key);
    
    uint32_t hash(const std::string& digest, int idx);

    std::string select(const std::string& rpcServiceKey);

    std::string selectForKey(uint32_t hashCode);

    uint32_t getIdentityHashCode();

    void setIdentityHashCode(uint32_t hashcode);

private:
    std::map<uint32_t, std::string> virtualInvokers;
    
    uint32_t identityHashCode;      
};

class ConsistentHashLoadBalance : public LoadBalance {
public:
    // ConsistentHashLoadBalance() = default;
    // ConsistentHashLoadBalance(const ConsistentHashLoadBalance&) = default;
    // ~ConsistentHashLoadBalance() = default;

    std::string doSelect(const std::vector<std::string>& serviceAddresses, const std::string& rpcServiceName, const std::string& args);
private:
    std::unordered_map<std::string, ConsistentHashSelector*> selectors;
};



#endif