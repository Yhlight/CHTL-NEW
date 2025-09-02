#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS上下文类型枚举（完全独立于CHTL）
 */
enum class CHTLJSContextType {
    GLOBAL_CONTEXT,         // 全局上下文
    MODULE_CONTEXT,         // 模块上下文
    LISTEN_CONTEXT,         // listen函数上下文
    DELEGATE_CONTEXT,       // delegate函数上下文
    ANIMATE_CONTEXT,        // animate函数上下文
    VIR_CONTEXT,            // 虚对象上下文
    SELECTOR_CONTEXT,       // 增强选择器上下文
    EVENT_CONTEXT           // 事件处理上下文
};

/**
 * CHTL JS作用域类型枚举（完全独立于CHTL）
 */
enum class CHTLJSScopeType {
    FILE_SCOPE,             // 文件作用域
    MODULE_SCOPE,           // 模块作用域
    FUNCTION_SCOPE,         // CHTL JS函数作用域
    VIR_SCOPE,              // 虚对象作用域
    EVENT_SCOPE             // 事件处理作用域
};

/**
 * CHTL JS上下文信息结构（完全独立于CHTL）
 */
struct CHTLJSContextInfo {
    CHTLJSContextType Type;                 // 上下文类型
    CHTLJSScopeType Scope;                 // 作用域类型
    std::string Name;                       // 上下文名称
    std::string ParentName;                 // 父上下文名称
    
    // CHTL JS特有的上下文信息
    std::unordered_map<std::string, std::string> VirtualObjects;    // 虚对象映射
    std::unordered_map<std::string, std::string> SelectorMappings;  // 选择器映射
    std::unordered_map<std::string, std::string> EventHandlers;     // 事件处理器映射
    std::unordered_map<std::string, std::string> AnimationStates;   // 动画状态映射
    std::vector<std::string> ModulePaths;                           // 模块路径列表
    
    // 引用和缓存
    std::string CurrentSelector;            // 当前选择器
    std::string CurrentVirObject;           // 当前虚对象
    std::string CurrentAnimation;           // 当前动画
    std::string CurrentDelegate;            // 当前事件委托
    
    CHTLJSContextInfo() = default;
    CHTLJSContextInfo(CHTLJSContextType type, CHTLJSScopeType scope, const std::string& name)
        : Type(type), Scope(scope), Name(name) {}
};

/**
 * 虚对象键解析结果
 */
struct VirKeyResolution {
    std::string ResolvedValue;              // 解析后的值
    std::string KeyType;                    // 键类型（function/object/array）
    bool IsResolved;                        // 是否成功解析
    std::string ErrorMessage;               // 错误信息
    
    VirKeyResolution() : IsResolved(false) {}
    VirKeyResolution(const std::string& value, const std::string& type)
        : ResolvedValue(value), KeyType(type), IsResolved(true) {}
};

/**
 * CHTL JS上下文管理器（完全独立于CHTL上下文管理器）
 * 管理CHTL JS解析过程中的上下文信息和作用域
 * 专门处理CHTL JS特有的语法特征
 */
class CHTLJSContextManager {
private:
    std::stack<CHTLJSContextInfo*> m_ContextStack;                     // 上下文栈
    std::unique_ptr<CHTLJSContextInfo> m_CurrentContext;                // 当前上下文
    
    // CHTL JS特有的全局管理
    std::unordered_map<std::string, std::string> m_GlobalVirtualObjects;   // 全局虚对象
    std::unordered_map<std::string, std::vector<std::string>> m_GlobalEventDelegates; // 全局事件委托
    std::unordered_map<std::string, std::string> m_GlobalAnimations;       // 全局动画
    std::unordered_map<std::string, std::vector<std::string>> m_GlobalModules; // 全局模块
    
    // 选择器解析缓存
    std::unordered_map<std::string, std::string> m_SelectorCache;          // 选择器缓存
    
    // 计数器
    int m_VirObjectCounter;
    int m_AnimationCounter;
    int m_DelegateCounter;

public:
    /**
     * 构造函数
     */
    CHTLJSContextManager();
    
    /**
     * 析构函数
     */
    ~CHTLJSContextManager() = default;
    
    /**
     * 进入新上下文
     * @param type 上下文类型
     * @param scope 作用域类型
     * @param name 上下文名称
     * @return 是否成功进入
     */
    bool EnterContext(CHTLJSContextType type, CHTLJSScopeType scope, const std::string& name);
    
