#include "TemplateNode.h"
#include "Visitor.h"
#include "ElementNode.h"
#include "StyleNode.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType templateType, const std::string& templateName, 
                          size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::TEMPLATE_STYLE_NODE, templateName, line, column),
      m_TemplateType(templateType), m_TemplateName(templateName), 
      m_IsInheritanceTemplate(false), m_HasExplicitInherit(false) {
    
    // 根据模板类型设置节点类型
    switch (templateType) {
        case TemplateType::STYLE_TEMPLATE:
            m_Type = CHTLNodeType::TEMPLATE_STYLE_NODE;
            break;
        case TemplateType::ELEMENT_TEMPLATE:
            m_Type = CHTLNodeType::TEMPLATE_ELEMENT_NODE;
            break;
        case TemplateType::VAR_TEMPLATE:
            m_Type = CHTLNodeType::TEMPLATE_VAR_NODE;
            break;
    }
    
    SetName(templateName);
}

void TemplateNode::SetTemplateName(const std::string& name) {
    if (IsValidTemplateName(name)) {
        m_TemplateName = name;
        SetName(name);
    }
}

void TemplateNode::SetTemplateContent(const std::string& content) {
    m_TemplateContent = content;
    SetContent(content);
    ParseTemplateContent(content);
}

void TemplateNode::ParseTemplateContent(const std::string& content) {
    switch (m_TemplateType) {
        case TemplateType::STYLE_TEMPLATE:
            ParseStyleTemplateContent(content);
            break;
        case TemplateType::ELEMENT_TEMPLATE:
            ParseElementTemplateContent(content);
            break;
        case TemplateType::VAR_TEMPLATE:
            ParseVarTemplateContent(content);
            break;
    }
    
    // 处理模板继承
    ProcessTemplateInheritance(content);
}

void TemplateNode::ParseStyleTemplateContent(const std::string& content) {
    // 解析样式组模板内容
    // 格式：property: value; property2: value2;
    
    std::regex propertyPattern(R"(([a-zA-Z\-]+)\s*:\s*([^;]+);?)");
    std::sregex_iterator iter(content.begin(), content.end(), propertyPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string property = match[1].str();
        std::string value = match[2].str();
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t\"'"));
        value.erase(value.find_last_not_of(" \t\"'") + 1);
        
        AddTemplateProperty(property, value);
    }
}

void TemplateNode::ParseElementTemplateContent(const std::string& content) {
    // 解析元素模板内容
    // 包含一系列HTML元素定义
    
    std::regex elementPattern(R"(([a-zA-Z][a-zA-Z0-9]*)\s*\{([^}]*)\})");
    std::sregex_iterator iter(content.begin(), content.end(), elementPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string tagName = match[1].str();
        std::string elementContent = match[2].str();
        
        // 创建元素节点
        auto elementNode = std::make_unique<ElementNode>(tagName);
        elementNode->SetContent(elementContent);
        
        // 解析元素内容（可能包含样式、文本等）
        // 这里需要递归解析，暂时简化实现
        
        AddTemplateElement(std::move(elementNode));
    }
}

void TemplateNode::ParseVarTemplateContent(const std::string& content) {
    // 解析变量组模板内容
    // 格式：varName: "value"; varName2: "value2";
    
    std::regex variablePattern(R"(([a-zA-Z][a-zA-Z0-9_]*)\s*:\s*([^;]+);?)");
    std::sregex_iterator iter(content.begin(), content.end(), variablePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string varName = match[1].str();
        std::string varValue = match[2].str();
        
        // 清理值（去除引号）
        varValue.erase(0, varValue.find_first_not_of(" \t"));
        varValue.erase(varValue.find_last_not_of(" \t") + 1);
        
        if ((varValue.front() == '"' && varValue.back() == '"') ||
            (varValue.front() == '\'' && varValue.back() == '\'')) {
            varValue = varValue.substr(1, varValue.length() - 2);
        }
        
        AddTemplateProperty(varName, varValue);
    }
}

