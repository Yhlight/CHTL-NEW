#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/CHTLBaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include <memory>
#include <vector>
#include <stack>

namespace CHTL {

/**
 * 解析结果结构
 * 存储解析过程的结果和状态信息
 */
struct ParseResult {
    std::unique_ptr<CHTLBaseNode> RootNode;     // 根节点
    bool IsSuccess;                             // 是否解析成功
    std::string ErrorMessage;                   // 错误信息
    std::vector<std::string> Warnings;         // 警告信息
    size_t TokensProcessed;                     // 已处理的令牌数量
    
    ParseResult() : IsSuccess(false), TokensProcessed(0) {}
};

/**
 * CHTL解析器
 * 负责将CHTL令牌流转换为AST（抽象语法树）
 * 严格按照CHTL语法文档规范进行解析，不偏离不简化
 */
class CHTLParser {
private:
    std::unique_ptr<CHTLLexer> m_Lexer;                 // CHTL词法分析器
    std::vector<CHTLToken> m_Tokens;                    // 令牌列表
    size_t m_CurrentTokenIndex;                         // 当前令牌索引
    std::unique_ptr<CHTLBaseNode> m_RootNode;           // AST根节点
    
    // 解析状态管理
    std::stack<std::unique_ptr<CHTLBaseNode>> m_NodeStack;      // 节点栈
    std::stack<std::string> m_ScopeStack;               // 作用域栈
    
    // 错误和警告管理
    bool m_HasError;                                    // 是否有解析错误
    std::string m_ErrorMessage;                         // 错误信息
    std::vector<std::string> m_Warnings;               // 警告列表
    
    // 解析选项
    bool m_StrictMode;                                  // 严格模式
    bool m_ValidateConstraints;                         // 是否验证约束
    bool m_ProcessInheritance;                          // 是否处理继承

public:
    /**
     * 构造函数
     * @param sourceCode CHTL源代码
     */
    explicit CHTLParser(const std::string& sourceCode);
    
    /**
     * 默认构造函数
     */
    CHTLParser();
    
    /**
     * 析构函数
     */
    ~CHTLParser() = default;
    
    /**
     * 执行解析
     * @return 解析结果
     */
    ParseResult Parse();
    
    /**
     * 获取AST根节点
     * @return AST根节点指针
     */
    CHTLBaseNode* GetRootNode();
    
    /**
     * 获取AST根节点（智能指针）
     * @return AST根节点智能指针
     */
    std::shared_ptr<CHTLBaseNode> GetAST();
    
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
     * 设置解析选项
     * @param strictMode 严格模式
     * @param validateConstraints 验证约束
     * @param processInheritance 处理继承
     */
    void SetParseOptions(bool strictMode = true, bool validateConstraints = true, bool processInheritance = true);
    
    /**
     * 重置解析器
     */
    void Reset();
    
    /**
     * 设置新的源代码
     * @param sourceCode 新源代码
     */
    void SetSourceCode(const std::string& sourceCode);

private:
    // 令牌操作
    /**
     * 获取当前令牌
     * @return 当前令牌引用
     */
    const CHTLToken& CurrentToken();
    
    /**
     * 窥视下一个令牌
     * @param offset 偏移量
     * @return 令牌引用
     */
    const CHTLToken& PeekToken(size_t offset = 1);
    
    /**
     * 前进到下一个令牌
     * @return 是否成功前进
     */
    bool AdvanceToken();
    
    /**
     * 检查是否到达令牌流末尾
     * @return 是否到达末尾
     */
    bool IsAtEnd();
    
    /**
     * 消费指定类型的令牌
     * @param expectedType 期望的令牌类型
     * @param errorMessage 错误信息
     * @return 是否成功消费
     */
    bool ConsumeToken(CHTLTokenType expectedType, const std::string& errorMessage = "");
    
    /**
     * 匹配令牌类型
     * @param type 令牌类型
     * @return 是否匹配
     */
    bool MatchToken(CHTLTokenType type);
    
    // 核心解析方法
    /**
     * 解析CHTL文档
     * @return 文档根节点
     */
    std::unique_ptr<CHTLBaseNode> ParseDocument();
    
    /**
     * 安全解析CHTL文档
     * @return 文档根节点
     */
    std::unique_ptr<CHTLBaseNode> ParseDocumentSafe();
    
