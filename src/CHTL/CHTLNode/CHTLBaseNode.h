#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 前向声明
class CHTLNodeVisitor;

/**
 * CHTL节点类型枚举
 * 涵盖所有CHTL语法特征的节点类型
 */
enum class CHTLNodeType {
    // 基础节点
    ELEMENT_NODE,           // 元素节点（HTML元素）
    TEXT_NODE,              // 文本节点（text{}）
    COMMENT_NODE,           // 注释节点（//、/**/、--）
    
    // 样式和脚本节点
    STYLE_NODE,             // 局部样式块节点（style{}）
    SCRIPT_NODE,            // 局部脚本块节点（script{}，属于CHTL）
    
    // 模板节点
    TEMPLATE_STYLE_NODE,    // 样式组模板节点（[Template] @Style）
    TEMPLATE_ELEMENT_NODE,  // 元素模板节点（[Template] @Element）
    TEMPLATE_VAR_NODE,      // 变量组模板节点（[Template] @Var）
    TEMPLATE_USAGE_NODE,    // 模板使用节点（@Style、@Element、@Var）
    
    // 自定义节点
    CUSTOM_STYLE_NODE,      // 自定义样式组节点（[Custom] @Style）
    CUSTOM_ELEMENT_NODE,    // 自定义元素节点（[Custom] @Element）
    CUSTOM_VAR_NODE,        // 自定义变量组节点（[Custom] @Var）
    CUSTOM_USAGE_NODE,      // 自定义使用节点
    
    // 原始嵌入节点
    ORIGIN_HTML_NODE,       // HTML原始嵌入节点（[Origin] @Html）
    ORIGIN_STYLE_NODE,      // 样式原始嵌入节点（[Origin] @Style）
    ORIGIN_JAVASCRIPT_NODE, // JavaScript原始嵌入节点（[Origin] @JavaScript）
    ORIGIN_CUSTOM_NODE,     // 自定义类型原始嵌入节点（[Origin] @Vue等）
    ORIGIN_USAGE_NODE,      // 原始嵌入使用节点
    
    // 配置节点
    CONFIGURATION_NODE,     // 配置块节点（[Configuration]）
    NAME_CONFIG_NODE,       // Name配置节点（[Name]）
    ORIGIN_TYPE_CONFIG_NODE, // OriginType配置节点（[OriginType]）
    
    // 导入节点
    IMPORT_NODE,            // 导入节点（[Import]）
    
    // 命名空间节点
    NAMESPACE_NODE,         // 命名空间节点（[Namespace]）
    
    // 操作节点
    INHERIT_NODE,           // 继承节点（inherit）
    DELETE_NODE,            // 删除节点（delete）
    INSERT_NODE,            // 插入节点（insert）
    EXCEPT_NODE,            // 约束节点（except）
    
    // 选择器节点
    CLASS_SELECTOR_NODE,    // 类选择器节点（.className）
    ID_SELECTOR_NODE,       // ID选择器节点（#idName）
    CONTEXT_REFERENCE_NODE, // 上下文引用节点（&）
    PSEUDO_CLASS_NODE,      // 伪类节点（:hover等）
    PSEUDO_ELEMENT_NODE,    // 伪元素节点（::before等）
    
    // 特殊节点
    ATTRIBUTE_NODE,         // 属性节点
    PROPERTY_NODE,          // CSS属性节点
    INDEX_ACCESS_NODE,      // 索引访问节点（[0]、[1]等）
    VARIABLE_REFERENCE_NODE, // 变量引用节点（ThemeColor(tableColor)）
    
    // use语法节点
    USE_NODE,               // use节点（use html5; use @Config）
    
    // 信息节点（CMOD相关）
    INFO_NODE,              // 信息节点（[Info]）
    EXPORT_NODE             // 导出节点（[Export]）
};

/**
 * CHTL AST基础节点
 * 所有CHTL节点的基类，提供通用功能
 */
class CHTLBaseNode {
protected:
    CHTLNodeType m_Type;                                    // 节点类型
    std::string m_Name;                                     // 节点名称
    std::string m_Content;                                  // 节点内容
    std::vector<std::unique_ptr<CHTLBaseNode>> m_Children;  // 子节点列表
    CHTLBaseNode* m_Parent;                                 // 父节点指针
    std::unordered_map<std::string, std::string> m_Attributes; // 属性映射
    std::unordered_map<std::string, std::string> m_Properties; // CSS属性映射
    
    // 位置信息
    size_t m_Line;                                          // 行号
    size_t m_Column;                                        // 列号
    size_t m_Position;                                      // 在源代码中的位置
    
