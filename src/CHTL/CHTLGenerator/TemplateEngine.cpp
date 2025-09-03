#include "TemplateEngine.h"
#include <regex>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace CHTL {

TemplateEngine::TemplateEngine() : m_CurrentNamespace("") {
}

void TemplateEngine::RegisterStyleTemplate(const std::string& name, 
                                          TemplateNode* templateNode,
                                          const std::string& namespaceName) {
    if (!templateNode || name.empty()) {
        return;
    }
    
    auto templateInfo = std::make_unique<TemplateInfo>();
    templateInfo->Name = name;
    templateInfo->Type = TemplateEngineType::STYLE_TEMPLATE;
    templateInfo->Namespace = namespaceName.empty() ? m_CurrentNamespace : namespaceName;
    
    // 解析模板内容
    templateInfo->Properties = ParseTemplateProperties(templateNode->GetTemplateContent());
    
    // 处理继承信息
    const auto& inheritList = templateNode->GetInheritedTemplates();
    templateInfo->InheritedTemplates = inheritList;
    
    std::string fullName = templateInfo->Namespace.empty() ? name : templateInfo->Namespace + "::" + name;
    m_StyleTemplates[fullName] = std::move(templateInfo);
    
    std::cout << "📝 注册样式组模板: " << fullName << std::endl;
}

void TemplateEngine::RegisterElementTemplate(const std::string& name, 
                                            TemplateNode* templateNode,
                                            const std::string& namespaceName) {
    if (!templateNode || name.empty()) {
        return;
    }
    
    auto templateInfo = std::make_unique<TemplateInfo>();
    templateInfo->Name = name;
    templateInfo->Type = TemplateEngineType::ELEMENT_TEMPLATE;
    templateInfo->Namespace = namespaceName.empty() ? m_CurrentNamespace : namespaceName;
    
    // 克隆模板元素
    const auto& templateElements = templateNode->GetChildren();
    for (const auto& element : templateElements) {
        if (auto elementNode = dynamic_cast<ElementNode*>(element.get())) {
            templateInfo->Elements.push_back(CloneElement(elementNode));
        }
    }
    
    // 处理继承信息
    const auto& inheritList = templateNode->GetInheritedTemplates();
    templateInfo->InheritedTemplates = inheritList;
    
    std::string fullName = templateInfo->Namespace.empty() ? name : templateInfo->Namespace + "::" + name;
    m_ElementTemplates[fullName] = std::move(templateInfo);
    
    std::cout << "📝 注册元素模板: " << fullName << std::endl;
}

void TemplateEngine::RegisterVarTemplate(const std::string& name, 
                                        TemplateNode* templateNode,
                                        const std::string& namespaceName) {
    if (!templateNode || name.empty()) {
        return;
    }
    
    auto varGroup = std::make_unique<VariableGroup>();
    varGroup->Name = name;
    varGroup->Namespace = namespaceName.empty() ? m_CurrentNamespace : namespaceName;
    
    // 解析变量组内容
    varGroup->Variables = ParseVariableGroup(templateNode->GetTemplateContent());
    
    std::string fullName = varGroup->Namespace.empty() ? name : varGroup->Namespace + "::" + name;
    m_VarTemplates[fullName] = std::move(varGroup);
    
    std::cout << "📝 注册变量组模板: " << fullName << std::endl;
}

bool TemplateEngine::ApplyStyleTemplate(const std::string& templateName,
                                       ElementNode* targetElement,
                                       const std::unordered_map<std::string, std::string>& overrideProperties) {
    if (!targetElement || templateName.empty()) {
        return false;
    }
    
    // 解析模板名称 (可能包含命名空间)
    auto [namespaceName, simpleName] = ParseNamespacedName(templateName);
    std::string fullTemplateName = namespaceName.empty() ? simpleName : namespaceName + "::" + simpleName;
    
    // 查找模板
    auto it = m_StyleTemplates.find(fullTemplateName);
    if (it == m_StyleTemplates.end()) {
        std::cerr << "❌ 未找到样式组模板: " << fullTemplateName << std::endl;
        return false;
    }
    
    auto& templateInfo = it->second;
    
    // 处理继承 (如果尚未处理)
    if (!templateInfo->IsProcessed) {
        ProcessTemplateInheritance(fullTemplateName, TemplateEngineType::STYLE_TEMPLATE);
    }
    
    // 应用模板属性
    for (const auto& [property, value] : templateInfo->Properties) {
        // 检查是否有覆盖属性
        auto overrideIt = overrideProperties.find(property);
        if (overrideIt != overrideProperties.end()) {
            targetElement->AddInlineStyle(property, overrideIt->second);
        } else {
            targetElement->AddInlineStyle(property, value);
        }
    }
    
    std::cout << "✅ 应用样式组模板: " << fullTemplateName << " → " << targetElement->GetTagName() << std::endl;
    return true;
}

