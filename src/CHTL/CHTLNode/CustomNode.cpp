#include "CustomNode.h"
#include "Visitor.h"
#include "ElementNode.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTL {

CustomNode::CustomNode(CustomType customType, const std::string& customName, 
                      size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::CUSTOM_STYLE_NODE, customName, line, column),
      m_CustomType(customType), m_CustomName(customName), 
      m_IsValuelessStyleGroup(false), m_HasSpecialization(false), 
      m_HasInheritance(false), m_HasDeletion(false), m_HasInsertion(false) {
    
    // 根据自定义类型设置节点类型
    switch (customType) {
        case CustomType::STYLE_CUSTOM:
            m_Type = CHTLNodeType::CUSTOM_STYLE_NODE;
            break;
        case CustomType::ELEMENT_CUSTOM:
            m_Type = CHTLNodeType::CUSTOM_ELEMENT_NODE;
            break;
        case CustomType::VAR_CUSTOM:
            m_Type = CHTLNodeType::CUSTOM_VAR_NODE;
            break;
    }
    
    SetName(customName);
}

void CustomNode::SetCustomName(const std::string& name) {
    if (IsValidCustomName(name)) {
        m_CustomName = name;
        SetName(name);
    }
}

void CustomNode::SetCustomContent(const std::string& content) {
    m_CustomContent = content;
    SetContent(content);
    ParseCustomContent(content);
}

void CustomNode::ParseCustomContent(const std::string& content) {
    // 解析自定义内容
    switch (m_CustomType) {
        case CustomType::STYLE_CUSTOM:
            ParseStyleCustomContent(content);
            break;
        case CustomType::ELEMENT_CUSTOM:
            ParseElementCustomContent(content);
            break;
        case CustomType::VAR_CUSTOM:
            ParseVarCustomContent(content);
            break;
    }
    
    // 解析特例化语法
    ParseSpecializationSyntax(content);
}

void CustomNode::ParseStyleCustomContent(const std::string& content) {
    // 检查是否为无值样式组
    std::regex valuelessPattern(R"(([a-zA-Z\-]+)\s*,?)");
    std::sregex_iterator valuelessIter(content.begin(), content.end(), valuelessPattern);
    std::sregex_iterator end;
    
    bool hasValuelessProps = false;
    for (; valuelessIter != end; ++valuelessIter) {
        std::smatch match = *valuelessIter;
        std::string property = match[1].str();
        
        // 检查是否有值
        size_t colonPos = content.find(property + ":");
        if (colonPos == std::string::npos) {
            // 无值属性
            AddValuelessProperty(property);
            hasValuelessProps = true;
        }
    }
    
    if (hasValuelessProps) {
        SetValuelessStyleGroup(true);
    }
    
    // 解析有值属性
    std::regex propertyPattern(R"(([a-zA-Z\-]+)\s*:\s*([^;,}]+)[;,]?)");
    std::sregex_iterator propIter(content.begin(), content.end(), propertyPattern);
    
    for (; propIter != end; ++propIter) {
        std::smatch match = *propIter;
        std::string property = match[1].str();
        std::string value = match[2].str();
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t\"'"));
        value.erase(value.find_last_not_of(" \t\"'") + 1);
        
        AddCustomProperty(property, value);
    }
}

void CustomNode::ParseElementCustomContent(const std::string& content) {
    // 解析自定义元素内容
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
        
        AddCustomElement(std::move(elementNode));
    }
}

void CustomNode::ParseVarCustomContent(const std::string& content) {
    // 解析自定义变量组内容
    std::regex variablePattern(R"(([a-zA-Z][a-zA-Z0-9_]*)\s*:\s*([^;]+);?)");
    std::sregex_iterator iter(content.begin(), content.end(), variablePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string varName = match[1].str();
        std::string varValue = match[2].str();
        
        // 清理值
        varValue.erase(0, varValue.find_first_not_of(" \t"));
        varValue.erase(varValue.find_last_not_of(" \t") + 1);
        
        if ((varValue.front() == '"' && varValue.back() == '"') ||
            (varValue.front() == '\'' && varValue.back() == '\'')) {
            varValue = varValue.substr(1, varValue.length() - 2);
        }
        
        AddCustomProperty(varName, varValue);
    }
}

