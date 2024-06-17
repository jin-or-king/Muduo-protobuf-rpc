#include "mprpccontroller.h"

MprpcController::MprpcController()
{
    m_failed = false;
    m_errText = "";
}

// 将状态重置为初始值
void MprpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}

// 判断是否出现错误（false没有出现，true出现错误）
bool MprpcController::Failed() const
{
    return m_failed;
}

// 获取错误信息
std::string MprpcController::ErrorText() const
{
    return m_errText;
}

void MprpcController::SetFailed(const std::string& reason)
{
    m_failed = true;
    m_errText = reason;
}   

// 目前未实现的功能
void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const{return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback){}