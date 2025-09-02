#include "CHTLBaseNode.h"
#include "Visitor.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLBaseNode::CHTLBaseNode(CHTLNodeType type, const std::string& name, size_t line, size_t column)
    : m_Type(type), m_Name(name), m_Parent(nullptr), m_Line(line), m_Column(column), 
      m_Position(0), m_HasAutoClass(false), m_HasAutoId(false) {
}

void CHTLBaseNode::AddChild(std::unique_ptr<CHTLBaseNode> child) {
    if (child) {
        child->SetParent(this);
        m_Children.push_back(std::move(child));
    }
}

std::vector<CHTLBaseNode*> CHTLBaseNode::GetChildrenByType(CHTLNodeType type) const {
    std::vector<CHTLBaseNode*> result;
    
    for (const auto& child : m_Children) {
        if (child->GetType() == type) {
            result.push_back(child.get());
        }
    }
    
    return result;
}

CHTLBaseNode* CHTLBaseNode::GetChildByName(const std::string& name) const {
    for (const auto& child : m_Children) {
        if (child->GetName() == name) {
            return child.get();
        }
    }
    
    return nullptr;
}

bool CHTLBaseNode::RemoveChild(size_t index) {
    if (index >= m_Children.size()) {
        return false;
    }
    
    m_Children.erase(m_Children.begin() + index);
    return true;
}

bool CHTLBaseNode::RemoveChild(CHTLBaseNode* child) {
    auto it = std::find_if(m_Children.begin(), m_Children.end(),
                          [child](const std::unique_ptr<CHTLBaseNode>& ptr) {
                              return ptr.get() == child;
                          });
    
    if (it != m_Children.end()) {
        m_Children.erase(it);
        return true;
    }
    
    return false;
}

void CHTLBaseNode::SetAttribute(const std::string& name, const std::string& value) {
    m_Attributes[name] = value;
    
    // 处理自动化选择器生成
    if (name == "class") {
        SetAutoClassName(value);
    }
    else if (name == "id") {
        SetAutoIdName(value);
    }
}

std::string CHTLBaseNode::GetAttribute(const std::string& name) const {
    auto it = m_Attributes.find(name);
    return (it != m_Attributes.end()) ? it->second : "";
}

bool CHTLBaseNode::HasAttribute(const std::string& name) const {
    return m_Attributes.find(name) != m_Attributes.end();
}

void CHTLBaseNode::SetProperty(const std::string& name, const std::string& value) {
    m_Properties[name] = value;
}

std::string CHTLBaseNode::GetProperty(const std::string& name) const {
    auto it = m_Properties.find(name);
    return (it != m_Properties.end()) ? it->second : "";
}

bool CHTLBaseNode::HasProperty(const std::string& name) const {
    return m_Properties.find(name) != m_Properties.end();
}

void CHTLBaseNode::SetPosition(size_t line, size_t column, size_t position) {
    m_Line = line;
    m_Column = column;
    m_Position = position;
}

void CHTLBaseNode::SetAutoClassName(const std::string& className) {
    m_AutoClassName = className;
    m_HasAutoClass = !className.empty();
    
    // 自动添加class属性
    if (m_HasAutoClass) {
        SetAttribute("class", className);
    }
}

void CHTLBaseNode::SetAutoIdName(const std::string& idName) {
    m_AutoIdName = idName;
    m_HasAutoId = !idName.empty();
    
    // 自动添加id属性
    if (m_HasAutoId) {
        SetAttribute("id", idName);
    }
}

