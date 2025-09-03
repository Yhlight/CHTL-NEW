#include "CHTLJSGlobalMap.h"
#include "CHTLJSToken.h"
#include <iostream>
#include <algorithm>

namespace CHTLJS {

    // 静态成员初始化
    std::shared_ptr<CHTLJSGlobalMap> CHTLJSGlobalMap::instance = nullptr;

    std::shared_ptr<CHTLJSGlobalMap> CHTLJSGlobalMap::GetInstance() {
        if (instance == nullptr) {
            instance = std::shared_ptr<CHTLJSGlobalMap>(new CHTLJSGlobalMap());
        }
        return instance;
    }

    // 模块管理实现
    void CHTLJSGlobalMap::RegisterModule(const std::string& moduleName, const std::vector<std::string>& loadPaths) {
        moduleLoadMap[moduleName] = loadPaths;
    }

    void CHTLJSGlobalMap::AddModuleLoad(const std::string& moduleName, const std::string& loadPath) {
        moduleLoadMap[moduleName].push_back(loadPath);
    }

    void CHTLJSGlobalMap::MarkModuleLoaded(const std::string& moduleName) {
        loadedModules.insert(moduleName);
    }

    std::vector<std::string> CHTLJSGlobalMap::GetModuleLoads(const std::string& moduleName) const {
        auto it = moduleLoadMap.find(moduleName);
        return (it != moduleLoadMap.end()) ? it->second : std::vector<std::string>();
    }

    bool CHTLJSGlobalMap::IsModuleLoaded(const std::string& moduleName) const {
        return loadedModules.find(moduleName) != loadedModules.end();
    }

    std::vector<std::string> CHTLJSGlobalMap::GetAllModules() const {
        std::vector<std::string> modules;
        for (const auto& pair : moduleLoadMap) {
            modules.push_back(pair.first);
        }
        return modules;
    }

    // 选择器管理实现
    void CHTLJSGlobalMap::RegisterSelector(const std::string& selectorId, const std::string& selectorValue) {
        selectorMap[selectorId] = selectorValue;
    }

    void CHTLJSGlobalMap::SetSelectorType(const std::string& selectorId, const std::string& type) {
        selectorTypeMap[selectorId] = type;
    }

    std::string CHTLJSGlobalMap::GetSelector(const std::string& selectorId) const {
        auto it = selectorMap.find(selectorId);
        return (it != selectorMap.end()) ? it->second : "";
    }

    std::string CHTLJSGlobalMap::GetSelectorType(const std::string& selectorId) const {
        auto it = selectorTypeMap.find(selectorId);
        return (it != selectorTypeMap.end()) ? it->second : "";
    }

    bool CHTLJSGlobalMap::HasSelector(const std::string& selectorId) const {
        return selectorMap.find(selectorId) != selectorMap.end();
    }

    // 事件监听器管理实现
    void CHTLJSGlobalMap::RegisterListener(const std::string& selectorId, const std::string& event, const std::string& handler) {
        listenerMap[selectorId][event] = handler;
    }

    void CHTLJSGlobalMap::RemoveListener(const std::string& selectorId, const std::string& event) {
        auto selectorIt = listenerMap.find(selectorId);
        if (selectorIt != listenerMap.end()) {
            selectorIt->second.erase(event);
        }
    }

    std::unordered_map<std::string, std::string> CHTLJSGlobalMap::GetListeners(const std::string& selectorId) const {
        auto it = listenerMap.find(selectorId);
        return (it != listenerMap.end()) ? it->second : std::unordered_map<std::string, std::string>();
    }

    std::string CHTLJSGlobalMap::GetListener(const std::string& selectorId, const std::string& event) const {
        auto selectorIt = listenerMap.find(selectorId);
        if (selectorIt != listenerMap.end()) {
            auto eventIt = selectorIt->second.find(event);
            if (eventIt != selectorIt->second.end()) {
                return eventIt->second;
            }
        }
        return "";
    }

