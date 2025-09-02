#pragma once

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSNode/EnhanceSelectorNode.h"
#include "../CHTLJSNode/VirObjectNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include <memory>
#include <vector>
#include <stack>

namespace CHTLJS {

/**
 * CHTL JS解析结果结构（完全独立于CHTL）
 */
struct CHTLJSParseResult {
    std::unique_ptr<CHTLJSBaseNode> RootNode;   // CHTL JS AST根节点
    bool IsSuccess;                             // 是否解析成功
    std::string ErrorMessage;                   // 错误信息
    std::vector<std::string> Warnings;         // 警告信息
    size_t TokensProcessed;                     // 已处理的令牌数量
    
    CHTLJSParseResult() : IsSuccess(false), TokensProcessed(0) {}
};

/**
 * CHTL JS解析器（完全独立于CHTL解析器）
 * 负责将CHTL JS令牌流转换为CHTL JS AST
 * 专门处理CHTL JS特有的语法特征
 */
class CHTLJSParser {
private:
    std::unique_ptr<CHTLJSLexer> m_Lexer;               // CHTL JS词法分析器
    std::vector<CHTLJSToken> m_Tokens;                  // CHTL JS令牌列表
    size_t m_CurrentTokenIndex;                         // 当前令牌索引
    std::unique_ptr<CHTLJSBaseNode> m_RootNode;         // CHTL JS AST根节点
    
    // CHTL JS解析状态管理
    std::stack<std::unique_ptr<CHTLJSBaseNode>> m_NodeStack;    // CHTL JS节点栈
    std::stack<std::string> m_CHTLJSScopeStack;         // CHTL JS作用域栈
    
    // 错误和警告管理
    bool m_HasError;                                    // 是否有解析错误
    std::string m_ErrorMessage;                         // 错误信息
    std::vector<std::string> m_Warnings;               // 警告列表
    
    // CHTL JS解析选项
    bool m_StrictCHTLJSMode;                           // 严格CHTL JS模式
    bool m_ValidateSelectors;                          // 是否验证选择器
    bool m_ProcessVirtualObjects;                      // 是否处理虚对象

public:
    /**
     * 构造函数
     * @param sourceCode CHTL JS源代码
     */
    explicit CHTLJSParser(const std::string& sourceCode);
    
    /**
     * 析构函数
     */
    ~CHTLJSParser() = default;
    
    /**
     * 执行CHTL JS解析
     * @return CHTL JS解析结果
     */
    CHTLJSParseResult Parse();
    
    /**
     * 获取CHTL JS AST根节点
     * @return AST根节点指针
     */
    CHTLJSBaseNode* GetRootNode();
    
    /**
     * 检查是否有解析错误
     * @return 是否有错误
     */
    bool HasError() const { return m_HasError; }
    
    /**
     * 获取错误信息
     * @return 错误信息
     */
    const std::string& GetErrorMessage() const { return m_ErrorMessage; }
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& GetWarnings() const { return m_Warnings; }
    
    /**
     * 设置CHTL JS解析选项
     * @param strictMode 严格模式
     * @param validateSelectors 验证选择器
     * @param processVirtualObjects 处理虚对象
     */
    void SetCHTLJSParseOptions(bool strictMode = true, bool validateSelectors = true, bool processVirtualObjects = true);
    
    /**
     * 重置CHTL JS解析器
     */
    void Reset();
    
    /**
     * 设置新的CHTL JS源代码
     * @param sourceCode 新源代码
     */
    void SetSourceCode(const std::string& sourceCode);

private:
    // CHTL JS令牌操作
    /**
     * 获取当前CHTL JS令牌
     * @return 当前令牌引用
     */
    const CHTLJSToken& CurrentCHTLJSToken();
    
    /**
     * 窥视下一个CHTL JS令牌
     * @param offset 偏移量
     * @return 令牌引用
     */
    const CHTLJSToken& PeekCHTLJSToken(size_t offset = 1);
    
    /**
     * 前进到下一个CHTL JS令牌
     * @return 是否成功前进
     */
    bool AdvanceCHTLJSToken();
    
    /**
     * 检查是否到达CHTL JS令牌流末尾
     * @return 是否到达末尾
     */
    bool IsAtCHTLJSEnd();
    
    /**
     * 消费指定类型的CHTL JS令牌
     * @param expectedType 期望的令牌类型
     * @param errorMessage 错误信息
     * @return 是否成功消费
     */
    bool ConsumeCHTLJSToken(CHTLJSTokenType expectedType, const std::string& errorMessage = "");
    
    /**
     * 匹配CHTL JS令牌类型
     * @param type 令牌类型
     * @return 是否匹配
     */
    bool MatchCHTLJSToken(CHTLJSTokenType type);
    
    // CHTL JS核心解析方法
    /**
     * 解析CHTL JS文档
     * @return 文档根节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseCHTLJSDocument();
    
    /**
     * 解析模块导入（module{}）
     * @return 模块节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseModuleBlock();
    
    /**
     * 解析模块加载语句（load:）
     * @return 加载节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseModuleLoad();
    
    /**
     * 解析增强选择器（{{selector}}）
     * @return 选择器节点
     */
    std::unique_ptr<EnhancedSelectorNode> ParseEnhancedSelector();
    
