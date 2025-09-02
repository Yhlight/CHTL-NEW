#include "CHTL/Core/GlobalMap.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"
#include <algorithm>

namespace CHTL {

void GlobalMap::Clear() {
    templateStyles.clear();
    templateElements.clear();
    templateVars.clear();
    customStyles.clear();
    customElements.clear();
    customVars.clear();
    originBlocks.clear();
    namespaces.clear();
    configurations.clear();
    defaultConfiguration.reset();
    namespaceStack.clear();
}

std::string GlobalMap::GetFullNamespacePath(const std::string& name) const {
    if (namespaceStack.empty()) {
        return name;
    }
    
    std::string fullPath;
    for (const auto& ns : namespaceStack) {
        if (!fullPath.empty()) {
            fullPath += ".";
        }
        fullPath += ns;
    }
    
    if (!fullPath.empty()) {
        fullPath += ".";
    }
    fullPath += name;
    
    return fullPath;
}

void GlobalMap::AddTemplateStyle(const std::string& name, std::shared_ptr<TemplateStyleNode> node) {
    templateStyles[GetFullNamespacePath(name)] = node;
}

void GlobalMap::AddTemplateElement(const std::string& name, std::shared_ptr<TemplateElementNode> node) {
    templateElements[GetFullNamespacePath(name)] = node;
}

void GlobalMap::AddTemplateVar(const std::string& name, std::shared_ptr<TemplateVarNode> node) {
    std::string fullPath = GetFullNamespacePath(name);
    LOG_DEBUG("添加变量组模板: " + name + " (完整路径: " + fullPath + ")");
    templateVars[fullPath] = node;
}

std::shared_ptr<TemplateStyleNode> GlobalMap::GetTemplateStyle(const std::string& name) const {
    auto it = templateStyles.find(GetFullNamespacePath(name));
    return it != templateStyles.end() ? it->second : nullptr;
}

std::shared_ptr<TemplateElementNode> GlobalMap::GetTemplateElement(const std::string& name) const {
    auto it = templateElements.find(GetFullNamespacePath(name));
    return it != templateElements.end() ? it->second : nullptr;
}

std::shared_ptr<TemplateVarNode> GlobalMap::GetTemplateVar(const std::string& name) const {
    std::string fullPath = GetFullNamespacePath(name);
    LOG_DEBUG("查找变量组模板: " + name + " (完整路径: " + fullPath + ")");
    auto it = templateVars.find(fullPath);
    if (it != templateVars.end()) {
        LOG_DEBUG("找到变量组模板: " + name);
        return it->second;
    }
    LOG_DEBUG("未找到变量组模板: " + name);
    return nullptr;
}

void GlobalMap::AddCustomStyle(const std::string& name, std::shared_ptr<CustomStyleNode> node) {
    customStyles[GetFullNamespacePath(name)] = node;
}

void GlobalMap::AddCustomElement(const std::string& name, std::shared_ptr<CustomElementNode> node) {
    customElements[GetFullNamespacePath(name)] = node;
}

void GlobalMap::AddCustomVar(const std::string& name, std::shared_ptr<CustomVarNode> node) {
    customVars[GetFullNamespacePath(name)] = node;
}

std::shared_ptr<CustomStyleNode> GlobalMap::GetCustomStyle(const std::string& name) const {
    auto it = customStyles.find(GetFullNamespacePath(name));
    return it != customStyles.end() ? it->second : nullptr;
}

std::shared_ptr<CustomElementNode> GlobalMap::GetCustomElement(const std::string& name) const {
    auto it = customElements.find(GetFullNamespacePath(name));
    return it != customElements.end() ? it->second : nullptr;
}

std::shared_ptr<CustomVarNode> GlobalMap::GetCustomVar(const std::string& name) const {
    auto it = customVars.find(GetFullNamespacePath(name));
    return it != customVars.end() ? it->second : nullptr;
}

void GlobalMap::AddOriginBlock(const std::string& key, std::shared_ptr<OriginNode> node) {
    originBlocks[key] = node;
}

std::shared_ptr<OriginNode> GlobalMap::GetOriginBlock(const std::string& key) const {
    auto it = originBlocks.find(key);
    return it != originBlocks.end() ? it->second : nullptr;
}

void GlobalMap::AddNamespace(const std::string& name, std::shared_ptr<NamespaceNode> node) {
    namespaces[name] = node;
}

std::shared_ptr<NamespaceNode> GlobalMap::GetNamespace(const std::string& name) const {
    auto it = namespaces.find(name);
    return it != namespaces.end() ? it->second : nullptr;
}

void GlobalMap::EnterNamespace(const std::string& name) {
    namespaceStack.push_back(name);
}

void GlobalMap::ExitNamespace() {
    if (!namespaceStack.empty()) {
        namespaceStack.pop_back();
    }
}

std::string GlobalMap::GetCurrentNamespace() const {
    if (namespaceStack.empty()) {
        return "";
    }
    return namespaceStack.back();
}

void GlobalMap::AddConfiguration(const std::string& name, std::shared_ptr<ConfigurationNode> node) {
    configurations[name] = node;
}

std::shared_ptr<ConfigurationNode> GlobalMap::GetConfiguration(const std::string& name) const {
    auto it = configurations.find(name);
    return it != configurations.end() ? it->second : nullptr;
}

void GlobalMap::SetDefaultConfiguration(std::shared_ptr<ConfigurationNode> node) {
    defaultConfiguration = node;
}

std::shared_ptr<ConfigurationNode> GlobalMap::GetDefaultConfiguration() const {
    return defaultConfiguration;
}

bool GlobalMap::HasTemplateStyle(const std::string& name) const {
    return templateStyles.find(GetFullNamespacePath(name)) != templateStyles.end();
}

bool GlobalMap::HasTemplateElement(const std::string& name) const {
    return templateElements.find(GetFullNamespacePath(name)) != templateElements.end();
}

bool GlobalMap::HasTemplateVar(const std::string& name) const {
    return templateVars.find(GetFullNamespacePath(name)) != templateVars.end();
}

bool GlobalMap::HasCustomStyle(const std::string& name) const {
    return customStyles.find(GetFullNamespacePath(name)) != customStyles.end();
}

bool GlobalMap::HasCustomElement(const std::string& name) const {
    return customElements.find(GetFullNamespacePath(name)) != customElements.end();
}

bool GlobalMap::HasCustomVar(const std::string& name) const {
    return customVars.find(GetFullNamespacePath(name)) != customVars.end();
}

bool GlobalMap::HasOriginBlock(const std::string& key) const {
    return originBlocks.find(key) != originBlocks.end();
}

bool GlobalMap::HasNamespace(const std::string& name) const {
    return namespaces.find(name) != namespaces.end();
}

bool GlobalMap::HasConfiguration(const std::string& name) const {
    return configurations.find(name) != configurations.end();
}

void GlobalMap::Merge(const GlobalMap& other, const std::string& namespacePrefix) {
    // TODO: 实现合并逻辑
}

} // namespace CHTL