    /**
     * 解析use语句
     * @return use节点
     */
    std::unique_ptr<CHTLBaseNode> ParseUseStatement();
    
    /**
     * 解析元素节点
     * @return 元素节点
     */
    std::unique_ptr<ElementNode> ParseElement();
    
    /**
     * 解析文本节点
     * @return 文本节点
     */
    std::unique_ptr<TextNode> ParseText();
    
    /**
     * 解析属性列表
     * @param element 目标元素节点
     */
    void ParseAttributes(ElementNode* element);
    
    /**
     * 解析单个属性
     * @param element 目标元素节点
     */
    void ParseAttribute(ElementNode* element);
    
    /**
     * 解析局部样式块
     * @return 样式节点
     */
    std::unique_ptr<StyleNode> ParseLocalStyleBlock();
    
    /**
     * 解析局部脚本块（属于CHTL）
     * @return 脚本节点
     */
    std::unique_ptr<ScriptNode> ParseLocalScriptBlock();
    
    // 模板解析
    /**
     * 解析模板定义
     * @return 模板节点
     */
    std::unique_ptr<TemplateNode> ParseTemplateDefinition();
    
    /**
     * 解析样式组模板
     * @param templateName 模板名称
     * @return 模板节点
     */
    std::unique_ptr<TemplateNode> ParseStyleTemplate(const std::string& templateName);
    
    /**
     * 解析元素模板
     * @param templateName 模板名称
     * @return 模板节点
     */
    std::unique_ptr<TemplateNode> ParseElementTemplate(const std::string& templateName);
    
    /**
     * 解析变量组模板
     * @param templateName 模板名称
     * @return 模板节点
     */
    std::unique_ptr<TemplateNode> ParseVarTemplate(const std::string& templateName);
    
    /**
     * 解析模板使用
     * @return 模板使用节点
     */
    std::unique_ptr<CHTLBaseNode> ParseTemplateUsage();
    
    // 自定义解析
    /**
     * 解析自定义定义
     * @return 自定义节点
     */
    std::unique_ptr<CustomNode> ParseCustomDefinition();
    
    /**
     * 解析自定义样式组
     * @param customName 自定义名称
     * @return 自定义节点
     */
    std::unique_ptr<CustomNode> ParseCustomStyle(const std::string& customName);
    
    /**
     * 解析自定义元素
     * @param customName 自定义名称
     * @return 自定义节点
     */
    std::unique_ptr<CustomNode> ParseCustomElement(const std::string& customName);
    
    /**
     * 解析自定义变量组
     * @param customName 自定义名称
     * @return 自定义节点
     */
    std::unique_ptr<CustomNode> ParseCustomVar(const std::string& customName);
    
    /**
     * 解析自定义使用
     * @return 自定义使用节点
     */
    std::unique_ptr<CHTLBaseNode> ParseCustomUsage();
    
    // 特例化解析
    /**
     * 解析继承语句
     * @param parentNode 父节点
     */
    void ParseInheritance(CHTLBaseNode* parentNode);
    
    /**
     * 解析删除操作
     * @param parentNode 父节点
     */
    void ParseDeleteOperation(CHTLBaseNode* parentNode);
    
    /**
     * 解析插入操作
     * @param parentNode 父节点
     */
    void ParseInsertOperation(CHTLBaseNode* parentNode);
    
    /**
     * 解析索引访问
     * @return 索引值
     */
    size_t ParseIndexAccess();
    
    // 原始嵌入解析
    /**
     * 解析原始嵌入定义
     * @return 原始嵌入节点
     */
    std::unique_ptr<CHTLBaseNode> ParseOriginDefinition();
    
    /**
     * 解析原始嵌入使用
     * @return 原始嵌入使用节点
     */
    std::unique_ptr<CHTLBaseNode> ParseOriginUsage();
    
    // 配置解析
    /**
     * 解析配置块
     * @return 配置节点
     */
    std::unique_ptr<CHTLBaseNode> ParseConfiguration();
    
    /**
     * 解析Name配置块
     * @return Name配置节点
     */
    std::unique_ptr<CHTLBaseNode> ParseNameConfiguration();
    
    /**
     * 解析OriginType配置块
     * @return OriginType配置节点
     */
    std::unique_ptr<CHTLBaseNode> ParseOriginTypeConfiguration();
    
    // 导入解析
    /**
     * 解析导入语句
     * @return 导入节点
     */
    std::unique_ptr<CHTLBaseNode> ParseImportStatement();
    
