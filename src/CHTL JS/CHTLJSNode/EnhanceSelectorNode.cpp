#include "EnhanceSelectorNode.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTLJS {

EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector, size_t line, size_t column)
    : CHTLJSBaseNode(CHTLJSNodeType::ENHANCED_SELECTOR_NODE, "enhanced_selector", line, column),
      m_OriginalSelector(selector), m_Priority(SelectorPriority::ID_PRIORITY), 
      m_IndexAccess(SIZE_MAX), m_IsClassSelector(false), m_IsIdSelector(false), 
      m_IsTagSelector(false), m_IsDescendantSelector(false), m_HasIndexAccess(false), 
      m_IsAutoDetectSelector(false) {
    
    SetContent(selector);
    ParseSelectorContent();
    AnalyzeSelectorType();
    GenerateDOMQueryCode();
}

void EnhancedSelectorNode::ParseSelectorContent() {
    // 提取{{}}内的选择器内容
    if (m_OriginalSelector.length() >= 4 && 
        m_OriginalSelector.substr(0, 2) == "{{" && 
        m_OriginalSelector.substr(m_OriginalSelector.length() - 2) == "}}") {
        m_SelectorContent = m_OriginalSelector.substr(2, m_OriginalSelector.length() - 4);
    }
    else {
        m_SelectorContent = m_OriginalSelector;
    }
    
    // 清理空白字符
    m_SelectorContent.erase(0, m_SelectorContent.find_first_not_of(" \t"));
    m_SelectorContent.erase(m_SelectorContent.find_last_not_of(" \t") + 1);
}

void EnhancedSelectorNode::AnalyzeSelectorType() {
    if (m_SelectorContent.empty()) {
        return;
    }
    
    // 分析选择器类型
    if (m_SelectorContent.front() == '.') {
        // 类选择器：{{.box}}
        m_IsClassSelector = true;
        m_ClassName = ExtractClassName(m_SelectorContent);
        SetSelectorType("class");
    }
    else if (m_SelectorContent.front() == '#') {
        // ID选择器：{{#box}}
        m_IsIdSelector = true;
        m_IdName = ExtractIdName(m_SelectorContent);
        SetSelectorType("id");
    }
    else if (m_SelectorContent.find(' ') != std::string::npos) {
        // 后代选择器：{{.box button}}
        m_IsDescendantSelector = true;
        m_DescendantSelectors = ParseDescendantSelectors(m_SelectorContent);
        SetSelectorType("descendant");
    }
    else if (m_SelectorContent.find('[') != std::string::npos) {
        // 索引访问：{{button[0]}}
        m_HasIndexAccess = true;
        m_IndexAccess = ParseIndexAccess(m_SelectorContent);
        
        // 提取标签名
        size_t bracketPos = m_SelectorContent.find('[');
        if (bracketPos != std::string::npos) {
            m_TagName = m_SelectorContent.substr(0, bracketPos);
        }
        
        SetSelectorType("index");
    }
    else {
        // 自动检测选择器：{{box}}
        // 先判断是否为tag，然后查找类名/id（id优先）
        m_IsAutoDetectSelector = true;
        m_TagName = m_SelectorContent;
        m_Priority = SelectorPriority::ID_PRIORITY; // id优先
        SetSelectorType("auto");
    }
}

void EnhancedSelectorNode::GenerateDOMQueryCode() {
    std::ostringstream code;
    
    if (m_IsClassSelector) {
        code << "document.querySelector('" << m_SelectorContent << "')";
    }
    else if (m_IsIdSelector) {
        code << "document.getElementById('" << m_IdName << "')";
    }
    else if (m_IsDescendantSelector) {
        code << "document.querySelector('" << m_SelectorContent << "')";
    }
    else if (m_HasIndexAccess) {
        code << "document.getElementsByTagName('" << m_TagName << "')[" << m_IndexAccess << "]";
    }
    else if (m_IsAutoDetectSelector) {
        // 生成优先级查找代码
        code << GeneratePrioritySearchCode();
    }
    else {
        // 默认标签选择器
        code << "document.getElementsByTagName('" << m_SelectorContent << "')";
    }
    
    m_DOMQueryCode = code.str();
    SetGeneratedJavaScript(m_DOMQueryCode);
}

std::string EnhancedSelectorNode::GenerateDOMQuery() const {
    return m_DOMQueryCode;
}

std::string EnhancedSelectorNode::GeneratePrioritySearchCode() const {
    std::ostringstream code;
    
    // 生成优先级查找代码：先判断是否为tag，然后查找类名/id（id优先）
    code << "(function() { ";
    code << "var elem = document.getElementById('" << m_SelectorContent << "'); ";
    code << "if (elem) return elem; ";
    code << "elem = document.querySelector('." << m_SelectorContent << "'); ";
    code << "if (elem) return elem; ";
    code << "return document.getElementsByTagName('" << m_SelectorContent << "'); ";
    code << "})()";
    
    return code.str();
}

std::string EnhancedSelectorNode::GenerateClassSelectorQuery() const {
    return "document.querySelector('" + m_SelectorContent + "')";
}

std::string EnhancedSelectorNode::GenerateIdSelectorQuery() const {
    return "document.getElementById('" + m_IdName + "')";
}

std::string EnhancedSelectorNode::GenerateTagSelectorQuery() const {
    return "document.getElementsByTagName('" + m_TagName + "')";
}

