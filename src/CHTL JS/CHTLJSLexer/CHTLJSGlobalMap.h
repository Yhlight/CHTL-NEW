#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * 虚对象定义结构
 * 管理vir虚对象的元信息
 */
struct VirtualObjectDefinition {
    std::string Name;                           // 虚对象名称
    std::unordered_map<std::string, std::string> KeyTypes;     // 键的类型映射
    std::unordered_map<std::string, std::string> KeyValues;    // 键的值映射
    std::unordered_map<std::string, std::string> FunctionRefs; // 函数引用映射
    bool IsActive;                              // 是否激活
    
    VirtualObjectDefinition() = default;
    VirtualObjectDefinition(const std::string& name) : Name(name), IsActive(true) {}
};

/**
 * 事件委托定义结构
 * 管理事件委托的全局注册表
 */
struct EventDelegateDefinition {
    std::string ParentSelector;                 // 父元素选择器
    std::vector<std::string> TargetSelectors;  // 目标子元素选择器列表
    std::unordered_map<std::string, std::string> EventHandlers; // 事件处理器映射
    bool IsRegistered;                          // 是否已注册
    
    EventDelegateDefinition() = default;
    EventDelegateDefinition(const std::string& parent) : ParentSelector(parent), IsRegistered(false) {}
};

/**
 * 动画定义结构
 * 管理animate动画的定义信息
 */
struct AnimationDefinition {
    std::string Name;                           // 动画名称
    std::vector<std::string> Targets;          // 目标元素列表
    int Duration;                               // 持续时间（ms）
    std::string Easing;                         // 缓动函数
    std::unordered_map<std::string, std::string> BeginState;   // 起始状态
    std::unordered_map<std::string, std::string> EndState;     // 结束状态
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> Keyframes; // 关键帧
    int Loop;                                   // 循环次数
    std::string Direction;                      // 播放方向
    int Delay;                                  // 延迟（ms）
    std::string Callback;                       // 回调函数
    
    AnimationDefinition() = default;
    AnimationDefinition(const std::string& name) 
        : Name(name), Duration(0), Loop(1), Delay(0) {}
};

/**
 * 模块加载定义结构
 * 管理module{}模块加载信息
 */
struct ModuleLoadDefinition {
    std::vector<std::string> LoadPaths;        // 加载路径列表
    std::vector<std::string> Dependencies;     // 依赖关系
    std::string LoadOrder;                     // 加载顺序策略
    bool IsAMDStyle;                          // 是否为AMD风格
    
    ModuleLoadDefinition() : IsAMDStyle(true) {}
};

/**
 * 增强选择器缓存结构
 * 缓存{{}}选择器的解析结果
 */
struct EnhancedSelectorCache {
    std::string OriginalSelector;              // 原始选择器
    std::string ParsedSelector;               // 解析后的选择器
    std::string SelectorType;                 // 选择器类型（class/id/tag/descendant）
    bool IsValidSelector;                     // 是否为有效选择器
    std::string DOMMethod;                    // 对应的DOM方法
    
    EnhancedSelectorCache() = default;
    EnhancedSelectorCache(const std::string& original, const std::string& parsed, const std::string& type)
        : OriginalSelector(original), ParsedSelector(parsed), SelectorType(type), IsValidSelector(true) {}
};

/**
 * CHTL JS全局映射表
 * 管理所有CHTL JS语法元素的定义和引用关系（完全独立于CHTL）
 */
class CHTLJSGlobalMap {
private:
    // 虚对象映射表
    std::unordered_map<std::string, std::unique_ptr<VirtualObjectDefinition>> m_VirtualObjects;
    
    // 事件委托映射表
    std::unordered_map<std::string, std::unique_ptr<EventDelegateDefinition>> m_EventDelegates;
    
    // 动画映射表
    std::unordered_map<std::string, std::unique_ptr<AnimationDefinition>> m_Animations;
    
    // 模块加载映射表
    std::unordered_map<std::string, std::unique_ptr<ModuleLoadDefinition>> m_ModuleLoads;
    
    // 增强选择器缓存
    std::unordered_map<std::string, std::unique_ptr<EnhancedSelectorCache>> m_SelectorCache;
    
    // CHTL JS函数注册表
    std::unordered_map<std::string, bool> m_CHTLJSFunctions;
    
    // 全局计数器
    int m_VirtualObjectCounter;
    int m_AnimationCounter;
    int m_DelegateCounter;

public:
    /**
     * 构造函数
     */
    CHTLJSGlobalMap();
    
    /**
     * 析构函数
     */
    ~CHTLJSGlobalMap() = default;
    
    // 虚对象管理
    /**
     * 注册虚对象定义
     * @param definition 虚对象定义
     * @return 是否注册成功
     */
    bool RegisterVirtualObject(std::unique_ptr<VirtualObjectDefinition> definition);
    
    /**
     * 获取虚对象定义
     * @param name 虚对象名称
     * @return 虚对象定义指针
     */
    VirtualObjectDefinition* GetVirtualObject(const std::string& name);
    
    /**
     * 解析虚对象键引用
     * @param objectName 虚对象名称
     * @param keyName 键名称
     * @return 解析后的值
     */
    std::string ResolveVirtualObjectKey(const std::string& objectName, const std::string& keyName);
    
