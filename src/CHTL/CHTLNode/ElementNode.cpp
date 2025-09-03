#include "ElementNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

    // 静态成员初始化
    std::unordered_set<std::string> CHTLElementNode::selfClosingTags;

    void CHTLElementNode::InitializeSelfClosingTags() {
        if (selfClosingTags.empty()) {
            selfClosingTags = {
                "area", "base", "br", "col", "embed", "hr", "img", "input",
                "link", "meta", "param", "source", "track", "wbr"
            };
        }
    }

    CHTLElementNode::CHTLElementNode(const std::string& tag) 
        : CHTLBaseNode(CHTLNodeType::ELEMENT, tag), tagName(tag), 
          isSelfClosing(false), hasLocalStyle(false), hasLocalScript(false) {
        InitializeSelfClosingTags();
        isSelfClosing = (selfClosingTags.find(tag) != selfClosingTags.end());
    }

    // 标签管理实现
    const std::string& CHTLElementNode::GetTagName() const {
        return tagName;
    }

    void CHTLElementNode::SetTagName(const std::string& tag) {
        tagName = tag;
        SetNodeName(tag);
        isSelfClosing = (selfClosingTags.find(tag) != selfClosingTags.end());
    }

    bool CHTLElementNode::IsSelfClosing() const {
        return isSelfClosing;
    }

    // HTML属性管理实现
    void CHTLElementNode::SetHTMLAttribute(const std::string& name, const std::string& value) {
        htmlAttributes[name] = value;
        SetAttribute(name, value); // 同时设置基类属性
    }

    std::string CHTLElementNode::GetHTMLAttribute(const std::string& name) const {
        auto it = htmlAttributes.find(name);
        return (it != htmlAttributes.end()) ? it->second : "";
    }

    bool CHTLElementNode::HasHTMLAttribute(const std::string& name) const {
        return htmlAttributes.find(name) != htmlAttributes.end();
    }

    void CHTLElementNode::RemoveHTMLAttribute(const std::string& name) {
        htmlAttributes.erase(name);
        RemoveAttribute(name); // 同时移除基类属性
    }

    const std::unordered_map<std::string, std::string>& CHTLElementNode::GetAllHTMLAttributes() const {
        return htmlAttributes;
    }

    // 样式和脚本管理实现
    void CHTLElementNode::SetHasLocalStyle(bool hasStyle) {
        hasLocalStyle = hasStyle;
    }

    void CHTLElementNode::SetHasLocalScript(bool hasScript) {
        hasLocalScript = hasScript;
    }

    bool CHTLElementNode::HasLocalStyle() const {
        return hasLocalStyle;
    }

    bool CHTLElementNode::HasLocalScript() const {
        return hasLocalScript;
    }

    // 自动化管理实现
    void CHTLElementNode::SetAutoClassName(const std::string& className) {
        autoClassName = className;
        if (!className.empty()) {
            SetHTMLAttribute("class", className);
        }
    }

    void CHTLElementNode::SetAutoIdName(const std::string& idName) {
        autoIdName = idName;
        if (!idName.empty()) {
            SetHTMLAttribute("id", idName);
        }
    }

    const std::string& CHTLElementNode::GetAutoClassName() const {
        return autoClassName;
    }

    const std::string& CHTLElementNode::GetAutoIdName() const {
        return autoIdName;
    }

    bool CHTLElementNode::HasAutoClassName() const {
        return !autoClassName.empty();
    }

    bool CHTLElementNode::HasAutoIdName() const {
        return !autoIdName.empty();
    }

    // 子节点特化管理实现
    std::shared_ptr<CHTLBaseNode> CHTLElementNode::GetStyleNode() const {
        return FindFirstChildByType(CHTLNodeType::STYLE);
    }

    std::shared_ptr<CHTLBaseNode> CHTLElementNode::GetScriptNode() const {
        return FindFirstChildByType(CHTLNodeType::SCRIPT);
    }

    std::vector<std::shared_ptr<CHTLBaseNode>> CHTLElementNode::GetTextNodes() const {
        return FindChildrenByType(CHTLNodeType::TEXT);
    }

    std::vector<std::shared_ptr<CHTLBaseNode>> CHTLElementNode::GetElementNodes() const {
        return FindChildrenByType(CHTLNodeType::ELEMENT);
    }

    // 重写虚方法实现
    std::string CHTLElementNode::ToString() const {
        std::stringstream ss;
        ss << "Element[" << tagName << "]";
        if (HasAutoClassName()) {
            ss << " .class=" << autoClassName;
        }
        if (HasAutoIdName()) {
            ss << " #id=" << autoIdName;
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLElementNode::Clone() const {
        auto cloned = std::make_shared<CHTLElementNode>(tagName);
        
        // 复制基类数据
        cloned->SetNodeValue(GetNodeValue());
        cloned->htmlAttributes = htmlAttributes;
        cloned->hasLocalStyle = hasLocalStyle;
        cloned->hasLocalScript = hasLocalScript;
        cloned->autoClassName = autoClassName;
        cloned->autoIdName = autoIdName;
        
        // 递归克隆子节点
        for (const auto& child : GetChildren()) {
            cloned->AddChild(child->Clone());
        }
        
        return cloned;
    }

    bool CHTLElementNode::Validate() const {
        // 验证标签名是否有效
        if (!IsValidHTMLTag(tagName)) {
            return false;
        }
        
        // 验证自闭合标签不应该有子节点
        if (isSelfClosing && GetChildCount() > 0) {
            return false;
        }
        
        return CHTLBaseNode::Validate();
    }

    // HTML生成方法实现
    std::string CHTLElementNode::GenerateOpeningTag() const {
        std::stringstream ss;
        ss << "<" << tagName;
        
        // 添加HTML属性
        for (const auto& attr : htmlAttributes) {
            ss << " " << attr.first << "=\"" << attr.second << "\"";
        }
        
        if (isSelfClosing) {
            ss << " />";
        } else {
            ss << ">";
        }
        
        return ss.str();
    }

    std::string CHTLElementNode::GenerateClosingTag() const {
        if (isSelfClosing) {
            return "";
        }
        return "</" + tagName + ">";
    }

    std::string CHTLElementNode::GenerateHTML() const {
        std::stringstream ss;
        ss << GenerateOpeningTag();
        
        if (!isSelfClosing) {
            // 生成子节点内容
            for (const auto& child : GetChildren()) {
                if (child->GetNodeType() == CHTLNodeType::TEXT) {
                    ss << child->GetNodeValue();
                } else if (child->GetNodeType() == CHTLNodeType::ELEMENT) {
                    // 递归生成子元素
                    auto elementChild = std::dynamic_pointer_cast<CHTLElementNode>(child);
                    if (elementChild) {
                        ss << elementChild->GenerateHTML();
                    }
                }
            }
            
            ss << GenerateClosingTag();
        }
        
        return ss.str();
    }

    // 工具方法实现
    bool CHTLElementNode::IsBlockElement() const {
        static std::unordered_set<std::string> blockElements = {
            "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "ul", "ol", "li",
            "table", "tr", "td", "th", "form", "header", "footer", "main",
            "section", "article", "aside", "nav", "figure", "blockquote"
        };
        return blockElements.find(tagName) != blockElements.end();
    }

    bool CHTLElementNode::IsInlineElement() const {
        static std::unordered_set<std::string> inlineElements = {
            "span", "a", "strong", "em", "small", "mark", "del", "ins", "sub", "sup",
            "img", "input", "button", "select", "textarea", "label", "code", "kbd"
        };
        return inlineElements.find(tagName) != inlineElements.end();
    }

    bool CHTLElementNode::IsVoidElement() const {
        return isSelfClosing;
    }

    bool CHTLElementNode::IsValidHTMLTag(const std::string& tag) {
        if (tag.empty()) return false;
        
        // 简单验证：字母开头，字母数字连字符组成
        if (!std::isalpha(tag[0])) return false;
        
        for (char c : tag) {
            if (!std::isalnum(c) && c != '-') {
                return false;
            }
        }
        
        return true;
    }

} // namespace CHTL