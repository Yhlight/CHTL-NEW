#pragma once

#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CHTLBaseNode.h"
#include "TemplateEngine.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 特例化操作类型
 */
enum class SpecializationOperation {
    DELETE_PROPERTY,        // delete property
    DELETE_ELEMENT,         // delete element
    DELETE_INHERITANCE,     // delete @Style/@Element inheritance
    INSERT_AFTER,           // insert after
    INSERT_BEFORE,          // insert before
    INSERT_REPLACE,         // insert replace
    INSERT_AT_TOP,          // insert at top
    INSERT_AT_BOTTOM,       // insert at bottom
    MODIFY_PROPERTY,        // modify property
    ADD_PROPERTY,           // add property
    ADD_ELEMENT             // add element
};

/**
 * 特例化操作信息
 */
struct SpecializationInfo {
    SpecializationOperation Operation;      // 操作类型
    std::string Target;                     // 操作目标
    std::string Value;                      // 操作值
    size_t Index = SIZE_MAX;                // 索引 (用于数组访问)
    std::vector<std::string> Parameters;    // 操作参数
};

/**
 * 自定义信息结构
 */
struct CustomInfo {
    std::string Name;                                           // 自定义名称
    TemplateEngineType Type;                                    // 自定义类型
    std::unordered_map<std::string, std::string> Properties;   // 自定义属性
    std::vector<std::unique_ptr<CHTLBaseNode>> Elements;        // 自定义元素
    std::vector<std::string> InheritedTemplates;               // 继承的模板/自定义
    std::vector<SpecializationInfo> Specializations;           // 特例化操作
    std::string Namespace;                                      // 所属命名空间
    bool IsProcessed = false;                                   // 是否已处理
    
    // 无值样式组支持
    std::vector<std::string> ValuelessProperties;              // 无值属性列表
    bool HasValuelessProperties = false;                        // 是否包含无值属性
};

/**
 * 自定义引擎
 * 实现完整的自定义系统，包括特例化操作
 * 支持无值样式组、索引访问、插入删除等高级特性
 */
class CustomEngine {
private:
    // 自定义存储
    std::unordered_map<std::string, std::unique_ptr<CustomInfo>> m_StyleCustoms;
    std::unordered_map<std::string, std::unique_ptr<CustomInfo>> m_ElementCustoms;
    std::unordered_map<std::string, std::unique_ptr<CustomInfo>> m_VarCustoms;
    
    // 模板引擎引用 (用于模板和自定义的相互继承)
    TemplateEngine* m_TemplateEngine;
    
    // 处理状态
    std::unordered_set<std::string> m_ProcessingCustoms;
    
    // 命名空间管理
    std::string m_CurrentNamespace;

public:
    /**
     * 构造函数
     */
    CustomEngine(TemplateEngine* templateEngine = nullptr);
    
    /**
     * 析构函数
     */
    ~CustomEngine() = default;
    
    /**
     * 注册自定义样式组
     * @param name 自定义名称
     * @param customNode 自定义节点
     * @param namespaceName 命名空间
     */
    void RegisterStyleCustom(const std::string& name, 
                            CustomNode* customNode,
                            const std::string& namespaceName = "");
    
    /**
     * 注册自定义元素
     * @param name 自定义名称
     * @param customNode 自定义节点
     * @param namespaceName 命名空间
     */
    void RegisterElementCustom(const std::string& name, 
                              CustomNode* customNode,
                              const std::string& namespaceName = "");
    
    /**
     * 注册自定义变量组
     * @param name 自定义名称
     * @param customNode 自定义节点
     * @param namespaceName 命名空间
     */
    void RegisterVarCustom(const std::string& name, 
                          CustomNode* customNode,
                          const std::string& namespaceName = "");
    
    /**
     * 应用自定义样式组
     * @param customName 自定义名称
     * @param targetElement 目标元素
     * @param valueProperties 值属性 (用于无值样式组)
     * @param specializations 特例化操作
     * @return 是否成功应用
     */
    bool ApplyStyleCustom(const std::string& customName,
                         ElementNode* targetElement,
                         const std::unordered_map<std::string, std::string>& valueProperties = {},
                         const std::vector<SpecializationInfo>& specializations = {});
    
