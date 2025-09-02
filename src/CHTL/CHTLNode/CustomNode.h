#pragma once

#include "CHTLBaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * 自定义类型枚举
 */
enum class CustomType {
    STYLE_CUSTOM,       // 自定义样式组（@Style）
    ELEMENT_CUSTOM,     // 自定义元素（@Element）
    VAR_CUSTOM          // 自定义变量组（@Var）
};

/**
 * 特例化操作类型枚举
 */
enum class SpecializationType {
    DELETE_PROPERTY,    // 删除属性
    DELETE_INHERIT,     // 删除继承
    ADD_PROPERTY,       // 添加属性
    MODIFY_PROPERTY,    // 修改属性
    INSERT_ELEMENT,     // 插入元素
    DELETE_ELEMENT      // 删除元素
};

/**
 * 特例化操作结构
 */
struct SpecializationOperation {
    SpecializationType Type;                // 操作类型
    std::string Target;                     // 操作目标
    std::string Value;                      // 操作值
    std::string Position;                   // 位置（for insert: after/before/replace/at top/at bottom）
    std::string Selector;                   // 选择器（for element operations）
    
    SpecializationOperation() = default;
    SpecializationOperation(SpecializationType type, const std::string& target, const std::string& value = "")
        : Type(type), Target(target), Value(value) {}
};

/**
 * CHTL自定义节点
 * 表示自定义定义（[Custom] @Style/@Element/@Var）
 * 支持特例化操作和高级功能
 */
class CustomNode : public CHTLBaseNode {
private:
    CustomType m_CustomType;                            // 自定义类型
    std::string m_CustomName;                           // 自定义名称
    std::string m_CustomContent;                        // 自定义内容
    std::unordered_map<std::string, std::string> m_CustomProperties; // 自定义属性
    std::vector<std::string> m_InheritedTemplates;      // 继承的模板列表
    std::vector<std::string> m_InheritedCustoms;        // 继承的自定义列表
    std::vector<std::unique_ptr<CHTLBaseNode>> m_CustomElements; // 自定义元素（对于元素自定义）
    bool m_IsValuelessStyleGroup;                       // 是否为无值样式组
    std::vector<std::string> m_ValuelessSelectors;     // 无值样式选择器列表
    std::vector<std::string> m_ValuelessProperties;    // 无值属性列表
    std::vector<SpecializationOperation> m_Specializations; // 特例化操作列表
    
