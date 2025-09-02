#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

namespace CHTL {

/**
 * 上下文类型枚举
 */
enum class CHTLContextType {
    GLOBAL_CONTEXT,         // 全局上下文
    ELEMENT_CONTEXT,        // 元素上下文
    STYLE_CONTEXT,          // 样式上下文
    SCRIPT_CONTEXT,         // 脚本上下文（局部script属于CHTL）
    TEMPLATE_CONTEXT,       // 模板上下文
    CUSTOM_CONTEXT,         // 自定义上下文
    NAMESPACE_CONTEXT,      // 命名空间上下文
    IMPORT_CONTEXT,         // 导入上下文
    CONFIGURATION_CONTEXT   // 配置上下文
};

/**
 * 作用域类型枚举
 */
enum class CHTLScopeType {
    FILE_SCOPE,             // 文件作用域
    NAMESPACE_SCOPE,        // 命名空间作用域
    TEMPLATE_SCOPE,         // 模板作用域
    CUSTOM_SCOPE,           // 自定义作用域
    ELEMENT_SCOPE,          // 元素作用域
    STYLE_SCOPE,            // 样式作用域
    SCRIPT_SCOPE            // 脚本作用域
};

/**
 * 上下文信息结构
 */
struct ContextInfo {
    CHTLContextType Type;               // 上下文类型
    CHTLScopeType Scope;               // 作用域类型
    std::string Name;                   // 上下文名称
    std::string ParentName;             // 父上下文名称
    std::unordered_map<std::string, std::string> Variables; // 变量映射
    std::unordered_map<std::string, std::string> Properties; // 属性映射
    std::vector<std::string> ClassNames;    // 自动生成的类名
    std::vector<std::string> IdNames;       // 自动生成的ID名
    std::vector<std::string> Constraints;   // 约束列表
    bool HasAutoClass;                  // 是否有自动类名
    bool HasAutoId;                     // 是否有自动ID
    
    ContextInfo() = default;
    ContextInfo(CHTLContextType type, CHTLScopeType scope, const std::string& name)
        : Type(type), Scope(scope), Name(name), HasAutoClass(false), HasAutoId(false) {}
};

/**
 * 引用解析结果结构
 */
struct ReferenceResolution {
    std::string ResolvedValue;          // 解析后的值
    std::string ReferenceType;          // 引用类型（class/id/variable）
    bool IsResolved;                    // 是否成功解析
    std::string ErrorMessage;           // 错误信息
    
    ReferenceResolution() : IsResolved(false) {}
    ReferenceResolution(const std::string& value, const std::string& type)
        : ResolvedValue(value), ReferenceType(type), IsResolved(true) {}
};

/**
 * CHTL上下文管理器
 * 管理CHTL解析过程中的上下文信息和作用域
 * 支持上下文推导、变量解析、约束检查等功能
 */
class CHTLContextManager {
private:
    std::stack<ContextInfo*> m_ContextStack;                   // 上下文栈
    std::unique_ptr<ContextInfo> m_CurrentContext;              // 当前上下文
    std::unordered_map<std::string, std::string> m_GlobalVariables; // 全局变量
    std::unordered_map<std::string, std::vector<std::string>> m_ScopeChain; // 作用域链
    
    // 自动化选择器管理
    std::unordered_map<std::string, std::string> m_AutoClassMap;    // 自动类名映射
    std::unordered_map<std::string, std::string> m_AutoIdMap;       // 自动ID映射
    
    // 约束管理
    std::vector<std::string> m_GlobalConstraints;               // 全局约束
    std::unordered_map<std::string, std::vector<std::string>> m_ScopeConstraints; // 作用域约束

public:
    /**
     * 构造函数
     */
    CHTLContextManager();
    
    /**
     * 析构函数
     */
    ~CHTLContextManager() = default;
    
    /**
     * 进入新上下文
     * @param type 上下文类型
     * @param scope 作用域类型
     * @param name 上下文名称
     * @return 是否成功进入
     */
    bool EnterContext(CHTLContextType type, CHTLScopeType scope, const std::string& name);
    
    /**
     * 退出当前上下文
     * @return 是否成功退出
     */
    bool ExitContext();
    
    /**
     * 获取当前上下文
     * @return 当前上下文指针
     */
    ContextInfo* GetCurrentContext();
    
    /**
     * 获取上下文类型
     * @return 当前上下文类型
     */
    CHTLContextType GetCurrentContextType() const;
    
    /**
     * 获取作用域类型
     * @return 当前作用域类型
     */
    CHTLScopeType GetCurrentScopeType() const;
    
    // 变量管理
    /**
     * 设置变量
     * @param name 变量名
     * @param value 变量值
     * @param isGlobal 是否为全局变量
     */
    void SetVariable(const std::string& name, const std::string& value, bool isGlobal = false);
    
    /**
     * 获取变量值
     * @param name 变量名
     * @return 变量值，如果不存在返回空字符串
     */
    std::string GetVariable(const std::string& name);
    
    /**
     * 检查变量是否存在
     * @param name 变量名
     * @return 是否存在
     */
    bool HasVariable(const std::string& name);
    
    // 属性管理
    /**
     * 设置属性
     * @param name 属性名
     * @param value 属性值
     */
    void SetProperty(const std::string& name, const std::string& value);
    