bool TemplateEngine::ApplyElementTemplate(const std::string& templateName,
                                         ElementNode* parentElement,
                                         size_t insertPosition) {
    if (!parentElement || templateName.empty()) {
        return false;
    }
    
    // 解析模板名称
    auto [namespaceName, simpleName] = ParseNamespacedName(templateName);
    std::string fullTemplateName = namespaceName.empty() ? simpleName : namespaceName + "::" + simpleName;
    
    // 查找模板
    auto it = m_ElementTemplates.find(fullTemplateName);
    if (it == m_ElementTemplates.end()) {
        std::cerr << "❌ 未找到元素模板: " << fullTemplateName << std::endl;
        return false;
    }
    
    auto& templateInfo = it->second;
    
    // 处理继承 (如果尚未处理)
    if (!templateInfo->IsProcessed) {
        ProcessTemplateInheritance(fullTemplateName, TemplateEngineType::ELEMENT_TEMPLATE);
    }
    
    // 克隆并插入模板元素
    for (const auto& templateElement : templateInfo->Elements) {
        if (auto elementNode = dynamic_cast<ElementNode*>(templateElement.get())) {
            auto clonedElement = CloneElement(elementNode);
            
            if (insertPosition == SIZE_MAX) {
                parentElement->AddChild(std::move(clonedElement));
            } else {
                parentElement->InsertChild(insertPosition, std::move(clonedElement));
            }
        }
    }
    
    std::cout << "✅ 应用元素模板: " << fullTemplateName << " → " << parentElement->GetTagName() << std::endl;
    return true;
}

std::string TemplateEngine::ResolveVariableReference(const std::string& varExpression) {
    // 解析变量引用表达式: GroupName(variableName) 或 GroupName(variableName = value)
    std::regex varRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=\s*([^)]+))?\s*\))");
    std::smatch matches;
    
    if (!std::regex_search(varExpression, matches, varRegex)) {
        return varExpression; // 不是变量引用，直接返回
    }
    
    std::string groupName = matches[1].str();
    std::string variableName = matches[2].str();
    std::string overrideValue = matches.size() > 3 ? matches[3].str() : "";
    
    // 解析组名称 (可能包含命名空间)
    auto [namespaceName, simpleGroupName] = ParseNamespacedName(groupName);
    std::string fullGroupName = namespaceName.empty() ? simpleGroupName : namespaceName + "::" + simpleGroupName;
    
    // 查找变量组
    auto it = m_VarTemplates.find(fullGroupName);
    if (it == m_VarTemplates.end()) {
        std::cerr << "❌ 未找到变量组: " << fullGroupName << std::endl;
        return varExpression;
    }
    
    auto& varGroup = it->second;
    
    // 如果有覆盖值，使用覆盖值
    if (!overrideValue.empty()) {
        std::cout << "🔄 变量组特例化: " << groupName << "(" << variableName << " = " << overrideValue << ")" << std::endl;
        return overrideValue;
    }
    
    // 查找变量值
    auto varIt = varGroup->Variables.find(variableName);
    if (varIt != varGroup->Variables.end()) {
        std::cout << "✅ 变量解析: " << varExpression << " → " << varIt->second << std::endl;
        return varIt->second;
    }
    
    std::cerr << "❌ 变量未找到: " << variableName << " in " << fullGroupName << std::endl;
    return varExpression;
}

