#include "mprpcconfig.h"
#include <iostream>

// 负责解析加载配置文件
void MprpcConfig::LoadConfigFile(const char* config_file)
{
    FILE* pf = fopen(config_file, "r");
    if (nullptr == pf)
    {
        cout << config_file << "is not exist!" << endl;
        exit(EXIT_FAILURE);
    }

    // 处理：1.注释  2.正确的配置项=  3.去掉开头多余的空格
    while(!feof(pf))
    {
        char buf[512] = {0};
        // 每次读取数据从stream中读取一行
        fgets(buf, 512, pf);

        // 去掉字符串前后多余空格，但是字符串中可能还有多余的空格
        string read_buf(buf);
        Trim(read_buf);

        // 判断#的注释
        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }
        
        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            // 配置不合法，不读取
            continue;
        }

        // 获取key并且去掉空格
        string key;
        string value;
        key = read_buf.substr(0, idx);
        Trim(key);

        // 获取value（127.0.0.1\n，去除1和\n之间的空格）
        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);

        // 解析后添加进入map中
        m_configMap.insert({key, value});
        
    }
}

// 查询配置项信息
string MprpcConfig::Load(const string& key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return " ";
    }
    
    return it->second;
}

void MprpcConfig::Trim(string& src_buf)
{
    
    // 去掉字符串前面多余的空格
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去掉字符后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 字符串后面有空格
        src_buf = src_buf.substr(0, idx + 1);
    }
}