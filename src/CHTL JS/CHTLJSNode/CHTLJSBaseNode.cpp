#include "CHTLJSBaseNode.h"
#include <algorithm>
#include <sstream>

namespace CHTLJS {

CHTLJSBaseNode::CHTLJSBaseNode(CHTLJSNodeType type, const std::string& name, 
                              size_t line, size_t column)
    : m_Type(type), m_Name(name), m_Parent(nullptr), m_Line(line), m_Column(column), 
      m_Position(0), m_IsCHTLJSFunction(false), m_SupportsVirtualObject(false) {
}

void CHTLJSBaseNode::AddChild(std::unique_ptr<CHTLJSBaseNode> child) {
    if (child) {
        child->SetParent(this);
        m_Children.push_back(std::move(child));
    }
}

std::vector<CHTLJSBaseNode*> CHTLJSBaseNode::GetChildrenByType(CHTLJSNodeType type) const {
    std::vector<CHTLJSBaseNode*> result;
    
    for (const auto& child : m_Children) {
        if (child->GetType() == type) {
            result.push_back(child.get());
        }
    }
    
    return result;
}

CHTLJSBaseNode* CHTLJSBaseNode::GetChildByName(const std::string& name) const {
    for (const auto& child : m_Children) {
        if (child->GetName() == name) {
            return child.get();
        }
    }
    
    return nullptr;
}

void CHTLJSBaseNode::SetProperty(const std::string& name, const std::string& value) {
    m_Properties[name] = value;
}

std::string CHTLJSBaseNode::GetProperty(const std::string& name) const {
    auto it = m_Properties.find(name);
    return (it != m_Properties.end()) ? it->second : "";
}

bool CHTLJSBaseNode::HasProperty(const std::string& name) const {
    return m_Properties.find(name) != m_Properties.end();
}

void CHTLJSBaseNode::SetPosition(size_t line, size_t column, size_t position) {
    m_Line = line;
    m_Column = column;
    m_Position = position;
}

std::string CHTLJSBaseNode::GetTypeString() const {
    switch (m_Type) {
        case CHTLJSNodeType::MODULE_NODE: return "MODULE_NODE";
        case CHTLJSNodeType::LOAD_NODE: return "LOAD_NODE";
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE: return "ENHANCED_SELECTOR_NODE";
        case CHTLJSNodeType::SELECTOR_CONTENT_NODE: return "SELECTOR_CONTENT_NODE";
        case CHTLJSNodeType::CLASS_SELECTOR_NODE: return "CLASS_SELECTOR_NODE";
        case CHTLJSNodeType::ID_SELECTOR_NODE: return "ID_SELECTOR_NODE";
        case CHTLJSNodeType::TAG_SELECTOR_NODE: return "TAG_SELECTOR_NODE";
        case CHTLJSNodeType::DESCENDANT_SELECTOR_NODE: return "DESCENDANT_SELECTOR_NODE";
        case CHTLJSNodeType::INDEX_SELECTOR_NODE: return "INDEX_SELECTOR_NODE";
        case CHTLJSNodeType::ARROW_OPERATOR_NODE: return "ARROW_OPERATOR_NODE";
        case CHTLJSNodeType::EVENT_BIND_OPERATOR_NODE: return "EVENT_BIND_OPERATOR_NODE";
        case CHTLJSNodeType::LISTEN_NODE: return "LISTEN_NODE";
        case CHTLJSNodeType::DELEGATE_NODE: return "DELEGATE_NODE";
        case CHTLJSNodeType::ANIMATE_NODE: return "ANIMATE_NODE";
        case CHTLJSNodeType::VIR_DECLARATION_NODE: return "VIR_DECLARATION_NODE";
        case CHTLJSNodeType::VIR_ASSIGNMENT_NODE: return "VIR_ASSIGNMENT_NODE";
        case CHTLJSNodeType::VIR_REFERENCE_NODE: return "VIR_REFERENCE_NODE";
        case CHTLJSNodeType::VIR_KEY_NODE: return "VIR_KEY_NODE";
        case CHTLJSNodeType::EVENT_MAPPING_NODE: return "EVENT_MAPPING_NODE";
        case CHTLJSNodeType::EVENT_HANDLER_NODE: return "EVENT_HANDLER_NODE";
        case CHTLJSNodeType::EVENT_DELEGATE_TARGET_NODE: return "EVENT_DELEGATE_TARGET_NODE";
        case CHTLJSNodeType::ANIMATION_TARGET_NODE: return "ANIMATION_TARGET_NODE";
        case CHTLJSNodeType::ANIMATION_PROPERTY_NODE: return "ANIMATION_PROPERTY_NODE";
        case CHTLJSNodeType::ANIMATION_KEYFRAME_NODE: return "ANIMATION_KEYFRAME_NODE";
        case CHTLJSNodeType::ANIMATION_STATE_NODE: return "ANIMATION_STATE_NODE";
        case CHTLJSNodeType::COMMENT_NODE: return "COMMENT_NODE";
        case CHTLJSNodeType::UNKNOWN_NODE: return "UNKNOWN_NODE";
        default: return "UNKNOWN_CHTLJS_NODE";
    }
}

std::string CHTLJSBaseNode::ToString(int indent) const {
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
    
    if (m_IsCHTLJSFunction) {
        oss << " [CHTL JS Function]";
    }
    
    if (m_SupportsVirtualObject) {
        oss << " [Supports Vir]";
    }
    
    if (!m_SelectorType.empty()) {
        oss << " [Selector: " << m_SelectorType << "]";
    }
    
    if (!m_Properties.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& prop : m_Properties) {
            if (!first) oss << ", ";
            oss << prop.first << "=" << prop.second;
            first = false;
        }
        oss << "}";
    }
    