    // 命名空间解析
    /**
     * 解析命名空间定义
     * @return 命名空间节点
     */
    std::unique_ptr<CHTLBaseNode> ParseNamespaceDefinition();
    
    // 约束解析
    /**
     * 解析约束语句
     * @return 约束节点
     */
    std::unique_ptr<CHTLBaseNode> ParseConstraintStatement();
    
    // 选择器解析
    /**
     * 解析CSS选择器
     * @return 选择器字符串
     */
    std::string ParseCSSSelector();
    
    /**
     * 解析上下文引用（&）
     * @param contextClass 上下文类名
     * @param contextId 上下文ID
     * @return 解析后的选择器
     */
    std::string ParseContextReference(const std::string& contextClass, const std::string& contextId);
    
    /**
     * 解析伪类选择器
     * @return 伪类选择器字符串
     */
    std::string ParsePseudoClassSelector();
    
    /**
     * 解析伪元素选择器
     * @return 伪元素选择器字符串
     */
    std::string ParsePseudoElementSelector();
    
    // 变量和表达式解析
    /**
     * 解析变量引用
     * @return 变量引用字符串
     */
    std::string ParseVariableReference();
    
    /**
     * 解析变量特例化
     * @return 特例化表达式
     */
    std::string ParseVariableSpecialization();
    
    /**
     * 解析字面量
     * @return 字面量值
     */
    std::string ParseLiteral();
    
    /**
     * 解析字符串字面量
     * @return 字符串值
     */
    std::string ParseStringLiteral();
    
    /**
     * 解析无修饰字面量
     * @return 字面量值
     */
    std::string ParseUnquotedLiteral();
    
    // 块结构解析
    /**
     * 解析块内容
     * @param parentNode 父节点
     */
    void ParseBlockContent(CHTLBaseNode* parentNode);
    
    /**
     * 解析CSS属性块
     * @return CSS属性映射
     */
    std::unordered_map<std::string, std::string> ParseCSSPropertyBlock();
    
    /**
     * 解析CSS属性
     * @return 属性键值对
     */
    std::pair<std::string, std::string> ParseCSSProperty();
    
    // 语法验证和处理
    /**
     * 验证语法约束
     * @param node 节点
     * @param context 上下文
     * @return 是否通过验证
     */
    bool ValidateSyntaxConstraints(CHTLBaseNode* node, const std::string& context);
    
    /**
     * 处理自动化选择器生成
     * @param element 元素节点
     * @param styleNode 样式节点
     */
    void ProcessSelectorAutomation(ElementNode* element, StyleNode* styleNode);
    
    /**
     * 处理上下文推导
     * @param styleNode 样式节点
     * @param contextElement 上下文元素
     */
    void ProcessContextInference(StyleNode* styleNode, ElementNode* contextElement);
    
    /**
     * 处理模板继承
     * @param templateNode 模板节点
     */
    void ProcessTemplateInheritance(TemplateNode* templateNode);
    
    /**
     * 处理自定义继承
     * @param customNode 自定义节点
     */
    void ProcessCustomInheritance(CustomNode* customNode);
    
    /**
     * 处理特例化操作
     * @param customNode 自定义节点
     */
    void ProcessSpecializationOperations(CustomNode* customNode);
    
    // 错误处理
    /**
     * 设置解析错误
     * @param message 错误信息
     * @param token 相关令牌
     */
    void SetParseError(const std::string& message, const CHTLToken* token = nullptr);
    
    /**
     * 添加解析警告
     * @param message 警告信息
     * @param token 相关令牌
     */
    void AddParseWarning(const std::string& message, const CHTLToken* token = nullptr);
    
    /**
     * 格式化错误位置信息
     * @param token 令牌
     * @return 位置信息字符串
     */
    std::string FormatTokenPosition(const CHTLToken& token);
    
    // 辅助方法
    /**
     * 跳过空白和注释令牌
     */
    void SkipWhitespaceAndComments();
    
    /**
     * 检查是否为HTML元素令牌
     * @param token 令牌
     * @return 是否为HTML元素
     */
    bool IsHTMLElementToken(const CHTLToken& token);
    
    /**
     * 检查是否为块结构开始
     * @param token 令牌
     * @return 是否为块结构开始
     */
    bool IsBlockStructureStart(const CHTLToken& token);
    
