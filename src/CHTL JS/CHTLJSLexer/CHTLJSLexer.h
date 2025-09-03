#pragma once

#include "CHTLJSToken.h"
#include "CHTLJSGlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

    // CHTL JS词法分析器
    // 严格按照CHTL语法文档中的CHTL JS部分实现
    // 完全独立于CHTL词法分析器
    class CHTLJSLexer {
    private:
        std::string sourceCode;                     // 源代码
        size_t currentPos;                          // 当前位置
        int currentLine;                            // 当前行号
        int currentColumn;                          // 当前列号
        std::vector<CHTLJSToken> tokens;            // 生成的Token列表
        std::shared_ptr<CHTLJSGlobalMap> globalMap; // 全局映射表
        
        // 私有辅助方法
        void AdvancePosition();                     // 前进位置
        void AdvancePosition(int count);            // 前进指定位置
        char PeekChar(size_t offset = 0) const;     // 查看字符但不前进
        std::string PeekString(size_t length) const; // 查看字符串但不前进
        bool IsAtEnd() const;                       // 是否到达文件末尾
        
        // CHTL JS特有Token扫描方法
        void ScanToken();                           // 扫描单个Token
        void ScanIdentifierOrKeyword();             // 扫描标识符或关键字
        void ScanString();                          // 扫描字符串字面量
        void ScanNumber();                          // 扫描数字
        void ScanComment();                         // 扫描注释
        void ScanSelector();                        // 扫描{{选择器}}语法
        void ScanArrowOperator();                   // 扫描->操作符
        void ScanEventBindingOperator();            // 扫描&->操作符
        void ScanCHTLJSFunction();                  // 扫描CHTL JS函数语法
        void ScanUtilExpression();                  // 扫描util...then表达式
        
        // CHTL JS特有字符判断方法
        bool IsAlpha(char c) const;                 // 是否为字母
        bool IsDigit(char c) const;                 // 是否为数字
        bool IsAlphaNumeric(char c) const;          // 是否为字母或数字
        bool IsWhitespace(char c) const;            // 是否为空白字符
        bool IsNewline(char c) const;               // 是否为换行符
        bool IsValidSelectorChar(char c) const;     // 是否为有效的选择器字符
        bool IsValidIdentifierStart(char c) const;  // 是否为有效的标识符开始字符
        bool IsValidIdentifierChar(char c) const;   // 是否为有效的标识符字符
        
        // Token创建方法
        void AddToken(CHTLJSTokenType type, const std::string& value);  // 添加Token
        void AddToken(CHTLJSTokenType type);                            // 添加无值Token
        
        // 错误处理
        void ReportError(const std::string& message);       // 报告错误
        
        // 跳过方法
        void SkipWhitespace();                      // 跳过空白字符
        void SkipToEndOfLine();                     // 跳过到行末

    public:
        explicit CHTLJSLexer(const std::string& source);
        ~CHTLJSLexer() = default;

        // 主要方法
        std::vector<CHTLJSToken> Tokenize();        // 执行词法分析
        void Reset();                               // 重置词法分析器
        void Reset(const std::string& newSource);   // 重置并设置新源代码
        
        // 获取结果
        const std::vector<CHTLJSToken>& GetTokens() const;  // 获取Token列表
        
        // 状态查询
        size_t GetCurrentPosition() const;          // 获取当前位置
        int GetCurrentLine() const;                 // 获取当前行号
        int GetCurrentColumn() const;               // 获取当前列号
        
        // 配置方法
        void SetGlobalMap(std::shared_ptr<CHTLJSGlobalMap> map);  // 设置全局映射表
        
        // CHTL JS特有验证方法
        bool ValidateSelector(const std::string& selector) const;      // 验证选择器语法
        bool ValidateArrowChain(const std::string& chain) const;       // 验证->链式语法
        bool ValidateEventBinding(const std::string& binding) const;   // 验证&->事件绑定语法
        bool ValidateCHTLJSFunction(const std::string& function) const; // 验证CHTL JS函数语法
        
        // 调试方法
        void PrintTokens() const;                   // 打印所有Token（调试用）
        void PrintCurrentState() const;            // 打印当前状态（调试用）
    };

} // namespace CHTLJS