    oss << " @(" << m_Line << ":" << m_Column << ")\n";
    
    // 递归输出子节点
    for (const auto& child : m_Children) {
        oss << child->ToString(indent + 1);
    }
    
    return oss.str();
}

std::string CHTLJSBaseNode::GetCHTLJSNodeStatistics() const {
    std::ostringstream oss;
    
    oss << "CHTL JS Node Statistics:\n";
    oss << "  Type: " << GetTypeString() << "\n";
    oss << "  Name: " << m_Name << "\n";
    oss << "  Children: " << m_Children.size() << "\n";
    oss << "  Properties: " << m_Properties.size() << "\n";
    oss << "  Position: " << m_Line << ":" << m_Column << "\n";
    oss << "  Is CHTL JS Function: " << (m_IsCHTLJSFunction ? "Yes" : "No") << "\n";
    oss << "  Supports Virtual Object: " << (m_SupportsVirtualObject ? "Yes" : "No") << "\n";
    oss << "  Selector Type: " << m_SelectorType << "\n";
    oss << "  Generated JS Length: " << m_GeneratedJavaScript.length() << " chars\n";
    
    return oss.str();
}

std::string CHTLJSBaseNode::GenerateIndent(int level) const {
    return std::string(level * 2, ' ');
}

bool CHTLJSBaseNode::ValidateCHTLJSNode() const {
    // 基础验证：检查CHTL JS节点名称是否有效
    if (m_Name.empty() && 
        m_Type != CHTLJSNodeType::COMMENT_NODE && 
        m_Type != CHTLJSNodeType::UNKNOWN_NODE) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : m_Children) {
        if (!child->ValidateCHTLJSNode()) {
            return false;
        }
    }
    
    return true;
}