void CustomNode::ParseSpecializationSyntax(const std::string& content) {
    // 解析特例化语法
    
    // 1. 删除属性：delete property1, property2;
    std::regex deletePropertyPattern(R"(delete\s+([^;]+);?)");
    std::sregex_iterator deleteIter(content.begin(), content.end(), deletePropertyPattern);
    std::sregex_iterator end;
    
    for (; deleteIter != end; ++deleteIter) {
        std::smatch match = *deleteIter;
        std::string deleteTarget = match[1].str();
        
        if (deleteTarget.find("@Style") != std::string::npos || 
            deleteTarget.find("@Element") != std::string::npos ||
            deleteTarget.find("@Var") != std::string::npos) {
            // 删除继承
            AddDeleteInheritOperation(deleteTarget);
        }
        else {
            // 删除属性
            AddDeletePropertyOperation(deleteTarget);
        }
    }
    
    // 2. 插入元素：insert after/before/replace/at top/at bottom selector { content }
    std::regex insertPattern(R"(insert\s+(after|before|replace|at\s+(?:top|bottom))\s+([^{]+)\s*\{([^}]*)\})");
    std::sregex_iterator insertIter(content.begin(), content.end(), insertPattern);
    
    for (; insertIter != end; ++insertIter) {
        std::smatch match = *insertIter;
        std::string position = match[1].str();
        std::string selector = match[2].str();
        std::string insertContent = match[3].str();
        
        // 清理选择器
        selector.erase(0, selector.find_first_not_of(" \t"));
        selector.erase(selector.find_last_not_of(" \t") + 1);
        
        AddInsertElementOperation(position, selector, insertContent);
    }
    
    // 3. 索引访问：element[index]
    auto indexAccess = ParseIndexAccess(content);
    for (const auto& access : indexAccess) {
        // 处理索引访问的特例化
        SpecializationOperation op(SpecializationType::MODIFY_PROPERTY, 
                                  "index_" + std::to_string(access.first), 
                                  access.second);
        AddSpecializationOperation(op);
    }
}

void CustomNode::AddCustomProperty(const std::string& name, const std::string& value) {
    m_CustomProperties[name] = value;
}

std::string CustomNode::GetCustomProperty(const std::string& name) const {
    auto it = m_CustomProperties.find(name);
    return (it != m_CustomProperties.end()) ? it->second : "";
}

bool CustomNode::HasCustomProperty(const std::string& name) const {
    return m_CustomProperties.find(name) != m_CustomProperties.end();
}

void CustomNode::AddValuelessProperty(const std::string& property) {
    if (std::find(m_ValuelessProperties.begin(), m_ValuelessProperties.end(), property) 
        == m_ValuelessProperties.end()) {
        m_ValuelessProperties.push_back(property);
    }
}

void CustomNode::AddInheritedTemplate(const std::string& templateName) {
    if (std::find(m_InheritedTemplates.begin(), m_InheritedTemplates.end(), templateName) 
        == m_InheritedTemplates.end()) {
        m_InheritedTemplates.push_back(templateName);
        m_HasInheritance = true;
    }
}

void CustomNode::AddInheritedCustom(const std::string& customName) {
    if (std::find(m_InheritedCustoms.begin(), m_InheritedCustoms.end(), customName) 
        == m_InheritedCustoms.end()) {
        m_InheritedCustoms.push_back(customName);
        m_HasInheritance = true;
    }
}

