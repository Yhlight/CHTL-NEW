#include "OriginNode.h"
#include "Visitor.h"
#include <sstream>

namespace CHTL {

OriginNode::OriginNode(OriginType originType, const std::string& originName, 
                      const std::string& content, size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::ORIGIN_HTML_NODE, originName, line, column),
      m_OriginType(originType), m_OriginName(originName), m_OriginContent(content),
      m_IsNamedOrigin(false), m_IsDefinition(true) {
    InitializeOriginNode();
}

OriginNode::OriginNode(const std::string& originName, size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::ORIGIN_USAGE_NODE, originName, line, column),
      m_OriginType(OriginType::HTML_ORIGIN), m_OriginName(originName),
      m_IsNamedOrigin(false), m_IsDefinition(false) {
    InitializeOriginNode();
}

OriginNode::OriginNode(const std::string& customTypeName, const std::string& originName, 
                      const std::string& content, size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::ORIGIN_CUSTOM_NODE, originName, line, column),
      m_OriginType(OriginType::CUSTOM_ORIGIN), m_OriginName(originName), 
      m_OriginContent(content), m_IsNamedOrigin(true), m_IsDefinition(true),
      m_CustomTypeName(customTypeName) {
    InitializeOriginNode();
    ProcessCustomTypeConfiguration();
}

void OriginNode::InitializeOriginNode() {
    // 原始嵌入节点的初始化
    // 由于类型不具有作用，这里主要设置基础属性
    
    if (m_IsDefinition) {
        SetAttribute("origin-type", GetTypeString());
        SetAttribute("origin-name", m_OriginName);
        
        if (m_IsNamedOrigin) {
            SetAttribute("named-origin", "true");
        }
    }
}

void OriginNode::SetOriginName(const std::string& name) {
    m_OriginName = name;
    // SetValue(name); // 暂时注释，等待CHTLBaseNode方法确认
}

void OriginNode::SetOriginContent(const std::string& content) {
    m_OriginContent = content;
    // SetContent(content); // 暂时注释，等待CHTLBaseNode方法确认
}

void OriginNode::SetCustomTypeName(const std::string& typeName) {
    m_CustomTypeName = typeName;
    m_OriginType = OriginType::CUSTOM_ORIGIN;
}

void OriginNode::AddTypeAttribute(const std::string& name, const std::string& value) {
    m_TypeAttributes[name] = value;
}

std::string OriginNode::GetTypeAttribute(const std::string& name) const {
    auto it = m_TypeAttributes.find(name);
    return (it != m_TypeAttributes.end()) ? it->second : "";
}

std::string OriginNode::GetDirectOutput() const {
    // 核心功能：直接输出原始代码，不经过任何处理
    // 这是原始嵌入的本质：原样输出
    return m_OriginContent;
}

std::string OriginNode::GenerateHTML() const {
    if (!m_IsDefinition) {
        // 原始嵌入使用：直接输出内容
        return GetDirectOutput();
    }
    
    // 原始嵌入定义：生成HTML注释标记
    std::ostringstream html;
    
    html << "<!-- CHTL Origin Definition: " << m_OriginName;
    
    if (IsCustomType()) {
        html << " (Custom Type: " << m_CustomTypeName << ")";
    }
    else {
        html << " (Type: ";
        switch (m_OriginType) {
            case OriginType::HTML_ORIGIN:
                html << "Html";
                break;
            case OriginType::STYLE_ORIGIN:
                html << "Style";
                break;
            case OriginType::JAVASCRIPT_ORIGIN:
                html << "JavaScript";
                break;
            case OriginType::CUSTOM_ORIGIN:
                html << "Custom";
                break;
        }
        html << ")";
    }
    
    html << " -->\n";
    
    // 直接输出原始内容（这是关键）
    html << GetDirectOutput();
    
    html << "\n<!-- End CHTL Origin: " << m_OriginName << " -->";
    
    return html.str();
}

std::string OriginNode::GenerateCSS() const {
    // 如果是Style类型的原始嵌入，直接输出CSS内容
    if (m_OriginType == OriginType::STYLE_ORIGIN || 
        (IsCustomType() && m_CustomTypeName.find("Style") != std::string::npos)) {
        
        std::ostringstream css;
        css << "/* CHTL Origin CSS: " << m_OriginName << " */\n";
        css << GetDirectOutput();
        css << "\n/* End Origin CSS */\n";
        
        return css.str();
    }
    
    return "";
}

std::string OriginNode::GenerateJavaScript() const {
    // 如果是JavaScript类型的原始嵌入，直接输出JavaScript内容
    if (m_OriginType == OriginType::JAVASCRIPT_ORIGIN || 
        (IsCustomType() && m_CustomTypeName.find("Script") != std::string::npos)) {
        
        std::ostringstream js;
        js << "// CHTL Origin JavaScript: " << m_OriginName << "\n";
        js << GetDirectOutput();
        js << "\n// End Origin JavaScript\n";
        
        return js.str();
    }
    
    return "";
}

std::string OriginNode::GetTypeString() const {
    if (IsCustomType()) {
        return "Origin_" + m_CustomTypeName;
    }
    
    switch (m_OriginType) {
        case OriginType::HTML_ORIGIN:
            return "Origin_Html";
        case OriginType::STYLE_ORIGIN:
            return "Origin_Style";
        case OriginType::JAVASCRIPT_ORIGIN:
            return "Origin_JavaScript";
        case OriginType::CUSTOM_ORIGIN:
            return "Origin_Custom";
        default:
            return "Origin_Unknown";
    }
}

std::unique_ptr<CHTLBaseNode> OriginNode::Clone() const {
    if (IsCustomType()) {
        return std::make_unique<OriginNode>(m_CustomTypeName, m_OriginName, 
                                           m_OriginContent, GetLine(), GetColumn());
    }
    else if (m_IsDefinition) {
        return std::make_unique<OriginNode>(m_OriginType, m_OriginName, 
                                           m_OriginContent, GetLine(), GetColumn());
    }
    else {
        return std::make_unique<OriginNode>(m_OriginName, GetLine(), GetColumn());
    }
}

void OriginNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitOriginNode(this);
    }
}

