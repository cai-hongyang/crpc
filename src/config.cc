#include "config.h"

void Config::configure(const char* file_name){
	std::ifstream is(file_name);  //打开文件，以二进制方式打开文件
	if (!is.is_open()) {
        std::cout << "open file failed" << std::endl;
    }

    is >> js;
    config_map.insert({"serverip", js["serverip"]});
    config_map.insert({"serverport", js["serverport"]});
    config_map.insert({"zk", js["zk"]});

	is.close();
}

std::string Config::getConfigItem(const std::string& key) {
    auto it = config_map.find(key);
    if (it == config_map.end()) {
        return "";
    }

    return it->second;
}