void CustomNode::AddSpecializationOperation(const SpecializationOperation& operation) {
    m_Specializations.push_back(operation);
    m_HasSpecialization = true;
    
    switch (operation.Type) {
        case SpecializationType::DELETE_PROPERTY:
        case SpecializationType::DELETE_INHERIT:
        case SpecializationType::DELETE_ELEMENT:
            m_HasDeletion = true;
            break;
        case SpecializationType::INSERT_ELEMENT:
            m_HasInsertion = true;
            break;
        default:
            break;
    }
}

void CustomNode::AddDeletePropertyOperation(const std::string& properties) {
    SpecializationOperation op(SpecializationType::DELETE_PROPERTY, properties);
    AddSpecializationOperation(op);
}

void CustomNode::AddDeleteInheritOperation(const std::string& inherit) {
    SpecializationOperation op(SpecializationType::DELETE_INHERIT, inherit);
    AddSpecializationOperation(op);
}

void CustomNode::AddInsertElementOperation(const std::string& position, 
                                          const std::string& selector, 
                                          const std::string& content) {
    SpecializationOperation op(SpecializationType::INSERT_ELEMENT, selector, content);
    op.Position = position;
    AddSpecializationOperation(op);
}

void CustomNode::AddDeleteElementOperation(const std::string& selector) {
    SpecializationOperation op(SpecializationType::DELETE_ELEMENT, selector);
    AddSpecializationOperation(op);
}

CHTLBaseNode* CustomNode::GetElementByIndex(size_t index) const {
    if (index < m_CustomElements.size()) {
        return m_CustomElements[index].get();
    }
    
    return nullptr;
}

std::vector<CHTLBaseNode*> CustomNode::GetElementsByType(const std::string& elementType) const {
    std::vector<CHTLBaseNode*> result;
    
    for (const auto& element : m_CustomElements) {
        if (element->GetName() == elementType) {
            result.push_back(element.get());
        }
    }
    
    return result;
}

void CustomNode::AddCustomElement(std::unique_ptr<CHTLBaseNode> element) {
    if (element && m_CustomType == CustomType::ELEMENT_CUSTOM) {
        element->SetParent(this);
        m_CustomElements.push_back(std::move(element));
    }
}

std::string CustomNode::GenerateCSS() const {
    if (m_CustomType != CustomType::STYLE_CUSTOM) {
        return "";
    }
    
    std::ostringstream css;
    
    for (const auto& prop : m_CustomProperties) {
        css << prop.first << ": " << prop.second << ";\n";
    }
    
    return css.str();
}

std::string CustomNode::GenerateHTML() const {
    if (m_CustomType != CustomType::ELEMENT_CUSTOM) {
        return "";
    }
    
    std::ostringstream html;
    
    for (const auto& element : m_CustomElements) {
        if (element->GetType() == CHTLNodeType::ELEMENT_NODE) {
            ElementNode* elemNode = static_cast<ElementNode*>(element.get());
            html << elemNode->GenerateHTMLElement();
        }
    }
    
    return html.str();
}

std::unordered_map<std::string, std::string> CustomNode::GenerateVariableMap() const {
    if (m_CustomType != CustomType::VAR_CUSTOM) {
        return {};
    }
    
    return m_CustomProperties;
}

std::string CustomNode::ApplySpecializations(const std::string& baseContent) const {
    std::string result = baseContent;
    
    // 应用特例化操作
    for (const auto& op : m_Specializations) {
        switch (op.Type) {
            case SpecializationType::DELETE_PROPERTY:
                // 删除指定属性
                {
                    std::stringstream ss(op.Target);
                    std::string property;
                    while (std::getline(ss, property, ',')) {
                        property.erase(0, property.find_first_not_of(" \t"));
                        property.erase(property.find_last_not_of(" \t") + 1);
                        
                        std::regex deletePattern(property + R"(\s*:\s*[^;]+;?)");
                        result = std::regex_replace(result, deletePattern, "");
                    }
                }
                break;
                
            case SpecializationType::DELETE_INHERIT:
                // 删除指定继承
                {
                    std::regex inheritPattern(op.Target + R"(\s*;?)");
                    result = std::regex_replace(result, inheritPattern, "");
                }
                break;
                
            case SpecializationType::INSERT_ELEMENT:
                // 插入元素（复杂操作，需要DOM操作）
                result += "\n/* Insert: " + op.Position + " " + op.Target + " { " + op.Value + " } */";
                break;
                
            case SpecializationType::DELETE_ELEMENT:
                // 删除元素
                {
                    std::regex elementPattern(op.Target + R"(\s*\{[^}]*\})");
                    result = std::regex_replace(result, elementPattern, "");
                }
                break;
                
            default:
                break;
        }
    }
    
    return result;
}