    // 事件委托管理
    /**
     * 注册事件委托
     * @param definition 事件委托定义
     * @return 是否注册成功
     */
    bool RegisterEventDelegate(std::unique_ptr<EventDelegateDefinition> definition);
    
    /**
     * 获取事件委托定义
     * @param parentSelector 父选择器
     * @return 事件委托定义指针
     */
    EventDelegateDefinition* GetEventDelegate(const std::string& parentSelector);
    
    /**
     * 合并事件委托（避免重复绑定）
     * @param parentSelector 父选择器
     * @param targetSelector 目标选择器
     * @param eventType 事件类型
     * @param handler 处理器
     * @return 是否成功合并
     */
    bool MergeEventDelegate(const std::string& parentSelector, 
                           const std::string& targetSelector,
                           const std::string& eventType, 
                           const std::string& handler);
    
    // 动画管理
    /**
     * 注册动画定义
     * @param definition 动画定义
     * @return 是否注册成功
     */
    bool RegisterAnimation(std::unique_ptr<AnimationDefinition> definition);
    
    /**
     * 获取动画定义
     * @param name 动画名称
     * @return 动画定义指针
     */
    AnimationDefinition* GetAnimation(const std::string& name);
    
    /**
     * 生成动画名称
     * @return 自动生成的动画名称
     */
    std::string GenerateAnimationName();
    
    // 模块加载管理
    /**
     * 注册模块加载定义
     * @param definition 模块加载定义
     * @return 是否注册成功
     */
    bool RegisterModuleLoad(std::unique_ptr<ModuleLoadDefinition> definition);
    
    /**
     * 获取模块加载定义
     * @param contextName 上下文名称
     * @return 模块加载定义指针
     */
    ModuleLoadDefinition* GetModuleLoad(const std::string& contextName);
    
    /**
     * 分析模块依赖关系
     * @param loadPaths 加载路径列表
     * @return 排序后的加载顺序
     */
    std::vector<std::string> AnalyzeDependencies(const std::vector<std::string>& loadPaths);
    
    // 增强选择器管理
    /**
     * 缓存选择器解析结果
     * @param cache 选择器缓存
     * @return 是否缓存成功
     */
    bool CacheSelector(std::unique_ptr<EnhancedSelectorCache> cache);
    
    /**
     * 获取选择器缓存
     * @param selector 原始选择器
     * @return 缓存指针
     */
    EnhancedSelectorCache* GetSelectorCache(const std::string& selector);
    
    /**
     * 解析增强选择器
     * @param selector 选择器字符串
     * @return 解析结果
     */
    std::string ParseEnhancedSelector(const std::string& selector);
    
    /**
     * 生成DOM查询代码
     * @param selector 选择器
     * @param method 方法名称
     * @return 生成的JavaScript代码
     */
    std::string GenerateDOMQueryCode(const std::string& selector, const std::string& method = "");
    
    // CHTL JS函数管理
    /**
     * 注册CHTL JS函数
     * @param functionName 函数名称
     * @param supportVir 是否支持vir
     */
    void RegisterCHTLJSFunction(const std::string& functionName, bool supportVir = true);
    
    /**
     * 检查是否为CHTL JS函数
     * @param functionName 函数名称
     * @return 是否为CHTL JS函数
     */
    bool IsCHTLJSFunction(const std::string& functionName);
    
    /**
     * 获取所有CHTL JS函数
     * @return 函数名称列表
     */
    std::vector<std::string> GetAllCHTLJSFunctions() const;
    
    // 代码生成辅助
    /**
     * 生成事件监听器代码
     * @param selector 选择器
     * @param events 事件映射
     * @return 生成的JavaScript代码
     */
    std::string GenerateListenerCode(const std::string& selector, 
                                    const std::unordered_map<std::string, std::string>& events);
    
    /**
     * 生成事件委托代码
     * @param delegate 事件委托定义
     * @return 生成的JavaScript代码
     */
    std::string GenerateDelegateCode(const EventDelegateDefinition& delegate);
    
    /**
     * 生成动画代码
     * @param animation 动画定义
     * @return 生成的JavaScript代码
     */
    std::string GenerateAnimationCode(const AnimationDefinition& animation);
    
    /**
     * 生成模块加载代码
     * @param moduleLoad 模块加载定义
     * @return 生成的JavaScript代码
     */
    std::string GenerateModuleLoadCode(const ModuleLoadDefinition& moduleLoad);
    
    // 清理和统计
    /**
     * 清空所有映射
     */
    void Clear();
    
    /**
     * 获取统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;

private:
    /**
     * 生成唯一名称
     * @param prefix 前缀
     * @param counter 计数器引用
     * @return 唯一名称
     */
    std::string GenerateUniqueName(const std::string& prefix, int& counter);
    
    /**
     * 验证选择器有效性
     * @param selector 选择器
     * @return 是否有效
     */
    bool ValidateSelector(const std::string& selector);
    
    /**
     * 优化事件委托合并
     * @param parentSelector 父选择器
     */
    void OptimizeEventDelegateMerging(const std::string& parentSelector);
};

} // namespace CHTLJS