std::string EnhancedSelectorNode::GenerateDescendantSelectorQuery() const {
    return "document.querySelector('" + m_SelectorContent + "')";
}

std::string EnhancedSelectorNode::GenerateIndexAccessQuery() const {
    return "document.getElementsByTagName('" + m_TagName + "')[" + std::to_string(m_IndexAccess) + "]";
}

std::string EnhancedSelectorNode::ExtractClassName(const std::string& content) const {
    if (content.front() == '.' && content.length() > 1) {
        return content.substr(1);
    }
    return "";
}

std::string EnhancedSelectorNode::ExtractIdName(const std::string& content) const {
    if (content.front() == '#' && content.length() > 1) {
        return content.substr(1);
    }
    return "";
}

std::vector<std::string> EnhancedSelectorNode::ParseDescendantSelectors(const std::string& content) const {
    std::vector<std::string> selectors;
    
    std::istringstream iss(content);
    std::string selector;
    
    while (iss >> selector) {
        if (!selector.empty()) {
            selectors.push_back(selector);
        }
    }
    
    return selectors;
}

size_t EnhancedSelectorNode::ParseIndexAccess(const std::string& content) const {
    std::regex indexPattern(R"(\[(\d+)\])");
    std::smatch match;
    
    if (std::regex_search(content, match, indexPattern)) {
        return std::stoul(match[1].str());
    }
    
    return SIZE_MAX;
}

bool EnhancedSelectorNode::ValidateSelectorFormat() const {
    // 验证增强选择器格式
    if (m_OriginalSelector.length() < 4) {
        return false;
    }
    
    if (m_OriginalSelector.substr(0, 2) != "{{" || 
        m_OriginalSelector.substr(m_OriginalSelector.length() - 2) != "}}") {
        return false;
    }
    
    if (m_SelectorContent.empty()) {
        return false;
    }
    
    // 验证选择器内容格式
    std::regex validSelectorPattern(R"([.#]?[a-zA-Z][a-zA-Z0-9\-_]*(?:\s+[a-zA-Z][a-zA-Z0-9\-_]*)*(?:\[\d+\])?)");
    return std::regex_match(m_SelectorContent, validSelectorPattern);
}

void EnhancedSelectorNode::Accept(CHTLJSNodeVisitor* visitor) {
    if (visitor) {
        // visitor->VisitEnhancedSelectorNode(this);
        // 暂时使用基础访问
    }
}

std::unique_ptr<CHTLJSBaseNode> EnhancedSelectorNode::Clone() const {
    auto cloned = std::make_unique<EnhancedSelectorNode>(m_OriginalSelector, GetLine(), GetColumn());
    
    // 复制所有属性
    cloned->m_SelectorContent = m_SelectorContent;
    cloned->m_DOMQueryCode = m_DOMQueryCode;
    cloned->m_Priority = m_Priority;
    cloned->m_ClassName = m_ClassName;
    cloned->m_IdName = m_IdName;
    cloned->m_TagName = m_TagName;
    cloned->m_DescendantSelectors = m_DescendantSelectors;
    cloned->m_IndexAccess = m_IndexAccess;
    cloned->m_IsClassSelector = m_IsClassSelector;
    cloned->m_IsIdSelector = m_IsIdSelector;
    cloned->m_IsTagSelector = m_IsTagSelector;
    cloned->m_IsDescendantSelector = m_IsDescendantSelector;
    cloned->m_HasIndexAccess = m_HasIndexAccess;
    cloned->m_IsAutoDetectSelector = m_IsAutoDetectSelector;
    
    return cloned;
}

std::string EnhancedSelectorNode::GenerateJavaScript() const {
    return m_DOMQueryCode;
}

std::string EnhancedSelectorNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "EnhancedSelectorNode '" << m_OriginalSelector << "'";
    
    if (m_IsClassSelector) {
        oss << " [Class: " << m_ClassName << "]";
    }
    
    if (m_IsIdSelector) {
        oss << " [ID: " << m_IdName << "]";
    }
    
    if (m_IsTagSelector) {
        oss << " [Tag: " << m_TagName << "]";
    }
    
    if (m_IsDescendantSelector) {
        oss << " [Descendant: " << m_DescendantSelectors.size() << " parts]";
    }
    
    if (m_HasIndexAccess) {
        oss << " [Index: " << m_IndexAccess << "]";
    }
    
    if (m_IsAutoDetectSelector) {
        oss << " [Auto-Detect]";
        switch (m_Priority) {
            case SelectorPriority::ID_PRIORITY:
                oss << " [ID Priority]";
                break;
            case SelectorPriority::CLASS_PRIORITY:
                oss << " [Class Priority]";
                break;
            case SelectorPriority::TAG_PRIORITY:
                oss << " [Tag Priority]";
                break;
        }
    }
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    // 输出DOM查询代码
    if (!m_DOMQueryCode.empty()) {
        oss << indentStr << "  DOM Query: " << m_DOMQueryCode << "\n";
    }
    
    // 输出后代选择器详情
    if (m_IsDescendantSelector && !m_DescendantSelectors.empty()) {
        oss << indentStr << "  Descendant Parts:\n";
        for (const auto& desc : m_DescendantSelectors) {
            oss << indentStr << "    " << desc << "\n";
        }
    }
    
    return oss.str();
}

} // namespace CHTLJS