void TemplateNode::ProcessTemplateInheritance(const std::string& content) {
    // 处理组合式继承：@Style TemplateName;
    std::regex compositeInheritPattern(R"(@(Style|Element|Var)\s+([a-zA-Z][a-zA-Z0-9_]*)\s*;?)");
    std::sregex_iterator iter(content.begin(), content.end(), compositeInheritPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string inheritType = match[1].str();
        std::string inheritName = match[2].str();
        
        // 验证继承类型匹配
        if ((m_TemplateType == TemplateType::STYLE_TEMPLATE && inheritType == "Style") ||
            (m_TemplateType == TemplateType::ELEMENT_TEMPLATE && inheritType == "Element") ||
            (m_TemplateType == TemplateType::VAR_TEMPLATE && inheritType == "Var")) {
            AddInheritedTemplate("@" + inheritType + " " + inheritName, false);
        }
    }
    
    // 处理显式继承：inherit @Style TemplateName;
    std::regex explicitInheritPattern(R"(inherit\s+@(Style|Element|Var)\s+([a-zA-Z][a-zA-Z0-9_]*)\s*;?)");
    std::sregex_iterator explicitIter(content.begin(), content.end(), explicitInheritPattern);
    
    for (; explicitIter != end; ++explicitIter) {
        std::smatch match = *explicitIter;
        std::string inheritType = match[1].str();
        std::string inheritName = match[2].str();
        
        // 验证继承类型匹配
        if ((m_TemplateType == TemplateType::STYLE_TEMPLATE && inheritType == "Style") ||
            (m_TemplateType == TemplateType::ELEMENT_TEMPLATE && inheritType == "Element") ||
            (m_TemplateType == TemplateType::VAR_TEMPLATE && inheritType == "Var")) {
            AddInheritedTemplate("@" + inheritType + " " + inheritName, true);
        }
    }
}

void TemplateNode::AddTemplateProperty(const std::string& name, const std::string& value) {
    m_TemplateProperties[name] = value;
}

std::string TemplateNode::GetTemplateProperty(const std::string& name) const {
    auto it = m_TemplateProperties.find(name);
    return (it != m_TemplateProperties.end()) ? it->second : "";
}

bool TemplateNode::HasTemplateProperty(const std::string& name) const {
    return m_TemplateProperties.find(name) != m_TemplateProperties.end();
}

void TemplateNode::AddInheritedTemplate(const std::string& templateName, bool isExplicit) {
    if (std::find(m_InheritedTemplates.begin(), m_InheritedTemplates.end(), templateName) 
        == m_InheritedTemplates.end()) {
        m_InheritedTemplates.push_back(templateName);
        m_IsInheritanceTemplate = true;
        
        if (isExplicit) {
            m_HasExplicitInherit = true;
        }
    }
}

void TemplateNode::AddTemplateElement(std::unique_ptr<CHTLBaseNode> element) {
    if (element && m_TemplateType == TemplateType::ELEMENT_TEMPLATE) {
        element->SetParent(this);
        m_TemplateElements.push_back(std::move(element));
    }
}

void TemplateNode::ApplyTemplateInheritance(const TemplateNode* parentTemplate) {
    if (!parentTemplate || parentTemplate->GetTemplateType() != m_TemplateType) {
        return;
    }
    
    // 继承父模板的属性
    for (const auto& prop : parentTemplate->GetTemplateProperties()) {
        // 如果当前模板没有该属性，则继承
        if (!HasTemplateProperty(prop.first)) {
            AddTemplateProperty(prop.first, prop.second);
        }
    }
    
    // 继承父模板的元素（对于元素模板）
    if (m_TemplateType == TemplateType::ELEMENT_TEMPLATE) {
        for (const auto& element : parentTemplate->GetTemplateElements()) {
            // 克隆父模板的元素
            AddTemplateElement(element->Clone());
        }
    }
}

std::string TemplateNode::ExpandTemplateContent() const {
    std::ostringstream expanded;
    
    switch (m_TemplateType) {
        case TemplateType::STYLE_TEMPLATE:
            // 展开样式组模板
            for (const auto& prop : m_TemplateProperties) {
                expanded << prop.first << ": " << prop.second << ";\n";
            }
            break;
            
        case TemplateType::ELEMENT_TEMPLATE:
            // 展开元素模板
            for (const auto& element : m_TemplateElements) {
                if (element->GetType() == CHTLNodeType::ELEMENT_NODE) {
                    ElementNode* elemNode = static_cast<ElementNode*>(element.get());
                    expanded << elemNode->GenerateHTMLElement() << "\n";
                }
            }
            break;
            
        case TemplateType::VAR_TEMPLATE:
            // 展开变量组模板（返回变量映射的字符串表示）
            for (const auto& var : m_TemplateProperties) {
                expanded << var.first << " = " << var.second << "\n";
            }
            break;
    }
    
    return expanded.str();
}

