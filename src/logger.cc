#include "logger.h"
#include <iostream>

// 单例日志
Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        for (;;)
        {
            // 获取当前日期，然后取日志信息，写入相应的文件中a+
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
            
            FILE* pf = fopen(file_name, "a+");
            if (pf == nullptr)
            {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }
            
            std::string msg = m_lckQue.Pop();
            // 每条信息前面添加时间
            char time_buf[128];
            sprintf(time_buf, "%d-%d-%d-->[%s] ", 
                    nowtm->tm_hour, 
                    nowtm->tm_min, 
                    nowtm->tm_sec,
                    (m_loglevel == static_cast<int>(LogLevel::INFO) ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            // 如果队列为空，就可以先把文件关闭，假如一直有msg在队列中，就不用立即关闭
            fputs(msg.c_str(), pf);
            if (true)
            {
                fclose(pf);
            }


        }
        
    });
    writeLogTask.detach();
}

// 设置日志级别
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = static_cast<int>(level);
}

// 写日志，只需要将msg放入消息队列中就行
void Logger::Log(std::string msg)
{
    m_lckQue.Push(msg);
}