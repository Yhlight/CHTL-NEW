#include "CHTLJS/Core/GlobalMap.h"
#include "CHTLJS/AST/Nodes.h"
#include <algorithm>

namespace CHTL {
namespace JS {

void GlobalMap::Clear() {
    modules.clear();
    virtualObjects.clear();
    animations.clear();
    listeners.clear();
    delegates.clear();
    loadedModules.clear();
    moduleLoadOrder.clear();
}

void GlobalMap::AddModule(const std::string& path, std::shared_ptr<ModuleNode> node) {
    modules[path] = node;
    moduleLoadOrder.push_back(path);
}

bool GlobalMap::HasModule(const std::string& path) const {
    return modules.find(path) != modules.end();
}

void GlobalMap::AddLoadedModule(const std::string& path) {
    loadedModules.push_back(path);
}

bool GlobalMap::IsModuleLoaded(const std::string& path) const {
    return std::find(loadedModules.begin(), loadedModules.end(), path) != loadedModules.end();
}

std::shared_ptr<ModuleNode> GlobalMap::GetModule(const std::string& path) const {
    auto it = modules.find(path);
    return it != modules.end() ? it->second : nullptr;
}

void GlobalMap::AddVirtualObject(const std::string& name, std::shared_ptr<VirtualObjectNode> node) {
    virtualObjects[name] = node;
}

std::shared_ptr<VirtualObjectNode> GlobalMap::GetVirtualObject(const std::string& name) const {
    auto it = virtualObjects.find(name);
    return it != virtualObjects.end() ? it->second : nullptr;
}

bool GlobalMap::HasVirtualObject(const std::string& name) const {
    return virtualObjects.find(name) != virtualObjects.end();
}

void GlobalMap::AddAnimation(const std::string& name, std::shared_ptr<AnimationNode> node) {
    animations[name] = node;
}

std::shared_ptr<AnimationNode> GlobalMap::GetAnimation(const std::string& name) const {
    auto it = animations.find(name);
    return it != animations.end() ? it->second : nullptr;
}

bool GlobalMap::HasAnimation(const std::string& name) const {
    return animations.find(name) != animations.end();
}

void GlobalMap::AddListener(const std::string& key, std::shared_ptr<ListenerNode> node) {
    listeners[key] = node;
}

std::shared_ptr<ListenerNode> GlobalMap::GetListener(const std::string& key) const {
    auto it = listeners.find(key);
    return it != listeners.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<ListenerNode>> GlobalMap::GetListenersForElement(const std::string& selector) const {
    std::vector<std::shared_ptr<ListenerNode>> result;
    // TODO: 实现根据选择器查找监听器的逻辑
    return result;
}

void GlobalMap::AddDelegate(const std::string& key, std::shared_ptr<DelegateNode> node) {
    delegates[key] = node;
}

std::shared_ptr<DelegateNode> GlobalMap::GetDelegate(const std::string& key) const {
    auto it = delegates.find(key);
    return it != delegates.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<DelegateNode>> GlobalMap::GetDelegatesForParent(const std::string& parentSelector) const {
    std::vector<std::shared_ptr<DelegateNode>> result;
    // TODO: 实现根据父选择器查找委托的逻辑
    return result;
}

void GlobalMap::Merge(const GlobalMap& other) {
    // TODO: 实现合并逻辑
}

} // namespace JS
} // namespace CHTL