    /**
     * 解析->操作符
     * @return 操作符节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseArrowOperator();
    
    /**
     * 解析&->事件绑定操作符
     * @return 事件绑定节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseEventBindOperator();
    
    // CHTL JS函数解析
    /**
     * 解析listen函数
     * @return listen节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseListenFunction();
    
    /**
     * 解析delegate函数
     * @return delegate节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseDelegateFunction();
    
    /**
     * 解析animate函数
     * @return animate节点
     */
    std::unique_ptr<AnimateNode> ParseAnimateFunction();
    
    /**
     * 解析CHTL JS函数内容
     * @param functionType 函数类型
     * @return 函数内容映射
     */
    std::unordered_map<std::string, std::string> ParseCHTLJSFunctionContent(const std::string& functionType);
    
    // 虚对象解析
    /**
     * 解析vir声明
     * @return 虚对象节点
     */
    std::unique_ptr<VirObjectNode> ParseVirDeclaration();
    
    /**
     * 解析vir赋值
     * @param virName 虚对象名称
     * @return 虚对象节点
     */
    std::unique_ptr<VirObjectNode> ParseVirAssignment(const std::string& virName);
    
    /**
     * 解析vir引用（Test->click）
     * @return vir引用节点
     */
    std::unique_ptr<CHTLJSBaseNode> ParseVirReference();
    
    // 事件解析
    /**
     * 解析事件映射
     * @return 事件映射
     */
    std::unordered_map<std::string, std::string> ParseEventMapping();
    
    /**
     * 解析事件处理器
     * @return 处理器代码
     */
    std::string ParseEventHandler();
    
    /**
     * 解析delegate目标
     * @return 目标选择器列表
     */
    std::vector<std::string> ParseDelegateTargets();
    
    // 动画解析
    /**
     * 解析动画属性
     * @param animateNode 动画节点
     */
    void ParseAnimationProperties(AnimateNode* animateNode);
    
    /**
     * 解析动画目标
     * @return 目标选择器列表
     */
    std::vector<std::string> ParseAnimationTargets();
    
    /**
     * 解析动画状态块（begin/end）
     * @return 状态属性映射
     */
    std::unordered_map<std::string, std::string> ParseAnimationStateBlock();
    
    /**
     * 解析when关键帧
     * @return 关键帧列表
     */
    std::vector<AnimationKeyframe> ParseWhenKeyframes();
    
    // 选择器解析
    /**
     * 解析选择器内容
     * @param selectorContent 选择器内容
     * @return 解析后的选择器信息
     */
    std::string ParseSelectorContent(const std::string& selectorContent);
    
    /**
     * 验证增强选择器格式
     * @param selector 选择器
     * @return 是否有效
     */
    bool ValidateEnhancedSelector(const std::string& selector);
    
    /**
     * 分析选择器优先级
     * @param selector 选择器
     * @return 优先级类型
     */
    SelectorPriority AnalyzeSelectorPriority(const std::string& selector);
    
    // 辅助方法
    /**
     * 跳过CHTL JS空白和注释令牌
     */
    void SkipCHTLJSWhitespaceAndComments();
    
    /**
     * 检查是否为CHTL JS函数令牌
     * @param token 令牌
     * @return 是否为CHTL JS函数
     */
    bool IsCHTLJSFunctionToken(const CHTLJSToken& token);
    
    /**
     * 检查是否为增强选择器令牌
     * @param token 令牌
     * @return 是否为增强选择器
     */
    bool IsEnhancedSelectorToken(const CHTLJSToken& token);
    
    /**
     * 解析CHTL JS字面量
     * @return 字面量值
     */
    std::string ParseCHTLJSLiteral();
    
    /**
     * 解析CHTL JS标识符列表
     * @return 标识符列表
     */
    std::vector<std::string> ParseCHTLJSIdentifierList();
    
    /**
     * 验证CHTL JS语法
     * @param syntax 语法内容
     * @return 是否有效
     */
    bool ValidateCHTLJSSyntax(const std::string& syntax);
    
    // 错误处理
    /**
     * 设置CHTL JS解析错误
     * @param message 错误信息
     * @param token 相关令牌
     */
    void SetCHTLJSParseError(const std::string& message, const CHTLJSToken* token = nullptr);
    
    /**
     * 添加CHTL JS解析警告
     * @param message 警告信息
     * @param token 相关令牌
     */
    void AddCHTLJSParseWarning(const std::string& message, const CHTLJSToken* token = nullptr);
    
    /**
     * 格式化CHTL JS令牌位置信息
     * @param token 令牌
     * @return 位置信息字符串
     */
    std::string FormatCHTLJSTokenPosition(const CHTLJSToken& token);
    
    /**
     * 构建完整的CHTL JS AST
     */
    void BuildCompleteCHTLJSAST();
    
    /**
     * 应用CHTL JS后处理规则
     */
    void ApplyCHTLJSPostProcessingRules();
    
    /**
     * 验证CHTL JS AST完整性
     * @return 是否有效
     */
    bool ValidateCHTLJSASTIntegrity();
};

} // namespace CHTLJS