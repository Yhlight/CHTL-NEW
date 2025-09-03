#pragma once

#include "CHTLJSToken.h"
#include "CHTLJSGlobalMap.h"
#include "../CHTLJSState/CHTLJSState.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <string>
#include <vector>
#include <memory>
#include <codecvt>
#include <locale>

namespace CHTLJS {

/**
 * CHTL JS词法分析器（完全独立于CHTL词法分析器）
 * 负责将CHTL JS源代码转换为令牌流
 * 专门处理CHTL JS特有的语法特征
 */
class CHTLJSLexer {
private:
    std::string m_SourceCode;                              // 源代码
    size_t m_CurrentPosition;                              // 当前位置
    size_t m_CurrentLine;                                 // 当前行号
    size_t m_CurrentColumn;                               // 当前列号
    std::vector<CHTLJSToken> m_Tokens;                    // CHTL JS令牌列表
    std::unique_ptr<CHTLJSTokenManager> m_TokenManager;   // CHTL JS令牌管理器
    std::unique_ptr<CHTLJSGlobalMap> m_GlobalMap;         // CHTL JS全局映射表
    std::unique_ptr<CHTLJSStateMachine> m_StateMachine;   // CHTL JS状态机
    std::unique_ptr<CHTLJSContextManager> m_ContextManager; // CHTL JS上下文管理器
    std::wstring_convert<std::codecvt_utf8<wchar_t>> m_UTF8Converter; // UTF-8转换器
    
    bool m_HasError;                                      // 是否有错误
    std::string m_ErrorMessage;                           // 错误信息

public:
    /**
     * 构造函数
     * @param sourceCode CHTL JS源代码
     */
    explicit CHTLJSLexer(const std::string& sourceCode);
    
    /**
     * 默认构造函数
     */
    CHTLJSLexer();
    
    /**
     * 析构函数
     */
    ~CHTLJSLexer() = default;
    
    /**
     * 执行CHTL JS词法分析
     * @return 是否成功
     */
    bool Tokenize();
    
    /**
     * 获取CHTL JS令牌列表
     * @return 令牌向量的常引用
     */
    const std::vector<CHTLJSToken>& GetTokens() const;
    
    /**
     * 获取CHTL JS全局映射表
     * @return 全局映射表指针
     */
    CHTLJSGlobalMap* GetGlobalMap();
    
    /**
     * 获取CHTL JS状态机
     * @return 状态机指针
     */
    CHTLJSStateMachine* GetStateMachine();
    
    /**
     * 获取CHTL JS上下文管理器
     * @return 上下文管理器指针
     */
    CHTLJSContextManager* GetContextManager();
    
    /**
     * 重置CHTL JS词法分析器
     */
    void Reset();
    
    /**
     * 设置新的CHTL JS源代码
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
     * 扫描下一个CHTL JS令牌
     * @return 是否成功扫描
     */
    bool ScanNextCHTLJSToken();
    
    /**
     * 扫描CHTL JS标识符或关键字
     * @return 令牌
     */
    CHTLJSToken ScanCHTLJSIdentifierOrKeyword();
    
    /**
     * 扫描增强选择器 {{...}}
     * @return 令牌
     */
    CHTLJSToken ScanEnhancedSelector();
    
    /**
     * 扫描->操作符
     * @return 令牌
     */
    CHTLJSToken ScanArrowOperator();
    
    /**
     * 扫描&->事件绑定操作符
     * @return 令牌
     */
    CHTLJSToken ScanEventBindOperator();
    
    /**
     * 扫描CHTL JS函数块
     * @param functionType 函数类型（listen/delegate/animate/module）
     * @return 令牌
     */
    CHTLJSToken ScanCHTLJSFunctionBlock(const std::string& functionType);
    
    /**
     * 扫描vir虚对象声明
     * @return 令牌
     */
    CHTLJSToken ScanVirDeclaration();
    
    /**
     * 扫描模块加载语句
     * @return 令牌
     */
    CHTLJSToken ScanModuleLoad();
    
    /**
     * 扫描事件映射
     * @return 令牌
     */
    CHTLJSToken ScanEventMapping();
    
    /**
     * 扫描动画属性
     * @return 令牌
     */
    CHTLJSToken ScanAnimationProperty();
    
    /**
     * 扫描字符串字面量
     * @param quote 引号字符
     * @return 令牌
     */
    CHTLJSToken ScanStringLiteral(char quote);
    
    /**
     * 扫描数字
     * @return 令牌
     */
    CHTLJSToken ScanNumber();
    
    /**
     * 扫描操作符
     * @return 令牌
     */
    CHTLJSToken ScanOperator();
    
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
     * 创建CHTL JS令牌
     * @param type 令牌类型
     * @param value 令牌值
     * @param length 令牌长度
     * @return 创建的令牌
     */
    CHTLJSToken CreateToken(CHTLJSTokenType type, const std::string& value, size_t length);
    
    /**
     * 设置错误
     * @param message 错误信息
     */
    void SetError(const std::string& message);
    
    /**
     * 处理增强选择器解析
     * @param selector 选择器字符串
     * @return 解析后的DOM代码
     */
    std::string ProcessEnhancedSelector(const std::string& selector);
    
    /**
     * 处理虚对象键解析
     * @param virObject 虚对象名称
     * @param key 键名称
     * @return 解析后的代码
     */
    std::string ProcessVirtualObjectKey(const std::string& virObject, const std::string& key);
    
    /**
     * 处理CHTL JS函数解析
     * @param functionType 函数类型
     * @param content 函数内容
     * @return 解析后的JavaScript代码
     */
    std::string ProcessCHTLJSFunction(const std::string& functionType, const std::string& content);
    
    /**
     * 更新CHTL JS状态和上下文
     * @param token CHTL JS令牌
     * @return 是否成功更新
     */
    bool UpdateCHTLJSStateAndContext(const CHTLJSToken& token);
    
    /**
     * 验证UTF-8编码
     * @param str 字符串
     * @return 是否有效
     */
    bool ValidateUTF8(const std::string& str);
    
    /**
     * 检查当前上下文是否允许指定CHTL JS语法
     * @param syntaxType CHTL JS语法类型
     * @return 是否允许
     */
    bool IsAllowedCHTLJSSyntaxInCurrentContext(const std::string& syntaxType);
    
    /**
     * 处理AMD模块依赖分析
     * @param loadPaths 加载路径列表
     * @return 排序后的加载顺序
     */
    std::vector<std::string> ProcessAMDDependencies(const std::vector<std::string>& loadPaths);
    
    /**
     * 生成事件委托全局注册表代码
     * @return 生成的JavaScript代码
     */
    std::string GenerateEventDelegateRegistry();
    
    /**
     * 处理requestAnimationFrame封装
     * @param animationCode 动画代码
     * @return 封装后的代码
     */
    std::string WrapWithRequestAnimationFrame(const std::string& animationCode);
    
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

} // namespace CHTLJS