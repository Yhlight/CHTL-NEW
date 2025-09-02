#pragma once

#include "CHTLToken.h"
#include "CHTLGlobalMap.h"
#include "../CHTLState/CHTLState.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <vector>
#include <memory>
#include <codecvt>
#include <locale>

namespace CHTL {

/**
 * CHTL词法分析器
 * 负责将CHTL源代码转换为令牌流
 * 与CHTLUnifiedScanner配合工作，处理CHTL特定的语法特征
 */
class CHTLLexer {
private:
    std::string m_SourceCode;                          // 源代码
    size_t m_CurrentPosition;                          // 当前位置
    size_t m_CurrentLine;                             // 当前行号
    size_t m_CurrentColumn;                           // 当前列号
    std::vector<CHTLToken> m_Tokens;                  // 令牌列表
    std::unique_ptr<CHTLTokenManager> m_TokenManager; // 令牌管理器
    std::unique_ptr<CHTLGlobalMap> m_GlobalMap;       // 全局映射表
    std::unique_ptr<CHTLStateMachine> m_StateMachine; // 状态机
    std::unique_ptr<CHTLContextManager> m_ContextManager; // 上下文管理器
    std::wstring_convert<std::codecvt_utf8<wchar_t>> m_UTF8Converter; // UTF-8转换器
    
    bool m_HasError;                                  // 是否有错误
    std::string m_ErrorMessage;                       // 错误信息

public:
    /**
     * 构造函数
     * @param sourceCode 源代码
     */
    explicit CHTLLexer(const std::string& sourceCode);
    
    /**
     * 析构函数
     */
    ~CHTLLexer() = default;
    
    /**
     * 执行词法分析
     * @return 是否成功
     */
    bool Tokenize();
    
    /**
     * 获取令牌列表
     * @return 令牌向量的常引用
     */
    const std::vector<CHTLToken>& GetTokens() const;
    
    /**
     * 获取全局映射表
     * @return 全局映射表指针
     */
    CHTLGlobalMap* GetGlobalMap();
    
    /**
     * 获取状态机
     * @return 状态机指针
     */
    CHTLStateMachine* GetStateMachine();
    
    /**
     * 获取上下文管理器
     * @return 上下文管理器指针
     */
    CHTLContextManager* GetContextManager();
    
    /**
     * 重置词法分析器
     */
    void Reset();
    
    /**
     * 设置新的源代码
     * @param sourceCode 新源代码
     */
    void SetSourceCode(const std::string& sourceCode);
    
    /**
     * 检查是否有错误
     * @return 是否有错误
     */
    bool HasError() const;
    
    /**
     * 获取错误信息
     * @return 错误信息
     */
    std::string GetErrorMessage() const;

private:
    /**
     * 扫描下一个令牌
     * @return 是否成功扫描
     */
    bool ScanNextToken();
    
    /**
     * 扫描标识符或关键字
     * @return 令牌
     */
    CHTLToken ScanIdentifierOrKeyword();
    
    /**
     * 扫描字符串字面量
     * @param quote 引号字符
     * @return 令牌
     */
    CHTLToken ScanStringLiteral(char quote);
    
    /**
     * 扫描无修饰字面量
     * @return 令牌
     */
    CHTLToken ScanUnquotedLiteral();
    
    /**
     * 扫描数字
     * @return 令牌
     */
    CHTLToken ScanNumber();
    
    /**
     * 扫描注释
     * @return 令牌
     */
    CHTLToken ScanComment();
    
    /**
     * 扫描块结构
     * @return 令牌
     */
    CHTLToken ScanBlockStructure();
    
    /**
     * 扫描@类型标识符
     * @return 令牌
     */
    CHTLToken ScanAtType();
    
    /**
     * 扫描选择器
     * @return 令牌
     */
    CHTLToken ScanSelector();
    
    /**
     * 扫描上下文引用
     * @return 令牌
     */
    CHTLToken ScanContextReference();
    
    /**
     * 扫描索引访问
     * @return 令牌
     */
    CHTLToken ScanIndexAccess();
    