std::string TemplateNode::GenerateCSS() const {
    if (m_TemplateType != TemplateType::STYLE_TEMPLATE) {
        return "";
    }
    
    std::ostringstream css;
    
    for (const auto& prop : m_TemplateProperties) {
        css << prop.first << ": " << prop.second << ";\n";
    }
    
    return css.str();
}

std::string TemplateNode::GenerateHTML() const {
    if (m_TemplateType != TemplateType::ELEMENT_TEMPLATE) {
        return "";
    }
    
    std::ostringstream html;
    
    for (const auto& element : m_TemplateElements) {
        if (element->GetType() == CHTLNodeType::ELEMENT_NODE) {
            ElementNode* elemNode = static_cast<ElementNode*>(element.get());
            html << elemNode->GenerateHTMLElement();
        }
    }
    
    return html.str();
}

std::unordered_map<std::string, std::string> TemplateNode::GenerateVariableMap() const {
    if (m_TemplateType != TemplateType::VAR_TEMPLATE) {
        return {};
    }
    
    return m_TemplateProperties;
}

bool TemplateNode::ValidateTemplate() const {
    // 验证模板名称
    if (!IsValidTemplateName(m_TemplateName)) {
        return false;
    }
    
    // 验证模板内容
    if (m_TemplateContent.empty() && m_TemplateProperties.empty() && m_TemplateElements.empty()) {
        return false;
    }
    
    // 验证继承的有效性
    for (const auto& inherited : m_InheritedTemplates) {
        // 这里应该检查被继承的模板是否存在
        // 暂时跳过，实际验证在解析阶段进行
    }
    
    return true;
}

std::string TemplateNode::GetTemplateTypeString() const {
    switch (m_TemplateType) {
        case TemplateType::STYLE_TEMPLATE: return "@Style";
        case TemplateType::ELEMENT_TEMPLATE: return "@Element";
        case TemplateType::VAR_TEMPLATE: return "@Var";
        default: return "@Unknown";
    }
}

bool TemplateNode::IsValidTemplateName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 模板名称必须以字母开始，后续可以是字母、数字或下划线
    std::regex namePattern(R"([a-zA-Z][a-zA-Z0-9_]*)");
    return std::regex_match(name, namePattern);
}

void TemplateNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitTemplateNode(this);
    }
}

std::unique_ptr<CHTLBaseNode> TemplateNode::Clone() const {
    auto cloned = std::make_unique<TemplateNode>(m_TemplateType, m_TemplateName, GetLine(), GetColumn());
    
    // 复制基础属性
    cloned->SetContent(GetContent());
    cloned->SetNamespace(GetNamespace());
    cloned->m_TemplateContent = m_TemplateContent;
    cloned->m_TemplateProperties = m_TemplateProperties;
    cloned->m_InheritedTemplates = m_InheritedTemplates;
    cloned->m_IsInheritanceTemplate = m_IsInheritanceTemplate;
    cloned->m_HasExplicitInherit = m_HasExplicitInherit;
    
    // 克隆模板元素
    for (const auto& element : m_TemplateElements) {
        cloned->AddTemplateElement(element->Clone());
    }
    
    return cloned;
}

