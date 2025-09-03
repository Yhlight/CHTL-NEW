#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    // 代码类型枚举
    enum class CodeType {
        CHTL,       // CHTL代码片段
        CHTL_JS,    // CHTL JS代码片段
        CSS,        // CSS代码片段
        JS          // JavaScript代码片段
    };

    // 代码片段结构
    struct CodeChunk {
        CodeType type;          // 代码类型
        std::string content;    // 代码内容
        size_t startPos;        // 起始位置
        size_t endPos;          // 结束位置
        int lineNumber;         // 行号
        int columnNumber;       // 列号
        
        CodeChunk(CodeType t, const std::string& c, size_t start, size_t end, int line, int col)
            : type(t), content(c), startPos(start), endPos(end), lineNumber(line), columnNumber(col) {}
    };

    // CHTL统一扫描器
    // 核心功能：精准代码切割，基于可变长度切片实现
    // 严格按照目标规划.ini中的要求实现
    class CHTLUnifiedScanner {
    private:
        std::string sourceCode;         // 源代码
        size_t currentPos;              // 当前位置
        int currentLine;                // 当前行号
        int currentColumn;              // 当前列号
        std::vector<CodeChunk> chunks;  // 切割后的代码片段

        // 私有方法
        void AdvancePosition(char c);                           // 前进位置并更新行列号
        bool IsAtEnd() const;                                   // 是否到达文件末尾
        char PeekChar(size_t offset = 0) const;                // 查看字符但不前进
        std::string PeekString(size_t length) const;           // 查看字符串但不前进
        
        // 核心扫描方法
        bool ScanCHTLBlock(size_t& endPos);                    // 扫描CHTL代码块
        bool ScanCHTLJSBlock(size_t& endPos);                  // 扫描CHTL JS代码块
        bool ScanCSSBlock(size_t& endPos);                     // 扫描CSS代码块
        bool ScanJSBlock(size_t& endPos);                      // 扫描JS代码块
        
        // 辅助判断方法
        bool IsCHTLKeyword(const std::string& text) const;     // 判断是否为CHTL关键字
        bool IsCHTLJSKeyword(const std::string& text) const;   // 判断是否为CHTL JS关键字
        bool IsValidCHTLFragment(const std::string& fragment) const;    // 判断是否为有效CHTL片段
        bool IsValidCHTLJSFragment(const std::string& fragment) const;  // 判断是否为有效CHTL JS片段
        
        // 可变长度切片相关方法
        bool ShouldExtendSlice(size_t currentEnd) const;       // 判断是否需要扩展切片
        void PerformMinimalUnitCutting(std::string& fragment);  // 执行最小语法单元切割
        
        // 边界检测方法
        bool IsBlockBoundary(size_t pos) const;                // 检测块边界
        bool IsStatementBoundary(size_t pos) const;            // 检测语句边界

    public:
        explicit CHTLUnifiedScanner(const std::string& source);
        ~CHTLUnifiedScanner() = default;

        // 主要扫描方法
        std::vector<CodeChunk> Scan();                          // 执行扫描，返回代码片段列表
        
        // 工具方法
        void Reset();                                           // 重置扫描器状态
        const std::vector<CodeChunk>& GetChunks() const;       // 获取扫描结果
        
        // 调试方法
        void PrintChunks() const;                              // 打印所有代码片段（调试用）
        std::string GetChunkTypeString(CodeType type) const;   // 获取代码类型字符串
    };

} // namespace CHTL