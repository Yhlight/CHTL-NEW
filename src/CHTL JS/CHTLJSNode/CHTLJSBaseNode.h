#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTLJS {

// 前向声明
class CHTLJSNodeVisitor;

/**
 * CHTL JS节点类型枚举（完全独立于CHTL节点类型）
 * 涵盖所有CHTL JS语法特征的节点类型
 */
enum class CHTLJSNodeType {
    // 基础节点
    MODULE_NODE,                    // 模块导入节点（module{}）
    LOAD_NODE,                      // 模块加载节点（load:）
    
    // 增强选择器节点
    ENHANCED_SELECTOR_NODE,         // 增强选择器节点（{{selector}}）
    SELECTOR_CONTENT_NODE,          // 选择器内容节点
    CLASS_SELECTOR_NODE,            // 类选择器节点（.className）
    ID_SELECTOR_NODE,               // ID选择器节点（#idName）
    TAG_SELECTOR_NODE,              // 标签选择器节点（tagName）
    DESCENDANT_SELECTOR_NODE,       // 后代选择器节点（.parent .child）
    INDEX_SELECTOR_NODE,            // 索引选择器节点（element[0]）
    
    // 操作符节点
    ARROW_OPERATOR_NODE,            // ->操作符节点
    EVENT_BIND_OPERATOR_NODE,       // &->事件绑定操作符节点
    
    // CHTL JS函数节点
    LISTEN_NODE,                    // listen函数节点
    DELEGATE_NODE,                  // delegate函数节点
    ANIMATE_NODE,                   // animate函数节点
    
    // 虚对象节点
    VIR_DECLARATION_NODE,           // vir声明节点
    VIR_ASSIGNMENT_NODE,            // vir赋值节点
    VIR_REFERENCE_NODE,             // vir引用节点（Test->click）
    VIR_KEY_NODE,                   // vir键节点
    
    // 事件相关节点
    EVENT_MAPPING_NODE,             // 事件映射节点（click: handler）
    EVENT_HANDLER_NODE,             // 事件处理器节点
    EVENT_DELEGATE_TARGET_NODE,     // 事件委托目标节点
    
    // 动画相关节点
    ANIMATION_TARGET_NODE,          // 动画目标节点
    ANIMATION_PROPERTY_NODE,        // 动画属性节点（duration、easing等）
    ANIMATION_KEYFRAME_NODE,        // 动画关键帧节点（when）
    ANIMATION_STATE_NODE,           // 动画状态节点（begin、end）
    
    // 特殊节点
    COMMENT_NODE,                   // 注释节点
    UNKNOWN_NODE                    // 未知节点
};

/**
 * CHTL JS AST基础节点（完全独立于CHTL BaseNode）
 * 所有CHTL JS节点的基类，提供CHTL JS特有的功能
 */
class CHTLJSBaseNode {
protected:
    CHTLJSNodeType m_Type;                              // 节点类型
    std::string m_Name;                                 // 节点名称
    std::string m_Content;                              // 节点内容
    std::vector<std::unique_ptr<CHTLJSBaseNode>> m_Children; // 子节点列表
    CHTLJSBaseNode* m_Parent;                           // 父节点指针
    std::unordered_map<std::string, std::string> m_Properties; // CHTL JS属性映射
    
    // 位置信息
    size_t m_Line;                                      // 行号
    size_t m_Column;                                    // 列号
    size_t m_Position;                                  // 在源代码中的位置
    
    // CHTL JS特有标记
    bool m_IsCHTLJSFunction;                           // 是否为CHTL JS函数
    bool m_SupportsVirtualObject;                      // 是否支持虚对象
    std::string m_GeneratedJavaScript;                 // 生成的JavaScript代码
    std::string m_SelectorType;                        // 选择器类型（class/id/tag/descendant）

public:
    /**
     * 构造函数
     * @param type 节点类型
     * @param name 节点名称
     * @param line 行号
     * @param column 列号
     */
    CHTLJSBaseNode(CHTLJSNodeType type, const std::string& name = "", 
                  size_t line = 0, size_t column = 0);
    
    /**
     * 虚析构函数
     */
    virtual ~CHTLJSBaseNode() = default;
    
    // 基础访问器
    /**
     * 获取节点类型
     * @return 节点类型
     */
    CHTLJSNodeType GetType() const { return m_Type; }
    
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
    CHTLJSBaseNode* GetParent() const { return m_Parent; }
    
    /**
     * 设置父节点
     * @param parent 父节点指针
     */
    void SetParent(CHTLJSBaseNode* parent) { m_Parent = parent; }
    
    // 子节点管理
    /**
     * 添加子节点
     * @param child 子节点
     */
    void AddChild(std::unique_ptr<CHTLJSBaseNode> child);
    
    /**
     * 获取子节点列表
     * @return 子节点列表的常引用
     */
    const std::vector<std::unique_ptr<CHTLJSBaseNode>>& GetChildren() const { return m_Children; }
    
    /**
     * 获取指定类型的子节点
     * @param type 节点类型
     * @return 匹配的子节点列表
     */
    std::vector<CHTLJSBaseNode*> GetChildrenByType(CHTLJSNodeType type) const;
    
    /**
     * 获取指定名称的子节点
     * @param name 节点名称
     * @return 匹配的子节点指针
     */
    CHTLJSBaseNode* GetChildByName(const std::string& name) const;
    