OriginType OriginNode::DetermineOriginType(const std::string& typeName) {
    if (typeName == "Html") {
        return OriginType::HTML_ORIGIN;
    }
    else if (typeName == "Style") {
        return OriginType::STYLE_ORIGIN;
    }
    else if (typeName == "JavaScript") {
        return OriginType::JAVASCRIPT_ORIGIN;
    }
    else {
        return OriginType::CUSTOM_ORIGIN;
    }
}

bool OriginNode::ValidateOriginContent() const {
    // 原始嵌入内容不需要验证，因为是直接输出
    // 这是原始嵌入的特点：接受任何内容
    return true;
}

void OriginNode::ProcessCustomTypeConfiguration() {
    // 处理自定义类型配置
    if (IsCustomType()) {
        // 自定义类型可以有特殊的处理逻辑
        // 但本质上仍然是直接输出
        
        AddTypeAttribute("custom-type", m_CustomTypeName);
        AddTypeAttribute("direct-output", "true");
        
        // 示例：Vue类型的特殊标记
        if (m_CustomTypeName == "Vue") {
            AddTypeAttribute("framework", "Vue.js");
            AddTypeAttribute("component", "true");
        }
        // 示例：React类型的特殊标记
        else if (m_CustomTypeName == "React") {
            AddTypeAttribute("framework", "React");
            AddTypeAttribute("jsx", "true");
        }
        // 示例：Angular类型的特殊标记
        else if (m_CustomTypeName == "Angular") {
            AddTypeAttribute("framework", "Angular");
            AddTypeAttribute("template", "true");
        }
    }
}

// OriginNodeFactory实现
std::unique_ptr<OriginNode> OriginNodeFactory::CreateBasicOriginDefinition(
    const std::string& typeName, 
    const std::string& originName, 
    const std::string& content,
    size_t line, size_t column) {
    
    OriginType type = OriginType::HTML_ORIGIN;
    
    if (typeName == "Html") {
        type = OriginType::HTML_ORIGIN;
    }
    else if (typeName == "Style") {
        type = OriginType::STYLE_ORIGIN;
    }
    else if (typeName == "JavaScript") {
        type = OriginType::JAVASCRIPT_ORIGIN;
    }
    
    return std::make_unique<OriginNode>(type, originName, content, line, column);
}

std::unique_ptr<OriginNode> OriginNodeFactory::CreateNamedOriginDefinition(
    const std::string& typeName,
    const std::string& originName,
    const std::string& namedContent,
    size_t line, size_t column) {
    
    auto originNode = CreateBasicOriginDefinition(typeName, originName, namedContent, line, column);
    // originNode->m_IsNamedOrigin = true; // 私有成员，需要通过公共方法设置
    
    // 带名原始嵌入的特殊处理
    originNode->AddTypeAttribute("named", "true");
    originNode->AddTypeAttribute("name", originName);
    
    return originNode;
}

std::unique_ptr<OriginNode> OriginNodeFactory::CreateCustomOriginDefinition(
    const std::string& customTypeName,
    const std::string& originName,
    const std::string& content,
    size_t line, size_t column) {
    
    return std::make_unique<OriginNode>(customTypeName, originName, content, line, column);
}

std::unique_ptr<OriginNode> OriginNodeFactory::CreateOriginUsage(
    const std::string& originName,
    size_t line, size_t column) {
    
    return std::make_unique<OriginNode>(originName, line, column);
}

} // namespace CHTL