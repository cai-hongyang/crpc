#ifndef CRPC_SRC_INCLUDE_LOG_H
#define CRPC_SRC_INCLUDE_LOG_H

#include "log_queue.h"
#include <string>

#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Log& log = Log::getInstance(); \
        log.setLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        log.recordLog(c); \
    } while(0) \

#define LOG_ERROR(logmsgformat, ...) \
    do \
    {  \
        Log& log = Log::getInstance(); \
        log.setLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        log.recordLog(c); \
    } while(0) \

enum LogLevel {
    INFO, // 普通信息
    ERROR, // 错误信息
};

class Log {
public:
    static Log& getInstance();

    void setLogLevel(LogLevel level);

    void recordLog(std::string msg);
private:
    Log();
    Log(const Log&) = delete;
    Log(Log&&) = delete;

    int m_level;
    LogQueue<std::string> m_logQueue;
};

#endif