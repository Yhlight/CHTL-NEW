#pragma once

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "EnhancedSelectorEngine.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * 监听器配置信息
 */
struct ListenerConfig {
    std::string TargetSelector;                             // 目标选择器
    std::unordered_map<std::string, std::string> Events;   // 事件映射
    bool IsDeclarative = true;                              // 是否是声明式
};

/**
 * 事件委托配置信息
 */
struct DelegateConfig {
    std::string ParentSelector;                             // 父元素选择器
    std::vector<std::string> TargetSelectors;              // 目标选择器列表
    std::unordered_map<std::string, std::string> Events;   // 事件映射
    bool UseGlobalRegistry = true;                          // 是否使用全局注册表
};

/**
 * 动画配置信息
 */
struct AnimateConfig {
    std::string TargetSelector;                             // 目标选择器
    std::vector<std::string> TargetArray;                  // 目标数组
    int Duration = 1000;                                    // 持续时间(ms)
    std::string Easing = "ease-in-out";                     // 缓动函数
    std::unordered_map<std::string, std::string> BeginState;    // 起始状态
    std::unordered_map<std::string, std::string> EndState;      // 结束状态
    std::vector<std::unordered_map<std::string, std::string>> WhenStates; // 中间状态
    int Loop = 1;                                           // 循环次数
    std::string Direction = "normal";                       // 播放方向
    int Delay = 0;                                          // 延迟(ms)
    std::string Callback;                                   // 回调函数
};

/**
 * 虚拟对象信息
 */
struct VirObjectInfo {
    std::string Name;                                       // 虚拟对象名称
    std::string FunctionContent;                            // 函数内容
    std::unordered_map<std::string, std::string> KeyValues; // 键值映射
    std::unordered_map<std::string, std::string> KeyTypes;  // 键类型映射
    bool IsValid = false;                                   // 是否有效
};

/**
 * CHTL JS高级功能引擎
 * 实现完整的CHTL JS增强功能
 * 包括增强监听器、事件委托、动画API、虚拟对象等
 */
class CHTLJSAdvancedFeatures {
private:
    // 增强选择器引擎
    std::unique_ptr<EnhancedSelectorEngine> m_SelectorEngine;
    
    // 功能配置存储
    std::vector<ListenerConfig> m_Listeners;               // 监听器配置
    std::vector<DelegateConfig> m_Delegates;               // 委托配置
    std::vector<AnimateConfig> m_Animations;               // 动画配置
    std::unordered_map<std::string, VirObjectInfo> m_VirObjects; // 虚拟对象
    
    // 全局注册表 (事件委托用)
    std::unordered_map<std::string, std::vector<std::string>> m_GlobalDelegateRegistry;
    
    // 当前上下文
    std::string m_CurrentElementContext;

public:
    /**
     * 构造函数
     */
    CHTLJSAdvancedFeatures();
    
    /**
     * 析构函数
     */
    ~CHTLJSAdvancedFeatures() = default;
    
    /**
     * 处理增强监听器 listen{}
     * @param targetSelector 目标选择器
     * @param listenerContent 监听器内容
     * @return 生成的JavaScript代码
     */
    std::string ProcessEnhancedListener(const std::string& targetSelector, 
                                       const std::string& listenerContent);
    
    /**
     * 处理事件委托 delegate{}
     * @param parentSelector 父元素选择器
     * @param delegateContent 委托内容
     * @return 生成的JavaScript代码
     */
    std::string ProcessEventDelegate(const std::string& parentSelector, 
                                    const std::string& delegateContent);
    
    /**
     * 处理动画API animate{}
     * @param animateContent 动画内容
     * @return 生成的JavaScript代码
     */
    std::string ProcessAnimateAPI(const std::string& animateContent);
    
    /**
     * 处理虚拟对象 vir
     * @param virName 虚拟对象名称
     * @param virContent 虚拟对象内容
     * @return 生成的JavaScript代码
     */
    std::string ProcessVirObject(const std::string& virName, 
                                const std::string& virContent);
    
    /**
     * 处理箭头操作符 ->
     * @param leftExpression 左侧表达式
     * @param rightExpression 右侧表达式
     * @return 生成的JavaScript代码
     */
    std::string ProcessArrowOperator(const std::string& leftExpression, 
                                    const std::string& rightExpression);
    
    /**
     * 处理事件绑定操作符 &->
     * @param targetSelector 目标选择器
     * @param eventType 事件类型
     * @param eventHandler 事件处理器
     * @return 生成的JavaScript代码
     */
    std::string ProcessEventBindingOperator(const std::string& targetSelector,
                                           const std::string& eventType,
                                           const std::string& eventHandler);
    
    /**
     * 处理模块导入 module{}
     * @param moduleContent 模块内容
     * @return 生成的JavaScript代码 (AMD风格)
     */
    std::string ProcessModuleImport(const std::string& moduleContent);
    
    /**
     * 设置当前元素上下文
     * @param elementContext 元素上下文信息
     */
    void SetCurrentElementContext(const std::string& elementContext);
    
    /**
     * 获取增强选择器引擎
     * @return 选择器引擎引用
     */
    EnhancedSelectorEngine* GetSelectorEngine() { return m_SelectorEngine.get(); }
    
    /**
     * 获取功能统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetFeatureStatistics();
    
    /**
     * 验证所有高级功能
     * @return 验证结果
     */
    bool ValidateAllFeatures();
    
    /**
     * 清理所有功能
     */
    void Clear();

private:
    /**
     * 解析监听器内容
     * @param content 监听器内容
     * @return 监听器配置
     */
    ListenerConfig ParseListenerContent(const std::string& content);
    
    /**
     * 解析委托内容
     * @param content 委托内容
     * @return 委托配置
     */
    DelegateConfig ParseDelegateContent(const std::string& content);
    
    /**
     * 解析动画内容
     * @param content 动画内容
     * @return 动画配置
     */
    AnimateConfig ParseAnimateContent(const std::string& content);
    
    /**
     * 解析虚拟对象内容
     * @param content 虚拟对象内容
     * @return 虚拟对象信息
     */
    VirObjectInfo ParseVirObjectContent(const std::string& content);
    
    /**
     * 生成事件监听器代码
     * @param config 监听器配置
     * @return JavaScript代码
     */
    std::string GenerateListenerCode(const ListenerConfig& config);
    
    /**
     * 生成事件委托代码
     * @param config 委托配置
     * @return JavaScript代码
     */
    std::string GenerateDelegateCode(const DelegateConfig& config);
    
    /**
     * 生成动画代码
     * @param config 动画配置
     * @return JavaScript代码
     */
    std::string GenerateAnimateCode(const AnimateConfig& config);
    
    /**
     * 生成虚拟对象代码
     * @param virInfo 虚拟对象信息
     * @return JavaScript代码
     */
    std::string GenerateVirObjectCode(const VirObjectInfo& virInfo);
    
    /**
     * 管理全局委托注册表
     * @param parentSelector 父选择器
     * @param targetSelectors 目标选择器列表
     */
    void ManageGlobalDelegateRegistry(const std::string& parentSelector,
                                     const std::vector<std::string>& targetSelectors);
};

} // namespace CHTLJS