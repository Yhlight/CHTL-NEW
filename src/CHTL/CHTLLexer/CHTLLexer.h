#pragma once

#include "CHTLToken.h"
#include "CHTLGlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    // CHTL词法分析器
    // 严格按照CHTL语法文档实现
    // 完全独立于CHTL JS词法分析器
    class CHTLLexer {
    private:
        std::string sourceCode;                 // 源代码
        size_t currentPos;                      // 当前位置
        int currentLine;                        // 当前行号
        int currentColumn;                      // 当前列号
        std::vector<CHTLToken> tokens;          // 生成的Token列表
        std::shared_ptr<CHTLGlobalMap> globalMap;  // 全局映射表
        
        // 私有辅助方法
        void AdvancePosition();                 // 前进位置
        void AdvancePosition(int count);        // 前进指定位置
        char PeekChar(size_t offset = 0) const; // 查看字符但不前进
        std::string PeekString(size_t length) const; // 查看字符串但不前进
        bool IsAtEnd() const;                   // 是否到达文件末尾
        
        // Token扫描方法
        void ScanToken();                       // 扫描单个Token
        void ScanIdentifierOrKeyword();         // 扫描标识符或关键字
        void ScanString();                      // 扫描字符串字面量
        void ScanSingleQuoteString();           // 扫描单引号字符串
        void ScanUnquotedString();              // 扫描无修饰字面量
        void ScanNumber();                      // 扫描数字
        void ScanComment();                     // 扫描注释
        void ScanOperator();                    // 扫描操作符
        void ScanBlockKeyword();                // 扫描块关键字 [Template], [Custom]等
        void ScanTypeKeyword();                 // 扫描类型关键字 @Style, @Element等
        void ScanSelector();                    // 扫描CSS选择器
        
        // 特殊语法扫描
        void ScanCEEquivalent();                // 扫描CE对等式（: 和 = 等价）
        void ScanAtTopBottom();                 // 扫描"at top"/"at bottom"
        
        // 字符判断方法
        bool IsAlpha(char c) const;             // 是否为字母
        bool IsDigit(char c) const;             // 是否为数字
        bool IsAlphaNumeric(char c) const;      // 是否为字母或数字
        bool IsWhitespace(char c) const;        // 是否为空白字符
        bool IsNewline(char c) const;           // 是否为换行符
        
        // 特殊字符串判断
        bool IsValidUnquotedChar(char c) const; // 是否为有效的无修饰字符
        bool IsValidIdentifierStart(char c) const; // 是否为有效的标识符开始字符
        bool IsValidIdentifierChar(char c) const;  // 是否为有效的标识符字符
        
        // Token创建方法
        void AddToken(CHTLTokenType type, const std::string& value);  // 添加Token
        void AddToken(CHTLTokenType type);                            // 添加无值Token
        
        // 错误处理
        void ReportError(const std::string& message);       // 报告错误
        
        // 跳过方法
        void SkipWhitespace();                  // 跳过空白字符
        void SkipToEndOfLine();                 // 跳过到行末

    public:
        explicit CHTLLexer(const std::string& source);
        ~CHTLLexer() = default;

        // 主要方法
        std::vector<CHTLToken> Tokenize();      // 执行词法分析
        void Reset();                           // 重置词法分析器
        void Reset(const std::string& newSource); // 重置并设置新源代码
        
        // 获取结果
        const std::vector<CHTLToken>& GetTokens() const;    // 获取Token列表
        
        // 状态查询
        size_t GetCurrentPosition() const;      // 获取当前位置
        int GetCurrentLine() const;             // 获取当前行号
        int GetCurrentColumn() const;           // 获取当前列号
        
        // 配置方法
        void SetGlobalMap(std::shared_ptr<CHTLGlobalMap> map);  // 设置全局映射表
        
        // 调试方法
        void PrintTokens() const;               // 打印所有Token（调试用）
        void PrintCurrentState() const;        // 打印当前状态（调试用）
    };

} // namespace CHTL