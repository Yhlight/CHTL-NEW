#ifndef CHTL_COMMON_ERROR_H
#define CHTL_COMMON_ERROR_H

#include <exception>
#include <string>
#include <sstream>

namespace CHTL {

/**
 * CHTL基础异常类
 */
class CHTLException : public std::exception {
protected:
    std::string message;
    std::string file;
    size_t line;
    size_t column;
    
public:
    CHTLException(const std::string& msg, const std::string& f = "", 
                  size_t l = 0, size_t c = 0)
        : message(msg), file(f), line(l), column(c) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    std::string GetDetailedMessage() const {
        std::ostringstream oss;
        oss << message;
        if (!file.empty()) {
            oss << " (文件: " << file;
            if (line > 0) {
                oss << ", 行: " << line;
                if (column > 0) {
                    oss << ", 列: " << column;
                }
            }
            oss << ")";
        }
        return oss.str();
    }
};

/**
 * 词法分析错误
 */
class LexicalError : public CHTLException {
public:
    LexicalError(const std::string& msg, const std::string& file = "", 
                 size_t line = 0, size_t column = 0)
        : CHTLException("词法错误: " + msg, file, line, column) {}
};

/**
 * 语法分析错误
 */
class SyntaxError : public CHTLException {
public:
    SyntaxError(const std::string& msg, const std::string& file = "", 
                size_t line = 0, size_t column = 0)
        : CHTLException("语法错误: " + msg, file, line, column) {}
};

/**
 * 语义分析错误
 */
class SemanticError : public CHTLException {
public:
    SemanticError(const std::string& msg, const std::string& file = "", 
                  size_t line = 0, size_t column = 0)
        : CHTLException("语义错误: " + msg, file, line, column) {}
};

/**
 * 导入错误
 */
class ImportError : public CHTLException {
public:
    ImportError(const std::string& msg, const std::string& file = "", 
                size_t line = 0, size_t column = 0)
        : CHTLException("导入错误: " + msg, file, line, column) {}
};

/**
 * 运行时错误
 */
class RuntimeError : public CHTLException {
public:
    RuntimeError(const std::string& msg, const std::string& file = "", 
                 size_t line = 0, size_t column = 0)
        : CHTLException("运行时错误: " + msg, file, line, column) {}
};

} // namespace CHTL

#endif // CHTL_COMMON_ERROR_H