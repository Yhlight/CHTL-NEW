#pragma once

#include "CHTLJSBaseNode.h"
#include <unordered_map>

namespace CHTLJS {

/**
 * 选择器优先级枚举
 */
enum class SelectorPriority {
    ID_PRIORITY,        // ID优先（{{box}}先查找id=box）
    CLASS_PRIORITY,     // 类优先
    TAG_PRIORITY        // 标签优先
};

/**
 * CHTL JS增强选择器节点
 * 表示{{CSS选择器}}语法，支持优先级查找和DOM对象创建
 */
class EnhancedSelectorNode : public CHTLJSBaseNode {
private:
    std::string m_SelectorContent;                      // 选择器内容（去除{{}}）
    std::string m_OriginalSelector;                     // 原始选择器（包含{{}}）
    std::string m_DOMQueryCode;                         // 生成的DOM查询代码
    SelectorPriority m_Priority;                       // 选择器优先级
    
    // 选择器解析结果
    std::string m_ClassName;                            // 类名（如果是类选择器）
    std::string m_IdName;                               // ID名（如果是ID选择器）
    std::string m_TagName;                              // 标签名（如果是标签选择器）
    std::vector<std::string> m_DescendantSelectors;    // 后代选择器列表
    size_t m_IndexAccess;                               // 索引访问（如果有）
    
    bool m_IsClassSelector;                             // 是否为类选择器
    bool m_IsIdSelector;                                // 是否为ID选择器
    bool m_IsTagSelector;                               // 是否为标签选择器
    bool m_IsDescendantSelector;                        // 是否为后代选择器
    bool m_HasIndexAccess;                              // 是否有索引访问
    bool m_IsAutoDetectSelector;                        // 是否为自动检测选择器（{{box}}）

public:
    /**
     * 构造函数
     * @param selector 选择器内容（包含{{}}）
     * @param line 行号
     * @param column 列号
     */
    EnhancedSelectorNode(const std::string& selector, size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~EnhancedSelectorNode() override = default;
    
    /**
     * 获取选择器内容
     * @return 选择器内容（去除{{}}）
     */
    const std::string& GetSelectorContent() const { return m_SelectorContent; }
    
    /**
     * 获取原始选择器
     * @return 原始选择器（包含{{}}）
     */
    const std::string& GetOriginalSelector() const { return m_OriginalSelector; }
    
    /**
     * 获取DOM查询代码
     * @return DOM查询JavaScript代码
     */
    const std::string& GetDOMQueryCode() const { return m_DOMQueryCode; }
    
    /**
     * 获取选择器优先级
     * @return 选择器优先级
     */
    SelectorPriority GetPriority() const { return m_Priority; }
    
    // 选择器类型检查
    /**
     * 检查是否为类选择器
     * @return 是否为类选择器
     */
    bool IsClassSelector() const { return m_IsClassSelector; }
    
    /**
     * 检查是否为ID选择器
     * @return 是否为ID选择器
     */
    bool IsIdSelector() const { return m_IsIdSelector; }
    
    /**
     * 检查是否为标签选择器
     * @return 是否为标签选择器
     */
    bool IsTagSelector() const { return m_IsTagSelector; }
    
    /**
     * 检查是否为后代选择器
     * @return 是否为后代选择器
     */
    bool IsDescendantSelector() const { return m_IsDescendantSelector; }
    
    /**
     * 检查是否有索引访问
     * @return 是否有索引访问
     */
    bool HasIndexAccess() const { return m_HasIndexAccess; }
    
    /**
     * 检查是否为自动检测选择器
     * @return 是否为自动检测选择器
     */
    bool IsAutoDetectSelector() const { return m_IsAutoDetectSelector; }
    
    // 选择器信息获取
    /**
     * 获取类名
     * @return 类名
     */
    const std::string& GetClassName() const { return m_ClassName; }
    
    /**
     * 获取ID名
     * @return ID名
     */
    const std::string& GetIdName() const { return m_IdName; }
    
    /**
     * 获取标签名
     * @return 标签名
     */
    const std::string& GetTagName() const { return m_TagName; }
    
    /**
     * 获取后代选择器列表
     * @return 后代选择器列表
     */
    const std::vector<std::string>& GetDescendantSelectors() const { return m_DescendantSelectors; }
    
    /**
     * 获取索引访问值
     * @return 索引值
     */
    size_t GetIndexAccess() const { return m_IndexAccess; }
    
    // DOM查询代码生成
    /**
     * 生成DOM查询代码
     * @return JavaScript DOM查询代码
     */
    std::string GenerateDOMQuery() const;
    
    /**
     * 生成优先级查找代码（用于{{box}}类型）
     * @return 优先级查找JavaScript代码
     */
    std::string GeneratePrioritySearchCode() const;
    
    /**
     * 生成类选择器查询代码
     * @return 类选择器查询代码
     */
    std::string GenerateClassSelectorQuery() const;
    
    /**
     * 生成ID选择器查询代码
     * @return ID选择器查询代码
     */
    std::string GenerateIdSelectorQuery() const;
    
    /**
     * 生成标签选择器查询代码
     * @return 标签选择器查询代码
     */
    std::string GenerateTagSelectorQuery() const;
    
    /**
     * 生成后代选择器查询代码
     * @return 后代选择器查询代码
     */
    std::string GenerateDescendantSelectorQuery() const;
    
    /**
     * 生成索引访问查询代码
     * @return 索引访问查询代码
     */
    std::string GenerateIndexAccessQuery() const;
    
    // 访问者模式实现
    void Accept(CHTLJSNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLJSBaseNode> Clone() const override;
    
    // JavaScript代码生成
    std::string GenerateJavaScript() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 解析选择器内容
     */
    void ParseSelectorContent();
    
    /**
     * 分析选择器类型
     */
    void AnalyzeSelectorType();
    
    /**
     * 生成DOM查询代码
     */
    void GenerateDOMQueryCode();
    
    /**
     * 验证选择器格式
     * @return 是否有效
     */
    bool ValidateSelectorFormat() const;
    
    /**
     * 提取类名
     * @param content 选择器内容
     * @return 类名
     */
    std::string ExtractClassName(const std::string& content) const;
    
    /**
     * 提取ID名
     * @param content 选择器内容
     * @return ID名
     */
    std::string ExtractIdName(const std::string& content) const;
    
    /**
     * 解析后代选择器
     * @param content 选择器内容
     * @return 后代选择器列表
     */
    std::vector<std::string> ParseDescendantSelectors(const std::string& content) const;
    
    /**
     * 解析索引访问
     * @param content 选择器内容
     * @return 索引值，如果没有返回SIZE_MAX
     */
    size_t ParseIndexAccess(const std::string& content) const;
};

} // namespace CHTLJS