    // 语法特征标记
    bool m_HasAutoClass;                                    // 是否有自动生成的类名
    bool m_HasAutoId;                                       // 是否有自动生成的ID
    std::string m_AutoClassName;                            // 自动生成的类名
    std::string m_AutoIdName;                               // 自动生成的ID名
    std::string m_Namespace;                                // 所属命名空间

public:
    /**
     * 构造函数
     * @param type 节点类型
     * @param name 节点名称
     * @param line 行号
     * @param column 列号
     */
    CHTLBaseNode(CHTLNodeType type, const std::string& name = "", 
                size_t line = 0, size_t column = 0);
    
    /**
     * 虚析构函数
     */
    virtual ~CHTLBaseNode() = default;
    
    // 基础访问器
    /**
     * 获取节点类型
     * @return 节点类型
     */
    CHTLNodeType GetType() const { return m_Type; }
    
    /**
     * 获取节点名称
     * @return 节点名称
     */
    const std::string& GetName() const { return m_Name; }
    
    /**
     * 设置节点名称
     * @param name 新名称
     */
    void SetName(const std::string& name) { m_Name = name; }
    
    /**
     * 获取节点内容
     * @return 节点内容
     */
    const std::string& GetContent() const { return m_Content; }
    
    /**
     * 设置节点内容
     * @param content 新内容
     */
    void SetContent(const std::string& content) { m_Content = content; }
    
    /**
     * 获取父节点
     * @return 父节点指针
     */
    CHTLBaseNode* GetParent() const { return m_Parent; }
    
    /**
     * 设置父节点
     * @param parent 父节点指针
     */
    void SetParent(CHTLBaseNode* parent) { m_Parent = parent; }
    
    // 子节点管理
    /**
     * 添加子节点
     * @param child 子节点
     */
    void AddChild(std::unique_ptr<CHTLBaseNode> child);
    
    /**
     * 获取子节点列表
     * @return 子节点列表的常引用
     */
    const std::vector<std::unique_ptr<CHTLBaseNode>>& GetChildren() const { return m_Children; }
    
    /**
     * 获取指定类型的子节点
     * @param type 节点类型
     * @return 匹配的子节点列表
     */
    std::vector<CHTLBaseNode*> GetChildrenByType(CHTLNodeType type) const;
    
    /**
     * 获取指定名称的子节点
     * @param name 节点名称
     * @return 匹配的子节点指针，如果不存在返回nullptr
     */
    CHTLBaseNode* GetChildByName(const std::string& name) const;
    
    /**
     * 移除子节点
     * @param index 子节点索引
     * @return 是否成功移除
     */
    bool RemoveChild(size_t index);
    
    /**
     * 移除指定子节点
     * @param child 子节点指针
     * @return 是否成功移除
     */
    bool RemoveChild(CHTLBaseNode* child);
    
    // 属性管理
    /**
     * 设置属性
     * @param name 属性名
     * @param value 属性值
     */
    void SetAttribute(const std::string& name, const std::string& value);
    
    /**
     * 获取属性
     * @param name 属性名
     * @return 属性值，如果不存在返回空字符串
     */
    std::string GetAttribute(const std::string& name) const;
    
    /**
     * 检查是否有指定属性
     * @param name 属性名
     * @return 是否存在
     */
    bool HasAttribute(const std::string& name) const;
    