    /**
     * 退出当前上下文
     * @return 是否成功退出
     */
    bool ExitContext();
    
    /**
     * 获取当前上下文
     * @return 当前上下文指针
     */
    CHTLJSContextInfo* GetCurrentContext();
    
    /**
     * 获取上下文类型
     * @return 当前上下文类型
     */
    CHTLJSContextType GetCurrentContextType() const;
    
    /**
     * 获取作用域类型
     * @return 当前作用域类型
     */
    CHTLJSScopeType GetCurrentScopeType() const;
    
    // 虚对象管理
    /**
     * 注册虚对象
     * @param name 虚对象名称
     * @param functionContent CHTL JS函数内容
     * @param isGlobal 是否为全局虚对象
     */
    void RegisterVirtualObject(const std::string& name, const std::string& functionContent, bool isGlobal = false);
    
    /**
     * 解析虚对象键引用
     * @param objectName 虚对象名称
     * @param keyName 键名称
     * @return 解析结果
     */
    VirKeyResolution ResolveVirtualObjectKey(const std::string& objectName, const std::string& keyName);
    
    /**
     * 检查虚对象是否存在
     * @param name 虚对象名称
     * @return 是否存在
     */
    bool HasVirtualObject(const std::string& name);
    
    // 增强选择器管理
    /**
     * 注册选择器映射
     * @param selector 原始选择器
     * @param domCode 对应的DOM代码
     */
    void RegisterSelectorMapping(const std::string& selector, const std::string& domCode);
    
    /**
     * 解析增强选择器
     * @param selector 选择器字符串（{{...}}）
     * @return 解析后的DOM查询代码
     */
    std::string ResolveEnhancedSelector(const std::string& selector);
    
    /**
     * 生成选择器优先级查找代码
     * @param content 选择器内容
     * @return 生成的JavaScript代码
     */
    std::string GenerateSelectorPriorityCode(const std::string& content);
    
    // 事件管理
    /**
     * 注册事件处理器
     * @param selector 选择器
     * @param eventType 事件类型
     * @param handler 处理器代码
     * @param isGlobal 是否为全局事件
     */
    void RegisterEventHandler(const std::string& selector, const std::string& eventType, 
                            const std::string& handler, bool isGlobal = false);
    
    /**
     * 注册事件委托
     * @param parentSelector 父选择器
     * @param targetSelectors 目标选择器列表
     * @param eventType 事件类型
     * @param handler 处理器
     */
    void RegisterEventDelegate(const std::string& parentSelector, 
                             const std::vector<std::string>& targetSelectors,
                             const std::string& eventType, 
                             const std::string& handler);
    
    /**
     * 获取事件委托代码
     * @param parentSelector 父选择器
     * @return 生成的委托代码
     */
    std::string GetEventDelegateCode(const std::string& parentSelector);
    
    // 动画管理
    /**
     * 注册动画定义
     * @param name 动画名称
     * @param targets 目标选择器列表
     * @param properties 动画属性
     * @param isGlobal 是否为全局动画
     */
    void RegisterAnimation(const std::string& name, const std::vector<std::string>& targets,
                         const std::unordered_map<std::string, std::string>& properties, 
                         bool isGlobal = false);
    
    /**
     * 生成动画代码
     * @param animationName 动画名称
     * @return 生成的动画JavaScript代码
     */
    std::string GenerateAnimationCode(const std::string& animationName);
    
    // 模块管理
    /**
     * 注册模块路径
     * @param path 模块路径
     * @param isGlobal 是否为全局模块
     */
    void RegisterModulePath(const std::string& path, bool isGlobal = false);
    
    /**
     * 获取所有模块路径
     * @return 模块路径列表
     */
    std::vector<std::string> GetAllModulePaths();
    
    /**
     * 生成模块加载代码
     * @return AMD风格的模块加载代码
     */
    std::string GenerateModuleLoadCode();
    
    // 代码生成辅助
    /**
     * 生成listen函数代码
     * @param selector 选择器
     * @param events 事件映射
     * @return 生成的JavaScript代码
     */
    std::string GenerateListenCode(const std::string& selector, 
                                  const std::unordered_map<std::string, std::string>& events);
    