    // 特例化标记
    bool m_HasSpecialization;                          // 是否有特例化操作
    bool m_HasInheritance;                             // 是否有继承
    bool m_HasDeletion;                                // 是否有删除操作
    bool m_HasInsertion;                               // 是否有插入操作

public:
    /**
     * 构造函数
     * @param customType 自定义类型
     * @param customName 自定义名称
     * @param line 行号
     * @param column 列号
     */
    CustomNode(CustomType customType, const std::string& customName, 
              size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~CustomNode() override = default;
    
    /**
     * 获取自定义类型
     * @return 自定义类型
     */
    CustomType GetCustomType() const { return m_CustomType; }
    
    /**
     * 获取自定义名称
     * @return 自定义名称
     */
    const std::string& GetCustomName() const { return m_CustomName; }
    
    /**
     * 设置自定义名称
     * @param name 新自定义名称
     */
    void SetCustomName(const std::string& name);
    
    /**
     * 获取自定义内容
     * @return 自定义内容
     */
    const std::string& GetCustomContent() const { return m_CustomContent; }
    
    /**
     * 设置自定义内容
     * @param content 新自定义内容
     */
    void SetCustomContent(const std::string& content);
    
    // 自定义属性管理
    /**
     * 添加自定义属性
     * @param name 属性名
     * @param value 属性值
     */
    void AddCustomProperty(const std::string& name, const std::string& value);
    
    /**
     * 获取自定义属性
     * @param name 属性名
     * @return 属性值
     */
    std::string GetCustomProperty(const std::string& name) const;
    
    /**
     * 检查是否有指定属性
     * @param name 属性名
     * @return 是否存在
     */
    bool HasCustomProperty(const std::string& name) const;
    
    /**
     * 获取所有自定义属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& GetCustomProperties() const { return m_CustomProperties; }
    
    // 无值样式组功能
    /**
     * 设置为无值样式组
     * @param isValueless 是否为无值
     */
    void SetValuelessStyleGroup(bool isValueless);
    
    /**
     * 检查是否为无值样式组
     * @return 是否为无值样式组
     */
    bool IsValuelessStyleGroup() const { return m_IsValuelessStyleGroup; }
    
    /**
     * 添加无值样式选择器
     * @param selector 选择器
     */
    void AddValuelessSelector(const std::string& selector);
    
    /**
     * 获取无值样式选择器列表
     * @return 选择器列表
     */
    const std::vector<std::string>& GetValuelessSelectors() const { return m_ValuelessSelectors; }
    
    /**
     * 生成无值样式组CSS
     * @return 生成的CSS
     */
    std::string GenerateValuelessStyleCSS() const;
    
    // 无值样式组管理（方法已在上面声明）
    
    /**
     * 添加无值属性
     * @param property 属性名
     */
    void AddValuelessProperty(const std::string& property);
    
    /**
     * 获取无值属性列表
     * @return 属性列表
     */
    const std::vector<std::string>& GetValuelessProperties() const { return m_ValuelessProperties; }
    
    // 继承管理
    /**
     * 添加继承的模板
     * @param templateName 模板名称
     */
    void AddInheritedTemplate(const std::string& templateName);
    
    /**
     * 添加继承的自定义
     * @param customName 自定义名称
     */
    void AddInheritedCustom(const std::string& customName);
    
    /**
     * 获取继承的模板列表
     * @return 模板列表
     */
    const std::vector<std::string>& GetInheritedTemplates() const { return m_InheritedTemplates; }
    
    /**
     * 获取继承的自定义列表
     * @return 自定义列表
     */
    const std::vector<std::string>& GetInheritedCustoms() const { return m_InheritedCustoms; }
    
    /**
     * 检查是否有继承
     * @return 是否有继承
     */
    bool HasInheritance() const { return m_HasInheritance; }
    
    // 特例化操作管理
    /**
     * 添加特例化操作
     * @param operation 特例化操作
     */
    void AddSpecializationOperation(const SpecializationOperation& operation);
    
    /**
     * 添加删除属性操作
     * @param properties 要删除的属性列表（逗号分隔）
     */
    void AddDeletePropertyOperation(const std::string& properties);
    
    /**
     * 添加删除继承操作
     * @param inherit 要删除的继承
     */
    void AddDeleteInheritOperation(const std::string& inherit);
    
    /**
     * 添加插入元素操作
     * @param position 插入位置（after/before/replace/at top/at bottom）
     * @param selector 目标选择器
     * @param content 插入内容
     */
    void AddInsertElementOperation(const std::string& position, 
                                  const std::string& selector, 
                                  const std::string& content);
    
    /**
     * 添加删除元素操作
     * @param selector 要删除的元素选择器
     */
    void AddDeleteElementOperation(const std::string& selector);
    
    /**
     * 获取所有特例化操作
     * @return 操作列表
     */
    const std::vector<SpecializationOperation>& GetSpecializationOperations() const { return m_Specializations; }
    
    /**
     * 检查是否有特例化操作
     * @return 是否有特例化
     */
    bool HasSpecialization() const { return m_HasSpecialization; }
    
    /**
     * 检查是否有删除操作
     * @return 是否有删除
     */
    bool HasDeletion() const { return m_HasDeletion; }
    
    /**
     * 检查是否有插入操作
     * @return 是否有插入
     */
    bool HasInsertion() const { return m_HasInsertion; }
    
    // 索引访问管理
    /**
     * 获取指定索引的元素
     * @param index 索引
     * @return 元素节点指针
     */
    CHTLBaseNode* GetElementByIndex(size_t index) const;
    
    /**
     * 获取指定类型的元素列表
     * @param elementType 元素类型
     * @return 元素列表
     */
    std::vector<CHTLBaseNode*> GetElementsByType(const std::string& elementType) const;
    
    // 自定义元素管理
    /**
     * 添加自定义元素
     * @param element 元素节点
     */
    void AddCustomElement(std::unique_ptr<CHTLBaseNode> element);
    
    /**
     * 获取自定义元素列表
     * @return 元素列表
     */
    const std::vector<std::unique_ptr<CHTLBaseNode>>& GetCustomElements() const { return m_CustomElements; }
    
    // 代码生成
    /**
     * 生成CSS代码（用于样式自定义）
     * @return CSS代码
     */
    std::string GenerateCSS() const;
    
    /**
     * 生成HTML代码（用于元素自定义）
     * @return HTML代码
     */
    std::string GenerateHTML() const;
    
    /**
     * 生成变量映射（用于变量自定义）
     * @return 变量映射
     */
    std::unordered_map<std::string, std::string> GenerateVariableMap() const;
    
    /**
     * 应用特例化操作
     * @param baseContent 基础内容
     * @return 特例化后的内容
     */
    std::string ApplySpecializations(const std::string& baseContent) const;
    
    // 验证
    /**
     * 验证自定义定义
     * @return 是否有效
     */
    bool ValidateCustom() const;
    
    /**
     * 获取自定义类型字符串
     * @return 类型字符串
     */
    std::string GetCustomTypeString() const;
    
    // 访问者模式实现
    void Accept(CHTLNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLBaseNode> Clone() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 解析自定义内容
     * @param content 内容
     */
    void ParseCustomContent(const std::string& content);
    
    /**
     * 解析特例化语法
     * @param content 内容
     */
    void ParseSpecializationSyntax(const std::string& content);
    
    /**
     * 处理删除语法
     * @param deleteContent 删除内容
     */
    void ProcessDeleteSyntax(const std::string& deleteContent);
    
    /**
     * 处理插入语法
     * @param insertContent 插入内容
     */
    void ProcessInsertSyntax(const std::string& insertContent);
    
    /**
     * 验证自定义名称
     * @param name 自定义名称
     * @return 是否有效
     */
    bool IsValidCustomName(const std::string& name) const;
    
    /**
     * 解析索引访问语法
     * @param content 内容
     * @return 索引访问映射
     */
    std::unordered_map<size_t, std::string> ParseIndexAccess(const std::string& content) const;
    
    /**
     * 解析样式自定义内容
     * @param content 内容
     */
    void ParseStyleCustomContent(const std::string& content);
    
    /**
     * 解析元素自定义内容
     * @param content 内容
     */
    void ParseElementCustomContent(const std::string& content);
    
    /**
     * 解析变量自定义内容
     * @param content 内容
     */
    void ParseVarCustomContent(const std::string& content);
};

} // namespace CHTL