#include <unistd.h>
#include <string>
#include <iostream>
#include "application.h"

Config Application::m_config;

void showArgsHelp() {
    std::cout << "format: command -i <configfile>" << std::endl;
}

void Application::init(int argc, char** argv) {
    if (argc < 2) {
        showArgsHelp();
        exit(EXIT_FAILURE);
    }

    int option;
    char* file_name;
    while ((option = getopt(argc, argv, "i:")) != -1) {
        switch(option) {
            case 'i':
                file_name = optarg;
                break;
            case '?':
                showArgsHelp();
                exit(EXIT_FAILURE);
            case ':':
                showArgsHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    m_config.configure(file_name);
}

Application& Application::getInstance() {
    static Application app;
    return app;
}

Config& Application::getConfig() {
    return m_config;
}