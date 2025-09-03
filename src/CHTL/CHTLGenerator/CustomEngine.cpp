#include "CustomEngine.h"
#include <iostream>
#include <regex>
#include <algorithm>

namespace CHTL {

CustomEngine::CustomEngine(TemplateEngine* templateEngine) 
    : m_TemplateEngine(templateEngine) {
}

void CustomEngine::RegisterStyleCustom(const std::string& name, 
                                      CustomNode* customNode,
                                      const std::string& namespaceName) {
    if (!customNode || name.empty()) {
        return;
    }
    
    auto customInfo = std::make_unique<CustomInfo>();
    customInfo->Name = name;
    customInfo->Type = TemplateEngineType::STYLE_TEMPLATE;
    customInfo->Namespace = namespaceName.empty() ? m_CurrentNamespace : namespaceName;
    
    // 解析自定义内容
    customInfo->Properties = ParseCustomContent(customNode->GetContent(), TemplateEngineType::STYLE_TEMPLATE)->Properties;
    
    std::string fullName = customInfo->Namespace.empty() ? name : customInfo->Namespace + "::" + name;
    m_StyleCustoms[fullName] = std::move(customInfo);
    
    std::cout << "📝 注册自定义样式组: " << fullName << std::endl;
}

void CustomEngine::RegisterElementCustom(const std::string& name, 
                                        CustomNode* customNode,
                                        const std::string& namespaceName) {
    if (!customNode || name.empty()) {
        return;
    }
    
    auto customInfo = std::make_unique<CustomInfo>();
    customInfo->Name = name;
    customInfo->Type = TemplateEngineType::ELEMENT_TEMPLATE;
    customInfo->Namespace = namespaceName.empty() ? m_CurrentNamespace : namespaceName;
    
    // 克隆自定义元素
    const auto& customElements = customNode->GetChildren();
    for (const auto& element : customElements) {
        if (auto elementNode = dynamic_cast<ElementNode*>(element.get())) {
            // TODO: 实现元素克隆
        }
    }
    
    std::string fullName = customInfo->Namespace.empty() ? name : customInfo->Namespace + "::" + name;
    m_ElementCustoms[fullName] = std::move(customInfo);
    
    std::cout << "📝 注册自定义元素: " << fullName << std::endl;
}

bool CustomEngine::ApplyStyleCustom(const std::string& customName,
                                   ElementNode* targetElement,
                                   const std::unordered_map<std::string, std::string>& valueProperties,
                                   const std::vector<SpecializationInfo>& specializations) {
    if (!targetElement || customName.empty()) {
        return false;
    }
    
    // 解析自定义名称
    auto [namespaceName, simpleName] = ParseNamespacedName(customName);
    std::string fullCustomName = namespaceName.empty() ? simpleName : namespaceName + "::" + simpleName;
    
    // 查找自定义
    auto it = m_StyleCustoms.find(fullCustomName);
    if (it == m_StyleCustoms.end()) {
        std::cerr << "❌ 未找到自定义样式组: " << fullCustomName << std::endl;
        return false;
    }
    
    auto& customInfo = it->second;
    
    // 处理无值样式组
    if (customInfo->HasValuelessProperties) {
        ProcessValuelessStyleGroup(customInfo.get(), valueProperties);
    }
    
    // 处理特例化操作
    if (!specializations.empty()) {
        ProcessSpecializations(customInfo.get(), specializations);
    }
    
    // 应用自定义属性
    for (const auto& [property, value] : customInfo->Properties) {
        targetElement->AddInlineStyle(property, value);
    }
    
    std::cout << "✅ 应用自定义样式组: " << fullCustomName << " → " << targetElement->GetTagName() << std::endl;
    return true;
}

void CustomEngine::ProcessSpecializations(CustomInfo* customInfo, 
                                         const std::vector<SpecializationInfo>& specializations) {
    if (!customInfo) {
        return;
    }
    
    for (const auto& spec : specializations) {
        switch (spec.Operation) {
            case SpecializationOperation::DELETE_PROPERTY:
                ProcessDeleteOperation(customInfo, spec.Target);
                break;
            case SpecializationOperation::DELETE_ELEMENT:
                ProcessDeleteOperation(customInfo, spec.Target);
                break;
            case SpecializationOperation::DELETE_INHERITANCE:
                ProcessDeleteOperation(customInfo, spec.Target);
                break;
            case SpecializationOperation::INSERT_AFTER:
            case SpecializationOperation::INSERT_BEFORE:
            case SpecializationOperation::INSERT_REPLACE:
            case SpecializationOperation::INSERT_AT_TOP:
            case SpecializationOperation::INSERT_AT_BOTTOM:
                ProcessInsertOperation(customInfo, spec);
                break;
            default:
                std::cerr << "⚠️ 未实现的特例化操作" << std::endl;
                break;
        }
    }
}

void CustomEngine::ProcessDeleteOperation(CustomInfo* customInfo, const std::string& deleteTarget) {
    if (!customInfo) {
        return;
    }
    
    // 删除属性
    auto propIt = customInfo->Properties.find(deleteTarget);
    if (propIt != customInfo->Properties.end()) {
        customInfo->Properties.erase(propIt);
        std::cout << "🗑️ 删除属性: " << deleteTarget << std::endl;
        return;
    }
    
    // 删除继承
    auto inheritIt = std::find(customInfo->InheritedTemplates.begin(), 
                              customInfo->InheritedTemplates.end(), 
                              deleteTarget);
    if (inheritIt != customInfo->InheritedTemplates.end()) {
        customInfo->InheritedTemplates.erase(inheritIt);
        std::cout << "🗑️ 删除继承: " << deleteTarget << std::endl;
        return;
    }
    
    std::cout << "⚠️ 删除目标未找到: " << deleteTarget << std::endl;
}

void CustomEngine::ProcessValuelessStyleGroup(CustomInfo* customInfo,
                                             const std::unordered_map<std::string, std::string>& valueProperties) {
    if (!customInfo) {
        return;
    }
    
    // 为无值属性填充值
    for (const auto& valuelessProp : customInfo->ValuelessProperties) {
        auto valueIt = valueProperties.find(valuelessProp);
        if (valueIt != valueProperties.end()) {
            customInfo->Properties[valuelessProp] = valueIt->second;
            std::cout << "🔄 无值样式组填充: " << valuelessProp << " = " << valueIt->second << std::endl;
        } else {
            std::cerr << "❌ 无值属性缺少值: " << valuelessProp << std::endl;
        }
    }
}

std::vector<size_t> CustomEngine::ProcessIndexAccess(const std::vector<std::unique_ptr<CHTLBaseNode>>& elements,
                                                     const std::string& selector) {
    std::vector<size_t> indices;
    
    // 解析选择器 (如: div[1])
    std::regex indexRegex(R"(([a-zA-Z][a-zA-Z0-9]*)\[(\d+)\])");
    std::smatch matches;
    
    if (std::regex_search(selector, matches, indexRegex)) {
        std::string tagName = matches[1].str();
        size_t index = std::stoull(matches[2].str());
        
        // 查找匹配的元素
        size_t currentIndex = 0;
        for (size_t i = 0; i < elements.size(); ++i) {
            if (auto elementNode = dynamic_cast<ElementNode*>(elements[i].get())) {
                if (elementNode->GetTagName() == tagName) {
                    if (currentIndex == index) {
                        indices.push_back(i);
                        break;
                    }
                    currentIndex++;
                }
            }
        }
    }
    
    return indices;
}

std::unique_ptr<CustomInfo> CustomEngine::ParseCustomContent(const std::string& customContent, 
                                                           TemplateEngineType customType) {
    auto customInfo = std::make_unique<CustomInfo>();
    customInfo->Type = customType;
    
    if (customType == TemplateEngineType::STYLE_TEMPLATE) {
        // 解析样式属性，支持无值属性
        std::istringstream stream(customContent);
        std::string line;
        
        while (std::getline(stream, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.substr(0, 2) == "//") {
                continue;
            }
            
            // 检查是否是无值属性 (property,)
            if (line.back() == ',' || (line.back() == ';' && line.find(':') == std::string::npos)) {
                std::string propName = line;
                if (propName.back() == ',' || propName.back() == ';') {
                    propName.pop_back();
                }
                customInfo->ValuelessProperties.push_back(propName);
                customInfo->HasValuelessProperties = true;
                std::cout << "📝 无值属性: " << propName << std::endl;
            } else {
                // 常规属性
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string property = line.substr(0, colonPos);
                    std::string value = line.substr(colonPos + 1);
                    
                    // 去除分号
                    if (value.back() == ';') {
                        value.pop_back();
                    }
                    
                    customInfo->Properties[property] = value;
                }
            }
        }
    }
    
