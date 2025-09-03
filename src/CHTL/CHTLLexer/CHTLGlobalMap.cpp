#include "CHTLGlobalMap.h"
#include "CHTLToken.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

    // 静态成员初始化
    std::shared_ptr<CHTLGlobalMap> CHTLGlobalMap::instance = nullptr;

    std::shared_ptr<CHTLGlobalMap> CHTLGlobalMap::GetInstance() {
        if (instance == nullptr) {
            instance = std::shared_ptr<CHTLGlobalMap>(new CHTLGlobalMap());
        }
        return instance;
    }

    // 模板管理实现
    void CHTLGlobalMap::RegisterTemplateStyle(const std::string& name, const std::string& content) {
        templateStyleMap[name] = content;
    }

    void CHTLGlobalMap::RegisterTemplateElement(const std::string& name, const std::string& content) {
        templateElementMap[name] = content;
    }

    void CHTLGlobalMap::RegisterTemplateVar(const std::string& name, const std::string& content) {
        templateVarMap[name] = content;
    }

    std::string CHTLGlobalMap::GetTemplateStyle(const std::string& name) const {
        auto it = templateStyleMap.find(name);
        return (it != templateStyleMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetTemplateElement(const std::string& name) const {
        auto it = templateElementMap.find(name);
        return (it != templateElementMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetTemplateVar(const std::string& name) const {
        auto it = templateVarMap.find(name);
        return (it != templateVarMap.end()) ? it->second : "";
    }

    bool CHTLGlobalMap::HasTemplateStyle(const std::string& name) const {
        return templateStyleMap.find(name) != templateStyleMap.end();
    }

    bool CHTLGlobalMap::HasTemplateElement(const std::string& name) const {
        return templateElementMap.find(name) != templateElementMap.end();
    }

    bool CHTLGlobalMap::HasTemplateVar(const std::string& name) const {
        return templateVarMap.find(name) != templateVarMap.end();
    }

    // 自定义管理实现
    void CHTLGlobalMap::RegisterCustomStyle(const std::string& name, const std::string& content) {
        customStyleMap[name] = content;
    }

    void CHTLGlobalMap::RegisterCustomElement(const std::string& name, const std::string& content) {
        customElementMap[name] = content;
    }

    void CHTLGlobalMap::RegisterCustomVar(const std::string& name, const std::string& content) {
        customVarMap[name] = content;
    }

    std::string CHTLGlobalMap::GetCustomStyle(const std::string& name) const {
        auto it = customStyleMap.find(name);
        return (it != customStyleMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetCustomElement(const std::string& name) const {
        auto it = customElementMap.find(name);
        return (it != customElementMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetCustomVar(const std::string& name) const {
        auto it = customVarMap.find(name);
        return (it != customVarMap.end()) ? it->second : "";
    }

    bool CHTLGlobalMap::HasCustomStyle(const std::string& name) const {
        return customStyleMap.find(name) != customStyleMap.end();
    }

    bool CHTLGlobalMap::HasCustomElement(const std::string& name) const {
        return customElementMap.find(name) != customElementMap.end();
    }

    bool CHTLGlobalMap::HasCustomVar(const std::string& name) const {
        return customVarMap.find(name) != customVarMap.end();
    }

    // 原始嵌入管理实现
    void CHTLGlobalMap::RegisterOriginHtml(const std::string& name, const std::string& content) {
        originHtmlMap[name] = content;
    }

    void CHTLGlobalMap::RegisterOriginStyle(const std::string& name, const std::string& content) {
        originStyleMap[name] = content;
    }

    void CHTLGlobalMap::RegisterOriginJS(const std::string& name, const std::string& content) {
        originJSMap[name] = content;
    }

    void CHTLGlobalMap::RegisterCustomOrigin(const std::string& type, const std::string& name, const std::string& content) {
        std::string key = type + "::" + name;
        customOriginMap[key] = content;
    }

    std::string CHTLGlobalMap::GetOriginHtml(const std::string& name) const {
        auto it = originHtmlMap.find(name);
        return (it != originHtmlMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetOriginStyle(const std::string& name) const {
        auto it = originStyleMap.find(name);
        return (it != originStyleMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetOriginJS(const std::string& name) const {
        auto it = originJSMap.find(name);
        return (it != originJSMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetCustomOrigin(const std::string& type, const std::string& name) const {
        std::string key = type + "::" + name;
        auto it = customOriginMap.find(key);
        return (it != customOriginMap.end()) ? it->second : "";
    }

    // 命名空间管理实现
    void CHTLGlobalMap::RegisterNamespace(const std::string& name) {
        namespaceMap[name] = std::unordered_set<std::string>();
    }

    void CHTLGlobalMap::AddToNamespace(const std::string& namespaceName, const std::string& itemName) {
        namespaceMap[namespaceName].insert(itemName);
    }

    void CHTLGlobalMap::SetNamespaceHierarchy(const std::string& child, const std::string& parent) {
        namespaceHierarchy[child] = parent;
    }

    bool CHTLGlobalMap::HasNamespace(const std::string& name) const {
        return namespaceMap.find(name) != namespaceMap.end();
    }

    std::unordered_set<std::string> CHTLGlobalMap::GetNamespaceContent(const std::string& name) const {
        auto it = namespaceMap.find(name);
        return (it != namespaceMap.end()) ? it->second : std::unordered_set<std::string>();
    }

    std::string CHTLGlobalMap::GetFullNamespacePath(const std::string& name) const {
        std::string fullPath = name;
        std::string current = name;
        
        while (namespaceHierarchy.find(current) != namespaceHierarchy.end()) {
            current = namespaceHierarchy.at(current);
            fullPath = current + "." + fullPath;
        }
        
        return fullPath;
    }

    // 导入管理实现
    void CHTLGlobalMap::RegisterImport(const std::string& path, const std::string& alias) {
        importPathMap[path] = alias;
        if (!alias.empty()) {
            importAliasMap[alias] = path;
        }
    }

    void CHTLGlobalMap::RegisterImportAlias(const std::string& alias, const std::string& originalName) {
        importAliasMap[alias] = originalName;
    }

    std::string CHTLGlobalMap::GetImportPath(const std::string& alias) const {
        auto it = importAliasMap.find(alias);
        return (it != importAliasMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetImportAlias(const std::string& originalName) const {
        auto it = importPathMap.find(originalName);
        return (it != importPathMap.end()) ? it->second : "";
    }

    bool CHTLGlobalMap::HasImport(const std::string& path) const {
        return importPathMap.find(path) != importPathMap.end();
    }

    // 配置管理实现
    void CHTLGlobalMap::SetConfiguration(const std::string& key, const std::string& value) {
        configurationMap[key] = value;
    }

    void CHTLGlobalMap::SetNamedConfiguration(const std::string& configName, const std::string& key, const std::string& value) {
        namedConfigMap[configName][key] = value;
    }

    std::string CHTLGlobalMap::GetConfiguration(const std::string& key) const {
        auto it = configurationMap.find(key);
        return (it != configurationMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetNamedConfiguration(const std::string& configName, const std::string& key) const {
        auto configIt = namedConfigMap.find(configName);
        if (configIt != namedConfigMap.end()) {
            auto keyIt = configIt->second.find(key);
            if (keyIt != configIt->second.end()) {
                return keyIt->second;
            }
        }
        return "";
    }

    bool CHTLGlobalMap::HasConfiguration(const std::string& key) const {
        return configurationMap.find(key) != configurationMap.end();
    }

    // 选择器自动化管理实现
    void CHTLGlobalMap::RegisterAutoClass(const std::string& elementId, const std::string& className) {
        autoClassMap[elementId] = className;
    }

    void CHTLGlobalMap::RegisterAutoId(const std::string& elementId, const std::string& idName) {
        autoIdMap[elementId] = idName;
    }

    std::string CHTLGlobalMap::GetAutoClass(const std::string& elementId) const {
        auto it = autoClassMap.find(elementId);
        return (it != autoClassMap.end()) ? it->second : "";
    }

    std::string CHTLGlobalMap::GetAutoId(const std::string& elementId) const {
        auto it = autoIdMap.find(elementId);
        return (it != autoIdMap.end()) ? it->second : "";
    }

    // 约束管理实现
    void CHTLGlobalMap::AddConstraint(const std::string& scope, const std::string& constraint) {
        constraintMap[scope].insert(constraint);
    }

    bool CHTLGlobalMap::IsConstrained(const std::string& scope, const std::string& item) const {
        auto it = constraintMap.find(scope);
        if (it != constraintMap.end()) {
            return it->second.find(item) != it->second.end();
        }
        return false;
    }

    std::unordered_set<std::string> CHTLGlobalMap::GetConstraints(const std::string& scope) const {
        auto it = constraintMap.find(scope);
        return (it != constraintMap.end()) ? it->second : std::unordered_set<std::string>();
    }

    // 工具方法实现
    void CHTLGlobalMap::Clear() {
        templateStyleMap.clear();
        templateElementMap.clear();
        templateVarMap.clear();
        customStyleMap.clear();
        customElementMap.clear();
        customVarMap.clear();
        originHtmlMap.clear();
        originStyleMap.clear();
        originJSMap.clear();
        customOriginMap.clear();
        namespaceMap.clear();
        namespaceHierarchy.clear();
        importPathMap.clear();
        importAliasMap.clear();
        configurationMap.clear();
        namedConfigMap.clear();
        autoClassMap.clear();
        autoIdMap.clear();
        constraintMap.clear();
    }

    void CHTLGlobalMap::Reset() {
        Clear();
    }

    std::vector<std::string> CHTLGlobalMap::GetAllTemplateNames() const {
        std::vector<std::string> names;
        for (const auto& pair : templateStyleMap) {
            names.push_back("@Style " + pair.first);
        }
        for (const auto& pair : templateElementMap) {
            names.push_back("@Element " + pair.first);
        }
        for (const auto& pair : templateVarMap) {
            names.push_back("@Var " + pair.first);
        }
        return names;
    }

    std::vector<std::string> CHTLGlobalMap::GetAllCustomNames() const {
        std::vector<std::string> names;
        for (const auto& pair : customStyleMap) {
            names.push_back("@Style " + pair.first);
        }
        for (const auto& pair : customElementMap) {
            names.push_back("@Element " + pair.first);
        }
        for (const auto& pair : customVarMap) {
            names.push_back("@Var " + pair.first);
        }
        return names;
    }

    std::vector<std::string> CHTLGlobalMap::GetAllNamespaces() const {
        std::vector<std::string> names;
        for (const auto& pair : namespaceMap) {
            names.push_back(pair.first);
        }
        return names;
    }

    void CHTLGlobalMap::PrintMappings() const {
        std::cout << "=== CHTL全局映射表状态 ===" << std::endl;
        
        std::cout << "模板样式组数量: " << templateStyleMap.size() << std::endl;
        std::cout << "模板元素数量: " << templateElementMap.size() << std::endl;
        std::cout << "模板变量组数量: " << templateVarMap.size() << std::endl;
        
        std::cout << "自定义样式组数量: " << customStyleMap.size() << std::endl;
        std::cout << "自定义元素数量: " << customElementMap.size() << std::endl;
        std::cout << "自定义变量组数量: " << customVarMap.size() << std::endl;
        
        std::cout << "命名空间数量: " << namespaceMap.size() << std::endl;
        std::cout << "导入映射数量: " << importPathMap.size() << std::endl;
        std::cout << "配置项数量: " << configurationMap.size() << std::endl;
        
        std::cout << "=========================" << std::endl;
    }

} // namespace CHTL