#include "Error.h"
#include <sstream>
#include <iomanip>

namespace CHTL {

// ErrorPosition实现
std::string ErrorPosition::ToString() const {
    std::ostringstream oss;
    if (!FileName.empty()) {
        oss << FileName << ":";
    }
    oss << Line << ":" << Column;
    return oss.str();
}

// CHTLError实现
CHTLError::CHTLError(ErrorLevel level, ErrorType type, const std::string& message,
                     const ErrorPosition& position, const std::string& sourceCode)
    : m_Level(level), m_Type(type), m_Message(message), m_Position(position), m_SourceCode(sourceCode) {
    
    // 生成错误代码
    std::ostringstream codeStream;
    codeStream << "CHTL" << std::setfill('0') << std::setw(4) << static_cast<int>(type) * 100 + static_cast<int>(level);
    m_ErrorCode = codeStream.str();
}

std::string CHTLError::FormatError(bool includeSourceCode) const {
    std::ostringstream oss;
    
    // 错误级别和类型
    oss << "[" << GetLevelString() << "] " << GetTypeString() << " (" << m_ErrorCode << ")" << std::endl;
    
    // 错误位置
    if (m_Position.Line > 0) {
        oss << "位置: " << m_Position.ToString() << std::endl;
    }
    
    // 错误消息
    oss << "消息: " << m_Message << std::endl;
    
    // 详细消息
    if (!m_DetailedMessage.empty()) {
        oss << "详细: " << m_DetailedMessage << std::endl;
    }
    
    // 源代码上下文
    if (includeSourceCode && !m_SourceCode.empty() && m_Position.Line > 0) {
        oss << "源代码:" << std::endl;
        
        std::istringstream sourceStream(m_SourceCode);
        std::string line;
        size_t currentLine = 1;
        
        // 显示错误行前后的上下文
        std::vector<std::string> lines;
        while (std::getline(sourceStream, line)) {
            lines.push_back(line);
        }
        
        size_t startLine = (m_Position.Line > 2) ? m_Position.Line - 2 : 1;
        size_t endLine = std::min(m_Position.Line + 2, lines.size());
        
        for (size_t i = startLine; i <= endLine && i <= lines.size(); ++i) {
            bool isErrorLine = (i == m_Position.Line);
            oss << std::setw(4) << i << " | ";
            
            if (isErrorLine) {
                oss << ">>> " << lines[i-1] << std::endl;
                
                // 显示错误位置指示器
                if (m_Position.Column > 0) {
                    oss << "     | " << std::string(m_Position.Column - 1, ' ') << "^" << std::endl;
                }
            } else {
                oss << "    " << lines[i-1] << std::endl;
            }
        }
    }
    
    // 建议
    if (!m_Suggestions.empty()) {
        oss << "建议:" << std::endl;
        for (const auto& suggestion : m_Suggestions) {
            oss << "  • " << suggestion << std::endl;
        }
    }
    
    return oss.str();
}

std::string CHTLError::GetLevelString() const {
    switch (m_Level) {
        case ErrorLevel::INFO: return "信息";
        case ErrorLevel::WARNING: return "警告";
        case ErrorLevel::ERROR: return "错误";
        case ErrorLevel::FATAL: return "致命错误";
        case ErrorLevel::CRITICAL: return "严重错误";
        default: return "未知";
    }
}

std::string CHTLError::GetTypeString() const {
    switch (m_Type) {
        case ErrorType::LEXICAL_ERROR: return "词法错误";
        case ErrorType::SYNTAX_ERROR: return "语法错误";
        case ErrorType::SEMANTIC_ERROR: return "语义错误";
        case ErrorType::CONSTRAINT_ERROR: return "约束错误";
        case ErrorType::IMPORT_ERROR: return "导入错误";
        case ErrorType::GENERATION_ERROR: return "生成错误";
        case ErrorType::SYSTEM_ERROR: return "系统错误";
        case ErrorType::CJMOD_ERROR: return "CJMOD错误";
        default: return "未知错误";
    }
}

// ErrorFactory实现
CHTLError ErrorFactory::CreateLexicalError(const std::string& message, const ErrorPosition& position, 
                                           const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::LEXICAL_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查字符编码是否为UTF-8");
    error.AddSuggestion("确认词法符号是否正确");
    return error;
}

CHTLError ErrorFactory::CreateSyntaxError(const std::string& message, const ErrorPosition& position,
                                          const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查语法结构是否完整");
    error.AddSuggestion("确认括号、引号是否匹配");
    return error;
}

CHTLError ErrorFactory::CreateSemanticError(const std::string& message, const ErrorPosition& position,
                                            const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::SEMANTIC_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查变量或函数是否已定义");
    error.AddSuggestion("确认类型是否匹配");
    return error;
}

CHTLError ErrorFactory::CreateConstraintError(const std::string& message, const ErrorPosition& position,
                                              const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::CONSTRAINT_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查except约束条件");
    error.AddSuggestion("确认语法边界是否正确");
    return error;
}

CHTLError ErrorFactory::CreateImportError(const std::string& message, const ErrorPosition& position,
                                          const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::IMPORT_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查导入路径是否正确");
    error.AddSuggestion("确认模块是否存在");
    return error;
}

CHTLError ErrorFactory::CreateGenerationError(const std::string& message, const ErrorPosition& position,
                                              const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::GENERATION_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查代码生成配置");
    error.AddSuggestion("确认目标语言语法");
    return error;
}

CHTLError ErrorFactory::CreateCJMODError(const std::string& message, const ErrorPosition& position,
                                         const std::string& sourceCode) {
    CHTLError error(ErrorLevel::ERROR, ErrorType::CJMOD_ERROR, message, position, sourceCode);
    error.AddSuggestion("检查CJMOD扩展是否正确加载");
    error.AddSuggestion("确认CJMOD语法是否正确");
    return error;
}

CHTLError ErrorFactory::CreateSystemError(const std::string& message, const ErrorPosition& position) {
    CHTLError error(ErrorLevel::FATAL, ErrorType::SYSTEM_ERROR, message, position);
    error.AddSuggestion("检查系统环境配置");
    error.AddSuggestion("确认文件权限和路径");
    return error;
}

} // namespace CHTL