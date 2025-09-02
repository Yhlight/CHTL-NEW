#ifndef CHTL_COMMON_LOGGER_H
#define CHTL_COMMON_LOGGER_H

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>

namespace CHTL {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
private:
    static bool debugMode;
    
    static std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string GetLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "[DEBUG]";
            case LogLevel::Info: return "[INFO]";
            case LogLevel::Warning: return "[警告]";
            case LogLevel::Error: return "[错误]";
            case LogLevel::Fatal: return "[致命错误]";
            default: return "[未知]";
        }
    }
    
public:
    static void SetDebugMode(bool enable) {
        debugMode = enable;
    }
    
    static void Log(LogLevel level, const std::string& message, 
                    const std::string& file = "", int line = 0) {
        if (level == LogLevel::Debug && !debugMode) {
            return;
        }
        
        std::ostream& out = (level >= LogLevel::Error) ? std::cerr : std::cout;
        
        out << GetTimestamp() << " " << GetLevelString(level) << " " << message;
        
        if (!file.empty()) {
            out << " (" << file;
            if (line > 0) {
                out << ":" << line;
            }
            out << ")";
        }
        
        out << std::endl;
    }
    
    static void Debug(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::Debug, message, file, line);
    }
    
    static void Info(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::Info, message, file, line);
    }
    
    static void Warning(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::Warning, message, file, line);
    }
    
    static void Error(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::Error, message, file, line);
    }
    
    static void Fatal(const std::string& message, const std::string& file = "", int line = 0) {
        Log(LogLevel::Fatal, message, file, line);
    }
};

// 静态成员初始化
inline bool Logger::debugMode = false;

} // namespace CHTL

// 便捷宏定义
#define LOG_DEBUG(msg) CHTL::Logger::Debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) CHTL::Logger::Info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) CHTL::Logger::Warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) CHTL::Logger::Error(msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) CHTL::Logger::Fatal(msg, __FILE__, __LINE__)

#endif // CHTL_COMMON_LOGGER_H