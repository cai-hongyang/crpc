#include "crpc_controller.h"

CRpcController::CRpcController()
{
    m_failed = false;
    m_errText = "";
}

void CRpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}

bool CRpcController::Failed() const
{
    return m_failed;
}

std::string CRpcController::ErrorText() const
{
    return m_errText;
}

void CRpcController::SetFailed(const std::string& reason)
{
    m_failed = true;
    m_errText = reason;
}

// 目前未实现具体的功能
void CRpcController::StartCancel(){}
bool CRpcController::IsCanceled() const {return false;}
void CRpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}