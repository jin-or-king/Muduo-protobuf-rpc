#pragma once 
#include <queue>
#include <thread>
#include <mutex> // pthread_mutex_t
#include <condition_variable> // pthread_condition_t

// 异步写日志的日志队列
template <typename T>
class LockQueue
{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
public:
    // 多个worker线程都会写日志queue
    void Push(const T& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one();
    }

    // 一个线程读日志queue，写日志文件
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        // 队列为空就阻塞，不为空就退出循环
        while (m_queue.empty())
        {
            // 日志队列为空，线程进入wait状态(若有锁，则线程阻塞)
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
    bool IsEmpty();

};
