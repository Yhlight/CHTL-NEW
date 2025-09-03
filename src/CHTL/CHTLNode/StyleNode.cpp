#include "StyleNode.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

    // CHTLStyleNode实现
    CHTLStyleNode::CHTLStyleNode(CHTLStyleNodeType type) 
        : CHTLBaseNode(CHTLNodeType::STYLE, "style"), 
          styleType(type), isLocal(type == CHTLStyleNodeType::LOCAL_STYLE),
          shouldAddToGlobal(false), hasAutoClass(false), hasAutoId(false) {
    }

    // 样式类型管理实现
    CHTLStyleNodeType CHTLStyleNode::GetStyleType() const {
        return styleType;
    }

    void CHTLStyleNode::SetStyleType(CHTLStyleNodeType type) {
        styleType = type;
        isLocal = (type == CHTLStyleNodeType::LOCAL_STYLE);
    }

    bool CHTLStyleNode::IsLocal() const {
        return isLocal;
    }

    void CHTLStyleNode::SetIsLocal(bool local) {
        isLocal = local;
        if (local) {
            styleType = CHTLStyleNodeType::LOCAL_STYLE;
        } else {
            styleType = CHTLStyleNodeType::GLOBAL_STYLE;
        }
    }

    // 全局样式块管理实现
    bool CHTLStyleNode::ShouldAddToGlobal() const {
        return shouldAddToGlobal;
    }

    void CHTLStyleNode::SetShouldAddToGlobal(bool shouldAdd) {
        shouldAddToGlobal = shouldAdd;
    }

    // 父元素关联实现
    const std::string& CHTLStyleNode::GetParentElementId() const {
        return parentElementId;
    }

    void CHTLStyleNode::SetParentElementId(const std::string& elementId) {
        parentElementId = elementId;
    }

    // 自动化类名/ID管理实现
    const std::string& CHTLStyleNode::GetAutoClassName() const {
        return autoClassName;
    }

    const std::string& CHTLStyleNode::GetAutoIdName() const {
        return autoIdName;
    }

    void CHTLStyleNode::SetAutoClassName(const std::string& className) {
        autoClassName = className;
        hasAutoClass = !className.empty();
    }

    void CHTLStyleNode::SetAutoIdName(const std::string& idName) {
        autoIdName = idName;
        hasAutoId = !idName.empty();
    }

    bool CHTLStyleNode::HasAutoClass() const {
        return hasAutoClass;
    }

    bool CHTLStyleNode::HasAutoId() const {
        return hasAutoId;
    }

    // 样式属性管理实现
    void CHTLStyleNode::AddStyleProperty(const std::string& property, const std::string& value) {
        SetAttribute(property, value);
    }

    void CHTLStyleNode::RemoveStyleProperty(const std::string& property) {
        RemoveAttribute(property);
    }

    std::string CHTLStyleNode::GetStyleProperty(const std::string& property) const {
        return GetAttribute(property);
    }

    bool CHTLStyleNode::HasStyleProperty(const std::string& property) const {
        return HasAttribute(property);
    }

    std::unordered_map<std::string, std::string> CHTLStyleNode::GetAllStyleProperties() const {
        return GetAllAttributes();
    }

    // CSS选择器管理实现
    std::vector<std::shared_ptr<CHTLBaseNode>> CHTLStyleNode::GetSelectorNodes() const {
        return FindChildrenByType(CHTLNodeType::CSS_SELECTOR);
    }

    std::shared_ptr<CHTLBaseNode> CHTLStyleNode::GetFirstClassSelector() const {
        // 查找第一个类选择器
        for (const auto& child : GetChildren()) {
            if (child->GetNodeType() == CHTLNodeType::CSS_SELECTOR) {
                auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(child);
                if (selectorNode && selectorNode->GetSelectorType() == CHTLSelectorType::CLASS_SELECTOR) {
                    return child;
                }
            }
        }
        return nullptr;
    }

    std::shared_ptr<CHTLBaseNode> CHTLStyleNode::GetFirstIdSelector() const {
        // 查找第一个ID选择器
        for (const auto& child : GetChildren()) {
            if (child->GetNodeType() == CHTLNodeType::CSS_SELECTOR) {
                auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(child);
                if (selectorNode && selectorNode->GetSelectorType() == CHTLSelectorType::ID_SELECTOR) {
                    return child;
                }
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<CHTLBaseNode>> CHTLStyleNode::GetContextReferences() const {
        std::vector<std::shared_ptr<CHTLBaseNode>> contextRefs;
        
        for (const auto& child : GetChildren()) {
            if (child->GetNodeType() == CHTLNodeType::CSS_SELECTOR) {
                auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(child);
                if (selectorNode && selectorNode->GetSelectorType() == CHTLSelectorType::CONTEXT_REFERENCE) {
                    contextRefs.push_back(child);
                }
            }
        }
        
        return contextRefs;
    }

    // 自动化处理实现
    void CHTLStyleNode::ProcessAutomation() {
        // 严格按照CHTL语法文档中的选择器自动化规则实现
        
        // 1. 检查是否有类选择器，如果有且没有class属性，自动添加第一个类选择器
        auto firstClassSelector = GetFirstClassSelector();
        if (firstClassSelector && !hasAutoClass) {
            auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(firstClassSelector);
            if (selectorNode) {
                std::string className = selectorNode->GetSelectorValue();
                // 移除.前缀
                if (!className.empty() && className[0] == '.') {
                    className = className.substr(1);
                }
                SetAutoClassName(className);
                SetShouldAddToGlobal(true); // 选择器代码需要添加到全局样式块
            }
        }
        
        // 2. 检查是否有ID选择器，如果有且没有id属性，自动添加第一个ID选择器
        auto firstIdSelector = GetFirstIdSelector();
        if (firstIdSelector && !hasAutoId) {
            auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(firstIdSelector);
            if (selectorNode) {
                std::string idName = selectorNode->GetSelectorValue();
                // 移除#前缀
                if (!idName.empty() && idName[0] == '#') {
                    idName = idName.substr(1);
                }
                SetAutoIdName(idName);
                SetShouldAddToGlobal(true); // 选择器代码需要添加到全局样式块
            }
        }
        
        // 3. 处理&引用选择器（局部style优先选择class）
        auto contextRefs = GetContextReferences();
        for (const auto& contextRef : contextRefs) {
            auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(contextRef);
            if (selectorNode) {
                // 局部style中，&优先选择class
                if (hasAutoClass) {
                    selectorNode->SetContextReference("." + autoClassName);
                } else if (hasAutoId) {
                    selectorNode->SetContextReference("#" + autoIdName);
                }
                SetShouldAddToGlobal(true);
            }
        }
    }

    void CHTLStyleNode::ApplyAutoClass() {
        if (!autoClassName.empty()) {
            // 将自动类名应用到父元素（通过GlobalMap处理）
            // 这里先记录，实际应用在解析器中完成
        }
    }

    void CHTLStyleNode::ApplyAutoId() {
        if (!autoIdName.empty()) {
            // 将自动ID名应用到父元素（通过GlobalMap处理）
            // 这里先记录，实际应用在解析器中完成
        }
    }

    // 生成方法实现
    std::string CHTLStyleNode::GenerateInlineCSS() const {
        std::stringstream ss;
        
        // 生成内联样式
        const auto& properties = GetAllStyleProperties();
        for (const auto& prop : properties) {
            ss << prop.first << ": " << prop.second << "; ";
        }
        
        return ss.str();
    }

    std::string CHTLStyleNode::GenerateGlobalCSS() const {
        std::stringstream ss;
        
        // 生成全局CSS（选择器样式）
        for (const auto& child : GetChildren()) {
            if (child->GetNodeType() == CHTLNodeType::CSS_SELECTOR) {
                auto selectorNode = std::dynamic_pointer_cast<CHTLSelectorNode>(child);
                if (selectorNode) {
                    ss << selectorNode->GenerateCSS() << "\n";
                }
            }
        }
        
        return ss.str();
    }

    // 重写虚方法实现
    std::string CHTLStyleNode::ToString() const {
        std::stringstream ss;
        ss << "Style[" << StyleTypeToString(styleType) << "]";
        if (isLocal) ss << " (local)";
        if (shouldAddToGlobal) ss << " (to-global)";
        if (hasAutoClass) ss << " .class=" << autoClassName;
        if (hasAutoId) ss << " #id=" << autoIdName;
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLStyleNode::Clone() const {
        auto cloned = std::make_shared<CHTLStyleNode>(styleType);
        
        cloned->isLocal = isLocal;
        cloned->shouldAddToGlobal = shouldAddToGlobal;
        cloned->parentElementId = parentElementId;
        cloned->autoClassName = autoClassName;
        cloned->autoIdName = autoIdName;
        cloned->hasAutoClass = hasAutoClass;
        cloned->hasAutoId = hasAutoId;
        
        // 递归克隆子节点
        for (const auto& child : GetChildren()) {
            cloned->AddChild(child->Clone());
        }
        
        return cloned;
    }

    bool CHTLStyleNode::Validate() const {
        // 验证样式节点
        if (styleType == CHTLStyleNodeType::LOCAL_STYLE && !isLocal) {
            return false;
        }
        
        return CHTLBaseNode::Validate();
    }

    std::string CHTLStyleNode::StyleTypeToString(CHTLStyleNodeType type) {
        switch (type) {
            case CHTLStyleNodeType::LOCAL_STYLE: return "LOCAL_STYLE";
            case CHTLStyleNodeType::GLOBAL_STYLE: return "GLOBAL_STYLE";
            case CHTLStyleNodeType::INLINE_STYLE: return "INLINE_STYLE";
            case CHTLStyleNodeType::CSS_SELECTOR: return "CSS_SELECTOR";
            case CHTLStyleNodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
            default: return "UNKNOWN";
        }
    }

    // CHTLSelectorNode实现
    CHTLSelectorNode::CHTLSelectorNode(CHTLSelectorType type, const std::string& value) 
        : CHTLBaseNode(CHTLNodeType::CSS_SELECTOR, "selector", value), 
          selectorType(type), selectorValue(value), isAutoGenerated(false) {
    }

    // 选择器类型管理实现
    CHTLSelectorType CHTLSelectorNode::GetSelectorType() const {
        return selectorType;
    }

    void CHTLSelectorNode::SetSelectorType(CHTLSelectorType type) {
        selectorType = type;
    }

    const std::string& CHTLSelectorNode::GetSelectorValue() const {
        return selectorValue;
    }

    void CHTLSelectorNode::SetSelectorValue(const std::string& value) {
        selectorValue = value;
        SetNodeValue(value);
    }

    // 上下文引用管理实现
    const std::string& CHTLSelectorNode::GetContextReference() const {
        return contextReference;
    }

    void CHTLSelectorNode::SetContextReference(const std::string& reference) {
        contextReference = reference;
    }

    void CHTLSelectorNode::ResolveContextReference(const std::string& className, const std::string& idName) {
        // 局部style中，&优先选择class
        if (!className.empty()) {
            contextReference = "." + className;
        } else if (!idName.empty()) {
            contextReference = "#" + idName;
        }
    }

    // 自动生成标记实现
    bool CHTLSelectorNode::IsAutoGenerated() const {
        return isAutoGenerated;
    }

    void CHTLSelectorNode::SetAutoGenerated(bool autoGen) {
        isAutoGenerated = autoGen;
    }

    // 伪类/伪元素管理实现
    const std::string& CHTLSelectorNode::GetPseudoClass() const {
        return pseudoClass;
    }

    const std::string& CHTLSelectorNode::GetPseudoElement() const {
        return pseudoElement;
    }

    void CHTLSelectorNode::SetPseudoClass(const std::string& pseudo) {
        pseudoClass = pseudo;
    }

    void CHTLSelectorNode::SetPseudoElement(const std::string& pseudo) {
        pseudoElement = pseudo;
    }

    bool CHTLSelectorNode::HasPseudoClass() const {
        return !pseudoClass.empty();
    }

    bool CHTLSelectorNode::HasPseudoElement() const {
        return !pseudoElement.empty();
    }

    // CSS生成方法实现
    std::string CHTLSelectorNode::GenerateCSS() const {
        std::stringstream ss;
        
        // 生成选择器
        if (selectorType == CHTLSelectorType::CONTEXT_REFERENCE && !contextReference.empty()) {
            ss << contextReference;
        } else {
            ss << selectorValue;
        }
        
        // 添加伪类
        if (HasPseudoClass()) {
            ss << ":" << pseudoClass;
        }
        
        // 添加伪元素
        if (HasPseudoElement()) {
            ss << "::" << pseudoElement;
        }
        
        ss << " {\n";
        
        // 生成样式属性
        for (const auto& child : GetChildren()) {
            if (child->GetNodeType() == CHTLNodeType::STYLE_PROPERTY) {
                auto propNode = std::dynamic_pointer_cast<CHTLStylePropertyNode>(child);
                if (propNode) {
                    ss << "    " << propNode->GenerateCSS() << "\n";
                }
            }
        }
        
        ss << "}";
        
        return ss.str();
    }

    std::string CHTLSelectorNode::GenerateCSSWithContext(const std::string& contextClass, const std::string& contextId) const {
        std::stringstream ss;
        
        // 根据上下文生成选择器
        if (selectorType == CHTLSelectorType::CONTEXT_REFERENCE) {
            // 局部style优先选择class
            if (!contextClass.empty()) {
                ss << "." << contextClass;
            } else if (!contextId.empty()) {
                ss << "#" << contextId;
            } else {
                ss << selectorValue; // 回退到原始值
            }
        } else {
            ss << selectorValue;
        }
        
        // 添加伪类和伪元素
        if (HasPseudoClass()) ss << ":" << pseudoClass;
        if (HasPseudoElement()) ss << "::" << pseudoElement;
        
        return ss.str();
    }

    // 验证方法实现
    bool CHTLSelectorNode::IsValidSelector() const {
        return IsValidCSSSelector(selectorValue);
    }

    // 重写虚方法实现
    std::string CHTLSelectorNode::ToString() const {
        std::stringstream ss;
        ss << "Selector[" << SelectorTypeToString(selectorType) << "]";
        ss << " \"" << selectorValue << "\"";
        if (!contextReference.empty()) ss << " -> \"" << contextReference << "\"";
        if (isAutoGenerated) ss << " (auto)";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLSelectorNode::Clone() const {
        auto cloned = std::make_shared<CHTLSelectorNode>(selectorType, selectorValue);
        
        cloned->contextReference = contextReference;
        cloned->isAutoGenerated = isAutoGenerated;
        cloned->pseudoClass = pseudoClass;
        cloned->pseudoElement = pseudoElement;
        
        // 递归克隆子节点
        for (const auto& child : GetChildren()) {
            cloned->AddChild(child->Clone());
        }
        
        return cloned;
    }

    bool CHTLSelectorNode::Validate() const {
        return IsValidSelector() && CHTLBaseNode::Validate();
    }

    // 静态工具方法实现
    std::string CHTLSelectorNode::SelectorTypeToString(CHTLSelectorType type) {
        switch (type) {
            case CHTLSelectorType::CLASS_SELECTOR: return "CLASS_SELECTOR";
            case CHTLSelectorType::ID_SELECTOR: return "ID_SELECTOR";
            case CHTLSelectorType::CONTEXT_REFERENCE: return "CONTEXT_REFERENCE";
            case CHTLSelectorType::PSEUDO_CLASS: return "PSEUDO_CLASS";
            case CHTLSelectorType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
            case CHTLSelectorType::TAG_SELECTOR: return "TAG_SELECTOR";
            case CHTLSelectorType::DESCENDANT_SELECTOR: return "DESCENDANT_SELECTOR";
            default: return "UNKNOWN";
        }
    }

    CHTLSelectorType CHTLSelectorNode::ParseSelectorType(const std::string& selector) {
        if (selector.empty()) return CHTLSelectorType::TAG_SELECTOR;
        
        if (selector[0] == '.') return CHTLSelectorType::CLASS_SELECTOR;
        if (selector[0] == '#') return CHTLSelectorType::ID_SELECTOR;
        if (selector[0] == '&') {
            if (selector.find("::") != std::string::npos) {
                return CHTLSelectorType::PSEUDO_ELEMENT;
            } else if (selector.find(":") != std::string::npos) {
                return CHTLSelectorType::PSEUDO_CLASS;
            } else {
                return CHTLSelectorType::CONTEXT_REFERENCE;
            }
        }
        if (selector.find(' ') != std::string::npos) {
            return CHTLSelectorType::DESCENDANT_SELECTOR;
        }
        
        return CHTLSelectorType::TAG_SELECTOR;
    }

    bool CHTLSelectorNode::IsValidCSSSelector(const std::string& selector) {
        if (selector.empty()) return false;
        
        // 基本CSS选择器验证
        std::regex selectorRegex(R"(^[.#&]?[a-zA-Z][a-zA-Z0-9_-]*(::[a-zA-Z][a-zA-Z0-9_-]*|:[a-zA-Z][a-zA-Z0-9_-]*)?$)");
        return std::regex_match(selector, selectorRegex);
    }

    // CHTLStylePropertyNode实现
    CHTLStylePropertyNode::CHTLStylePropertyNode(const std::string& property, const std::string& value) 
        : CHTLBaseNode(CHTLNodeType::STYLE_PROPERTY, property, value), 
          propertyName(property), propertyValue(value), isImportant(false), hasVariableReference(false) {
        
        // 检查是否包含!important
        if (value.find("!important") != std::string::npos) {
            isImportant = true;
        }
        
        // 检查是否包含变量引用
        if (value.find("${") != std::string::npos || value.find("ThemeColor(") != std::string::npos) {
            hasVariableReference = true;
        }
    }

    // 属性管理实现
    const std::string& CHTLStylePropertyNode::GetPropertyName() const {
        return propertyName;
    }

    const std::string& CHTLStylePropertyNode::GetPropertyValue() const {
        return propertyValue;
    }

    void CHTLStylePropertyNode::SetPropertyName(const std::string& property) {
        propertyName = property;
        SetNodeName(property);
    }

    void CHTLStylePropertyNode::SetPropertyValue(const std::string& value) {
        propertyValue = value;
        SetNodeValue(value);
        
        // 重新检查!important和变量引用
        isImportant = (value.find("!important") != std::string::npos);
        hasVariableReference = (value.find("${") != std::string::npos || value.find("ThemeColor(") != std::string::npos);
    }

    // 重要性标记实现
    bool CHTLStylePropertyNode::IsImportant() const {
        return isImportant;
    }

    void CHTLStylePropertyNode::SetImportant(bool important) {
        isImportant = important;
    }

    // 变量引用实现
    bool CHTLStylePropertyNode::HasVariableReference() const {
        return hasVariableReference;
    }

    void CHTLStylePropertyNode::SetHasVariableReference(bool hasVar) {
        hasVariableReference = hasVar;
    }

    // CSS生成实现
    std::string CHTLStylePropertyNode::GenerateCSS() const {
        std::stringstream ss;
        ss << propertyName << ": " << propertyValue;
        if (isImportant) ss << " !important";
        ss << ";";
        return ss.str();
    }

    // 重写虚方法实现
    std::string CHTLStylePropertyNode::ToString() const {
        std::stringstream ss;
        ss << "Property[" << propertyName << ": " << propertyValue << "]";
        if (isImportant) ss << " !important";
        if (hasVariableReference) ss << " (has vars)";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLStylePropertyNode::Clone() const {
        auto cloned = std::make_shared<CHTLStylePropertyNode>(propertyName, propertyValue);
        cloned->isImportant = isImportant;
        cloned->hasVariableReference = hasVariableReference;
        return cloned;
    }

    bool CHTLStylePropertyNode::Validate() const {
        return IsValidCSSProperty(propertyName) && IsValidCSSValue(propertyValue);
    }

    // 静态验证方法实现
    bool CHTLStylePropertyNode::IsValidCSSProperty(const std::string& property) {
        if (property.empty()) return false;
        
        // 基本CSS属性名验证
        std::regex propRegex(R"(^[a-zA-Z][a-zA-Z0-9_-]*$)");
        return std::regex_match(property, propRegex);
    }

    bool CHTLStylePropertyNode::IsValidCSSValue(const std::string& value) {
        if (value.empty()) return false;
        
        // CSS值基本验证（允许几乎所有内容）
        return true;
    }

    // CHTLStyleNodeFactory实现
    std::shared_ptr<CHTLStyleNode> CHTLStyleNodeFactory::CreateLocalStyleNode() {
        return std::make_shared<CHTLStyleNode>(CHTLStyleNodeType::LOCAL_STYLE);
    }

    std::shared_ptr<CHTLStyleNode> CHTLStyleNodeFactory::CreateGlobalStyleNode() {
        return std::make_shared<CHTLStyleNode>(CHTLStyleNodeType::GLOBAL_STYLE);
    }

    std::shared_ptr<CHTLSelectorNode> CHTLStyleNodeFactory::CreateClassSelector(const std::string& className) {
        std::string selectorValue = (className[0] == '.') ? className : "." + className;
        return std::make_shared<CHTLSelectorNode>(CHTLSelectorType::CLASS_SELECTOR, selectorValue);
    }

    std::shared_ptr<CHTLSelectorNode> CHTLStyleNodeFactory::CreateIdSelector(const std::string& idName) {
        std::string selectorValue = (idName[0] == '#') ? idName : "#" + idName;
        return std::make_shared<CHTLSelectorNode>(CHTLSelectorType::ID_SELECTOR, selectorValue);
    }

    std::shared_ptr<CHTLSelectorNode> CHTLStyleNodeFactory::CreateContextReference() {
        return std::make_shared<CHTLSelectorNode>(CHTLSelectorType::CONTEXT_REFERENCE, "&");
    }

    std::shared_ptr<CHTLSelectorNode> CHTLStyleNodeFactory::CreatePseudoClassSelector(const std::string& pseudoClass) {
        std::string selectorValue = "&:" + pseudoClass;
        auto node = std::make_shared<CHTLSelectorNode>(CHTLSelectorType::PSEUDO_CLASS, selectorValue);
        node->SetPseudoClass(pseudoClass);
        return node;
    }

    std::shared_ptr<CHTLSelectorNode> CHTLStyleNodeFactory::CreatePseudoElementSelector(const std::string& pseudoElement) {
        std::string selectorValue = "&::" + pseudoElement;
        auto node = std::make_shared<CHTLSelectorNode>(CHTLSelectorType::PSEUDO_ELEMENT, selectorValue);
        node->SetPseudoElement(pseudoElement);
        return node;
    }

    std::shared_ptr<CHTLStylePropertyNode> CHTLStyleNodeFactory::CreateStyleProperty(const std::string& property, const std::string& value) {
        return std::make_shared<CHTLStylePropertyNode>(property, value);
    }

    CHTLSelectorType CHTLStyleNodeFactory::ParseSelectorFromText(const std::string& text) {
        return CHTLSelectorNode::ParseSelectorType(text);
    }

    std::shared_ptr<CHTLSelectorNode> CHTLStyleNodeFactory::ParseSelector(const std::string& selectorText) {
        CHTLSelectorType type = ParseSelectorFromText(selectorText);
        return std::make_shared<CHTLSelectorNode>(type, selectorText);
    }

} // namespace CHTL