    return customInfo;
}

std::pair<std::string, std::string> CustomEngine::ParseNamespacedName(const std::string& name) {
    size_t pos = name.find("::");
    if (pos != std::string::npos) {
        return {name.substr(0, pos), name.substr(pos + 2)};
    }
    return {"", name};
}

void CustomEngine::SetCurrentNamespace(const std::string& namespaceName) {
    m_CurrentNamespace = namespaceName;
}

std::unordered_map<std::string, size_t> CustomEngine::GetCustomStatistics() {
    return {
        {"StyleCustoms", m_StyleCustoms.size()},
        {"ElementCustoms", m_ElementCustoms.size()},
        {"VarCustoms", m_VarCustoms.size()}
    };
}

bool CustomEngine::ValidateAllCustoms() {
    bool isValid = true;
    
    // 验证所有自定义的完整性
    for (const auto& [name, customInfo] : m_StyleCustoms) {
        if (customInfo->HasValuelessProperties && customInfo->ValuelessProperties.empty()) {
            std::cerr << "❌ 无值样式组配置错误: " << name << std::endl;
            isValid = false;
        }
    }
    
    return isValid;
}

void CustomEngine::Clear() {
    m_StyleCustoms.clear();
    m_ElementCustoms.clear();
    m_VarCustoms.clear();
    m_ProcessingCustoms.clear();
    m_CurrentNamespace.clear();
}

} // namespace CHTL