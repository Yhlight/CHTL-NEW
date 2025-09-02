#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * 错误级别枚举
 */
enum class ErrorLevel {
    INFO,           // 信息
    WARNING,        // 警告
    ERROR,          // 错误
    FATAL,          // 致命错误
    CRITICAL        // 严重错误
};

/**
 * 错误类型枚举
 */
enum class ErrorType {
    LEXICAL_ERROR,      // 词法错误
    SYNTAX_ERROR,       // 语法错误
    SEMANTIC_ERROR,     // 语义错误
    CONSTRAINT_ERROR,   // 约束错误
    IMPORT_ERROR,       // 导入错误
    GENERATION_ERROR,   // 生成错误
    SYSTEM_ERROR,       // 系统错误
    CJMOD_ERROR        // CJMOD错误
};

/**
 * 错误位置信息
 */
struct ErrorPosition {
    size_t Line;            // 行号
    size_t Column;          // 列号
    size_t Position;        // 绝对位置
    std::string FileName;   // 文件名
    
    ErrorPosition(size_t line = 0, size_t column = 0, size_t position = 0, const std::string& fileName = "")
        : Line(line), Column(column), Position(position), FileName(fileName) {}
    
    std::string ToString() const;
};

/**
 * CHTL错误信息类
 */
class CHTLError {
private:
    ErrorLevel m_Level;
    ErrorType m_Type;
    std::string m_Message;
    std::string m_DetailedMessage;
    ErrorPosition m_Position;
    std::string m_SourceCode;
    std::vector<std::string> m_Suggestions;
    std::string m_ErrorCode;

public:
    /**
     * 构造函数
     */
    CHTLError(ErrorLevel level, ErrorType type, const std::string& message,
              const ErrorPosition& position = ErrorPosition(),
              const std::string& sourceCode = "");
    
    /**
     * 析构函数
     */
    ~CHTLError() = default;
    
    // Getter方法
    ErrorLevel GetLevel() const { return m_Level; }
    ErrorType GetType() const { return m_Type; }
    const std::string& GetMessage() const { return m_Message; }
    const std::string& GetDetailedMessage() const { return m_DetailedMessage; }
    const ErrorPosition& GetPosition() const { return m_Position; }
    const std::string& GetSourceCode() const { return m_SourceCode; }
    const std::vector<std::string>& GetSuggestions() const { return m_Suggestions; }
    const std::string& GetErrorCode() const { return m_ErrorCode; }
    
    // Setter方法
    void SetDetailedMessage(const std::string& detailedMessage) { m_DetailedMessage = detailedMessage; }
    void AddSuggestion(const std::string& suggestion) { m_Suggestions.push_back(suggestion); }
    void SetErrorCode(const std::string& errorCode) { m_ErrorCode = errorCode; }
    
    /**
     * 格式化错误信息
     * @param includeSourceCode 是否包含源代码
     * @return 格式化的错误信息
     */
    std::string FormatError(bool includeSourceCode = true) const;
    
    /**
     * 获取错误级别字符串
     * @return 错误级别字符串
     */
    std::string GetLevelString() const;
    
    /**
     * 获取错误类型字符串
     * @return 错误类型字符串
     */
    std::string GetTypeString() const;
    
    /**
     * 检查是否为致命错误
     * @return 是否为致命错误
     */
    bool IsFatal() const { return m_Level == ErrorLevel::FATAL || m_Level == ErrorLevel::CRITICAL; }
    
    /**
     * 检查是否为警告
     * @return 是否为警告
     */
    bool IsWarning() const { return m_Level == ErrorLevel::WARNING; }
};

/**
 * 错误工厂类
 * 用于快速创建常见类型的错误
 */
class ErrorFactory {
public:
    /**
     * 创建词法错误
     */
    static CHTLError CreateLexicalError(const std::string& message, const ErrorPosition& position, 
                                       const std::string& sourceCode = "");
    
    /**
     * 创建语法错误
     */
    static CHTLError CreateSyntaxError(const std::string& message, const ErrorPosition& position,
                                      const std::string& sourceCode = "");
    
    /**
     * 创建语义错误
     */
    static CHTLError CreateSemanticError(const std::string& message, const ErrorPosition& position,
                                        const std::string& sourceCode = "");
    
    /**
     * 创建约束错误
     */
    static CHTLError CreateConstraintError(const std::string& message, const ErrorPosition& position,
                                          const std::string& sourceCode = "");
    
    /**
     * 创建导入错误
     */
    static CHTLError CreateImportError(const std::string& message, const ErrorPosition& position,
                                      const std::string& sourceCode = "");
    
    /**
     * 创建生成错误
     */
    static CHTLError CreateGenerationError(const std::string& message, const ErrorPosition& position,
                                          const std::string& sourceCode = "");
    
    /**
     * 创建CJMOD错误
     */
    static CHTLError CreateCJMODError(const std::string& message, const ErrorPosition& position,
                                     const std::string& sourceCode = "");
    
    /**
     * 创建系统错误
     */
    static CHTLError CreateSystemError(const std::string& message, const ErrorPosition& position = ErrorPosition());
};

} // namespace CHTL