// CHTLJSNodeFactory实现
std::unique_ptr<CHTLJSBaseNode> CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType type, 
                                                                   const std::string& name,
                                                                   const std::string& content,
                                                                   size_t line, 
                                                                   size_t column) {
    // 临时实现：创建基础节点的派生类
    class ConcreteCHTLJSNode : public CHTLJSBaseNode {
    public:
        ConcreteCHTLJSNode(CHTLJSNodeType type, const std::string& name, size_t line, size_t column)
            : CHTLJSBaseNode(type, name, line, column) {}
        
        void Accept(CHTLJSNodeVisitor* visitor) override {
            // 临时实现
        }
        
        std::unique_ptr<CHTLJSBaseNode> Clone() const override {
            auto cloned = std::make_unique<ConcreteCHTLJSNode>(m_Type, m_Name, m_Line, m_Column);
            cloned->m_Content = m_Content;
            cloned->m_Properties = m_Properties;
            cloned->m_IsCHTLJSFunction = m_IsCHTLJSFunction;
            cloned->m_SupportsVirtualObject = m_SupportsVirtualObject;
            cloned->m_GeneratedJavaScript = m_GeneratedJavaScript;
            cloned->m_SelectorType = m_SelectorType;
            
            // 克隆子节点
            for (const auto& child : m_Children) {
                cloned->AddChild(child->Clone());
            }
            
            return cloned;
        }
        
        std::string GenerateJavaScript() const override {
            return m_GeneratedJavaScript.empty() ? "/* CHTL JS Node: " + m_Name + " */" : m_GeneratedJavaScript;
        }
    };
    
    auto node = std::make_unique<ConcreteCHTLJSNode>(type, name, line, column);
    node->SetContent(content);
    
    // 设置CHTL JS特有属性
    switch (type) {
        case CHTLJSNodeType::LISTEN_NODE:
        case CHTLJSNodeType::DELEGATE_NODE:
        case CHTLJSNodeType::ANIMATE_NODE:
            node->SetCHTLJSFunction(true);
            node->SetSupportsVirtualObject(true);
            break;
        case CHTLJSNodeType::MODULE_NODE:
            node->SetCHTLJSFunction(true);
            node->SetSupportsVirtualObject(false);
            break;
        case CHTLJSNodeType::VIR_DECLARATION_NODE:
        case CHTLJSNodeType::VIR_ASSIGNMENT_NODE:
        case CHTLJSNodeType::VIR_REFERENCE_NODE:
            node->SetSupportsVirtualObject(true);
            break;
        default:
            break;
    }
    
    return node;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSNodeFactory::CreateEnhancedSelectorNode(const std::string& selector,
                                                                             size_t line, 
                                                                             size_t column) {
    auto node = CreateCHTLJSNode(CHTLJSNodeType::ENHANCED_SELECTOR_NODE, "selector", selector, line, column);
    
    // 分析选择器类型
    if (selector.find('.') != std::string::npos) {
        node->SetSelectorType("class");
    }
    else if (selector.find('#') != std::string::npos) {
        node->SetSelectorType("id");
    }
    else if (selector.find(' ') != std::string::npos) {
        node->SetSelectorType("descendant");
    }
    else if (selector.find('[') != std::string::npos) {
        node->SetSelectorType("index");
    }
    else {
        node->SetSelectorType("tag");
    }
    
    return node;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSNodeFactory::CreateCHTLJSFunctionNode(const std::string& functionType,
                                                                           const std::string& functionContent,
                                                                           size_t line, 
                                                                           size_t column) {
    CHTLJSNodeType nodeType;
    
    if (functionType == "listen") {
        nodeType = CHTLJSNodeType::LISTEN_NODE;
    }
    else if (functionType == "delegate") {
        nodeType = CHTLJSNodeType::DELEGATE_NODE;
    }
    else if (functionType == "animate") {
        nodeType = CHTLJSNodeType::ANIMATE_NODE;
    }
    else if (functionType == "module") {
        nodeType = CHTLJSNodeType::MODULE_NODE;
    }
    else {
        nodeType = CHTLJSNodeType::UNKNOWN_NODE;
    }
    
    auto node = CreateCHTLJSNode(nodeType, functionType, functionContent, line, column);
    node->SetCHTLJSFunction(true);
    
    // 设置虚对象支持
    if (functionType != "module") {
        node->SetSupportsVirtualObject(true);
    }
    
    return node;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSNodeFactory::CreateVirtualObjectNode(const std::string& virName,
                                                                          const std::string& functionContent,
                                                                          size_t line, 
                                                                          size_t column) {
    auto node = CreateCHTLJSNode(CHTLJSNodeType::VIR_DECLARATION_NODE, virName, functionContent, line, column);
    node->SetSupportsVirtualObject(true);
    
    return node;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSNodeFactory::CreateEventBindNode(const std::string& selector,
                                                                      const std::string& eventType,
                                                                      const std::string& handler,
                                                                      size_t line, 
                                                                      size_t column) {
    auto node = CreateCHTLJSNode(CHTLJSNodeType::EVENT_BIND_OPERATOR_NODE, eventType, handler, line, column);
    node->SetProperty("selector", selector);
    node->SetProperty("event", eventType);
    node->SetProperty("handler", handler);
    
    return node;
}

} // namespace CHTLJS