    /**
     * 生成事件绑定操作符代码（&->）
     * @param selector 选择器
     * @param eventType 事件类型
     * @param handler 处理器
     * @return 生成的JavaScript代码
     */
    std::string GenerateEventBindCode(const std::string& selector, 
                                     const std::string& eventType, 
                                     const std::string& handler);
    
    // 作用域管理
    /**
     * 构建CHTL JS作用域链
     * @return 作用域链
     */
    std::vector<std::string> BuildCHTLJSScopeChain();
    
    /**
     * 获取完整的CHTL JS上下文路径
     * @return 上下文路径字符串
     */
    std::string GetFullCHTLJSContextPath();
    
    // 验证和检查
    /**
     * 检查CHTL JS语法是否在正确的上下文中
     * @param syntaxType CHTL JS语法类型
     * @return 是否允许
     */
    bool IsAllowedCHTLJSSyntax(const std::string& syntaxType);
    
    /**
     * 验证增强选择器格式
     * @param selector 选择器字符串
     * @return 是否有效
     */
    bool ValidateEnhancedSelector(const std::string& selector);
    
    /**
     * 验证CHTL JS函数格式
     * @param functionContent 函数内容
     * @return 是否有效
     */
    bool ValidateCHTLJSFunction(const std::string& functionContent);
    
    // 调试和统计
    /**
     * 获取CHTL JS上下文统计信息
     * @return 统计信息字符串
     */
    std::string GetCHTLJSContextStatistics() const;
    
    /**
     * 打印CHTL JS上下文栈
     * @return 上下文栈信息
     */
    std::string PrintCHTLJSContextStack() const;
    
    /**
     * 重置CHTL JS上下文管理器
     */
    void Reset();

private:
    /**
     * 创建默认CHTL JS全局上下文
     */
    void CreateDefaultCHTLJSGlobalContext();
    
    /**
     * 验证CHTL JS上下文转换的有效性
     * @param fromType 源上下文类型
     * @param toType 目标上下文类型
     * @return 是否有效
     */
    bool IsValidCHTLJSContextTransition(CHTLJSContextType fromType, CHTLJSContextType toType);
    
    /**
     * 继承父CHTL JS上下文的信息
     * @param parentContext 父上下文
     * @param childContext 子上下文
     */
    void InheritFromParentCHTLJSContext(CHTLJSContextInfo* parentContext, CHTLJSContextInfo* childContext);
    
    /**
     * 应用CHTL JS作用域规则
     * @param context 上下文
     */
    void ApplyCHTLJSScopeRules(CHTLJSContextInfo* context);
    
    /**
     * 解析CHTL JS函数键值对
     * @param functionContent 函数内容
     * @return 键值对映射
     */
    std::unordered_map<std::string, std::string> ParseCHTLJSFunctionKeys(const std::string& functionContent);
    
    /**
     * 生成唯一名称
     * @param prefix 前缀
     * @param counter 计数器引用
     * @return 唯一名称
     */
    std::string GenerateUniqueName(const std::string& prefix, int& counter);
};

/**
 * RAII CHTL JS上下文管理器（完全独立于CHTL）
 * 自动管理CHTL JS上下文的进入和退出
 */
class CHTLJSContextGuard {
private:
    CHTLJSContextManager* m_ContextManager;
    bool m_ShouldExit;

public:
    /**
     * 构造函数 - 自动进入CHTL JS上下文
     * @param contextManager 上下文管理器指针
     * @param type 上下文类型
     * @param scope 作用域类型
     * @param name 上下文名称
     */
    CHTLJSContextGuard(CHTLJSContextManager* contextManager, 
                       CHTLJSContextType type, 
                       CHTLJSScopeType scope, 
                       const std::string& name);
    
    /**
     * 析构函数 - 自动退出CHTL JS上下文
     */
    ~CHTLJSContextGuard();
    
    // 禁止拷贝和赋值
    CHTLJSContextGuard(const CHTLJSContextGuard&) = delete;
    CHTLJSContextGuard& operator=(const CHTLJSContextGuard&) = delete;
    
    // 支持移动
    CHTLJSContextGuard(CHTLJSContextGuard&& other) noexcept;
    CHTLJSContextGuard& operator=(CHTLJSContextGuard&& other) noexcept;
    
    /**
     * 手动释放上下文管理
     */
    void Release();
};

} // namespace CHTLJS