bool CustomNode::ValidateCustom() const {
    // 验证自定义名称
    if (!IsValidCustomName(m_CustomName)) {
        return false;
    }
    
    // 验证自定义内容
    if (m_CustomContent.empty() && m_CustomProperties.empty() && m_CustomElements.empty()) {
        return false;
    }
    
    // 验证无值样式组
    if (m_IsValuelessStyleGroup && m_ValuelessProperties.empty()) {
        return false;
    }
    
    return true;
}

std::string CustomNode::GetCustomTypeString() const {
    switch (m_CustomType) {
        case CustomType::STYLE_CUSTOM: return "@Style";
        case CustomType::ELEMENT_CUSTOM: return "@Element";
        case CustomType::VAR_CUSTOM: return "@Var";
        default: return "@Unknown";
    }
}

bool CustomNode::IsValidCustomName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 自定义名称必须以字母开始，后续可以是字母、数字或下划线
    std::regex namePattern(R"([a-zA-Z][a-zA-Z0-9_]*)");
    return std::regex_match(name, namePattern);
}

std::unordered_map<size_t, std::string> CustomNode::ParseIndexAccess(const std::string& content) const {
    std::unordered_map<size_t, std::string> indexAccess;
    
    // 解析索引访问：element[0], element[1]等
    std::regex indexPattern(R"(([a-zA-Z][a-zA-Z0-9]*)\[(\d+)\])");
    std::sregex_iterator iter(content.begin(), content.end(), indexPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string elementType = match[1].str();
        size_t index = std::stoul(match[2].str());
        
        indexAccess[index] = elementType;
    }
    
    return indexAccess;
}

void CustomNode::ProcessDeleteSyntax(const std::string& deleteContent) {
    // 处理删除语法的详细解析
    std::stringstream ss(deleteContent);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        if (item.find("@") != std::string::npos) {
            // 删除继承
            AddDeleteInheritOperation(item);
        }
        else if (item.find("[") != std::string::npos) {
            // 删除索引元素
            AddDeleteElementOperation(item);
        }
        else {
            // 删除属性或元素
            AddDeletePropertyOperation(item);
        }
    }
}

void CustomNode::ProcessInsertSyntax(const std::string& insertContent) {
    // 解析插入语法的详细内容
    std::regex insertDetailPattern(R"((after|before|replace|at\s+(?:top|bottom))\s+([^{]+)\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(insertContent, match, insertDetailPattern)) {
        std::string position = match[1].str();
        std::string selector = match[2].str();
        std::string content = match[3].str();
        
        // 清理选择器
        selector.erase(0, selector.find_first_not_of(" \t"));
        selector.erase(selector.find_last_not_of(" \t") + 1);
        
        AddInsertElementOperation(position, selector, content);
    }
}

void CustomNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitCustomNode(this);
    }
}

