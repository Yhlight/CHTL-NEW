#pragma once

#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CHTLBaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 模板引擎模板类型枚举 (避免与TemplateNode.h冲突)
 */
enum class TemplateEngineType {
    STYLE_TEMPLATE,     // @Style 样式组模板
    ELEMENT_TEMPLATE,   // @Element 元素模板
    VAR_TEMPLATE        // @Var 变量组模板
};

/**
 * 模板信息结构
 */
struct TemplateInfo {
    std::string Name;                                           // 模板名称
    TemplateEngineType Type;                                    // 模板类型
    std::unordered_map<std::string, std::string> Properties;   // 模板属性
    std::vector<std::unique_ptr<CHTLBaseNode>> Elements;        // 模板元素 (元素模板)
    std::vector<std::string> InheritedTemplates;               // 继承的模板列表
    std::string Namespace;                                      // 所属命名空间
    bool IsProcessed = false;                                   // 是否已处理继承
};

/**
 * 变量组信息
 */
struct VariableGroup {
    std::string Name;                                           // 变量组名称
    std::unordered_map<std::string, std::string> Variables;    // 变量映射
    std::string Namespace;                                      // 所属命名空间
};

/**
 * 模板引擎
 * 实现完整的模板系统，包括样式组、元素、变量组模板
 * 支持继承、组合、特例化等高级特性
 */
class TemplateEngine {
private:
    // 模板存储
    std::unordered_map<std::string, std::unique_ptr<TemplateInfo>> m_StyleTemplates;
    std::unordered_map<std::string, std::unique_ptr<TemplateInfo>> m_ElementTemplates;
    std::unordered_map<std::string, std::unique_ptr<VariableGroup>> m_VarTemplates;
    
    // 继承处理状态
    std::unordered_set<std::string> m_ProcessingTemplates;     // 正在处理的模板 (避免循环继承)
    
    // 命名空间管理
    std::string m_CurrentNamespace;
    std::unordered_map<std::string, std::string> m_NamespaceAliases;

public:
    /**
     * 构造函数
     */
    TemplateEngine();
    
    /**
     * 析构函数
     */
    ~TemplateEngine() = default;
    
    /**
     * 注册样式组模板
     * @param name 模板名称
     * @param templateNode 模板节点
     * @param namespaceName 命名空间
     */
    void RegisterStyleTemplate(const std::string& name, 
                              TemplateNode* templateNode,
                              const std::string& namespaceName = "");
    
    /**
     * 注册元素模板
     * @param name 模板名称
     * @param templateNode 模板节点
     * @param namespaceName 命名空间
     */
    void RegisterElementTemplate(const std::string& name, 
                                TemplateNode* templateNode,
                                const std::string& namespaceName = "");
    
    /**
     * 注册变量组模板
     * @param name 模板名称
     * @param templateNode 模板节点
     * @param namespaceName 命名空间
     */
    void RegisterVarTemplate(const std::string& name, 
                            TemplateNode* templateNode,
                            const std::string& namespaceName = "");
    
    /**
     * 应用样式组模板
     * @param templateName 模板名称
     * @param targetElement 目标元素
     * @param overrideProperties 覆盖属性
     * @return 是否成功应用
     */
    bool ApplyStyleTemplate(const std::string& templateName,
                           ElementNode* targetElement,
                           const std::unordered_map<std::string, std::string>& overrideProperties = {});
    
    /**
     * 应用元素模板
     * @param templateName 模板名称
     * @param parentElement 父元素
     * @param insertPosition 插入位置
     * @return 是否成功应用
     */
    bool ApplyElementTemplate(const std::string& templateName,
                             ElementNode* parentElement,
                             size_t insertPosition = SIZE_MAX);
    
    /**
     * 解析变量组引用
     * @param varExpression 变量表达式 (如: ThemeColor(tableColor))
     * @return 解析后的值
     */
    std::string ResolveVariableReference(const std::string& varExpression);
    
    /**
     * 处理模板继承
     * @param templateName 模板名称
     * @param templateType 模板类型
     */
    void ProcessTemplateInheritance(const std::string& templateName, TemplateEngineType templateType);
    
    /**
     * 处理显式继承 (inherit 关键字)
     * @param templateInfo 模板信息
     * @param inheritTarget 继承目标
     */
    void ProcessExplicitInheritance(TemplateInfo* templateInfo, const std::string& inheritTarget);
    
    /**
     * 处理组合式继承 (@Style TemplateName)
     * @param templateInfo 模板信息
     * @param compositeTarget 组合目标
     */
    void ProcessCompositeInheritance(TemplateInfo* templateInfo, const std::string& compositeTarget);
    
    /**
     * 检查循环继承
     * @param templateName 模板名称
     * @param inheritTarget 继承目标
     * @return 是否存在循环继承
     */
    bool HasCircularInheritance(const std::string& templateName, const std::string& inheritTarget);
    
    /**
     * 解析全缀名
     * @param fullName 全缀名 (如: [Template] @Style TemplateName)
     * @return 解析后的名称和类型
     */
    std::pair<std::string, TemplateEngineType> ParseFullQualifiedName(const std::string& fullName);
    
    /**
     * 设置当前命名空间
     * @param namespaceName 命名空间名称
     */
    void SetCurrentNamespace(const std::string& namespaceName);
    
    /**
     * 添加命名空间别名
     * @param alias 别名
     * @param namespaceName 实际命名空间
     */
    void AddNamespaceAlias(const std::string& alias, const std::string& namespaceName);
    
    /**
     * 获取模板统计信息
     * @return 模板数量统计
     */
    std::unordered_map<std::string, size_t> GetTemplateStatistics();
    
    /**
     * 验证所有模板的完整性
     * @return 验证结果
     */
    bool ValidateAllTemplates();
    
    /**
     * 清理所有模板
     */
    void Clear();

private:
    /**
     * 解析模板属性
     * @param templateContent 模板内容
     * @return 属性映射
     */
    std::unordered_map<std::string, std::string> ParseTemplateProperties(const std::string& templateContent);
    
    /**
     * 解析变量组
     * @param varContent 变量组内容
     * @return 变量映射
     */
    std::unordered_map<std::string, std::string> ParseVariableGroup(const std::string& varContent);
    
    /**
     * 合并模板属性 (处理继承)
     * @param baseProperties 基础属性
     * @param inheritedProperties 继承的属性
     * @return 合并后的属性
     */
    std::unordered_map<std::string, std::string> MergeTemplateProperties(
        const std::unordered_map<std::string, std::string>& baseProperties,
        const std::unordered_map<std::string, std::string>& inheritedProperties);
    
    /**
     * 克隆元素节点
     * @param sourceElement 源元素
     * @return 克隆的元素
     */
    std::unique_ptr<ElementNode> CloneElement(const ElementNode* sourceElement);
    
    /**
     * 解析命名空间限定名
     * @param name 可能包含命名空间的名称
     * @return 解析后的命名空间和名称
     */
    std::pair<std::string, std::string> ParseNamespacedName(const std::string& name);
};

} // namespace CHTL