void TemplateEngine::ProcessTemplateInheritance(const std::string& templateName, TemplateEngineType templateType) {
    // 防止循环继承
    if (m_ProcessingTemplates.find(templateName) != m_ProcessingTemplates.end()) {
        std::cerr << "❌ 检测到循环继承: " << templateName << std::endl;
        return;
    }
    
    m_ProcessingTemplates.insert(templateName);
    
    TemplateInfo* templateInfo = nullptr;
    
    // 获取模板信息
    switch (templateType) {
        case TemplateEngineType::STYLE_TEMPLATE:
            if (m_StyleTemplates.find(templateName) != m_StyleTemplates.end()) {
                templateInfo = m_StyleTemplates[templateName].get();
            }
            break;
        case TemplateEngineType::ELEMENT_TEMPLATE:
            if (m_ElementTemplates.find(templateName) != m_ElementTemplates.end()) {
                templateInfo = m_ElementTemplates[templateName].get();
            }
            break;
        case TemplateEngineType::VAR_TEMPLATE:
            // 变量组不需要继承处理
            break;
    }
    
    if (!templateInfo) {
        m_ProcessingTemplates.erase(templateName);
        return;
    }
    
    // 处理所有继承的模板
    for (const auto& inheritedTemplate : templateInfo->InheritedTemplates) {
        // 递归处理继承的模板
        ProcessTemplateInheritance(inheritedTemplate, templateType);
        
        // 合并继承的属性
        if (templateType == TemplateEngineType::STYLE_TEMPLATE) {
            auto inheritedIt = m_StyleTemplates.find(inheritedTemplate);
            if (inheritedIt != m_StyleTemplates.end()) {
                templateInfo->Properties = MergeTemplateProperties(
                    inheritedIt->second->Properties, 
                    templateInfo->Properties
                );
            }
        }
    }
    
    templateInfo->IsProcessed = true;
    m_ProcessingTemplates.erase(templateName);
    
    std::cout << "✅ 模板继承处理完成: " << templateName << std::endl;
}

std::unordered_map<std::string, std::string> TemplateEngine::ParseTemplateProperties(const std::string& templateContent) {
    std::unordered_map<std::string, std::string> properties;
    
    // 解析CSS属性格式: property: value;
    std::regex propertyRegex(R"(([a-zA-Z-]+)\s*:\s*([^;]+);?)");
    std::sregex_iterator iter(templateContent.begin(), templateContent.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string property = match[1].str();
        std::string value = match[2].str();
        
        // 去除前后空白
        property.erase(0, property.find_first_not_of(" \t"));
        property.erase(property.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        properties[property] = value;
    }
    
    return properties;
}

std::unordered_map<std::string, std::string> TemplateEngine::ParseVariableGroup(const std::string& varContent) {
    std::unordered_map<std::string, std::string> variables;
    
    // 解析变量格式: variableName: "value";
    std::regex varRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*([^;]+);?)");
    std::sregex_iterator iter(varContent.begin(), varContent.end(), varRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string varName = match[1].str();
        std::string varValue = match[2].str();
        
        // 去除引号
        if ((varValue.front() == '"' && varValue.back() == '"') ||
            (varValue.front() == '\'' && varValue.back() == '\'')) {
            varValue = varValue.substr(1, varValue.length() - 2);
        }
        
        variables[varName] = varValue;
    }
    
    return variables;
}

std::unordered_map<std::string, std::string> TemplateEngine::MergeTemplateProperties(
    const std::unordered_map<std::string, std::string>& baseProperties,
    const std::unordered_map<std::string, std::string>& inheritedProperties) {
    
    std::unordered_map<std::string, std::string> merged = baseProperties;
    
    // 继承的属性优先级更高，会覆盖基础属性
    for (const auto& [property, value] : inheritedProperties) {
        merged[property] = value;
    }
    
    return merged;
}

std::unique_ptr<ElementNode> TemplateEngine::CloneElement(const ElementNode* sourceElement) {
    if (!sourceElement) {
        return nullptr;
    }
    
    auto cloned = std::make_unique<ElementNode>(
        sourceElement->GetTagName(),
        sourceElement->GetLine(),
        sourceElement->GetColumn()
    );
    
    // 克隆属性
    const auto& attributes = sourceElement->GetAttributes();
    for (const auto& [name, value] : attributes) {
        cloned->SetAttribute(name, value);
    }
    
    // 克隆内联样式
    const auto& inlineStyles = sourceElement->GetInlineStyles();
    for (const auto& [property, value] : inlineStyles) {
        cloned->AddInlineStyle(property, value);
    }
    
    // 递归克隆子元素
    const auto& children = sourceElement->GetChildren();
    for (const auto& child : children) {
        if (auto childElement = dynamic_cast<ElementNode*>(child.get())) {
            cloned->AddChild(CloneElement(childElement));
        }
        // TODO: 克隆其他类型的子节点
    }
    
    return cloned;
}

std::pair<std::string, std::string> TemplateEngine::ParseNamespacedName(const std::string& name) {
    size_t pos = name.find("::");
    if (pos != std::string::npos) {
        return {name.substr(0, pos), name.substr(pos + 2)};
    }
    return {"", name};
}

