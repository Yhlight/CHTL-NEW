#include "CHTLGlobalMap.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

CHTLGlobalMap::CHTLGlobalMap() : m_ActiveConfiguration(nullptr) {
    // 初始化默认配置
    auto defaultConfig = std::make_unique<ConfigurationDefinition>("", "");
    
    // 设置默认配置值
    defaultConfig->Settings["INDEX_INITIAL_COUNT"] = "0";
    defaultConfig->Settings["DISABLE_NAME_GROUP"] = "true";
    defaultConfig->Settings["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    defaultConfig->Settings["DEBUG_MODE"] = "false";
    defaultConfig->Settings["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    defaultConfig->Settings["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    defaultConfig->Settings["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    defaultConfig->Settings["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
    defaultConfig->Settings["DISABLE_DEFAULT_NAMESPACE"] = "false";
    
    // 设置默认Name映射
    defaultConfig->NameMappings["CUSTOM_STYLE"] = {"@Style", "@style", "@CSS", "@Css", "@css"};
    defaultConfig->NameMappings["CUSTOM_ELEMENT"] = {"@Element"};
    defaultConfig->NameMappings["CUSTOM_VAR"] = {"@Var"};
    defaultConfig->NameMappings["TEMPLATE_STYLE"] = {"@Style"};
    defaultConfig->NameMappings["TEMPLATE_ELEMENT"] = {"@Element"};
    defaultConfig->NameMappings["TEMPLATE_VAR"] = {"@Var"};
    defaultConfig->NameMappings["ORIGIN_HTML"] = {"@Html"};
    defaultConfig->NameMappings["ORIGIN_STYLE"] = {"@Style"};
    defaultConfig->NameMappings["ORIGIN_JAVASCRIPT"] = {"@JavaScript"};
    defaultConfig->NameMappings["IMPORT_HTML"] = {"@Html"};
    defaultConfig->NameMappings["IMPORT_STYLE"] = {"@Style"};
    defaultConfig->NameMappings["IMPORT_JAVASCRIPT"] = {"@JavaScript"};
    defaultConfig->NameMappings["IMPORT_CHTL"] = {"@Chtl"};
    defaultConfig->NameMappings["IMPORT_CRMOD"] = {"@CJmod"};
    defaultConfig->NameMappings["KEYWORD_INHERIT"] = {"inherit"};
    defaultConfig->NameMappings["KEYWORD_DELETE"] = {"delete"};
    defaultConfig->NameMappings["KEYWORD_INSERT"] = {"insert"};
    defaultConfig->NameMappings["KEYWORD_AFTER"] = {"after"};
    defaultConfig->NameMappings["KEYWORD_BEFORE"] = {"before"};
    defaultConfig->NameMappings["KEYWORD_REPLACE"] = {"replace"};
    defaultConfig->NameMappings["KEYWORD_ATTOP"] = {"at top"};
    defaultConfig->NameMappings["KEYWORD_ATBOTTOM"] = {"at bottom"};
    defaultConfig->NameMappings["KEYWORD_FROM"] = {"from"};
    defaultConfig->NameMappings["KEYWORD_AS"] = {"as"};
    defaultConfig->NameMappings["KEYWORD_EXCEPT"] = {"except"};
    defaultConfig->NameMappings["KEYWORD_USE"] = {"use"};
    defaultConfig->NameMappings["KEYWORD_HTML5"] = {"html5"};
    defaultConfig->NameMappings["KEYWORD_TEXT"] = {"text"};
    defaultConfig->NameMappings["KEYWORD_STYLE"] = {"style"};
    defaultConfig->NameMappings["KEYWORD_SCRIPT"] = {"script"};
    defaultConfig->NameMappings["KEYWORD_CUSTOM"] = {"[Custom]"};
    defaultConfig->NameMappings["KEYWORD_TEMPLATE"] = {"[Template]"};
    defaultConfig->NameMappings["KEYWORD_ORIGIN"] = {"[Origin]"};
    defaultConfig->NameMappings["KEYWORD_IMPORT"] = {"[Import]"};
    defaultConfig->NameMappings["KEYWORD_NAMESPACE"] = {"[Namespace]"};
    
    m_ActiveConfiguration = defaultConfig.get();
    m_Configurations[""] = std::move(defaultConfig);
}

bool CHTLGlobalMap::RegisterTemplate(std::unique_ptr<TemplateDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    std::string qualifiedName = BuildQualifiedName(definition->Name, definition->Namespace);
    
    // 检查是否已存在
    if (m_Templates.find(qualifiedName) != m_Templates.end()) {
        return false; // 模板已存在
    }
    
    // 应用继承
    ApplyTemplateInheritance(definition.get());
    
    m_Templates[qualifiedName] = std::move(definition);
    return true;
}

TemplateDefinition* CHTLGlobalMap::GetTemplate(const std::string& name, 
                                              CHTLTemplateType type, 
                                              const std::string& ns) {
    std::string qualifiedName = BuildQualifiedName(name, ns);
    
    auto it = m_Templates.find(qualifiedName);
    if (it != m_Templates.end() && it->second->Type == type) {
        return it->second.get();
    }
    
    // 如果指定命名空间没找到，尝试当前命名空间
    if (!ns.empty()) {
        std::string currentNsName = BuildQualifiedName(name, GetCurrentNamespace());
        auto currentIt = m_Templates.find(currentNsName);
        if (currentIt != m_Templates.end() && currentIt->second->Type == type) {
            return currentIt->second.get();
        }
    }
    
    return nullptr;
}

bool CHTLGlobalMap::HasTemplate(const std::string& name, CHTLTemplateType type, const std::string& ns) {
    return GetTemplate(name, type, ns) != nullptr;
}

bool CHTLGlobalMap::RegisterCustom(std::unique_ptr<CustomDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    std::string qualifiedName = BuildQualifiedName(definition->Name, definition->Namespace);
    
    // 检查是否已存在
    if (m_Customs.find(qualifiedName) != m_Customs.end()) {
        return false; // 自定义已存在
    }
    
    // 应用继承
    ApplyCustomInheritance(definition.get());
    
    m_Customs[qualifiedName] = std::move(definition);
    return true;
}

CustomDefinition* CHTLGlobalMap::GetCustom(const std::string& name, 
                                          CHTLCustomType type, 
                                          const std::string& ns) {
    std::string qualifiedName = BuildQualifiedName(name, ns);
    
    auto it = m_Customs.find(qualifiedName);
    if (it != m_Customs.end() && it->second->Type == type) {
        return it->second.get();
    }
    
    // 如果指定命名空间没找到，尝试当前命名空间
    if (!ns.empty()) {
        std::string currentNsName = BuildQualifiedName(name, GetCurrentNamespace());
        auto currentIt = m_Customs.find(currentNsName);
        if (currentIt != m_Customs.end() && currentIt->second->Type == type) {
            return currentIt->second.get();
        }
    }
    
    return nullptr;
}

bool CHTLGlobalMap::RegisterOrigin(std::unique_ptr<OriginDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    std::string qualifiedName = BuildQualifiedName(definition->Name, definition->Namespace);
    
    m_Origins[qualifiedName] = std::move(definition);
    return true;
}

OriginDefinition* CHTLGlobalMap::GetOrigin(const std::string& name, 
                                          CHTLOriginType type, 
                                          const std::string& ns) {
    std::string qualifiedName = BuildQualifiedName(name, ns);
    
    auto it = m_Origins.find(qualifiedName);
    if (it != m_Origins.end() && it->second->Type == type) {
        return it->second.get();
    }
    
    return nullptr;
}

bool CHTLGlobalMap::RegisterConfiguration(std::unique_ptr<ConfigurationDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    std::string qualifiedName = BuildQualifiedName(definition->Name, definition->Namespace);
    m_Configurations[qualifiedName] = std::move(definition);
    
    // 如果是无名配置且当前没有活动配置，设为活动配置
    if (definition->Name.empty() && !m_ActiveConfiguration) {
        m_ActiveConfiguration = m_Configurations[qualifiedName].get();
    }
    
    return true;
}

void CHTLGlobalMap::SetActiveConfiguration(const std::string& name) {
    std::string qualifiedName = BuildQualifiedName(name, GetCurrentNamespace());
    
    auto it = m_Configurations.find(qualifiedName);
    if (it != m_Configurations.end()) {
        m_ActiveConfiguration = it->second.get();
    }
}

ConfigurationDefinition* CHTLGlobalMap::GetActiveConfiguration() {
    return m_ActiveConfiguration;
}

std::string CHTLGlobalMap::GetConfigurationValue(const std::string& key, const std::string& defaultValue) {
    if (m_ActiveConfiguration) {
        auto it = m_ActiveConfiguration->Settings.find(key);
        if (it != m_ActiveConfiguration->Settings.end()) {
            return it->second;
        }
    }
    
    return defaultValue;
}

bool CHTLGlobalMap::RegisterNamespace(std::unique_ptr<NamespaceDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    std::string qualifiedName = BuildQualifiedName(definition->Name, definition->ParentNamespace);
    
    // 检查是否已存在同名命名空间，如果存在则合并
    auto it = m_Namespaces.find(qualifiedName);
    if (it != m_Namespaces.end()) {
        MergeNamespaces(qualifiedName);
        return true;
    }
    
    // 检测命名空间冲突
    if (DetectNamespaceConflict(definition->Name)) {
        return false;
    }
    
    m_Namespaces[qualifiedName] = std::move(definition);
    return true;
}

void CHTLGlobalMap::EnterNamespace(const std::string& name) {
    m_NamespaceStack.push_back(name);
}

void CHTLGlobalMap::ExitNamespace() {
    if (!m_NamespaceStack.empty()) {
        m_NamespaceStack.pop_back();
    }
}

std::string CHTLGlobalMap::GetCurrentNamespace() {
    if (m_NamespaceStack.empty()) {
        return "";
    }
    return m_NamespaceStack.back();
}

std::string CHTLGlobalMap::GetFullNamespacePath() const {
    if (m_NamespaceStack.empty()) {
        return "";
    }
    
    std::string path;
    for (size_t i = 0; i < m_NamespaceStack.size(); ++i) {
        if (i > 0) {
            path += ".";
        }
        path += m_NamespaceStack[i];
    }
    
    return path;
}

bool CHTLGlobalMap::RegisterImport(std::unique_ptr<ImportDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    // 检查循环依赖
    for (const auto& existingImport : m_Imports) {
        if (existingImport->FilePath == definition->FilePath && 
            existingImport->Type == definition->Type &&
            existingImport->Target == definition->Target) {
            return false; // 重复导入
        }
    }
    
    m_Imports.push_back(std::move(definition));
    return true;
}

const std::vector<std::unique_ptr<ImportDefinition>>& CHTLGlobalMap::GetImports() const {
    return m_Imports;
}

std::string CHTLGlobalMap::GenerateAutoClassName(const std::string& baseName) {
    auto& counter = m_AutoClassCounters[baseName];
    
    if (counter == 0) {
        counter++;
        return baseName; // 第一次使用原名
    }
    
    std::string generatedName = baseName + std::to_string(counter);
    counter++;
    return generatedName;
}

std::string CHTLGlobalMap::GenerateAutoIdName(const std::string& baseName) {
    auto& counter = m_AutoIdCounters[baseName];
    
    if (counter == 0) {
        counter++;
        return baseName; // 第一次使用原名
    }
    
    std::string generatedName = baseName + std::to_string(counter);
    counter++;
    return generatedName;
}

bool CHTLGlobalMap::IsAutoGeneratedClass(const std::string& className) {
    return m_AutoClassCounters.find(className) != m_AutoClassCounters.end();
}

bool CHTLGlobalMap::IsAutoGeneratedId(const std::string& idName) {
    return m_AutoIdCounters.find(idName) != m_AutoIdCounters.end();
}

std::string CHTLGlobalMap::ResolveVariableReference(const std::string& varRef) {
    // 解析变量引用格式：ThemeColor(tableColor) 或 ThemeColor(tableColor = newValue)
    std::regex varPattern(R"((\w+)\((\w+)(?:\s*=\s*([^)]+))?\))");
    std::smatch match;
    
    if (std::regex_match(varRef, match, varPattern)) {
        std::string varGroupName = match[1].str();
        std::string varName = match[2].str();
        std::string newValue = match[3].str();
        
        // 查找变量组定义
        auto varTemplate = GetTemplate(varGroupName, CHTLTemplateType::VAR_TEMPLATE);
        if (varTemplate) {
            auto propIt = varTemplate->Properties.find(varName);
            if (propIt != varTemplate->Properties.end()) {
                return newValue.empty() ? propIt->second : newValue;
            }
        }
        
        auto varCustom = GetCustom(varGroupName, CHTLCustomType::VAR_CUSTOM);
        if (varCustom) {
            auto propIt = varCustom->Properties.find(varName);
            if (propIt != varCustom->Properties.end()) {
                return newValue.empty() ? propIt->second : newValue;
            }
        }
    }
    
    return varRef; // 无法解析，返回原值
}

bool CHTLGlobalMap::CheckConstraints(const std::string& elementType, const std::string& context) {
    // 检查当前命名空间的约束
    std::string currentNs = GetCurrentNamespace();
    if (!currentNs.empty()) {
        auto nsIt = m_Namespaces.find(currentNs);
        if (nsIt != m_Namespaces.end()) {
            for (const auto& constraint : nsIt->second->Constraints) {
                if (constraint == elementType || 
                    constraint == "[Template]" || 
                    constraint == "[Custom]" ||
                    constraint == "@Html") {
                    return false; // 违反约束
                }
            }
        }
    }
    
    return true; // 通过约束检查
}

std::vector<std::string> CHTLGlobalMap::GetInheritanceChain(const std::string& name, const std::string& type) {
    std::vector<std::string> chain;
    
    if (type == "template") {
        // 处理模板继承链
        auto it = m_Templates.find(name);
        if (it != m_Templates.end()) {
            for (const auto& inherit : it->second->Inherits) {
                chain.push_back(inherit);
                auto subChain = GetInheritanceChain(inherit, type);
                chain.insert(chain.end(), subChain.begin(), subChain.end());
            }
        }
    }
    else if (type == "custom") {
        // 处理自定义继承链
        auto it = m_Customs.find(name);
        if (it != m_Customs.end()) {
            for (const auto& inherit : it->second->Inherits) {
                chain.push_back(inherit);
                auto subChain = GetInheritanceChain(inherit, type);
                chain.insert(chain.end(), subChain.begin(), subChain.end());
            }
        }
    }
    
    return chain;
}

void CHTLGlobalMap::Clear() {
    m_Templates.clear();
    m_Customs.clear();
    m_Origins.clear();
    m_Configurations.clear();
    m_Namespaces.clear();
    m_Imports.clear();
    m_NamespaceStack.clear();
    m_AutoClassCounters.clear();
    m_AutoIdCounters.clear();
    m_ActiveConfiguration = nullptr;
}

std::string CHTLGlobalMap::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL GlobalMap Statistics:\n";
    oss << "Templates: " << m_Templates.size() << "\n";
    oss << "Customs: " << m_Customs.size() << "\n";
    oss << "Origins: " << m_Origins.size() << "\n";
    oss << "Configurations: " << m_Configurations.size() << "\n";
    oss << "Namespaces: " << m_Namespaces.size() << "\n";
    oss << "Imports: " << m_Imports.size() << "\n";
    oss << "Auto Classes: " << m_AutoClassCounters.size() << "\n";
    oss << "Auto IDs: " << m_AutoIdCounters.size() << "\n";
    oss << "Current Namespace: " << GetFullNamespacePath() << "\n";
    return oss.str();
}

std::string CHTLGlobalMap::BuildQualifiedName(const std::string& name, const std::string& ns) {
    if (ns.empty()) {
        return name;
    }
    return ns + "::" + name;
}

std::vector<std::string> CHTLGlobalMap::ParseNamespacePath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

void CHTLGlobalMap::MergeNamespaces(const std::string& qualifiedName) {
    // 同名命名空间自动合并逻辑
    auto it = m_Namespaces.find(qualifiedName);
    if (it != m_Namespaces.end()) {
        // 合并子命名空间和约束
        // 这里可以根据需要实现具体的合并逻辑
    }
}

bool CHTLGlobalMap::DetectNamespaceConflict(const std::string& name) {
    // 命名空间冲突检测策略
    // 检查是否有同名但不同父命名空间的情况
    for (const auto& pair : m_Namespaces) {
        if (pair.second->Name == name && pair.second->ParentNamespace != GetCurrentNamespace()) {
            return true; // 检测到冲突
        }
    }
    
    return false;
}

void CHTLGlobalMap::ApplyTemplateInheritance(TemplateDefinition* definition) {
    if (!definition) {
        return;
    }
    
    // 处理模板继承
    for (const auto& inheritName : definition->Inherits) {
        auto parentTemplate = GetTemplate(inheritName, definition->Type);
        if (parentTemplate) {
            // 继承父模板的属性
            for (const auto& prop : parentTemplate->Properties) {
                // 如果当前模板没有该属性，则继承
                if (definition->Properties.find(prop.first) == definition->Properties.end()) {
                    definition->Properties[prop.first] = prop.second;
                }
            }
        }
    }
}

void CHTLGlobalMap::ApplyCustomInheritance(CustomDefinition* definition) {
    if (!definition) {
        return;
    }
    
    // 处理自定义继承（可以继承模板或其他自定义）
    for (const auto& inheritName : definition->Inherits) {
        // 先尝试查找同类型的自定义
        auto parentCustom = GetCustom(inheritName, definition->Type);
        if (parentCustom) {
            // 继承父自定义的属性
            for (const auto& prop : parentCustom->Properties) {
                if (definition->Properties.find(prop.first) == definition->Properties.end()) {
                    definition->Properties[prop.first] = prop.second;
                }
            }
        }
        else {
            // 尝试查找对应类型的模板
            CHTLTemplateType templateType;
            switch (definition->Type) {
                case CHTLCustomType::STYLE_CUSTOM:
                    templateType = CHTLTemplateType::STYLE_TEMPLATE;
                    break;
                case CHTLCustomType::ELEMENT_CUSTOM:
                    templateType = CHTLTemplateType::ELEMENT_TEMPLATE;
                    break;
                case CHTLCustomType::VAR_CUSTOM:
                    templateType = CHTLTemplateType::VAR_TEMPLATE;
                    break;
            }
            
            auto parentTemplate = GetTemplate(inheritName, templateType);
            if (parentTemplate) {
                // 继承模板的属性
                for (const auto& prop : parentTemplate->Properties) {
                    if (definition->Properties.find(prop.first) == definition->Properties.end()) {
                        definition->Properties[prop.first] = prop.second;
                    }
                }
            }
        }
    }
    
    // 处理删除的属性和继承
    for (const auto& deletedProp : definition->DeletedProperties) {
        definition->Properties.erase(deletedProp);
    }
    
    for (const auto& deletedInherit : definition->DeletedInherits) {
        auto it = std::find(definition->Inherits.begin(), definition->Inherits.end(), deletedInherit);
        if (it != definition->Inherits.end()) {
            definition->Inherits.erase(it);
        }
    }
}

} // namespace CHTL