    /**
     * 检查是否为@类型令牌
     * @param token 令牌
     * @return 是否为@类型
     */
    bool IsAtTypeToken(const CHTLToken& token);
    
    /**
     * 检查CE对等式（:和=等价）
     * @param token 令牌
     * @return 是否为CE对等式令牌
     */
    bool IsCEEquivalenceToken(const CHTLToken& token);
    
    /**
     * 解析标识符列表（逗号分隔）
     * @return 标识符列表
     */
    std::vector<std::string> ParseIdentifierList();
    
    /**
     * 解析路径表达式
     * @return 路径字符串
     */
    std::string ParsePathExpression();
    
    /**
     * 验证HTML标签名
     * @param tagName 标签名
     * @return 是否有效
     */
    bool IsValidHTMLTag(const std::string& tagName);
    
    /**
     * 验证CSS属性名
     * @param property 属性名
     * @return 是否有效
     */
    bool IsValidCSSProperty(const std::string& property);
    
    /**
     * 处理UTF-8字符验证
     * @param text 文本内容
     * @return 是否为有效UTF-8
     */
    bool ValidateUTF8Content(const std::string& text);
    
    /**
     * 构建完整的AST
     */
    void BuildCompleteAST();
    
    /**
     * 应用后处理规则
     */
    void ApplyPostProcessingRules();
    
    /**
     * 优化AST结构
     */
    void OptimizeASTStructure();
    
    /**
     * 验证AST完整性
     * @return 是否有效
     */
    bool ValidateASTIntegrity();
    
    // 完全修正的解析方法 - 为Web生态注入新动力
    /**
     * 正确解析CHTL文档
     * @return 文档根节点
     */
    std::unique_ptr<CHTLBaseNode> ParseCHTLDocumentCorrect();
    
    /**
     * 正确解析HTML元素
     * @return HTML元素节点
     */
    std::unique_ptr<CHTLBaseNode> ParseHTMLElementCorrect();
    
    /**
     * 正确解析head元素
     * @return head元素节点
     */
    std::unique_ptr<CHTLBaseNode> ParseHeadElementCorrect();
    
    /**
     * 正确解析body元素
     * @return body元素节点
     */
    std::unique_ptr<CHTLBaseNode> ParseBodyElementCorrect();
    
    /**
     * 正确解析title元素
     * @return title元素节点
     */
    std::unique_ptr<CHTLBaseNode> ParseTitleElementCorrect();
    
    /**
     * 正确解析通用元素
     * @return 元素节点
     */
    std::unique_ptr<CHTLBaseNode> ParseElementCorrect();
    
    /**
     * 正确解析text元素
     * @return 文本节点
     */
    std::unique_ptr<CHTLBaseNode> ParseTextElementCorrect();
    
    /**
     * 正确解析style元素
     * @return 样式节点
     */
    std::unique_ptr<StyleNode> ParseStyleElementCorrect();
    
    /**
     * 正确解析script元素
     * @return 脚本节点
     */
    std::unique_ptr<ScriptNode> ParseScriptElementCorrect();
    
    /**
     * 正确解析顶级元素
     * @return 元素节点
     */
    std::unique_ptr<CHTLBaseNode> ParseTopLevelElementCorrect();
    
    /**
     * 正确解析特殊语法
     */
    void ParseSpecialSyntaxCorrect();
    
    /**
     * 期望特定类型的token
     * @param expectedType 期望的token类型
     * @return 是否匹配
     */
    bool ExpectToken(CHTLTokenType expectedType);
    
    // 简单但正确的解析方法
    std::unique_ptr<CHTLBaseNode> ParseCHTLDocumentSimpleCorrect();
    std::unique_ptr<CHTLBaseNode> ParseHTMLElementSimpleCorrect();
    std::unique_ptr<CHTLBaseNode> ParseHeadElementSimpleCorrect();
    std::unique_ptr<CHTLBaseNode> ParseBodyElementSimpleCorrect();
    std::unique_ptr<CHTLBaseNode> ParseTitleElementSimpleCorrect();
    std::unique_ptr<CHTLBaseNode> ParseElementSimpleCorrect();
    std::unique_ptr<CHTLBaseNode> ParseTextElementSimpleCorrect();
    std::unique_ptr<StyleNode> ParseStyleElementSimpleCorrect();
    std::unique_ptr<ScriptNode> ParseScriptElementSimpleCorrect();
};

} // namespace CHTL