#include "log.h"
#include <time.h>
#include <iostream>

Log::Log() {
    std::thread writeLogTask([&]() {
        for ( ; ; ) {
            // 获取当前日期，将日志信息写入到对应的日志文件中
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", 
                    nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            FILE* p = fopen(file_name, "a+");
            if (p == nullptr) {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_logQueue.pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d-%d-%d => [%s] ", 
                    nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec,
                    (m_level == INFO ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), p);
            fclose(p);
        }
        
    });

    writeLogTask.detach();
}

Log& Log::getInstance() {
    static Log log;
    return log;
}

void Log::setLogLevel(LogLevel level) {
    m_level = level;
}

// 把日志信息记录到缓冲队列 m_logQueue
void Log::recordLog(std::string msg) {
    m_logQueue.push(msg);
}