    /**
     * 获取属性值
     * @param name 属性名
     * @return 属性值
     */
    std::string GetProperty(const std::string& name);
    
    /**
     * 检查属性是否存在
     * @param name 属性名
     * @return 是否存在
     */
    bool HasProperty(const std::string& name);
    
    // 选择器自动化管理
    /**
     * 添加自动生成的类名
     * @param className 类名
     */
    void AddAutoClass(const std::string& className);
    
    /**
     * 添加自动生成的ID名
     * @param idName ID名
     */
    void AddAutoId(const std::string& idName);
    
    /**
     * 获取第一个自动类名（用于&引用）
     * @return 第一个类名，如果没有返回空字符串
     */
    std::string GetFirstAutoClass();
    
    /**
     * 获取第一个自动ID名（用于&引用）
     * @return 第一个ID名，如果没有返回空字符串
     */
    std::string GetFirstAutoId();
    
    /**
     * 解析上下文引用（&）
     * @param referenceType 引用类型（默认优先class）
     * @return 引用解析结果
     */
    ReferenceResolution ResolveContextReference(const std::string& referenceType = "");
    
    // 约束管理
    /**
     * 添加约束
     * @param constraint 约束条件
     * @param isGlobal 是否为全局约束
     */
    void AddConstraint(const std::string& constraint, bool isGlobal = false);
    
    /**
     * 检查约束
     * @param elementType 元素类型
     * @return 是否通过约束检查
     */
    bool CheckConstraint(const std::string& elementType);
    
    /**
     * 获取当前作用域的所有约束
     * @return 约束列表
     */
    std::vector<std::string> GetCurrentConstraints();
    
    // 作用域管理
    /**
     * 构建作用域链
     * @return 作用域链
     */
    std::vector<std::string> BuildScopeChain();
    
    /**
     * 在作用域链中查找变量
     * @param name 变量名
     * @return 变量值
     */
    std::string FindInScopeChain(const std::string& name);
    
    /**
     * 获取完整的上下文路径
     * @return 上下文路径字符串
     */
    std::string GetFullContextPath();
    
    // 配置相关
    /**
     * 检查是否禁用样式自动添加类选择器
     * @return 是否禁用
     */
    bool IsStyleAutoAddClassDisabled();
    
    /**
     * 检查是否禁用样式自动添加ID选择器
     * @return 是否禁用
     */
    bool IsStyleAutoAddIdDisabled();
    
    /**
     * 检查是否禁用脚本自动添加类选择器
     * @return 是否禁用
     */
    bool IsScriptAutoAddClassDisabled();
    
    /**
     * 检查是否禁用脚本自动添加ID选择器
     * @return 是否禁用
     */
    bool IsScriptAutoAddIdDisabled();
    
    /**
     * 获取索引起始计数
     * @return 起始计数值
     */
    int GetIndexInitialCount();
    
    // 调试和统计
    /**
     * 获取上下文统计信息
     * @return 统计信息字符串
     */
    std::string GetContextStatistics() const;
    
    /**
     * 打印当前上下文栈
     * @return 上下文栈信息
     */
    std::string PrintContextStack() const;
    
    /**
     * 重置上下文管理器
     */
    void Reset();

private:
    /**
     * 创建默认全局上下文
     */
    void CreateDefaultGlobalContext();
    
    /**
     * 验证上下文转换的有效性
     * @param fromType 源上下文类型
     * @param toType 目标上下文类型
     * @return 是否有效
     */
    bool IsValidContextTransition(CHTLContextType fromType, CHTLContextType toType);
    
    /**
     * 继承父上下文的信息
     * @param parentContext 父上下文
     * @param childContext 子上下文
     */
    void InheritFromParentContext(ContextInfo* parentContext, ContextInfo* childContext);
    
    /**
     * 应用作用域规则
     * @param context 上下文
     */
    void ApplyScopeRules(ContextInfo* context);
    
    /**
     * 获取配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string GetConfigValue(const std::string& key, const std::string& defaultValue);
};

/**
 * RAII上下文管理器
 * 自动管理上下文的进入和退出
 */
class CHTLContextGuard {
private:
    CHTLContextManager* m_ContextManager;
    bool m_ShouldExit;

public:
    /**
     * 构造函数 - 自动进入上下文
     * @param contextManager 上下文管理器指针
     * @param type 上下文类型
     * @param scope 作用域类型
     * @param name 上下文名称
     */
    CHTLContextGuard(CHTLContextManager* contextManager, 
                     CHTLContextType type, 
                     CHTLScopeType scope, 
                     const std::string& name);
    
    /**
     * 析构函数 - 自动退出上下文
     */
    ~CHTLContextGuard();
    
    // 禁止拷贝和赋值
    CHTLContextGuard(const CHTLContextGuard&) = delete;
    CHTLContextGuard& operator=(const CHTLContextGuard&) = delete;
    
    // 支持移动
    CHTLContextGuard(CHTLContextGuard&& other) noexcept;
    CHTLContextGuard& operator=(CHTLContextGuard&& other) noexcept;
    
    /**
     * 手动释放上下文管理
     */
    void Release();
};

} // namespace CHTL