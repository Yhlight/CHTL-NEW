#pragma once

#include "../CHTLNode/CHTLBaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/Visitor.h"
// 暂时注释高级功能引用，确保核心编译成功
// #include "SelectorAutomation.h"
// #include "TemplateEngine.h"
// #include "CustomEngine.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * 生成结果结构
 * 存储CHTL生成器的输出结果
 */
struct GenerationResult {
    std::string HTMLContent;                    // 生成的HTML内容
    std::string CSSContent;                     // 生成的CSS内容
    std::string JavaScriptContent;              // 生成的JavaScript内容（来自局部script）
    std::string FullHTMLDocument;               // 完整的HTML文档
    bool IsSuccess;                             // 是否生成成功
    std::string ErrorMessage;                   // 错误信息
    std::vector<std::string> Warnings;         // 警告信息
    
    GenerationResult() : IsSuccess(false) {}
};

/**
 * CHTL生成器
 * 负责从CHTL AST生成HTML和CSS代码
 * 严格按照CHTL语法文档规范进行生成
 */
class CHTLGenerator : public CHTLDefaultVisitor {
private:
    std::unique_ptr<CHTLBaseNode> m_RootNode;           // AST根节点
    
    // 生成内容
    std::string m_GeneratedHTML;                        // 生成的HTML
    std::string m_GeneratedCSS;                         // 生成的CSS
    std::string m_GeneratedJS;                          // 生成的JavaScript
    
    // 模板和自定义管理
    std::unordered_map<std::string, TemplateNode*> m_StyleTemplates;     // 样式组模板映射
    std::unordered_map<std::string, TemplateNode*> m_ElementTemplates;   // 元素模板映射
    std::unordered_map<std::string, TemplateNode*> m_VarTemplates;       // 变量组模板映射
    std::unordered_map<std::string, CustomNode*> m_StyleCustoms;         // 样式自定义映射
    std::unordered_map<std::string, CustomNode*> m_ElementCustoms;       // 元素自定义映射
    std::unordered_map<std::string, CustomNode*> m_VarCustoms;           // 变量自定义映射
    
    // 全局样式块管理
    std::vector<std::string> m_GlobalStyleRules;        // 全局样式规则
    std::unordered_map<std::string, std::string> m_GlobalSelectors; // 全局选择器映射
    
    // 自动化选择器管理
    std::unordered_map<std::string, std::string> m_AutoClassMap;     // 自动类名映射
    std::unordered_map<std::string, std::string> m_AutoIdMap;        // 自动ID映射
    
    // 错误和警告
    bool m_HasError;                                    // 是否有生成错误
    std::string m_ErrorMessage;                         // 错误信息
    std::vector<std::string> m_Warnings;               // 警告列表
    
    // 生成选项
    bool m_GenerateIndentedHTML;                        // 是否生成缩进的HTML
    bool m_MinifyCSS;                                   // 是否压缩CSS
    bool m_AddSourceComments;                           // 是否添加源码注释
    int m_IndentLevel;                                  // 当前缩进级别
    
    // 暂时注释高级功能引擎
    // std::unique_ptr<SelectorAutomationEngine> m_SelectorEngine;     // 选择器自动化引擎
    // std::unique_ptr<TemplateEngine> m_TemplateEngine;               // 模板引擎
    // std::unique_ptr<CustomEngine> m_CustomEngine;                   // 自定义引擎

public:
    /**
     * 构造函数
     */
    CHTLGenerator();
    
    /**
     * 析构函数
     */
    ~CHTLGenerator() = default;
    
    /**
     * 从AST生成代码
     * @param rootNode AST根节点
     * @return 生成结果
     */
    GenerationResult Generate(std::unique_ptr<CHTLBaseNode> rootNode);
    
    /**
     * 获取生成的HTML内容
     * @return HTML内容
     */
    const std::string& GetHTMLContent() const { return m_GeneratedHTML; }
    
    /**
     * 获取生成的CSS内容
     * @return CSS内容
     */
    const std::string& GetCSSContent() const { return m_GeneratedCSS; }
    
    /**
     * 获取生成的JavaScript内容
     * @return JavaScript内容
     */
    const std::string& GetJavaScriptContent() const { return m_GeneratedJS; }
    
    /**
     * 设置生成选项
     * @param indentedHTML 是否生成缩进HTML
     * @param minifyCSS 是否压缩CSS
     * @param sourceComments 是否添加源码注释
     */
    void SetGenerationOptions(bool indentedHTML = true, bool minifyCSS = false, bool sourceComments = true);
    
    /**
     * 重置生成器
     */
    void Reset();
    
    // 重写访问者方法
    void VisitElementNode(ElementNode* node) override;
    void VisitTextNode(TextNode* node) override;
    void VisitStyleNode(StyleNode* node) override;
    void VisitScriptNode(ScriptNode* node) override;

private:
    // 预处理阶段
    /**
     * 收集模板和自定义定义
     * @param node 节点
     */
    void CollectTemplatesAndCustoms(CHTLBaseNode* node);
    
    /**
     * 处理模板继承
     */
    void ProcessTemplateInheritance();
    
    /**
     * 处理自定义继承和特例化
     */
    void ProcessCustomInheritanceAndSpecialization();
    
    /**
     * 解析变量引用
     */
    void ResolveVariableReferences();
    
    // HTML生成
    /**
     * 生成HTML文档结构
     * @param node 根节点
     */
    void GenerateHTMLDocument(CHTLBaseNode* node);
    