    bool CHTLJSGlobalMap::HasListener(const std::string& selectorId, const std::string& event) const {
        auto selectorIt = listenerMap.find(selectorId);
        if (selectorIt != listenerMap.end()) {
            return selectorIt->second.find(event) != selectorIt->second.end();
        }
        return false;
    }

    // 事件委托管理实现
    void CHTLJSGlobalMap::RegisterDelegate(const std::string& parentSelector, const std::string& event, const std::vector<std::string>& targets) {
        delegateMap[parentSelector][event] = targets;
    }

    void CHTLJSGlobalMap::AddDelegateTarget(const std::string& parentSelector, const std::string& event, const std::string& target) {
        delegateMap[parentSelector][event].push_back(target);
    }

    std::vector<std::string> CHTLJSGlobalMap::GetDelegateTargets(const std::string& parentSelector, const std::string& event) const {
        auto parentIt = delegateMap.find(parentSelector);
        if (parentIt != delegateMap.end()) {
            auto eventIt = parentIt->second.find(event);
            if (eventIt != parentIt->second.end()) {
                return eventIt->second;
            }
        }
        return std::vector<std::string>();
    }

    bool CHTLJSGlobalMap::HasDelegate(const std::string& parentSelector, const std::string& event) const {
        auto parentIt = delegateMap.find(parentSelector);
        if (parentIt != delegateMap.end()) {
            return parentIt->second.find(event) != parentIt->second.end();
        }
        return false;
    }

    // 动画管理实现
    void CHTLJSGlobalMap::RegisterAnimation(const std::string& animationId, const std::string& animationDef) {
        animationMap[animationId] = animationDef;
    }

    std::string CHTLJSGlobalMap::GetAnimation(const std::string& animationId) const {
        auto it = animationMap.find(animationId);
        return (it != animationMap.end()) ? it->second : "";
    }

    bool CHTLJSGlobalMap::HasAnimation(const std::string& animationId) const {
        return animationMap.find(animationId) != animationMap.end();
    }

    // 虚对象管理实现
    void CHTLJSGlobalMap::RegisterVirtualObject(const std::string& objName, const std::string& key, const std::string& value) {
        virtualObjectMap[objName][key] = value;
    }

    void CHTLJSGlobalMap::RemoveVirtualObjectKey(const std::string& objName, const std::string& key) {
        auto objIt = virtualObjectMap.find(objName);
        if (objIt != virtualObjectMap.end()) {
            objIt->second.erase(key);
        }
    }

    std::unordered_map<std::string, std::string> CHTLJSGlobalMap::GetVirtualObject(const std::string& objName) const {
        auto it = virtualObjectMap.find(objName);
        return (it != virtualObjectMap.end()) ? it->second : std::unordered_map<std::string, std::string>();
    }

    std::string CHTLJSGlobalMap::GetVirtualObjectKey(const std::string& objName, const std::string& key) const {
        auto objIt = virtualObjectMap.find(objName);
        if (objIt != virtualObjectMap.end()) {
            auto keyIt = objIt->second.find(key);
            if (keyIt != objIt->second.end()) {
                return keyIt->second;
            }
        }
        return "";
    }

    bool CHTLJSGlobalMap::HasVirtualObject(const std::string& objName) const {
        return virtualObjectMap.find(objName) != virtualObjectMap.end();
    }

    bool CHTLJSGlobalMap::HasVirtualObjectKey(const std::string& objName, const std::string& key) const {
        auto objIt = virtualObjectMap.find(objName);
        if (objIt != virtualObjectMap.end()) {
            return objIt->second.find(key) != objIt->second.end();
        }
        return false;
    }

    // CHTL JS函数管理实现
    void CHTLJSGlobalMap::RegisterCHTLJSFunction(const std::string& functionName, const std::string& functionDef) {
        chtljsFunctionMap[functionName] = functionDef;
    }