std::string CHTLBaseNode::GetTypeString() const {
    switch (m_Type) {
        case CHTLNodeType::ELEMENT_NODE: return "ELEMENT_NODE";
        case CHTLNodeType::TEXT_NODE: return "TEXT_NODE";
        case CHTLNodeType::COMMENT_NODE: return "COMMENT_NODE";
        case CHTLNodeType::STYLE_NODE: return "STYLE_NODE";
        case CHTLNodeType::SCRIPT_NODE: return "SCRIPT_NODE";
        case CHTLNodeType::TEMPLATE_STYLE_NODE: return "TEMPLATE_STYLE_NODE";
        case CHTLNodeType::TEMPLATE_ELEMENT_NODE: return "TEMPLATE_ELEMENT_NODE";
        case CHTLNodeType::TEMPLATE_VAR_NODE: return "TEMPLATE_VAR_NODE";
        case CHTLNodeType::TEMPLATE_USAGE_NODE: return "TEMPLATE_USAGE_NODE";
        case CHTLNodeType::CUSTOM_STYLE_NODE: return "CUSTOM_STYLE_NODE";
        case CHTLNodeType::CUSTOM_ELEMENT_NODE: return "CUSTOM_ELEMENT_NODE";
        case CHTLNodeType::CUSTOM_VAR_NODE: return "CUSTOM_VAR_NODE";
        case CHTLNodeType::CUSTOM_USAGE_NODE: return "CUSTOM_USAGE_NODE";
        case CHTLNodeType::ORIGIN_HTML_NODE: return "ORIGIN_HTML_NODE";
        case CHTLNodeType::ORIGIN_STYLE_NODE: return "ORIGIN_STYLE_NODE";
        case CHTLNodeType::ORIGIN_JAVASCRIPT_NODE: return "ORIGIN_JAVASCRIPT_NODE";
        case CHTLNodeType::ORIGIN_CUSTOM_NODE: return "ORIGIN_CUSTOM_NODE";
        case CHTLNodeType::ORIGIN_USAGE_NODE: return "ORIGIN_USAGE_NODE";
        case CHTLNodeType::CONFIGURATION_NODE: return "CONFIGURATION_NODE";
        case CHTLNodeType::NAME_CONFIG_NODE: return "NAME_CONFIG_NODE";
        case CHTLNodeType::ORIGIN_TYPE_CONFIG_NODE: return "ORIGIN_TYPE_CONFIG_NODE";
        case CHTLNodeType::IMPORT_NODE: return "IMPORT_NODE";
        case CHTLNodeType::NAMESPACE_NODE: return "NAMESPACE_NODE";
        case CHTLNodeType::INHERIT_NODE: return "INHERIT_NODE";
        case CHTLNodeType::DELETE_NODE: return "DELETE_NODE";
        case CHTLNodeType::INSERT_NODE: return "INSERT_NODE";
        case CHTLNodeType::EXCEPT_NODE: return "EXCEPT_NODE";
        case CHTLNodeType::CLASS_SELECTOR_NODE: return "CLASS_SELECTOR_NODE";
        case CHTLNodeType::ID_SELECTOR_NODE: return "ID_SELECTOR_NODE";
        case CHTLNodeType::CONTEXT_REFERENCE_NODE: return "CONTEXT_REFERENCE_NODE";
        case CHTLNodeType::PSEUDO_CLASS_NODE: return "PSEUDO_CLASS_NODE";
        case CHTLNodeType::PSEUDO_ELEMENT_NODE: return "PSEUDO_ELEMENT_NODE";
        case CHTLNodeType::ATTRIBUTE_NODE: return "ATTRIBUTE_NODE";
        case CHTLNodeType::PROPERTY_NODE: return "PROPERTY_NODE";
        case CHTLNodeType::INDEX_ACCESS_NODE: return "INDEX_ACCESS_NODE";
        case CHTLNodeType::VARIABLE_REFERENCE_NODE: return "VARIABLE_REFERENCE_NODE";
        case CHTLNodeType::USE_NODE: return "USE_NODE";
        case CHTLNodeType::INFO_NODE: return "INFO_NODE";
        case CHTLNodeType::EXPORT_NODE: return "EXPORT_NODE";
        default: return "UNKNOWN_NODE";
    }
}

std::string CHTLBaseNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << GetTypeString() << " '" << m_Name << "'";
    
    if (!m_Content.empty()) {
        oss << " [" << m_Content.substr(0, 50);
        if (m_Content.length() > 50) {
            oss << "...";
        }
        oss << "]";
    }
    
    if (!m_Attributes.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& attr : m_Attributes) {
            if (!first) oss << ", ";
            oss << attr.first << "=" << attr.second;
            first = false;
        }
        oss << "}";
    }
    
    if (m_HasAutoClass) {
        oss << " [AutoClass: " << m_AutoClassName << "]";
    }
    
    if (m_HasAutoId) {
        oss << " [AutoId: " << m_AutoIdName << "]";
    }
    
    if (!m_Namespace.empty()) {
        oss << " [NS: " << m_Namespace << "]";
    }
    
    oss << " @(" << m_Line << ":" << m_Column << ")\n";
    
    // 递归输出子节点
    for (const auto& child : m_Children) {
        oss << child->ToString(indent + 1);
    }
    
    return oss.str();
}

std::string CHTLBaseNode::GetNodeStatistics() const {
    std::ostringstream oss;
    
    oss << "Node Statistics:\n";
    oss << "  Type: " << GetTypeString() << "\n";
    oss << "  Name: " << m_Name << "\n";
    oss << "  Children: " << m_Children.size() << "\n";
    oss << "  Attributes: " << m_Attributes.size() << "\n";
    oss << "  Properties: " << m_Properties.size() << "\n";
    oss << "  Position: " << m_Line << ":" << m_Column << "\n";
    oss << "  Namespace: " << m_Namespace << "\n";
    oss << "  Auto Class: " << (m_HasAutoClass ? m_AutoClassName : "None") << "\n";
    oss << "  Auto ID: " << (m_HasAutoId ? m_AutoIdName : "None") << "\n";
    
    return oss.str();
}

std::string CHTLBaseNode::GenerateIndent(int level) const {
    return std::string(level * 2, ' ');
}

