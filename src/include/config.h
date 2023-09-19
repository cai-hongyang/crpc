#ifndef CRPC_SRC_INCLUDE_CONFIG_H
#define CRPC_SRC_INCLUDE_CONFIG_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include "json.hpp"

using json =  nlohmann::json;

class Config {
public:
    void configure(const char* file_name);

    // 获取某个配置项信息
    std::string getConfigItem(const std::string& key);
private:
    json js;
    std::unordered_map<std::string, std::string> config_map;
};

#endif