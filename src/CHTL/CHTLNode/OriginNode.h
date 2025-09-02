#pragma once

#include "CHTLBaseNode.h"
#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * 原始嵌入类型枚举
 * 注意：类型不具有实际作用，仅用于标识
 */
enum class OriginType {
    HTML_ORIGIN,        // @Html类型（仅标识，无实际作用）
    STYLE_ORIGIN,       // @Style类型（仅标识，无实际作用）
    JAVASCRIPT_ORIGIN,  // @JavaScript类型（仅标识，无实际作用）
    CUSTOM_ORIGIN       // 自定义类型（如@Vue，仅标识，无实际作用）
};

/**
 * 原始嵌入节点
 * 表示原始嵌入定义和使用，可以在任意地方使用，直接输出代码
 * 类型仅用于标识，不具有实际作用
 */
class OriginNode : public CHTLBaseNode {
private:
    OriginType m_OriginType;                            // 原始嵌入类型（仅标识）
    std::string m_OriginName;                           // 原始嵌入名称
    std::string m_OriginContent;                        // 原始嵌入内容
    bool m_IsNamedOrigin;                               // 是否为带名原始嵌入
    bool m_IsDefinition;                                // 是否为定义（否则为使用）
    
    // 自定义类型支持
    std::string m_CustomTypeName;                       // 自定义类型名称（如Vue）
    std::unordered_map<std::string, std::string> m_TypeAttributes; // 类型属性

public:
    /**
     * 构造函数（定义）
     * @param originType 原始嵌入类型
     * @param originName 原始嵌入名称
     * @param content 原始内容
     * @param line 行号
     * @param column 列号
     */
    OriginNode(OriginType originType, const std::string& originName, 
              const std::string& content, size_t line = 0, size_t column = 0);
    
    /**
     * 构造函数（使用）
     * @param originName 原始嵌入名称
     * @param line 行号
     * @param column 列号
     */
    OriginNode(const std::string& originName, size_t line = 0, size_t column = 0);
    
    /**
     * 构造函数（自定义类型）
     * @param customTypeName 自定义类型名称
     * @param originName 原始嵌入名称
     * @param content 原始内容
     * @param line 行号
     * @param column 列号
     */
    OriginNode(const std::string& customTypeName, const std::string& originName, 
              const std::string& content, size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~OriginNode() override = default;
    
    /**
     * 获取原始嵌入类型
     * @return 原始嵌入类型
     */
    OriginType GetOriginType() const { return m_OriginType; }
    
    /**
     * 获取原始嵌入名称
     * @return 原始嵌入名称
     */
    const std::string& GetOriginName() const { return m_OriginName; }
    
    /**
     * 设置原始嵌入名称
     * @param name 新名称
     */
    void SetOriginName(const std::string& name);
    
    /**
     * 获取原始嵌入内容
     * @return 原始嵌入内容
     */
    const std::string& GetOriginContent() const { return m_OriginContent; }
    
    /**
     * 设置原始嵌入内容
     * @param content 新内容
     */
    void SetOriginContent(const std::string& content);
    
    /**
     * 检查是否为带名原始嵌入
     * @return 是否为带名
     */
    bool IsNamedOrigin() const { return m_IsNamedOrigin; }
    
    /**
     * 检查是否为定义
     * @return 是否为定义
     */
    bool IsDefinition() const { return m_IsDefinition; }
    
    /**
     * 检查是否为自定义类型
     * @return 是否为自定义类型
     */
    bool IsCustomType() const { return !m_CustomTypeName.empty(); }
    
    /**
     * 获取自定义类型名称
     * @return 自定义类型名称
     */
    const std::string& GetCustomTypeName() const { return m_CustomTypeName; }
    
    /**
     * 设置自定义类型名称
     * @param typeName 类型名称
     */
    void SetCustomTypeName(const std::string& typeName);
    
    /**
     * 添加类型属性
     * @param name 属性名
     * @param value 属性值
     */
    void AddTypeAttribute(const std::string& name, const std::string& value);
    
    /**
     * 获取类型属性
     * @param name 属性名
     * @return 属性值
     */
    std::string GetTypeAttribute(const std::string& name) const;
    
    /**
     * 获取所有类型属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& GetTypeAttributes() const { return m_TypeAttributes; }
    
    /**
     * 直接输出原始代码（核心功能）
     * 原始嵌入的核心作用：不经过任何处理，直接输出
     * @return 直接输出的代码
     */
    std::string GetDirectOutput() const;
    
    /**
     * 生成原始嵌入的HTML表示
     * @return HTML代码
     */
    std::string GenerateHTML() const;
    
    /**
     * 生成原始嵌入的CSS表示
     * @return CSS代码
     */
    std::string GenerateCSS() const;
    
    /**
     * 生成原始嵌入的JavaScript表示
     * @return JavaScript代码
     */
    std::string GenerateJavaScript() const;
    
    /**
     * 获取类型字符串表示
     * @return 类型字符串
     */
    std::string GetTypeString() const;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::unique_ptr<CHTLBaseNode> Clone() const override;
    
    /**
     * 接受访问者
     * @param visitor 访问者
     */
    void Accept(CHTLNodeVisitor* visitor) override;

private:
    /**
     * 初始化原始嵌入节点
     */
    void InitializeOriginNode();
    
    /**
     * 确定原始嵌入类型
     * @param typeName 类型名称
     * @return 原始嵌入类型
     */
    OriginType DetermineOriginType(const std::string& typeName);
    
    /**
     * 验证原始嵌入内容
     * @return 是否有效
     */
    bool ValidateOriginContent() const;
    
    /**
     * 处理自定义类型配置
     */
    void ProcessCustomTypeConfiguration();
};

/**
 * 原始嵌入工厂
 * 用于创建不同类型的原始嵌入节点
 */
class OriginNodeFactory {
public:
    /**
     * 创建基本类型原始嵌入定义
     * @param typeName 类型名称（Html/Style/JavaScript）
     * @param originName 原始嵌入名称
     * @param content 原始内容
     * @param line 行号
     * @param column 列号
     * @return 原始嵌入节点
     */
    static std::unique_ptr<OriginNode> CreateBasicOriginDefinition(
        const std::string& typeName, 
        const std::string& originName, 
        const std::string& content,
        size_t line = 0, size_t column = 0);
    
    /**
     * 创建带名原始嵌入定义
     * @param typeName 类型名称
     * @param originName 原始嵌入名称
     * @param namedContent 带名内容
     * @param line 行号
     * @param column 列号
     * @return 原始嵌入节点
     */
    static std::unique_ptr<OriginNode> CreateNamedOriginDefinition(
        const std::string& typeName,
        const std::string& originName,
        const std::string& namedContent,
        size_t line = 0, size_t column = 0);
    
    /**
     * 创建自定义类型原始嵌入定义
     * @param customTypeName 自定义类型名称（如Vue）
     * @param originName 原始嵌入名称
     * @param content 原始内容
     * @param line 行号
     * @param column 列号
     * @return 原始嵌入节点
     */
    static std::unique_ptr<OriginNode> CreateCustomOriginDefinition(
        const std::string& customTypeName,
        const std::string& originName,
        const std::string& content,
        size_t line = 0, size_t column = 0);
    
    /**
     * 创建原始嵌入使用
     * @param originName 原始嵌入名称
     * @param line 行号
     * @param column 列号
     * @return 原始嵌入节点
     */
    static std::unique_ptr<OriginNode> CreateOriginUsage(
        const std::string& originName,
        size_t line = 0, size_t column = 0);
};

} // namespace CHTL