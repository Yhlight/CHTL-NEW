#pragma once

#include "CHTLBaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// 前向声明
class CHTLGlobalMap;

/**
 * 模板类型枚举
 */
enum class TemplateType {
    STYLE_TEMPLATE,     // 样式组模板（@Style）
    ELEMENT_TEMPLATE,   // 元素模板（@Element）
    VAR_TEMPLATE        // 变量组模板（@Var）
};

/**
 * CHTL模板节点
 * 表示模板定义（[Template] @Style/@Element/@Var）
 */
class TemplateNode : public CHTLBaseNode {
private:
    TemplateType m_TemplateType;                        // 模板类型
    std::string m_TemplateName;                         // 模板名称
    std::string m_TemplateContent;                      // 模板内容
    std::unordered_map<std::string, std::string> m_TemplateProperties; // 模板属性
    std::vector<std::string> m_InheritedTemplates;      // 继承的模板列表
    std::string m_FullNamespacePath;                    // 完整命名空间路径
    std::vector<std::unique_ptr<CHTLBaseNode>> m_TemplateElements; // 模板元素（对于元素模板）
    
    bool m_IsInheritanceTemplate;                       // 是否为继承模板
    bool m_HasExplicitInherit;                         // 是否有显式继承（inherit关键字）

public:
    /**
     * 构造函数
     * @param templateType 模板类型
     * @param templateName 模板名称
     * @param line 行号
     * @param column 列号
     */
    TemplateNode(TemplateType templateType, const std::string& templateName, 
                size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~TemplateNode() override = default;
    
    /**
     * 获取模板类型
     * @return 模板类型
     */
    TemplateType GetTemplateType() const { return m_TemplateType; }
    
    /**
     * 获取模板名称
     * @return 模板名称
     */
    const std::string& GetTemplateName() const { return m_TemplateName; }
    
    /**
     * 设置模板名称
     * @param name 新模板名称
     */
    void SetTemplateName(const std::string& name);
    
    /**
     * 获取模板内容
     * @return 模板内容
     */
    const std::string& GetTemplateContent() const { return m_TemplateContent; }
    
    /**
     * 设置模板内容
     * @param content 新模板内容
     */
    void SetTemplateContent(const std::string& content);
    
    // 模板属性管理
    /**
     * 添加模板属性
     * @param name 属性名
     * @param value 属性值
     */
    void AddTemplateProperty(const std::string& name, const std::string& value);
    
    /**
     * 获取模板属性
     * @param name 属性名
     * @return 属性值
     */
    std::string GetTemplateProperty(const std::string& name) const;
    
    /**
     * 检查是否有指定属性
     * @param name 属性名
     * @return 是否存在
     */
    bool HasTemplateProperty(const std::string& name) const;
    
    /**
     * 获取所有模板属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& GetTemplateProperties() const { return m_TemplateProperties; }
    
    // 继承管理
    /**
     * 添加继承的模板
     * @param templateName 被继承的模板名称
     * @param isExplicit 是否为显式继承（inherit关键字）
     */
    void AddInheritedTemplate(const std::string& templateName, bool isExplicit = false);
    
    /**
     * 获取继承的模板列表
     * @return 继承模板列表
     */
    const std::vector<std::string>& GetInheritedTemplates() const { return m_InheritedTemplates; }
    
    // 全缀名访问功能
    /**
     * 设置模板的完整命名空间路径
     * @param namespacePath 命名空间路径
     */
    void SetFullNamespacePath(const std::string& namespacePath);
    
    /**
     * 获取模板的完整命名空间路径
     * @return 命名空间路径
     */
    const std::string& GetFullNamespacePath() const { return m_FullNamespacePath; }
    
    /**
     * 获取全缀名（命名空间::模板名）
     * @return 全缀名
     */
    std::string GetFullQualifiedName() const;
    
    /**
     * 解析全缀名引用
     * @param fullName 全缀名（如 UI::Button::Style）
     * @return 解析结果（命名空间和模板名）
     */
    static std::pair<std::string, std::string> ParseFullQualifiedName(const std::string& fullName);
    
    /**
     * 查找指定命名空间中的模板
     * @param namespacePath 命名空间路径
     * @param templateName 模板名称
     * @param globalMap 全局映射表
     * @return 找到的模板节点
     */
    static TemplateNode* FindTemplateInNamespace(const std::string& namespacePath, 
                                                 const std::string& templateName,
                                                 CHTLGlobalMap* globalMap);
    
    /**
     * 解析模板引用路径
     * @param referencePath 引用路径（支持相对路径和绝对路径）
     * @param currentNamespace 当前命名空间
     * @return 解析后的完整路径
     */
    static std::string ResolveTemplateReferencePath(const std::string& referencePath, 
                                                    const std::string& currentNamespace);
    
    /**
     * 检查是否有继承
     * @return 是否有继承
     */
    bool HasInheritance() const { return !m_InheritedTemplates.empty(); }
    
    /**
     * 检查是否为显式继承
     * @return 是否为显式继承
     */
    bool HasExplicitInherit() const { return m_HasExplicitInherit; }
    
    // 模板元素管理（用于元素模板）
    /**
     * 添加模板元素
     * @param element 元素节点
     */
    void AddTemplateElement(std::unique_ptr<CHTLBaseNode> element);
    
    /**
     * 获取模板元素列表
     * @return 元素列表
     */
    const std::vector<std::unique_ptr<CHTLBaseNode>>& GetTemplateElements() const { return m_TemplateElements; }
    
    /**
     * 检查是否有模板元素
     * @return 是否有模板元素
     */
    bool HasTemplateElements() const { return !m_TemplateElements.empty(); }
    
    // 模板解析和处理
    /**
     * 解析模板内容
     * @param content 模板内容
     */
    void ParseTemplateContent(const std::string& content);
    
    /**
     * 应用模板继承
     * @param parentTemplate 父模板节点
     */
    void ApplyTemplateInheritance(const TemplateNode* parentTemplate);
    
    /**
     * 展开模板内容
     * @return 展开后的内容
     */
    std::string ExpandTemplateContent() const;
    
    // 代码生成
    /**
     * 生成CSS代码（用于样式组模板）
     * @return CSS代码
     */
    std::string GenerateCSS() const;
    
    /**
     * 生成HTML代码（用于元素模板）
     * @return HTML代码
     */
    std::string GenerateHTML() const;
    
    /**
     * 生成变量映射（用于变量组模板）
     * @return 变量映射
     */
    std::unordered_map<std::string, std::string> GenerateVariableMap() const;
    
    // 验证
    /**
     * 验证模板定义
     * @return 是否有效
     */
    bool ValidateTemplate() const;
    
    /**
     * 获取模板类型字符串
     * @return 类型字符串
     */
    std::string GetTemplateTypeString() const;
    
    // 访问者模式实现
    void Accept(CHTLNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLBaseNode> Clone() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 解析样式组模板内容
     * @param content 内容
     */
    void ParseStyleTemplateContent(const std::string& content);
    
    /**
     * 解析元素模板内容
     * @param content 内容
     */
    void ParseElementTemplateContent(const std::string& content);
    
    /**
     * 解析变量组模板内容
     * @param content 内容
     */
    void ParseVarTemplateContent(const std::string& content);
    
    /**
     * 处理模板继承语法
     * @param content 内容
     */
    void ProcessTemplateInheritance(const std::string& content);
    
    /**
     * 验证模板名称
     * @param name 模板名称
     * @return 是否有效
     */
    bool IsValidTemplateName(const std::string& name) const;
};

} // namespace CHTL