    /**
     * 获取所有属性
     * @return 属性映射的常引用
     */
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return m_Attributes; }
    
    // CSS属性管理
    /**
     * 设置CSS属性
     * @param name 属性名
     * @param value 属性值
     */
    void SetProperty(const std::string& name, const std::string& value);
    
    /**
     * 获取CSS属性
     * @param name 属性名
     * @return 属性值
     */
    std::string GetProperty(const std::string& name) const;
    
    /**
     * 检查是否有指定CSS属性
     * @param name 属性名
     * @return 是否存在
     */
    bool HasProperty(const std::string& name) const;
    
    /**
     * 获取所有CSS属性
     * @return CSS属性映射的常引用
     */
    const std::unordered_map<std::string, std::string>& GetProperties() const { return m_Properties; }
    
    // 位置信息
    /**
     * 获取行号
     * @return 行号
     */
    size_t GetLine() const { return m_Line; }
    
    /**
     * 获取列号
     * @return 列号
     */
    size_t GetColumn() const { return m_Column; }
    
    /**
     * 获取位置
     * @return 位置
     */
    size_t GetPosition() const { return m_Position; }
    
    /**
     * 设置位置信息
     * @param line 行号
     * @param column 列号
     * @param position 位置
     */
    void SetPosition(size_t line, size_t column, size_t position);
    
    // 自动化选择器管理
    /**
     * 设置自动生成的类名
     * @param className 类名
     */
    void SetAutoClassName(const std::string& className);
    
    /**
     * 获取自动生成的类名
     * @return 类名
     */
    const std::string& GetAutoClassName() const { return m_AutoClassName; }
    
    /**
     * 设置自动生成的ID名
     * @param idName ID名
     */
    void SetAutoIdName(const std::string& idName);
    
    /**
     * 获取自动生成的ID名
     * @return ID名
     */
    const std::string& GetAutoIdName() const { return m_AutoIdName; }
    
    /**
     * 检查是否有自动类名
     * @return 是否有自动类名
     */
    bool HasAutoClass() const { return m_HasAutoClass; }
    
    /**
     * 检查是否有自动ID
     * @return 是否有自动ID
     */
    bool HasAutoId() const { return m_HasAutoId; }
    
    // 命名空间管理
    /**
     * 设置命名空间
     * @param ns 命名空间
     */
    void SetNamespace(const std::string& ns) { m_Namespace = ns; }
    
    /**
     * 获取命名空间
     * @return 命名空间
     */
    const std::string& GetNamespace() const { return m_Namespace; }
    
    // 访问者模式
    /**
     * 接受访问者
     * @param visitor 访问者
     */
    virtual void Accept(CHTLNodeVisitor* visitor) = 0;
    
    // 克隆和复制
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    virtual std::unique_ptr<CHTLBaseNode> Clone() const = 0;
    
    // 调试和输出
    /**
     * 获取节点类型名称
     * @return 类型名称字符串
     */
    std::string GetTypeString() const;
    
    /**
     * 转换为字符串表示
     * @param indent 缩进级别
     * @return 字符串表示
     */
    virtual std::string ToString(int indent = 0) const;
    
    /**
     * 获取节点统计信息
     * @return 统计信息
     */
    std::string GetNodeStatistics() const;

protected:
    /**
     * 生成缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string GenerateIndent(int level) const;
    
    /**
     * 验证节点有效性
     * @return 是否有效
     */
    virtual bool ValidateNode() const;
    
    /**
     * 应用继承规则
     * @param parentNode 父节点
     */
    virtual void ApplyInheritanceRules(CHTLBaseNode* parentNode);
};

/**
 * CHTL节点工厂
 * 负责创建各种类型的CHTL节点
 */
class CHTLNodeFactory {
public:
    /**
     * 创建节点
     * @param type 节点类型
     * @param name 节点名称
     * @param content 节点内容
     * @param line 行号
     * @param column 列号
     * @return 创建的节点
     */
    static std::unique_ptr<CHTLBaseNode> CreateNode(CHTLNodeType type, 
                                                   const std::string& name = "",
                                                   const std::string& content = "",
                                                   size_t line = 0, 
                                                   size_t column = 0);
    
    /**
     * 创建元素节点
     * @param tagName 标签名
     * @param line 行号
     * @param column 列号
     * @return 元素节点
     */
    static std::unique_ptr<CHTLBaseNode> CreateElementNode(const std::string& tagName,
                                                          size_t line = 0, 
                                                          size_t column = 0);
    
    /**
     * 创建文本节点
     * @param text 文本内容
     * @param line 行号
     * @param column 列号
     * @return 文本节点
     */
    static std::unique_ptr<CHTLBaseNode> CreateTextNode(const std::string& text,
                                                       size_t line = 0, 
                                                       size_t column = 0);
    
    /**
     * 创建模板节点
     * @param templateType 模板类型（@Style/@Element/@Var）
     * @param templateName 模板名称
     * @param content 模板内容
     * @param line 行号
     * @param column 列号
     * @return 模板节点
     */
    static std::unique_ptr<CHTLBaseNode> CreateTemplateNode(const std::string& templateType,
                                                           const std::string& templateName,
                                                           const std::string& content,
                                                           size_t line = 0, 
                                                           size_t column = 0);
    
    /**
     * 创建自定义节点
     * @param customType 自定义类型（@Style/@Element/@Var）
     * @param customName 自定义名称
     * @param content 自定义内容
     * @param line 行号
     * @param column 列号
     * @return 自定义节点
     */
    static std::unique_ptr<CHTLBaseNode> CreateCustomNode(const std::string& customType,
                                                         const std::string& customName,
                                                         const std::string& content,
                                                         size_t line = 0, 
                                                         size_t column = 0);
};

} // namespace CHTL