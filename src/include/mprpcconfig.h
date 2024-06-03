#pragma once
#include <unordered_map>
using namespace std;

// 框架读取配置文件类
class MprpcConfig
{
public:

    // 负责解析加载配置文件
    void LoadConfigFile(const char* config_file);
    // 查询配置项信息
    string Load(const string& key);
    // 去掉前后空格
    void Trim(string& str);
private:
    unordered_map<string, string> m_configMap;

};