std::unique_ptr<CHTLBaseNode> CustomNode::Clone() const {
    auto cloned = std::make_unique<CustomNode>(m_CustomType, m_CustomName, GetLine(), GetColumn());
    
    // 复制基础属性
    cloned->SetContent(GetContent());
    cloned->SetNamespace(GetNamespace());
    cloned->m_CustomContent = m_CustomContent;
    cloned->m_CustomProperties = m_CustomProperties;
    cloned->m_InheritedTemplates = m_InheritedTemplates;
    cloned->m_InheritedCustoms = m_InheritedCustoms;
    cloned->m_Specializations = m_Specializations;
    cloned->m_IsValuelessStyleGroup = m_IsValuelessStyleGroup;
    cloned->m_ValuelessProperties = m_ValuelessProperties;
    cloned->m_HasSpecialization = m_HasSpecialization;
    cloned->m_HasInheritance = m_HasInheritance;
    cloned->m_HasDeletion = m_HasDeletion;
    cloned->m_HasInsertion = m_HasInsertion;
    
    // 克隆自定义元素
    for (const auto& element : m_CustomElements) {
        cloned->AddCustomElement(element->Clone());
    }
    
    return cloned;
}

std::string CustomNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "CustomNode [Custom] " << GetCustomTypeString() << " '" << m_CustomName << "'";
    
    if (m_IsValuelessStyleGroup) {
        oss << " [Valueless: " << m_ValuelessProperties.size() << " props]";
    }
    
    if (m_HasInheritance) {
        oss << " [Inherits: " << (m_InheritedTemplates.size() + m_InheritedCustoms.size()) << "]";
    }
    
    if (m_HasSpecialization) {
        oss << " [Specializations: " << m_Specializations.size() << "]";
    }
    
    if (m_HasDeletion) {
        oss << " [Deletions]";
    }
    
    if (m_HasInsertion) {
        oss << " [Insertions]";
    }
    
    if (!m_CustomProperties.empty()) {
        oss << " [Props: " << m_CustomProperties.size() << "]";
    }
    
    if (!m_CustomElements.empty()) {
        oss << " [Elements: " << m_CustomElements.size() << "]";
    }
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    // 输出无值属性
    if (m_IsValuelessStyleGroup && !m_ValuelessProperties.empty()) {
        oss << indentStr << "  Valueless Properties:\n";
        for (const auto& prop : m_ValuelessProperties) {
            oss << indentStr << "    " << prop << "\n";
        }
    }
    
    // 输出继承信息
    if (m_HasInheritance) {
        if (!m_InheritedTemplates.empty()) {
            oss << indentStr << "  Inherited Templates:\n";
            for (const auto& inherited : m_InheritedTemplates) {
                oss << indentStr << "    " << inherited << "\n";
            }
        }
        
        if (!m_InheritedCustoms.empty()) {
            oss << indentStr << "  Inherited Customs:\n";
            for (const auto& inherited : m_InheritedCustoms) {
                oss << indentStr << "    " << inherited << "\n";
            }
        }
    }
    
    // 输出自定义属性
    if (!m_CustomProperties.empty()) {
        oss << indentStr << "  Properties:\n";
        for (const auto& prop : m_CustomProperties) {
            oss << indentStr << "    " << prop.first << ": " << prop.second << "\n";
        }
    }
    
    // 输出特例化操作
    if (m_HasSpecialization) {
        oss << indentStr << "  Specializations:\n";
        for (const auto& op : m_Specializations) {
            oss << indentStr << "    ";
            switch (op.Type) {
                case SpecializationType::DELETE_PROPERTY:
                    oss << "DELETE PROPERTY: " << op.Target;
                    break;
                case SpecializationType::DELETE_INHERIT:
                    oss << "DELETE INHERIT: " << op.Target;
                    break;
                case SpecializationType::INSERT_ELEMENT:
                    oss << "INSERT " << op.Position << " " << op.Target << " { " << op.Value << " }";
                    break;
                case SpecializationType::DELETE_ELEMENT:
                    oss << "DELETE ELEMENT: " << op.Target;
                    break;
                default:
                    oss << "UNKNOWN OPERATION";
                    break;
            }
            oss << "\n";
        }
    }
    
    // 输出自定义元素
    if (!m_CustomElements.empty()) {
        oss << indentStr << "  Elements:\n";
        for (const auto& element : m_CustomElements) {
            oss << element->ToString(indent + 2);
        }
    }
    
    return oss.str();
}

} // namespace CHTL