#pragma once

#include "CHTLBaseNode.h"
#include <vector>

namespace CHTL {

/**
 * CHTL脚本节点
 * 表示局部脚本块（script{}），属于CHTL编译器处理
 * 支持CHTL语法元素：模板变量、自定义变量组、变量组特例化、命名空间from、{{&}}等特供语法
 */
class ScriptNode : public CHTLBaseNode {
private:
    std::string m_ScriptContent;                        // 脚本内容
    std::vector<std::string> m_TemplateVariableUsages; // 模板变量使用列表
    std::vector<std::string> m_CustomVariableUsages;   // 自定义变量组使用列表
    std::vector<std::string> m_VariableSpecializations; // 变量组特例化列表
    std::vector<std::string> m_NamespaceFromUsages;    // 命名空间from使用列表
    std::vector<std::string> m_ContextReferences;      // 上下文引用列表（{{&}}）
    std::vector<std::string> m_CHTLJSSyntaxBlocks;     // CHTL JS语法块列表
    std::vector<std::string> m_VariableReferences;     // 变量引用列表
    
    bool m_HasCHTLJSSyntax;                            // 是否包含CHTL JS语法
    bool m_HasContextReference;                        // 是否有上下文引用
    bool m_HasTemplateUsage;                           // 是否有模板使用
    bool m_HasCustomUsage;                             // 是否有自定义使用

public:
    /**
     * 构造函数
     * @param scriptContent 脚本内容
     * @param line 行号
     * @param column 列号
     */
    ScriptNode(const std::string& scriptContent = "", size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~ScriptNode() override = default;
    
    /**
     * 获取脚本内容
     * @return 脚本内容
     */
    const std::string& GetScriptContent() const { return m_ScriptContent; }
    
    /**
     * 设置脚本内容
     * @param content 新脚本内容
     */
    void SetScriptContent(const std::string& content);
    
    // CHTL语法元素管理（局部script允许的CHTL语法）
    /**
     * 添加模板变量使用
     * @param templateVar 模板变量引用
     */
    void AddTemplateVariableUsage(const std::string& templateVar);
    
    /**
     * 添加自定义变量组使用
     * @param customVar 自定义变量组引用
     */
    void AddCustomVariableUsage(const std::string& customVar);
    
    /**
     * 添加变量组特例化
     * @param specialization 特例化表达式
     */
    void AddVariableSpecialization(const std::string& specialization);
    
    /**
     * 添加命名空间from使用
     * @param fromUsage from表达式
     */
    void AddNamespaceFromUsage(const std::string& fromUsage);
    
    /**
     * 添加上下文引用（{{&}}特供语法）
     * @param contextRef 上下文引用
     */
    void AddContextReference(const std::string& contextRef);
    
    /**
     * 获取模板变量使用列表
     * @return 使用列表
     */
    const std::vector<std::string>& GetTemplateVariableUsages() const { return m_TemplateVariableUsages; }
    
    /**
     * 获取自定义变量组使用列表
     * @return 使用列表
     */
    const std::vector<std::string>& GetCustomVariableUsages() const { return m_CustomVariableUsages; }
    
    /**
     * 获取变量组特例化列表
     * @return 特例化列表
     */
    const std::vector<std::string>& GetVariableSpecializations() const { return m_VariableSpecializations; }
    
    /**
     * 获取命名空间from使用列表
     * @return from使用列表
     */
    const std::vector<std::string>& GetNamespaceFromUsages() const { return m_NamespaceFromUsages; }
    
    /**
     * 获取上下文引用列表
     * @return 引用列表
     */
    const std::vector<std::string>& GetContextReferences() const { return m_ContextReferences; }
    
    // CHTL JS语法管理
    /**
     * 添加CHTL JS语法块
     * @param chtljsBlock CHTL JS语法块
     */
    void AddCHTLJSSyntaxBlock(const std::string& chtljsBlock);
    
    /**
     * 获取CHTL JS语法块列表
     * @return 语法块列表
     */
    const std::vector<std::string>& GetCHTLJSSyntaxBlocks() const { return m_CHTLJSSyntaxBlocks; }
    
    /**
     * 检查是否包含CHTL JS语法
     * @return 是否包含CHTL JS语法
     */
    bool HasCHTLJSSyntax() const { return m_HasCHTLJSSyntax; }
    
    /**
     * 检查是否有上下文引用
     * @return 是否有上下文引用
     */
    bool HasContextReference() const { return m_HasContextReference; }
    
    /**
     * 检查是否有模板使用
     * @return 是否有模板使用
     */
    bool HasTemplateUsage() const { return m_HasTemplateUsage; }
    
    /**
     * 检查是否有自定义使用
     * @return 是否有自定义使用
     */
    bool HasCustomUsage() const { return m_HasCustomUsage; }
    
    // 代码生成
    /**
     * 生成JavaScript代码
     * @param contextClass 上下文类名（用于{{&}}解析）
     * @param contextId 上下文ID（用于{{&}}解析）
     * @return 生成的JavaScript代码
     */
    std::string GenerateJavaScript(const std::string& contextClass = "", 
                                  const std::string& contextId = "") const;
    
    /**
     * 解析上下文引用（{{&}}）
     * @param contextClass 上下文类名
     * @param contextId 上下文ID
     * @return 解析后的选择器
     */
    std::string ResolveContextReference(const std::string& contextClass, const std::string& contextId) const;
    
    // 解析和处理
    /**
     * 解析脚本内容
     * @param scriptContent 脚本内容字符串
     */
    void ParseScriptContent(const std::string& scriptContent);
    
    /**
     * 分离CHTL语法和CHTL JS语法
     * @param content 原始内容
     */
    void SeparateCHTLAndCHTLJSSyntax(const std::string& content);
    
    /**
     * 验证允许的CHTL语法
     * @param syntax 语法内容
     * @return 是否允许
     */
    bool IsAllowedCHTLSyntax(const std::string& syntax) const;
    
    // 访问者模式实现
    void Accept(CHTLNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLBaseNode> Clone() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 检测CHTL JS语法特征
     * @param content 内容
     * @return 是否包含CHTL JS语法
     */
    bool DetectCHTLJSSyntax(const std::string& content);
    
    /**
     * 提取CHTL语法元素
     * @param content 内容
     */
    void ExtractCHTLSyntaxElements(const std::string& content);
    
    /**
     * 提取CHTL JS语法块
     * @param content 内容
     */
    void ExtractCHTLJSSyntaxBlocks(const std::string& content);
    
    /**
     * 处理变量引用解析
     * @param variableRef 变量引用
     * @return 解析后的值
     */
    std::string ResolveVariableReference(const std::string& variableRef) const;
};

} // namespace CHTL