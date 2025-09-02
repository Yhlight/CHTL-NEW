#ifndef CHTL_CHTLJS_CORE_GLOBALMAP_H
#define CHTL_CHTLJS_CORE_GLOBALMAP_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace CHTL {
namespace JS {

// 前向声明
class ModuleNode;
class VirtualObjectNode;
class AnimationNode;
class ListenerNode;
class DelegateNode;

/**
 * CHTL JS全局映射表
 * 管理模块、虚对象等全局定义
 */
class GlobalMap {
private:
    // 模块映射
    std::unordered_map<std::string, std::shared_ptr<ModuleNode>> modules;
    
    // 虚对象映射
    std::unordered_map<std::string, std::shared_ptr<VirtualObjectNode>> virtualObjects;
    
    // 动画定义映射
    std::unordered_map<std::string, std::shared_ptr<AnimationNode>> animations;
    
    // 事件监听器映射
    std::unordered_map<std::string, std::shared_ptr<ListenerNode>> listeners;
    
    // 事件委托映射
    std::unordered_map<std::string, std::shared_ptr<DelegateNode>> delegates;
    
    // 已加载的模块路径
    std::vector<std::string> loadedModules;
    
    // 模块加载顺序
    std::vector<std::string> moduleLoadOrder;
    
public:
    GlobalMap() = default;
    ~GlobalMap() = default;
    
    // 模块操作
    void AddModule(const std::string& path, std::shared_ptr<ModuleNode> node);
    std::shared_ptr<ModuleNode> GetModule(const std::string& path) const;
    bool HasModule(const std::string& path) const;
    void AddLoadedModule(const std::string& path);
    bool IsModuleLoaded(const std::string& path) const;
    const std::vector<std::string>& GetModuleLoadOrder() const { return moduleLoadOrder; }
    
    // 虚对象操作
    void AddVirtualObject(const std::string& name, std::shared_ptr<VirtualObjectNode> node);
    std::shared_ptr<VirtualObjectNode> GetVirtualObject(const std::string& name) const;
    bool HasVirtualObject(const std::string& name) const;
    
    // 动画操作
    void AddAnimation(const std::string& name, std::shared_ptr<AnimationNode> node);
    std::shared_ptr<AnimationNode> GetAnimation(const std::string& name) const;
    bool HasAnimation(const std::string& name) const;
    
    // 事件监听器操作
    void AddListener(const std::string& key, std::shared_ptr<ListenerNode> node);
    std::shared_ptr<ListenerNode> GetListener(const std::string& key) const;
    std::vector<std::shared_ptr<ListenerNode>> GetListenersForElement(const std::string& selector) const;
    
    // 事件委托操作
    void AddDelegate(const std::string& key, std::shared_ptr<DelegateNode> node);
    std::shared_ptr<DelegateNode> GetDelegate(const std::string& key) const;
    std::vector<std::shared_ptr<DelegateNode>> GetDelegatesForParent(const std::string& parentSelector) const;
    
    // 清空所有映射
    void Clear();
    
    // 合并另一个GlobalMap
    void Merge(const GlobalMap& other);
};

} // namespace JS
} // namespace CHTL

#endif // CHTL_CHTLJS_CORE_GLOBALMAP_H