std::pair<std::string, TemplateEngineType> TemplateEngine::ParseFullQualifiedName(const std::string& fullName) {
    // 解析 [Template] @Style TemplateName 格式
    std::regex fqnRegex(R"(\[Template\]\s*@(Style|Element|Var)\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    std::smatch matches;
    
    if (std::regex_search(fullName, matches, fqnRegex)) {
        std::string typeStr = matches[1].str();
        std::string name = matches[2].str();
        
        TemplateEngineType type;
        if (typeStr == "Style") {
            type = TemplateEngineType::STYLE_TEMPLATE;
        } else if (typeStr == "Element") {
            type = TemplateEngineType::ELEMENT_TEMPLATE;
        } else if (typeStr == "Var") {
            type = TemplateEngineType::VAR_TEMPLATE;
        }
        
        return {name, type};
    }
    
    // 简单格式: @Style TemplateName
    std::regex simpleRegex(R"(@(Style|Element|Var)\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    if (std::regex_search(fullName, matches, simpleRegex)) {
        std::string typeStr = matches[1].str();
        std::string name = matches[2].str();
        
        TemplateEngineType type;
        if (typeStr == "Style") {
            type = TemplateEngineType::STYLE_TEMPLATE;
        } else if (typeStr == "Element") {
            type = TemplateEngineType::ELEMENT_TEMPLATE;
        } else if (typeStr == "Var") {
            type = TemplateEngineType::VAR_TEMPLATE;
        }
        
        return {name, type};
    }
    
    return {fullName, TemplateEngineType::STYLE_TEMPLATE};
}

void TemplateEngine::SetCurrentNamespace(const std::string& namespaceName) {
    m_CurrentNamespace = namespaceName;
    std::cout << "📁 设置当前命名空间: " << namespaceName << std::endl;
}

void TemplateEngine::AddNamespaceAlias(const std::string& alias, const std::string& namespaceName) {
    m_NamespaceAliases[alias] = namespaceName;
    std::cout << "📁 添加命名空间别名: " << alias << " → " << namespaceName << std::endl;
}

std::unordered_map<std::string, size_t> TemplateEngine::GetTemplateStatistics() {
    return {
        {"StyleTemplates", m_StyleTemplates.size()},
        {"ElementTemplates", m_ElementTemplates.size()},
        {"VarTemplates", m_VarTemplates.size()}
    };
}

bool TemplateEngine::ValidateAllTemplates() {
    bool isValid = true;
    
    // 验证样式组模板
    for (const auto& [name, templateInfo] : m_StyleTemplates) {
        if (templateInfo->Properties.empty()) {
            std::cerr << "⚠️ 样式组模板为空: " << name << std::endl;
        }
        
        // 验证继承关系
        for (const auto& inheritedTemplate : templateInfo->InheritedTemplates) {
            if (m_StyleTemplates.find(inheritedTemplate) == m_StyleTemplates.end()) {
                std::cerr << "❌ 继承的样式组模板不存在: " << inheritedTemplate << std::endl;
                isValid = false;
            }
        }
    }
    
    // 验证元素模板
    for (const auto& [name, templateInfo] : m_ElementTemplates) {
        if (templateInfo->Elements.empty()) {
            std::cerr << "⚠️ 元素模板为空: " << name << std::endl;
        }
    }
    
    // 验证变量组模板
    for (const auto& [name, varGroup] : m_VarTemplates) {
        if (varGroup->Variables.empty()) {
            std::cerr << "⚠️ 变量组为空: " << name << std::endl;
        }
    }
    
    return isValid;
}

void TemplateEngine::Clear() {
    m_StyleTemplates.clear();
    m_ElementTemplates.clear();
    m_VarTemplates.clear();
    m_ProcessingTemplates.clear();
    m_NamespaceAliases.clear();
    m_CurrentNamespace.clear();
}

bool TemplateEngine::HasCircularInheritance(const std::string& templateName, const std::string& inheritTarget) {
    return m_ProcessingTemplates.find(inheritTarget) != m_ProcessingTemplates.end();
}

void TemplateEngine::ProcessExplicitInheritance(TemplateInfo* templateInfo, const std::string& inheritTarget) {
    if (!templateInfo) {
        return;
    }
    
    templateInfo->InheritedTemplates.push_back(inheritTarget);
    std::cout << "🔗 显式继承: " << templateInfo->Name << " inherit " << inheritTarget << std::endl;
}

void TemplateEngine::ProcessCompositeInheritance(TemplateInfo* templateInfo, const std::string& compositeTarget) {
    if (!templateInfo) {
        return;
    }
    
    templateInfo->InheritedTemplates.push_back(compositeTarget);
    std::cout << "🔗 组合式继承: " << templateInfo->Name << " 组合 " << compositeTarget << std::endl;
}

} // namespace CHTL