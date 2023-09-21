#include "load_balance.h"


ConsistentHashSelector::ConsistentHashSelector(const std::vector<std::string>& invokers, int replicaNumber) {
    virtualInvokers.clear();
    for (const std::string& invoker : invokers) {
        for (int i = 0; i < replicaNumber / 4; i++) {
            std::string digest = md5(invoker + std::to_string(i));
            for (int h = 0; h < 4; h++) {
                uint32_t m = hash(digest, h);
                virtualInvokers[m] = invoker;
            }
        }
    }
}

std::string ConsistentHashSelector::md5(const std::string& key) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(key.c_str()), key.size(), digest);

        std::stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
        }

        return ss.str();
}

    
uint32_t ConsistentHashSelector::hash(const std::string& digest, int idx) {
    uint32_t result = (static_cast<uint32_t>(digest[3 + idx * 4] & 255) << 24)
        | (static_cast<uint32_t>(digest[2 + idx * 4] & 255) << 16)
        | (static_cast<uint32_t>(digest[1 + idx * 4] & 255) << 8)
        | (static_cast<uint32_t>(digest[idx * 4] & 255)) & 4294967295L;

    return result;
}

std::string ConsistentHashSelector::select(const std::string& rpcServiceKey) {
            std::string digest = md5(rpcServiceKey);
            return selectForKey(hash(digest, 0));
}


std::string ConsistentHashSelector::selectForKey(uint32_t hashCode) {
    auto entry = virtualInvokers.lower_bound(hashCode);

    if (entry == virtualInvokers.end()) {
        entry = virtualInvokers.begin();
    }

    return entry->second;
}

uint32_t ConsistentHashSelector::getIdentityHashCode() {
    return identityHashCode;
}

void ConsistentHashSelector::setIdentityHashCode(uint32_t hashcode) {
    identityHashCode = hashcode;
}



std::string ConsistentHashLoadBalance::doSelect(const std::vector<std::string>& serviceAddresses, const std::string& rpcServiceName, const std::string& args) {
    std::string hash_str;
    for (int i = 0; i < serviceAddresses.size(); i++) {
        hash_str += serviceAddresses[i];
    }
    uint32_t identityHashCode = std::hash<std::string>{}(hash_str);
    ConsistentHashSelector* selector = selectors[rpcServiceName];

    // Check for updates
    if (selector == nullptr || selector->getIdentityHashCode() != identityHashCode) {
        selector = new ConsistentHashSelector(serviceAddresses, 160);
        selector->setIdentityHashCode(identityHashCode);
        selectors.insert({rpcServiceName, selector});
    }

    return selector->select(rpcServiceName + args);
}





// int main() {
//     ConsistentHashLoadBalance loadBalance;
//     std::vector<std::string> serviceAddresses = {"Address1", "Address2", "Address3"};
//     std::string rpcServiceName = "ServiceName";
//     std::string selectedAddress = loadBalance.doSelect(serviceAddresses, rpcServiceName);
//     std::cout << "Selected Address: " << selectedAddress << std::endl;

//     return 0;
// }