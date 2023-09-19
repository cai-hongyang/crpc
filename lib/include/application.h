#ifndef CRPC_SRC_INCLUDE_APPLICATION_H
#define CRPC_SRC_INCLUDE_APPLICATION_H

#include "config.h"
#include "crpc_channel.h"
#include "crpc_controller.h"

class Application {
public:
    static void init(int argc, char** argv);

    static Application& getInstance();

    static Config& getConfig();
private:
    Application() {}
    Application(const Application&) = delete;
    Application(Application&&) = delete;

    static Config m_config;
};

#endif