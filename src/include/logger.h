#pragma once
#include <string>
#include "lockqueue.h"

enum class LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};

// Mprpc框架提供的日志系统
class Logger
{
public:
    // 单例日志
    static Logger& GetInstance();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);
private:
    int m_loglevel; // 记录日志级别
    // 日志缓冲队列
    LockQueue<std::string> m_lckQue;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

// 定义宏？？？
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger& logger = Logger::GetInstance(); \
        logger.SetLogLevel(LogLevel::INFO); \ 
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \ 
        logger.Log(c); \
    } while(0);

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        Logger& logger = Logger::GetInstance(); \
        logger.SetLogLevel(LogLevel::ERROR); \ 
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \ 
        logger.Log(c); \
    } while(0);