    /**
     * 生成HTML元素
     * @param element 元素节点
     */
    void GenerateHTMLElement(ElementNode* element);
    
    /**
     * 生成HTML属性
     * @param element 元素节点
     * @return 属性字符串
     */
    std::string GenerateHTMLAttributes(ElementNode* element);
    
    /**
     * 处理自闭合标签
     * @param element 元素节点
     * @return 是否为自闭合标签
     */
    bool HandleSelfClosingTag(ElementNode* element);
    
    // CSS生成
    /**
     * 生成CSS样式表
     * @param node 根节点
     */
    void GenerateCSSStylesheet(CHTLBaseNode* node);
    
    /**
     * 生成局部样式的全局CSS
     * @param styleNode 样式节点
     * @param contextElement 上下文元素
     */
    void GenerateLocalStyleGlobalCSS(StyleNode* styleNode, ElementNode* contextElement);
    
    /**
     * 生成选择器CSS规则
     * @param selector 选择器
     * @param properties CSS属性
     */
    void GenerateSelectorCSSRule(const std::string& selector, 
                                const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 处理上下文引用解析
     * @param selector 选择器
     * @param contextClass 上下文类名
     * @param contextId 上下文ID
     * @return 解析后的选择器
     */
    std::string ResolveContextReference(const std::string& selector, 
                                       const std::string& contextClass, 
                                       const std::string& contextId);
    
    /**
     * 生成伪类和伪元素CSS
     * @param baseSelector 基础选择器
     * @param pseudoSelector 伪选择器
     * @param properties CSS属性
     */
    void GeneratePseudoSelectorCSS(const std::string& baseSelector,
                                  const std::string& pseudoSelector,
                                  const std::unordered_map<std::string, std::string>& properties);
    
    // JavaScript生成（来自局部script）
    /**
     * 生成JavaScript代码
     * @param node 根节点
     */
    void GenerateJavaScriptCode(CHTLBaseNode* node);
    
    /**
     * 生成局部脚本JavaScript
     * @param scriptNode 脚本节点
     * @param contextElement 上下文元素
     */
    void GenerateLocalScriptJavaScript(ScriptNode* scriptNode, ElementNode* contextElement);
    
    /**
     * 处理{{&}}上下文引用
     * @param scriptContent 脚本内容
     * @param contextClass 上下文类名
     * @param contextId 上下文ID
     * @return 处理后的脚本内容
     */
    std::string ProcessScriptContextReferences(const std::string& scriptContent,
                                              const std::string& contextClass,
                                              const std::string& contextId);
    
    // 模板和自定义展开
    /**
     * 展开样式组模板
     * @param templateName 模板名称
     * @return 展开的CSS属性
     */
    std::unordered_map<std::string, std::string> ExpandStyleTemplate(const std::string& templateName);
    
    /**
     * 展开元素模板
     * @param templateName 模板名称
     * @return 展开的HTML内容
     */
    std::string ExpandElementTemplate(const std::string& templateName);
    
    /**
     * 展开变量组模板
     * @param templateName 模板名称
     * @return 变量映射
     */
    std::unordered_map<std::string, std::string> ExpandVarTemplate(const std::string& templateName);
    
    /**
     * 展开自定义样式组
     * @param customName 自定义名称
     * @param specialization 特例化参数
     * @return 展开的CSS属性
     */
    std::unordered_map<std::string, std::string> ExpandCustomStyle(const std::string& customName,
                                                                  const std::unordered_map<std::string, std::string>& specialization = {});
    
    /**
     * 展开自定义元素
     * @param customName 自定义名称
     * @param specialization 特例化参数
     * @return 展开的HTML内容
     */
    std::string ExpandCustomElement(const std::string& customName,
                                   const std::unordered_map<std::string, std::string>& specialization = {});
    
    // 变量解析
    /**
     * 解析变量引用
     * @param variableRef 变量引用（如ThemeColor(tableColor)）
     * @return 解析后的值
     */
    std::string ResolveVariableReference(const std::string& variableRef);
    
    /**
     * 处理变量特例化
     * @param variableRef 变量引用
     * @param newValue 新值
     * @return 特例化后的值
     */
    std::string ProcessVariableSpecialization(const std::string& variableRef, const std::string& newValue);
    
    // 工具方法
    /**
     * 生成缩进
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string GenerateIndent(int level = -1);
    
    /**
     * 添加源码注释
     * @param comment 注释内容
     * @param type 注释类型（HTML/CSS/JS）
     * @return 格式化的注释
     */
    std::string AddSourceComment(const std::string& comment, const std::string& type = "HTML");
    
    /**
     * 转义HTML特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string EscapeHTML(const std::string& text);
    
    /**
     * 处理UTF-8字符编码
     * @param text 文本内容
     * @return 处理后的文本
     */
    std::string ProcessUTF8Text(const std::string& text);
    
    /**
     * 验证生成的代码
     * @return 是否有效
     */
    bool ValidateGeneratedCode();
    
    /**
     * 优化生成的CSS
     */
    void OptimizeGeneratedCSS();
    
    /**
     * 优化生成的HTML
     */
    void OptimizeGeneratedHTML();
    
    /**
     * 设置生成错误
     * @param message 错误信息
     */
    void SetGenerationError(const std::string& message);
    
    /**
     * 添加生成警告
     * @param message 警告信息
     */
    void AddGenerationWarning(const std::string& message);
};

} // namespace CHTL