std::string TemplateNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "TemplateNode [Template] " << GetTemplateTypeString() << " '" << m_TemplateName << "'";
    
    if (m_IsInheritanceTemplate) {
        oss << " [Inherits: " << m_InheritedTemplates.size() << "]";
    }
    
    if (m_HasExplicitInherit) {
        oss << " [Explicit]";
    }
    
    if (!m_TemplateProperties.empty()) {
        oss << " [Props: " << m_TemplateProperties.size() << "]";
    }
    
    if (!m_TemplateElements.empty()) {
        oss << " [Elements: " << m_TemplateElements.size() << "]";
    }
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    // 输出继承信息
    if (m_IsInheritanceTemplate) {
        oss << indentStr << "  Inherits:\n";
        for (const auto& inherited : m_InheritedTemplates) {
            oss << indentStr << "    " << inherited << "\n";
        }
    }
    
    // 输出模板属性
    if (!m_TemplateProperties.empty()) {
        oss << indentStr << "  Properties:\n";
        for (const auto& prop : m_TemplateProperties) {
            oss << indentStr << "    " << prop.first << ": " << prop.second << "\n";
        }
    }
    
    // 输出模板元素
    if (!m_TemplateElements.empty()) {
        oss << indentStr << "  Elements:\n";
        for (const auto& element : m_TemplateElements) {
            oss << element->ToString(indent + 2);
        }
    }
    
    return oss.str();
}

// 全缀名访问功能实现
void TemplateNode::SetFullNamespacePath(const std::string& namespacePath) {
    m_FullNamespacePath = namespacePath;
}

std::string TemplateNode::GetFullQualifiedName() const {
    if (m_FullNamespacePath.empty()) {
        return m_TemplateName;
    }
    else {
        return m_FullNamespacePath + "::" + m_TemplateName;
    }
}

std::pair<std::string, std::string> TemplateNode::ParseFullQualifiedName(const std::string& fullName) {
    // 解析全缀名：UI::Button::Style → (UI::Button, Style)
    size_t lastDoubleColon = fullName.find_last_of("::");
    
    if (lastDoubleColon == std::string::npos) {
        // 没有命名空间，直接返回模板名
        return {"", fullName};
    }
    
    // 找到最后一个::的位置
    if (lastDoubleColon > 0 && fullName[lastDoubleColon-1] == ':') {
        lastDoubleColon--; // 指向第一个:
    }
    
    std::string namespacePath = fullName.substr(0, lastDoubleColon);
    std::string templateName = fullName.substr(lastDoubleColon + 2);
    
    return {namespacePath, templateName};
}

TemplateNode* TemplateNode::FindTemplateInNamespace(const std::string& namespacePath, 
                                                   const std::string& templateName,
                                                   CHTLGlobalMap* globalMap) {
    if (!globalMap) {
        return nullptr;
    }
    
    // 构建完整的模板名称
    std::string fullTemplateName;
    if (namespacePath.empty()) {
        fullTemplateName = templateName;
    }
    else {
        fullTemplateName = namespacePath + "::" + templateName;
    }
    
    // 在全局映射表中查找模板
    // 这里需要根据CHTLGlobalMap的实际API来实现
    // 暂时返回nullptr，等待CHTLGlobalMap完善
    return nullptr;
}

std::string TemplateNode::ResolveTemplateReferencePath(const std::string& referencePath, 
                                                       const std::string& currentNamespace) {
    // 解析模板引用路径
    
    if (referencePath.empty()) {
        return "";
    }
    
    // 如果是绝对路径（以::开头），直接返回
    if (referencePath.find("::") == 0) {
        return referencePath.substr(2); // 移除开头的::
    }
    
    // 如果包含::，说明是相对或绝对路径
    if (referencePath.find("::") != std::string::npos) {
        // 检查是否为相对路径
        if (referencePath.find("../") == 0) {
            // 相对路径：../ParentNamespace::Template
            std::string relativePath = referencePath.substr(3); // 移除../
            
            // 从当前命名空间向上一级
            size_t lastSeparator = currentNamespace.find_last_of("::");
            if (lastSeparator != std::string::npos && lastSeparator > 0) {
                std::string parentNamespace = currentNamespace.substr(0, lastSeparator - 1);
                return parentNamespace + "::" + relativePath;
            }
            else {
                // 已经在根命名空间
                return relativePath;
            }
        }
        else {
            // 绝对路径或当前命名空间的相对路径
            return referencePath;
        }
    }
    else {
        // 简单模板名，在当前命名空间中查找
        if (currentNamespace.empty()) {
            return referencePath;
        }
        else {
            return currentNamespace + "::" + referencePath;
        }
    }
}

} // namespace CHTL