    /**
     * 应用自定义元素
     * @param customName 自定义名称
     * @param parentElement 父元素
     * @param specializations 特例化操作
     * @return 是否成功应用
     */
    bool ApplyElementCustom(const std::string& customName,
                           ElementNode* parentElement,
                           const std::vector<SpecializationInfo>& specializations = {});
    
    /**
     * 处理特例化操作
     * @param customInfo 自定义信息
     * @param specializations 特例化操作列表
     */
    void ProcessSpecializations(CustomInfo* customInfo, 
                               const std::vector<SpecializationInfo>& specializations);
    
    /**
     * 处理删除操作
     * @param customInfo 自定义信息
     * @param deleteTarget 删除目标
     */
    void ProcessDeleteOperation(CustomInfo* customInfo, const std::string& deleteTarget);
    
    /**
     * 处理插入操作
     * @param customInfo 自定义信息
     * @param insertInfo 插入信息
     */
    void ProcessInsertOperation(CustomInfo* customInfo, const SpecializationInfo& insertInfo);
    
    /**
     * 处理索引访问
     * @param elements 元素列表
     * @param selector 选择器 (如: div[1])
     * @return 匹配的元素索引列表
     */
    std::vector<size_t> ProcessIndexAccess(const std::vector<std::unique_ptr<CHTLBaseNode>>& elements,
                                          const std::string& selector);
    
    /**
     * 解析特例化操作
     * @param operationText 操作文本
     * @return 特例化操作信息
     */
    SpecializationInfo ParseSpecializationOperation(const std::string& operationText);
    
    /**
     * 处理无值样式组
     * @param customInfo 自定义信息
     * @param valueProperties 提供的值属性
     */
    void ProcessValuelessStyleGroup(CustomInfo* customInfo,
                                   const std::unordered_map<std::string, std::string>& valueProperties);
    
    /**
     * 验证索引访问
     * @param index 索引值
     * @param maxSize 最大大小
     * @return 是否有效
     */
    bool ValidateIndexAccess(size_t index, size_t maxSize);
    
    /**
     * 设置模板引擎引用
     * @param templateEngine 模板引擎
     */
    void SetTemplateEngine(TemplateEngine* templateEngine);
    
    /**
     * 设置当前命名空间
     * @param namespaceName 命名空间名称
     */
    void SetCurrentNamespace(const std::string& namespaceName);
    
    /**
     * 获取自定义统计信息
     * @return 自定义数量统计
     */
    std::unordered_map<std::string, size_t> GetCustomStatistics();
    
    /**
     * 验证所有自定义的完整性
     * @return 验证结果
     */
    bool ValidateAllCustoms();
    
    /**
     * 清理所有自定义
     */
    void Clear();

private:
    /**
     * 解析自定义内容
     * @param customContent 自定义内容
     * @param customType 自定义类型
     * @return 解析后的自定义信息
     */
    std::unique_ptr<CustomInfo> ParseCustomContent(const std::string& customContent, 
                                                  TemplateEngineType customType);
    
    /**
     * 检查循环继承
     * @param customName 自定义名称
     * @param inheritTarget 继承目标
     * @return 是否存在循环继承
     */
    bool HasCircularInheritance(const std::string& customName, const std::string& inheritTarget);
    
    /**
     * 合并继承的属性
     * @param baseCustom 基础自定义
     * @param inheritedCustom 继承的自定义
     */
    void MergeInheritedCustom(CustomInfo* baseCustom, const CustomInfo* inheritedCustom);
    
    /**
     * 解析命名空间限定名
     * @param name 可能包含命名空间的名称
     * @return 解析后的命名空间和名称
     */
    std::pair<std::string, std::string> ParseNamespacedName(const std::string& name);
};

} // namespace CHTL