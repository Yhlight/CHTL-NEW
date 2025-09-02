#include "ErrorHandler.h"
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iostream>

namespace CHTL {

// CHTLErrorHandler实现
CHTLErrorHandler::CHTLErrorHandler() 
    : m_EnableDetailedOutput(true), m_EnableSourceCodeContext(true), m_MaxErrorCount(100) {
}

void CHTLErrorHandler::AddError(const CHTLError& error) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    // 检查是否超过最大错误数量
    if (m_Errors.size() >= m_MaxErrorCount) {
        return;
    }
    
    m_Errors.push_back(error);
    UpdateStatistics(error);
    TriggerCallbacks(error);
}

void CHTLErrorHandler::AddError(ErrorLevel level, ErrorType type, const std::string& message,
                                const ErrorPosition& position, const std::string& sourceCode) {
    CHTLError error(level, type, message, position, sourceCode);
    AddError(error);
}

void CHTLErrorHandler::AddWarning(const std::string& message, const ErrorPosition& position) {
    AddError(ErrorLevel::WARNING, ErrorType::SYSTEM_ERROR, message, position);
}

void CHTLErrorHandler::AddInfo(const std::string& message, const ErrorPosition& position) {
    AddError(ErrorLevel::INFO, ErrorType::SYSTEM_ERROR, message, position);
}

void CHTLErrorHandler::RegisterCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Callbacks.push_back(callback);
}

const std::vector<CHTLError>& CHTLErrorHandler::GetErrors() const {
    return m_Errors;
}

std::vector<CHTLError> CHTLErrorHandler::GetErrorsByLevel(ErrorLevel level) const {
    std::vector<CHTLError> result;
    std::copy_if(m_Errors.begin(), m_Errors.end(), std::back_inserter(result),
                 [level](const CHTLError& error) { return error.GetLevel() == level; });
    return result;
}

std::vector<CHTLError> CHTLErrorHandler::GetErrorsByType(ErrorType type) const {
    std::vector<CHTLError> result;
    std::copy_if(m_Errors.begin(), m_Errors.end(), std::back_inserter(result),
                 [type](const CHTLError& error) { return error.GetType() == type; });
    return result;
}

const ErrorStatistics& CHTLErrorHandler::GetStatistics() const {
    return m_Statistics;
}

bool CHTLErrorHandler::HasErrors() const {
    return m_Statistics.HasErrors();
}

bool CHTLErrorHandler::HasFatalErrors() const {
    return m_Statistics.HasFatalErrors();
}

void CHTLErrorHandler::Clear() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Errors.clear();
    m_Statistics = ErrorStatistics();
}

std::string CHTLErrorHandler::GenerateReport(bool includeSourceCode) const {
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    oss << "║                                🔍 CHTL错误报告 🔍                                          ║" << std::endl;
    oss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // 统计信息
    oss << "\n📊 错误统计:" << std::endl;
    oss << "  信息: " << m_Statistics.InfoCount << std::endl;
    oss << "  警告: " << m_Statistics.WarningCount << std::endl;
    oss << "  错误: " << m_Statistics.ErrorCount << std::endl;
    oss << "  致命错误: " << m_Statistics.FatalCount << std::endl;
    oss << "  严重错误: " << m_Statistics.CriticalCount << std::endl;
    oss << "  总计: " << m_Statistics.TotalCount() << std::endl;
    
    if (m_Errors.empty()) {
        oss << "\n✅ 没有发现任何错误！编译成功。" << std::endl;
        return oss.str();
    }
    
    // 详细错误信息
    oss << "\n📋 详细错误信息:" << std::endl;
    oss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    for (size_t i = 0; i < m_Errors.size(); ++i) {
        const auto& error = m_Errors[i];
        
        oss << "\n错误 #" << (i + 1) << ":" << std::endl;
        oss << error.FormatError(includeSourceCode && m_EnableSourceCodeContext);
        
        if (i < m_Errors.size() - 1) {
            oss << std::string(80, '-') << std::endl;
        }
    }
    
    return oss.str();
}

std::string CHTLErrorHandler::GenerateSummary() const {
    std::ostringstream oss;
    
    oss << "CHTL编译器错误摘要: ";
    
    if (m_Statistics.TotalCount() == 0) {
        oss << "✅ 编译成功，无错误";
    } else {
        oss << "总计 " << m_Statistics.TotalCount() << " 个问题";
        
        if (m_Statistics.ErrorCount > 0) {
            oss << " (错误: " << m_Statistics.ErrorCount << ")";
        }
        if (m_Statistics.WarningCount > 0) {
            oss << " (警告: " << m_Statistics.WarningCount << ")";
        }
        if (m_Statistics.FatalCount > 0) {
            oss << " (致命: " << m_Statistics.FatalCount << ")";
        }
    }
    
    return oss.str();
}

void CHTLErrorHandler::UpdateStatistics(const CHTLError& error) {
    switch (error.GetLevel()) {
        case ErrorLevel::INFO:
            m_Statistics.InfoCount++;
            break;
        case ErrorLevel::WARNING:
            m_Statistics.WarningCount++;
            break;
        case ErrorLevel::ERROR:
            m_Statistics.ErrorCount++;
            break;
        case ErrorLevel::FATAL:
            m_Statistics.FatalCount++;
            break;
        case ErrorLevel::CRITICAL:
            m_Statistics.CriticalCount++;
            break;
    }
}

void CHTLErrorHandler::TriggerCallbacks(const CHTLError& error) {
    for (const auto& callback : m_Callbacks) {
        try {
            callback(error);
        } catch (...) {
            // 忽略回调异常，避免无限递归
        }
    }
}

// GlobalErrorHandler实现
std::unique_ptr<CHTLErrorHandler> GlobalErrorHandler::s_Instance = nullptr;
std::once_flag GlobalErrorHandler::s_InitFlag;

CHTLErrorHandler* GlobalErrorHandler::GetInstance() {
    std::call_once(s_InitFlag, []() {
        s_Instance = std::make_unique<CHTLErrorHandler>();
    });
    return s_Instance.get();
}

void GlobalErrorHandler::Reset() {
    if (s_Instance) {
        s_Instance->Clear();
    }
}

void GlobalErrorHandler::AddError(ErrorLevel level, ErrorType type, const std::string& message,
                                  const ErrorPosition& position, const std::string& sourceCode) {
    GetInstance()->AddError(level, type, message, position, sourceCode);
}

void GlobalErrorHandler::AddWarning(const std::string& message, const ErrorPosition& position) {
    GetInstance()->AddWarning(message, position);
}

void GlobalErrorHandler::AddInfo(const std::string& message, const ErrorPosition& position) {
    GetInstance()->AddInfo(message, position);
}

bool GlobalErrorHandler::HasErrors() {
    return GetInstance()->HasErrors();
}

std::string GlobalErrorHandler::GenerateReport() {
    return GetInstance()->GenerateReport();
}

} // namespace CHTL