    std::string CHTLJSGlobalMap::GetCHTLJSFunction(const std::string& functionName) const {
        auto it = chtljsFunctionMap.find(functionName);
        return (it != chtljsFunctionMap.end()) ? it->second : "";
    }

    bool CHTLJSGlobalMap::HasCHTLJSFunction(const std::string& functionName) const {
        return chtljsFunctionMap.find(functionName) != chtljsFunctionMap.end();
    }

    // util...then表达式管理实现
    void CHTLJSGlobalMap::RegisterUtilThen(const std::string& condition, const std::string& changeAction, const std::string& thenAction) {
        utilThenMap[condition] = std::make_pair(changeAction, thenAction);
    }

    std::pair<std::string, std::string> CHTLJSGlobalMap::GetUtilThen(const std::string& condition) const {
        auto it = utilThenMap.find(condition);
        return (it != utilThenMap.end()) ? it->second : std::make_pair("", "");
    }

    bool CHTLJSGlobalMap::HasUtilThen(const std::string& condition) const {
        return utilThenMap.find(condition) != utilThenMap.end();
    }

    // 选择器自动化管理实现
    void CHTLJSGlobalMap::RegisterAutoClass(const std::string& elementId, const std::string& className) {
        autoClassMap[elementId] = className;
    }

    void CHTLJSGlobalMap::RegisterAutoId(const std::string& elementId, const std::string& idName) {
        autoIdMap[elementId] = idName;
    }

    std::string CHTLJSGlobalMap::GetAutoClass(const std::string& elementId) const {
        auto it = autoClassMap.find(elementId);
        return (it != autoClassMap.end()) ? it->second : "";
    }

    std::string CHTLJSGlobalMap::GetAutoId(const std::string& elementId) const {
        auto it = autoIdMap.find(elementId);
        return (it != autoIdMap.end()) ? it->second : "";
    }

    // 工具方法实现
    void CHTLJSGlobalMap::Clear() {
        moduleLoadMap.clear();
        loadedModules.clear();
        selectorMap.clear();
        selectorTypeMap.clear();
        listenerMap.clear();
        delegateMap.clear();
        animationMap.clear();
        virtualObjectMap.clear();
        chtljsFunctionMap.clear();
        utilThenMap.clear();
        autoClassMap.clear();
        autoIdMap.clear();
    }

    void CHTLJSGlobalMap::Reset() {
        Clear();
    }

    std::vector<std::string> CHTLJSGlobalMap::GetAllSelectors() const {
        std::vector<std::string> selectors;
        for (const auto& pair : selectorMap) {
            selectors.push_back(pair.first);
        }
        return selectors;
    }

    std::vector<std::string> CHTLJSGlobalMap::GetAllVirtualObjects() const {
        std::vector<std::string> objects;
        for (const auto& pair : virtualObjectMap) {
            objects.push_back(pair.first);
        }
        return objects;
    }

    void CHTLJSGlobalMap::PrintMappings() const {
        std::cout << "=== CHTL JS全局映射表状态 ===" << std::endl;
        
        std::cout << "模块数量: " << moduleLoadMap.size() << std::endl;
        std::cout << "已加载模块数量: " << loadedModules.size() << std::endl;
        std::cout << "选择器数量: " << selectorMap.size() << std::endl;
        std::cout << "事件监听器数量: " << listenerMap.size() << std::endl;
        std::cout << "事件委托数量: " << delegateMap.size() << std::endl;
        std::cout << "动画数量: " << animationMap.size() << std::endl;
        std::cout << "虚对象数量: " << virtualObjectMap.size() << std::endl;
        std::cout << "CHTL JS函数数量: " << chtljsFunctionMap.size() << std::endl;
        std::cout << "util表达式数量: " << utilThenMap.size() << std::endl;
        
        std::cout << "=============================" << std::endl;
    }

} // namespace CHTLJS