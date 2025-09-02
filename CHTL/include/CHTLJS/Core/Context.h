#ifndef CHTL_CHTLJS_CORE_CONTEXT_H
#define CHTL_CHTLJS_CORE_CONTEXT_H

#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include "CHTLJS/Core/GlobalMap.h"
#include "CHTLJS/Core/State.h"
#include "Common/Error.h"

namespace CHTL {
namespace JS {

// 前向声明
class VirtualObjectNode;
class SelectorCache;

/**
 * CHTL JS编译上下文
 */
class CompileContext {
private:
    // 全局映射表
    std::shared_ptr<GlobalMap> globalMap;
    
    // 状态机
    std::shared_ptr<StateMachine> stateMachine;
    
    // 当前文件信息
    std::string currentFile;
    std::string moduleDirectory;
    
    // 选择器缓存
    std::shared_ptr<SelectorCache> selectorCache;
    
    // 局部作用域栈
    struct LocalScope {
        std::unordered_map<std::string, std::string> variables;  // 局部变量
        std::shared_ptr<VirtualObjectNode> currentVirtualObject; // 当前虚对象
        std::string currentSelector;                              // 当前选择器
        bool inCHTLJSFunction;                                   // 是否在CHTL JS函数中
    };
    std::stack<LocalScope> scopeStack;
    
    // 编译选项
    struct CompileOptions {
        bool debugMode;
        bool enableSelectorCache;         // 启用选择器缓存
        bool enableEventDelegation;       // 启用事件委托优化
        bool enableAnimationOptimization; // 启用动画优化
        
        CompileOptions() : debugMode(false),
                          enableSelectorCache(true),
                          enableEventDelegation(true),
                          enableAnimationOptimization(true) {}
    } options;
    
public:
    // 生成的代码统计
    struct CodeStats {
        int selectorCount;        // 选择器数量
        int virtualObjectCount;   // 虚对象数量
        int animationCount;       // 动画数量
        int listenerCount;        // 监听器数量
        int delegateCount;        // 委托数量
        
        CodeStats() : selectorCount(0), virtualObjectCount(0),
                     animationCount(0), listenerCount(0), delegateCount(0) {}
    };
    
private:
    CodeStats stats;
    
public:
    CompileContext();
    ~CompileContext() = default;
    
    // 全局映射表访问
    std::shared_ptr<GlobalMap> GetGlobalMap() { return globalMap; }
    const std::shared_ptr<GlobalMap> GetGlobalMap() const { return globalMap; }
    
    // 状态机访问
    std::shared_ptr<StateMachine> GetStateMachine() { return stateMachine; }
    const std::shared_ptr<StateMachine> GetStateMachine() const { return stateMachine; }
    
    // 文件信息
    void SetCurrentFile(const std::string& file) { currentFile = file; }
    const std::string& GetCurrentFile() const { return currentFile; }
    
    void SetModuleDirectory(const std::string& dir) { moduleDirectory = dir; }
    const std::string& GetModuleDirectory() const { return moduleDirectory; }
    
    // 选择器缓存
    std::shared_ptr<SelectorCache> GetSelectorCache() { return selectorCache; }
    
    // 作用域管理
    void EnterScope();
    void ExitScope();
    LocalScope& GetCurrentScope();
    const LocalScope& GetCurrentScope() const;
    
    // 局部变量管理
    void SetLocalVariable(const std::string& name, const std::string& value);
    std::string GetLocalVariable(const std::string& name) const;
    bool HasLocalVariable(const std::string& name) const;
    
    // 当前虚对象管理
    void SetCurrentVirtualObject(std::shared_ptr<VirtualObjectNode> vobj);
    std::shared_ptr<VirtualObjectNode> GetCurrentVirtualObject() const;
    
    // 当前选择器管理
    void SetCurrentSelector(const std::string& selector);
    const std::string& GetCurrentSelector() const;
    
    // CHTL JS函数状态
    void SetInCHTLJSFunction(bool inFunc);
    bool IsInCHTLJSFunction() const;
    
    // 编译选项
    void SetDebugMode(bool enable) { options.debugMode = enable; }
    bool IsDebugMode() const { return options.debugMode; }
    
    void SetEnableSelectorCache(bool enable) { options.enableSelectorCache = enable; }
    bool IsSelectorCacheEnabled() const { return options.enableSelectorCache; }
    
    void SetEnableEventDelegation(bool enable) { options.enableEventDelegation = enable; }
    bool IsEventDelegationEnabled() const { return options.enableEventDelegation; }
    
    void SetEnableAnimationOptimization(bool enable) { options.enableAnimationOptimization = enable; }
    bool IsAnimationOptimizationEnabled() const { return options.enableAnimationOptimization; }
    
    // 代码统计
    void IncrementSelectorCount() { stats.selectorCount++; }
    void IncrementVirtualObjectCount() { stats.virtualObjectCount++; }
    void IncrementAnimationCount() { stats.animationCount++; }
    void IncrementListenerCount() { stats.listenerCount++; }
    void IncrementDelegateCount() { stats.delegateCount++; }
    
    const CodeStats& GetStats() const { return stats; }
    
    // 重置上下文
    void Reset();
    
    /**
     * RAII作用域管理器
     */
    class ScopeGuard {
    private:
        CompileContext& context;
        bool exited;
        
    public:
        explicit ScopeGuard(CompileContext& ctx) : context(ctx), exited(false) {
            context.EnterScope();
        }
        
        ~ScopeGuard() {
            if (!exited) {
                context.ExitScope();
            }
        }
        
        void Exit() {
            if (!exited) {
                context.ExitScope();
                exited = true;
            }
        }
        
        // 禁止拷贝
        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
    };
};

} // namespace JS
} // namespace CHTL

#endif // CHTL_CHTLJS_CORE_CONTEXT_H