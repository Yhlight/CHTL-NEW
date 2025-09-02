#pragma once

#include "Error.h"
#include <vector>
#include <functional>
#include <memory>
#include <mutex>

namespace CHTL {

/**
 * 错误处理器回调函数类型
 */
using ErrorCallback = std::function<void(const CHTLError& error)>;

/**
 * 错误统计信息
 */
struct ErrorStatistics {
    size_t InfoCount = 0;
    size_t WarningCount = 0;
    size_t ErrorCount = 0;
    size_t FatalCount = 0;
    size_t CriticalCount = 0;
    
    size_t TotalCount() const {
        return InfoCount + WarningCount + ErrorCount + FatalCount + CriticalCount;
    }
    
    bool HasErrors() const {
        return ErrorCount > 0 || FatalCount > 0 || CriticalCount > 0;
    }
    
    bool HasFatalErrors() const {
        return FatalCount > 0 || CriticalCount > 0;
    }
};

/**
 * CHTL统一错误处理器
 * 负责收集、分类、报告和处理编译过程中的所有错误
 */
class CHTLErrorHandler {
private:
    std::vector<CHTLError> m_Errors;
    std::vector<ErrorCallback> m_Callbacks;
    ErrorStatistics m_Statistics;
    mutable std::mutex m_Mutex;
    
    bool m_EnableDetailedOutput;
    bool m_EnableSourceCodeContext;
    size_t m_MaxErrorCount;

public:
    /**
     * 构造函数
     */
    CHTLErrorHandler();
    
    /**
     * 析构函数
     */
    ~CHTLErrorHandler() = default;
    
    /**
     * 添加错误
     * @param error 错误对象
     */
    void AddError(const CHTLError& error);
    
    /**
     * 添加错误（便捷方法）
     * @param level 错误级别
     * @param type 错误类型
     * @param message 错误消息
     * @param position 错误位置
     * @param sourceCode 源代码
     */
    void AddError(ErrorLevel level, ErrorType type, const std::string& message,
                  const ErrorPosition& position = ErrorPosition(),
                  const std::string& sourceCode = "");
    
    /**
     * 添加警告
     * @param message 警告消息
     * @param position 错误位置
     */
    void AddWarning(const std::string& message, const ErrorPosition& position = ErrorPosition());
    
    /**
     * 添加信息
     * @param message 信息消息
     * @param position 位置
     */
    void AddInfo(const std::string& message, const ErrorPosition& position = ErrorPosition());
    
    /**
     * 注册错误回调
     * @param callback 回调函数
     */
    void RegisterCallback(ErrorCallback callback);
    
    /**
     * 获取所有错误
     * @return 错误列表
     */
    const std::vector<CHTLError>& GetErrors() const;
    
    /**
     * 获取指定级别的错误
     * @param level 错误级别
     * @return 错误列表
     */
    std::vector<CHTLError> GetErrorsByLevel(ErrorLevel level) const;
    
    /**
     * 获取指定类型的错误
     * @param type 错误类型
     * @return 错误列表
     */
    std::vector<CHTLError> GetErrorsByType(ErrorType type) const;
    
    /**
     * 获取错误统计
     * @return 错误统计信息
     */
    const ErrorStatistics& GetStatistics() const;
    
    /**
     * 检查是否有错误
     * @return 是否有错误
     */
    bool HasErrors() const;
    
    /**
     * 检查是否有致命错误
     * @return 是否有致命错误
     */
    bool HasFatalErrors() const;
    
    /**
     * 清除所有错误
     */
    void Clear();
    
    /**
     * 生成错误报告
     * @param includeSourceCode 是否包含源代码
     * @return 错误报告字符串
     */
    std::string GenerateReport(bool includeSourceCode = true) const;
    
    /**
     * 生成简要报告
     * @return 简要报告字符串
     */
    std::string GenerateSummary() const;
    
    /**
     * 设置详细输出
     * @param enable 是否启用
     */
    void SetDetailedOutput(bool enable) { m_EnableDetailedOutput = enable; }
    
    /**
     * 设置源代码上下文
     * @param enable 是否启用
     */
    void SetSourceCodeContext(bool enable) { m_EnableSourceCodeContext = enable; }
    
    /**
     * 设置最大错误数量
     * @param maxCount 最大错误数量
     */
    void SetMaxErrorCount(size_t maxCount) { m_MaxErrorCount = maxCount; }

private:
    /**
     * 更新统计信息
     * @param error 错误对象
     */
    void UpdateStatistics(const CHTLError& error);
    
    /**
     * 触发回调
     * @param error 错误对象
     */
    void TriggerCallbacks(const CHTLError& error);
};

/**
 * 全局错误处理器
 * 单例模式，提供全局错误处理访问
 */
class GlobalErrorHandler {
private:
    static std::unique_ptr<CHTLErrorHandler> s_Instance;
    static std::once_flag s_InitFlag;

public:
    /**
     * 获取全局错误处理器实例
     * @return 错误处理器指针
     */
    static CHTLErrorHandler* GetInstance();
    
    /**
     * 重置全局错误处理器
     */
    static void Reset();
    
    /**
     * 便捷方法：添加错误
     */
    static void AddError(ErrorLevel level, ErrorType type, const std::string& message,
                        const ErrorPosition& position = ErrorPosition(),
                        const std::string& sourceCode = "");
    
    /**
     * 便捷方法：添加警告
     */
    static void AddWarning(const std::string& message, const ErrorPosition& position = ErrorPosition());
    
    /**
     * 便捷方法：添加信息
     */
    static void AddInfo(const std::string& message, const ErrorPosition& position = ErrorPosition());
    
    /**
     * 便捷方法：检查是否有错误
     */
    static bool HasErrors();
    
    /**
     * 便捷方法：生成报告
     */
    static std::string GenerateReport();
};

} // namespace CHTL