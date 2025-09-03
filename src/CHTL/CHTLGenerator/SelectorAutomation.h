#pragma once

#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * 选择器自动化配置
 * 基于完善选择器自动化与引用规则.ini的完整实现
 */
struct SelectorAutomationConfig {
    bool DisableStyleAutoAddClass = false;     // 禁止局部样式块自动添加类选择器
    bool DisableStyleAutoAddId = false;        // 禁止局部样式块自动添加id选择器
    bool DisableScriptAutoAddClass = true;     // 禁止局部脚本自动添加类选择器
    bool DisableScriptAutoAddId = true;        // 禁止局部脚本自动添加id选择器
};

/**
 * 选择器信息
 */
struct SelectorInfo {
    std::string Name;           // 选择器名称 (不含前缀)
    std::string FullSelector;   // 完整选择器 (.box, #box)
    std::string Type;           // 类型: "class", "id"
    bool IsReference;           // 是否是引用选择器 (&)
    size_t Order;               // 出现顺序
};

/**
 * 选择器自动化引擎
 * 实现完整的选择器自动化与引用规则
 */
class SelectorAutomationEngine {
private:
    SelectorAutomationConfig m_Config;
    
    // 选择器分析结果
    std::vector<SelectorInfo> m_StyleSelectors;    // 样式块中的选择器
    std::vector<SelectorInfo> m_ScriptSelectors;   // 脚本块中的选择器
    
    // 元素属性状态
    bool m_HasClassAttribute = false;
    bool m_HasIdAttribute = false;
    std::string m_ExistingClass;
    std::string m_ExistingId;

public:
    /**
     * 构造函数
     */
    SelectorAutomationEngine(const SelectorAutomationConfig& config = {});
    
    /**
     * 析构函数
     */
    ~SelectorAutomationEngine() = default;
    
    /**
     * 处理元素的选择器自动化
     * @param element 目标元素
     * @param styleNode 局部样式块 (可选)
     * @param scriptNode 局部脚本块 (可选)
     */
    void ProcessElementAutomation(ElementNode* element, 
                                 StyleNode* styleNode = nullptr, 
                                 ScriptNode* scriptNode = nullptr);
    
    /**
     * 分析样式块中的选择器
     * @param styleContent 样式块内容
     * @return 选择器列表
     */
    std::vector<SelectorInfo> AnalyzeStyleSelectors(const std::string& styleContent);
    
    /**
     * 分析脚本块中的选择器
     * @param scriptContent 脚本块内容
     * @return 选择器列表
     */
    std::vector<SelectorInfo> AnalyzeScriptSelectors(const std::string& scriptContent);
    
    /**
     * 处理样式块选择器自动化
     * @param element 目标元素
     * @param selectors 选择器列表
     */
    void ProcessStyleSelectorAutomation(ElementNode* element, 
                                       const std::vector<SelectorInfo>& selectors);
    
    /**
     * 处理脚本块选择器自动化
     * @param element 目标元素
     * @param selectors 选择器列表
     * @param hasStyleTriggered 样式块是否已触发自动化
     */
    void ProcessScriptSelectorAutomation(ElementNode* element, 
                                        const std::vector<SelectorInfo>& selectors,
                                        bool hasStyleTriggered);
    
    /**
     * 处理引用选择器 (&)
     * @param element 目标元素
     * @param isInStyle 是否在样式块中 (true: 优先class, false: 优先id)
     * @return 解析后的选择器
     */
    std::string ProcessReferenceSelector(ElementNode* element, bool isInStyle);
    
    /**
     * 获取第一个类选择器
     * @param selectors 选择器列表
     * @return 第一个类选择器名称 (不含.)
     */
    std::string GetFirstClassSelector(const std::vector<SelectorInfo>& selectors);
    
    /**
     * 获取第一个ID选择器
     * @param selectors 选择器列表
     * @return 第一个ID选择器名称 (不含#)
     */
    std::string GetFirstIdSelector(const std::vector<SelectorInfo>& selectors);
    
    /**
     * 检查是否有触发自动化的选择器
     * @param selectors 选择器列表
     * @return 是否有{{.box}}或{{#box}}类型的选择器
     */
    bool HasTriggerSelectors(const std::vector<SelectorInfo>& selectors);
    
    /**
     * 更新配置
     * @param config 新的配置
     */
    void UpdateConfig(const SelectorAutomationConfig& config);
    
    /**
     * 重置状态
     */
    void Reset();

private:
    /**
     * 解析选择器字符串
     * @param selector 选择器字符串
     * @return 选择器信息
     */
    SelectorInfo ParseSelector(const std::string& selector, size_t order);
    
    /**
     * 检查元素现有属性
     * @param element 目标元素
     */
    void AnalyzeElementAttributes(ElementNode* element);
    
    /**
     * 添加类名到元素
     * @param element 目标元素
     * @param className 类名
     */
    void AddClassToElement(ElementNode* element, const std::string& className);
    
    /**
     * 添加ID到元素
     * @param element 目标元素
     * @param idName ID名称
     */
    void AddIdToElement(ElementNode* element, const std::string& idName);
};

} // namespace CHTL