bool CHTLBaseNode::ValidateNode() const {
    // 基础验证：检查节点名称是否有效
    if (m_Name.empty() && 
        m_Type != CHTLNodeType::TEXT_NODE && 
        m_Type != CHTLNodeType::COMMENT_NODE) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : m_Children) {
        if (!child->ValidateNode()) {
            return false;
        }
    }
    
    return true;
}

void CHTLBaseNode::ApplyInheritanceRules(CHTLBaseNode* parentNode) {
    if (!parentNode) {
        return;
    }
    
    // 继承父节点的命名空间
    if (m_Namespace.empty() && !parentNode->GetNamespace().empty()) {
        m_Namespace = parentNode->GetNamespace();
    }
    
    // 递归应用到子节点
    for (auto& child : m_Children) {
        child->ApplyInheritanceRules(this);
    }
}

// CHTLNodeFactory实现
std::unique_ptr<CHTLBaseNode> CHTLNodeFactory::CreateNode(CHTLNodeType type, 
                                                         const std::string& name,
                                                         const std::string& content,
                                                         size_t line, 
                                                         size_t column) {
    // 这里需要根据具体的节点类型创建对应的节点实例
    // 暂时返回基础实现，后续需要为每种节点类型创建具体的类
    
    switch (type) {
        case CHTLNodeType::ELEMENT_NODE:
            return CreateElementNode(name, line, column);
        case CHTLNodeType::TEXT_NODE:
            return CreateTextNode(content, line, column);
        default:
            // 临时实现：创建基础节点的派生类
            class ConcreteCHTLNode : public CHTLBaseNode {
            public:
                ConcreteCHTLNode(CHTLNodeType type, const std::string& name, size_t line, size_t column)
                    : CHTLBaseNode(type, name, line, column) {}
                
                void Accept(CHTLNodeVisitor* visitor) override {
                    // 临时实现
                }
                
                std::unique_ptr<CHTLBaseNode> Clone() const override {
                    auto cloned = std::make_unique<ConcreteCHTLNode>(m_Type, m_Name, m_Line, m_Column);
                    cloned->m_Content = m_Content;
                    cloned->m_Attributes = m_Attributes;
                    cloned->m_Properties = m_Properties;
                    cloned->m_Namespace = m_Namespace;
                    cloned->m_HasAutoClass = m_HasAutoClass;
                    cloned->m_HasAutoId = m_HasAutoId;
                    cloned->m_AutoClassName = m_AutoClassName;
                    cloned->m_AutoIdName = m_AutoIdName;
                    
                    // 克隆子节点
                    for (const auto& child : m_Children) {
                        cloned->AddChild(child->Clone());
                    }
                    
                    return cloned;
                }
            };
            
            auto node = std::make_unique<ConcreteCHTLNode>(type, name, line, column);
            node->SetContent(content);
            return node;
    }
}

std::unique_ptr<CHTLBaseNode> CHTLNodeFactory::CreateElementNode(const std::string& tagName,
                                                                size_t line, 
                                                                size_t column) {
    return CreateNode(CHTLNodeType::ELEMENT_NODE, tagName, "", line, column);
}

std::unique_ptr<CHTLBaseNode> CHTLNodeFactory::CreateTextNode(const std::string& text,
                                                             size_t line, 
                                                             size_t column) {
    return CreateNode(CHTLNodeType::TEXT_NODE, "text", text, line, column);
}

std::unique_ptr<CHTLBaseNode> CHTLNodeFactory::CreateTemplateNode(const std::string& templateType,
                                                                 const std::string& templateName,
                                                                 const std::string& content,
                                                                 size_t line, 
                                                                 size_t column) {
    CHTLNodeType nodeType;
    
    if (templateType == "@Style") {
        nodeType = CHTLNodeType::TEMPLATE_STYLE_NODE;
    }
    else if (templateType == "@Element") {
        nodeType = CHTLNodeType::TEMPLATE_ELEMENT_NODE;
    }
    else if (templateType == "@Var") {
        nodeType = CHTLNodeType::TEMPLATE_VAR_NODE;
    }
    else {
        nodeType = CHTLNodeType::TEMPLATE_USAGE_NODE;
    }
    
    return CreateNode(nodeType, templateName, content, line, column);
}

std::unique_ptr<CHTLBaseNode> CHTLNodeFactory::CreateCustomNode(const std::string& customType,
                                                               const std::string& customName,
                                                               const std::string& content,
                                                               size_t line, 
                                                               size_t column) {
    CHTLNodeType nodeType;
    
    if (customType == "@Style") {
        nodeType = CHTLNodeType::CUSTOM_STYLE_NODE;
    }
    else if (customType == "@Element") {
        nodeType = CHTLNodeType::CUSTOM_ELEMENT_NODE;
    }
    else if (customType == "@Var") {
        nodeType = CHTLNodeType::CUSTOM_VAR_NODE;
    }
    else {
        nodeType = CHTLNodeType::CUSTOM_USAGE_NODE;
    }
    
    return CreateNode(nodeType, customName, content, line, column);
}

} // namespace CHTL