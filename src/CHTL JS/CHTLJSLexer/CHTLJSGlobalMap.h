#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTLJS {

    // 前向声明
    enum class CHTLJSTokenType;

    // CHTL JS全局映射表
    // 管理CHTL JS编译器的全局状态和映射关系
    // 完全独立于CHTL系统
    class CHTLJSGlobalMap {
    private:
        // 模块映射
        std::unordered_map<std::string, std::vector<std::string>> moduleLoadMap;    // 模块加载映射
        std::unordered_set<std::string> loadedModules;                              // 已加载模块集合
        
        // 选择器映射
        std::unordered_map<std::string, std::string> selectorMap;                   // 选择器映射
        std::unordered_map<std::string, std::string> selectorTypeMap;               // 选择器类型映射
        
        // 事件监听器映射
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> listenerMap;  // 事件监听器映射
        
        // 事件委托映射
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> delegateMap;  // 事件委托映射
        
        // 动画映射
        std::unordered_map<std::string, std::string> animationMap;                  // 动画定义映射
        
        // 虚对象映射
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> virtualObjectMap;  // 虚对象映射
        
        // CHTL JS函数映射
        std::unordered_map<std::string, std::string> chtljsFunctionMap;             // CHTL JS函数映射
        
        // util...then表达式映射
        std::unordered_map<std::string, std::pair<std::string, std::string>> utilThenMap;  // util表达式映射
        
        // 自动化选择器映射（与CHTL系统分离）
        std::unordered_map<std::string, std::string> autoClassMap;          // 自动类名映射
        std::unordered_map<std::string, std::string> autoIdMap;             // 自动ID映射

        static std::shared_ptr<CHTLJSGlobalMap> instance;

    public:
        // 单例模式
        static std::shared_ptr<CHTLJSGlobalMap> GetInstance();
        
        // 模块管理
        void RegisterModule(const std::string& moduleName, const std::vector<std::string>& loadPaths);
        void AddModuleLoad(const std::string& moduleName, const std::string& loadPath);
        void MarkModuleLoaded(const std::string& moduleName);
        
        std::vector<std::string> GetModuleLoads(const std::string& moduleName) const;
        bool IsModuleLoaded(const std::string& moduleName) const;
        std::vector<std::string> GetAllModules() const;
        
        // 选择器管理
        void RegisterSelector(const std::string& selectorId, const std::string& selectorValue);
        void SetSelectorType(const std::string& selectorId, const std::string& type);
        
        std::string GetSelector(const std::string& selectorId) const;
        std::string GetSelectorType(const std::string& selectorId) const;
        bool HasSelector(const std::string& selectorId) const;
        
        // 事件监听器管理
        void RegisterListener(const std::string& selectorId, const std::string& event, const std::string& handler);
        void RemoveListener(const std::string& selectorId, const std::string& event);
        
        std::unordered_map<std::string, std::string> GetListeners(const std::string& selectorId) const;
        std::string GetListener(const std::string& selectorId, const std::string& event) const;
        bool HasListener(const std::string& selectorId, const std::string& event) const;
        
        // 事件委托管理
        void RegisterDelegate(const std::string& parentSelector, const std::string& event, const std::vector<std::string>& targets);
        void AddDelegateTarget(const std::string& parentSelector, const std::string& event, const std::string& target);
        
        std::vector<std::string> GetDelegateTargets(const std::string& parentSelector, const std::string& event) const;
        bool HasDelegate(const std::string& parentSelector, const std::string& event) const;
        
        // 动画管理
        void RegisterAnimation(const std::string& animationId, const std::string& animationDef);
        std::string GetAnimation(const std::string& animationId) const;
        bool HasAnimation(const std::string& animationId) const;
        
        // 虚对象管理
        void RegisterVirtualObject(const std::string& objName, const std::string& key, const std::string& value);
        void RemoveVirtualObjectKey(const std::string& objName, const std::string& key);
        
        std::unordered_map<std::string, std::string> GetVirtualObject(const std::string& objName) const;
        std::string GetVirtualObjectKey(const std::string& objName, const std::string& key) const;
        bool HasVirtualObject(const std::string& objName) const;
        bool HasVirtualObjectKey(const std::string& objName, const std::string& key) const;
        
        // CHTL JS函数管理
        void RegisterCHTLJSFunction(const std::string& functionName, const std::string& functionDef);
        std::string GetCHTLJSFunction(const std::string& functionName) const;
        bool HasCHTLJSFunction(const std::string& functionName) const;
        
        // util...then表达式管理
        void RegisterUtilThen(const std::string& condition, const std::string& changeAction, const std::string& thenAction);
        std::pair<std::string, std::string> GetUtilThen(const std::string& condition) const;
        bool HasUtilThen(const std::string& condition) const;
        
        // 选择器自动化管理（CHTL JS特有）
        void RegisterAutoClass(const std::string& elementId, const std::string& className);
        void RegisterAutoId(const std::string& elementId, const std::string& idName);
        
        std::string GetAutoClass(const std::string& elementId) const;
        std::string GetAutoId(const std::string& elementId) const;
        
        // 工具方法
        void Clear();                                           // 清空所有映射
        void Reset();                                           // 重置为初始状态
        std::vector<std::string> GetAllSelectors() const;       // 获取所有选择器
        std::vector<std::string> GetAllVirtualObjects() const;  // 获取所有虚对象
        
        // 调试方法
        void PrintMappings() const;                             // 打印所有映射（调试用）
    };

} // namespace CHTLJS