    /**
     * 扫描操作符
     * @return 令牌
     */
    CHTLToken ScanOperator();
    
    /**
     * 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * 读取下一个字符
     * @return 下一个字符
     */
    char GetNextChar();
    
    /**
     * 窥视下一个字符
     * @param offset 偏移量
     * @return 字符
     */
    char PeekChar(size_t offset = 1);
    
    /**
     * 检查是否到达文件末尾
     * @return 是否到达末尾
     */
    bool IsAtEnd() const;
    
    /**
     * 更新行号和列号
     * @param ch 当前字符
     */
    void UpdateLineColumn(char ch);
    
    /**
     * 创建令牌
     * @param type 令牌类型
     * @param value 令牌值
     * @param length 令牌长度
     * @return 创建的令牌
     */
    CHTLToken CreateToken(CHTLTokenType type, const std::string& value, size_t length);
    
    /**
     * 设置错误
     * @param message 错误信息
     */
    void SetError(const std::string& message);
    
    /**
     * 处理CE对等式（:和=等价）
     * @param ch 当前字符
     * @return 令牌
     */
    CHTLToken HandleCEEquivalence(char ch);
    
    /**
     * 处理自动化类名/ID生成
     * @param selector 选择器字符串
     */
    void HandleAutoSelectorGeneration(const std::string& selector);
    
    /**
     * 处理模板和自定义的特例化语法
     * @return 是否成功处理
     */
    bool HandleSpecializationSyntax();
    
    /**
     * 处理继承语法
     * @return 是否成功处理
     */
    bool HandleInheritanceSyntax();
    
    /**
     * 处理删除语法
     * @return 是否成功处理
     */
    bool HandleDeleteSyntax();
    
    /**
     * 处理插入语法
     * @return 是否成功处理
     */
    bool HandleInsertSyntax();
    
    /**
     * 处理约束语法
     * @return 是否成功处理
     */
    bool HandleConstraintSyntax();
    
    /**
     * 验证UTF-8编码
     * @param str 字符串
     * @return 是否有效
     */
    bool ValidateUTF8(const std::string& str);
    
    /**
     * 处理变量引用解析
     * @param varRef 变量引用
     * @return 解析后的值
     */
    std::string ResolveVariableReference(const std::string& varRef);
    
    /**
     * 检查当前上下文是否允许指定语法
     * @param syntaxType 语法类型
     * @return 是否允许
     */
    bool IsAllowedInCurrentContext(const std::string& syntaxType);
    
    /**
     * 处理配置项解析
     * @param configKey 配置键
     * @param configValue 配置值
     */
    void HandleConfigurationItem(const std::string& configKey, const std::string& configValue);
    
    /**
     * 处理Name配置块
     * @param nameKey 名称键
     * @param nameValues 名称值列表
     */
    void HandleNameConfiguration(const std::string& nameKey, const std::vector<std::string>& nameValues);
    
    /**
     * 处理OriginType配置块
     * @param typeKey 类型键
     * @param typeName 类型名称
     */
    void HandleOriginTypeConfiguration(const std::string& typeKey, const std::string& typeName);
    
    /**
     * 更新状态和上下文
     * @param token 令牌
     * @return 是否成功
     */
    bool UpdateStateAndContext(const CHTLToken& token);
    
    /**
     * 解析事件映射
     * @param content 内容
     * @return 事件映射
     */
    std::unordered_map<std::string, std::string> ParseEventMappings(const std::string& content);
    
    /**
     * 解析委托信息
     * @param content 内容
     * @return 委托信息
     */
    std::string ParseDelegateInfo(const std::string& content);
    
    /**
     * 解析动画信息
     * @param content 内容
     * @return 动画信息
     */
    std::string ParseAnimationInfo(const std::string& content);
    
    /**
     * 解析模块路径
     * @param content 内容
     * @return 路径列表
     */
    std::vector<std::string> ParseModulePaths(const std::string& content);
};

} // namespace CHTL