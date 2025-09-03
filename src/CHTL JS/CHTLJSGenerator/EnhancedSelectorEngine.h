#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTLJS {

/**
 * 增强选择器类型
 */
enum class EnhancedSelectorType {
    BASIC_SELECTOR,         // {{box}} - 智能推导
    CLASS_SELECTOR,         // {{.box}} - 类选择器
    ID_SELECTOR,            // {{#box}} - ID选择器
    TAG_SELECTOR,           // {{button}} - 标签选择器
    COMPOUND_SELECTOR,      // {{.box button}} - 复合选择器
    INDEX_SELECTOR,         // {{button[0]}} - 索引选择器
    CONTEXT_REFERENCE       // {{&}} - 上下文引用
};

/**
 * 增强选择器信息
 */
struct EnhancedSelectorInfo {
    EnhancedSelectorType Type;              // 选择器类型
    std::string OriginalExpression;         // 原始表达式 {{...}}
    std::string SelectorContent;            // 选择器内容 (去掉{{}})
    std::string ResolvedSelector;           // 解析后的CSS选择器
    std::string JSExpression;               // 生成的JavaScript表达式
    bool IsArray = false;                   // 是否返回数组
    size_t Index = SIZE_MAX;                // 索引值 (用于索引选择器)
};

/**
 * 增强选择器引擎
 * 实现完整的CHTL JS增强选择器系统
 * 支持智能推导、复合选择器、索引访问等高级特性
 */
class EnhancedSelectorEngine {
private:
    // 上下文信息
    std::string m_CurrentElementClass;      // 当前元素的class
    std::string m_CurrentElementId;         // 当前元素的id
    std::string m_CurrentElementTag;        // 当前元素的标签
    
    // 选择器缓存
    std::unordered_map<std::string, EnhancedSelectorInfo> m_SelectorCache;

public:
    /**
     * 构造函数
     */
    EnhancedSelectorEngine();
    
    /**
     * 析构函数
     */
    ~EnhancedSelectorEngine() = default;
    
    /**
     * 设置当前元素上下文
     * @param elementClass 元素class属性
     * @param elementId 元素id属性
     * @param elementTag 元素标签名
     */
    void SetCurrentElementContext(const std::string& elementClass,
                                 const std::string& elementId,
                                 const std::string& elementTag);
    
    /**
     * 解析增强选择器表达式
     * @param expression 选择器表达式 {{...}}
     * @return 选择器信息
     */
    EnhancedSelectorInfo ParseEnhancedSelector(const std::string& expression);
    
    /**
     * 生成JavaScript选择器代码
     * @param selectorInfo 选择器信息
     * @return 生成的JavaScript代码
     */
    std::string GenerateJavaScriptSelector(const EnhancedSelectorInfo& selectorInfo);
    
    /**
     * 处理智能推导选择器 {{box}}
     * @param selectorContent 选择器内容
     * @return 解析后的选择器信息
     */
    EnhancedSelectorInfo ProcessIntelligentSelector(const std::string& selectorContent);
    
    /**
     * 处理类选择器 {{.box}}
     * @param className 类名
     * @return 选择器信息
     */
    EnhancedSelectorInfo ProcessClassSelector(const std::string& className);
    
    /**
     * 处理ID选择器 {{#box}}
     * @param idName ID名称
     * @return 选择器信息
     */
    EnhancedSelectorInfo ProcessIdSelector(const std::string& idName);
    
    /**
     * 处理标签选择器 {{button}}
     * @param tagName 标签名
     * @return 选择器信息
     */
    EnhancedSelectorInfo ProcessTagSelector(const std::string& tagName);
    
    /**
     * 处理复合选择器 {{.box button}}
     * @param compoundSelector 复合选择器
     * @return 选择器信息
     */
    EnhancedSelectorInfo ProcessCompoundSelector(const std::string& compoundSelector);
    
    /**
     * 处理索引选择器 {{button[0]}}
     * @param indexSelector 索引选择器
     * @return 选择器信息
     */
    EnhancedSelectorInfo ProcessIndexSelector(const std::string& indexSelector);
    
    /**
     * 处理上下文引用 {{&}}
     * @return 选择器信息
     */
    EnhancedSelectorInfo ProcessContextReference();
    
    /**
     * 验证选择器语法
     * @param selectorContent 选择器内容
     * @return 是否有效
     */
    bool ValidateSelectorSyntax(const std::string& selectorContent);
    
    /**
     * 检查是否是有效的CSS选择器
     * @param selector CSS选择器
     * @return 是否有效
     */
    bool IsValidCSSSelector(const std::string& selector);
    
    /**
     * 生成querySelector代码
     * @param cssSelector CSS选择器
     * @param isArray 是否返回数组
     * @return JavaScript代码
     */
    std::string GenerateQuerySelector(const std::string& cssSelector, bool isArray = false);
    
    /**
     * 生成索引访问代码
     * @param baseSelector 基础选择器
     * @param index 索引
     * @return JavaScript代码
     */
    std::string GenerateIndexAccess(const std::string& baseSelector, size_t index);
    
    /**
     * 优化选择器性能
     * @param selector CSS选择器
     * @return 优化后的选择器
     */
    std::string OptimizeSelector(const std::string& selector);
    
    /**
     * 清理选择器缓存
     */
    void ClearCache();
    
    /**
     * 获取选择器统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetSelectorStatistics();

private:
    /**
     * 解析选择器内容
     * @param content 选择器内容 (不含{{}})
     * @return 选择器类型
     */
    EnhancedSelectorType DetermineSelectorType(const std::string& content);
    
    /**
     * 提取索引值
     * @param indexExpression 索引表达式 (如: button[0])
     * @return 标签名和索引值
     */
    std::pair<std::string, size_t> ExtractIndexValue(const std::string& indexExpression);
    
    /**
     * 分析复合选择器
     * @param compoundSelector 复合选择器
     * @return 选择器组件列表
     */
    std::vector<std::string> AnalyzeCompoundSelector(const std::string& compoundSelector);
    
    /**
     * 检查是否是HTML标签
     * @param tagName 标签名
     * @return 是否是有效的HTML标签
     */
    bool IsValidHTMLTag(const std::string& tagName);
};

} // namespace CHTLJS