    // CHTL JS属性管理
    /**
     * 设置CHTL JS属性
     * @param name 属性名
     * @param value 属性值
     */
    void SetProperty(const std::string& name, const std::string& value);
    
    /**
     * 获取CHTL JS属性
     * @param name 属性名
     * @return 属性值
     */
    std::string GetProperty(const std::string& name) const;
    
    /**
     * 检查是否有指定属性
     * @param name 属性名
     * @return 是否存在
     */
    bool HasProperty(const std::string& name) const;
    
    /**
     * 获取所有CHTL JS属性
     * @return 属性映射的常引用
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
     * 设置位置信息
     * @param line 行号
     * @param column 列号
     * @param position 位置
     */
    void SetPosition(size_t line, size_t column, size_t position);
    
    // CHTL JS特有功能
    /**
     * 检查是否为CHTL JS函数
     * @return 是否为CHTL JS函数
     */
    bool IsCHTLJSFunction() const { return m_IsCHTLJSFunction; }
    
    /**
     * 设置是否为CHTL JS函数
     * @param isFunction 是否为函数
     */
    void SetCHTLJSFunction(bool isFunction) { m_IsCHTLJSFunction = isFunction; }
    
    /**
     * 检查是否支持虚对象
     * @return 是否支持虚对象
     */
    bool SupportsVirtualObject() const { return m_SupportsVirtualObject; }
    
    /**
     * 设置是否支持虚对象
     * @param supports 是否支持
     */
    void SetSupportsVirtualObject(bool supports) { m_SupportsVirtualObject = supports; }
    
    /**
     * 获取生成的JavaScript代码
     * @return JavaScript代码
     */
    const std::string& GetGeneratedJavaScript() const { return m_GeneratedJavaScript; }
    
    /**
     * 设置生成的JavaScript代码
     * @param jsCode JavaScript代码
     */
    void SetGeneratedJavaScript(const std::string& jsCode) { m_GeneratedJavaScript = jsCode; }
    
    /**
     * 获取选择器类型
     * @return 选择器类型
     */
    const std::string& GetSelectorType() const { return m_SelectorType; }
    
    /**
     * 设置选择器类型
     * @param type 选择器类型
     */
    void SetSelectorType(const std::string& type) { m_SelectorType = type; }
    
    // 访问者模式
    /**
     * 接受访问者
     * @param visitor 访问者
     */
    virtual void Accept(CHTLJSNodeVisitor* visitor) = 0;
    
    // 克隆和复制
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    virtual std::unique_ptr<CHTLJSBaseNode> Clone() const = 0;
    
    // 代码生成
    /**
     * 生成JavaScript代码
     * @return 生成的JavaScript代码
     */
    virtual std::string GenerateJavaScript() const = 0;
    
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
    std::string GetCHTLJSNodeStatistics() const;

protected:
    /**
     * 生成缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string GenerateIndent(int level) const;
    
    /**
     * 验证CHTL JS节点有效性
     * @return 是否有效
     */
    virtual bool ValidateCHTLJSNode() const;
};

/**
 * CHTL JS节点工厂（完全独立于CHTL节点工厂）
 * 负责创建各种类型的CHTL JS节点
 */
class CHTLJSNodeFactory {
public:
    /**
     * 创建CHTL JS节点
     * @param type 节点类型
     * @param name 节点名称
     * @param content 节点内容
     * @param line 行号
     * @param column 列号
     * @return 创建的节点
     */
    static std::unique_ptr<CHTLJSBaseNode> CreateCHTLJSNode(CHTLJSNodeType type, 
                                                           const std::string& name = "",
                                                           const std::string& content = "",
                                                           size_t line = 0, 
                                                           size_t column = 0);
    
    /**
     * 创建增强选择器节点
     * @param selector 选择器内容
     * @param line 行号
     * @param column 列号
     * @return 选择器节点
     */
    static std::unique_ptr<CHTLJSBaseNode> CreateEnhancedSelectorNode(const std::string& selector,
                                                                     size_t line = 0, 
                                                                     size_t column = 0);
    
    /**
     * 创建CHTL JS函数节点
     * @param functionType 函数类型（listen/delegate/animate/module）
     * @param functionContent 函数内容
     * @param line 行号
     * @param column 列号
     * @return 函数节点
     */
    static std::unique_ptr<CHTLJSBaseNode> CreateCHTLJSFunctionNode(const std::string& functionType,
                                                                   const std::string& functionContent,
                                                                   size_t line = 0, 
                                                                   size_t column = 0);
    
    /**
     * 创建虚对象节点
     * @param virName 虚对象名称
     * @param functionContent CHTL JS函数内容
     * @param line 行号
     * @param column 列号
     * @return 虚对象节点
     */
    static std::unique_ptr<CHTLJSBaseNode> CreateVirtualObjectNode(const std::string& virName,
                                                                  const std::string& functionContent,
                                                                  size_t line = 0, 
                                                                  size_t column = 0);
    
    /**
     * 创建事件绑定节点
     * @param selector 选择器
     * @param eventType 事件类型
     * @param handler 处理器
     * @param line 行号
     * @param column 列号
     * @return 事件绑定节点
     */
    static std::unique_ptr<CHTLJSBaseNode> CreateEventBindNode(const std::string& selector,
                                                              const std::string& eventType,
                                                              const std::string& handler,
                                                              size_t line = 0, 
                                                              size_t column = 0);
};

} // namespace CHTLJS