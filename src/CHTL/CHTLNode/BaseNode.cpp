#include "BaseNode.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTL {

    // CHTLBaseNode实现
    CHTLBaseNode::CHTLBaseNode(CHTLNodeType type, const std::string& name, const std::string& value)
        : nodeType(type), nodeName(name), nodeValue(value), 
          startPos(0), endPos(0), lineNumber(0), columnNumber(0) {
    }

    // 基本访问方法实现
    CHTLNodeType CHTLBaseNode::GetNodeType() const {
        return nodeType;
    }

    const std::string& CHTLBaseNode::GetNodeName() const {
        return nodeName;
    }

    const std::string& CHTLBaseNode::GetNodeValue() const {
        return nodeValue;
    }

    void CHTLBaseNode::SetNodeName(const std::string& name) {
        nodeName = name;
    }

    void CHTLBaseNode::SetNodeValue(const std::string& value) {
        nodeValue = value;
    }

    // 位置信息方法实现
    void CHTLBaseNode::SetPosition(size_t start, size_t end, int line, int column) {
        startPos = start;
        endPos = end;
        lineNumber = line;
        columnNumber = column;
    }

    size_t CHTLBaseNode::GetStartPos() const {
        return startPos;
    }

    size_t CHTLBaseNode::GetEndPos() const {
        return endPos;
    }

    int CHTLBaseNode::GetLineNumber() const {
        return lineNumber;
    }

    int CHTLBaseNode::GetColumnNumber() const {
        return columnNumber;
    }

    // 子节点管理实现
    void CHTLBaseNode::AddChild(std::shared_ptr<CHTLBaseNode> child) {
        if (child) {
            children.push_back(child);
            // 暂时不设置父节点，避免shared_from_this问题
            // child->SetParent(std::weak_ptr<CHTLBaseNode>(shared_from_this()));
        }
    }

    void CHTLBaseNode::RemoveChild(std::shared_ptr<CHTLBaseNode> child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            (*it)->SetParent(std::weak_ptr<CHTLBaseNode>());
            children.erase(it);
        }
    }

    void CHTLBaseNode::InsertChild(size_t index, std::shared_ptr<CHTLBaseNode> child) {
        if (child && index <= children.size()) {
            children.insert(children.begin() + index, child);
            // 暂时不设置父节点，避免shared_from_this问题
            // child->SetParent(std::weak_ptr<CHTLBaseNode>(shared_from_this()));
        }
    }

    std::shared_ptr<CHTLBaseNode> CHTLBaseNode::GetChild(size_t index) const {
        return (index < children.size()) ? children[index] : nullptr;
    }

    size_t CHTLBaseNode::GetChildCount() const {
        return children.size();
    }

    const std::vector<std::shared_ptr<CHTLBaseNode>>& CHTLBaseNode::GetChildren() const {
        return children;
    }

    // 父节点管理实现
    void CHTLBaseNode::SetParent(std::weak_ptr<CHTLBaseNode> parentNode) {
        parent = parentNode;
    }

    std::shared_ptr<CHTLBaseNode> CHTLBaseNode::GetParent() const {
        return parent.lock();
    }

    bool CHTLBaseNode::HasParent() const {
        return !parent.expired();
    }

    // 属性管理实现
    void CHTLBaseNode::SetAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    std::string CHTLBaseNode::GetAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        return (it != attributes.end()) ? it->second : "";
    }

    bool CHTLBaseNode::HasAttribute(const std::string& key) const {
        return attributes.find(key) != attributes.end();
    }

    void CHTLBaseNode::RemoveAttribute(const std::string& key) {
        attributes.erase(key);
    }

    const std::unordered_map<std::string, std::string>& CHTLBaseNode::GetAllAttributes() const {
        return attributes;
    }

    // 查找方法实现
    std::vector<std::shared_ptr<CHTLBaseNode>> CHTLBaseNode::FindChildrenByType(CHTLNodeType type) const {
        std::vector<std::shared_ptr<CHTLBaseNode>> result;
        
        for (const auto& child : children) {
            if (child->GetNodeType() == type) {
                result.push_back(child);
            }
        }
        
        return result;
    }

    std::shared_ptr<CHTLBaseNode> CHTLBaseNode::FindChildByName(const std::string& name) const {
        for (const auto& child : children) {
            if (child->GetNodeName() == name) {
                return child;
            }
        }
        return nullptr;
    }

    std::shared_ptr<CHTLBaseNode> CHTLBaseNode::FindFirstChildByType(CHTLNodeType type) const {
        for (const auto& child : children) {
            if (child->GetNodeType() == type) {
                return child;
            }
        }
        return nullptr;
    }

    // 工具方法实现
    bool CHTLBaseNode::IsLeafNode() const {
        return children.empty();
    }

    int CHTLBaseNode::GetDepth() const {
        int depth = 0;
        auto currentParent = GetParent();
        
        while (currentParent) {
            depth++;
            currentParent = currentParent->GetParent();
        }
        
        return depth;
    }

    std::string CHTLBaseNode::GetPath() const {
        std::vector<std::string> pathParts;
        auto currentNode = shared_from_this();
        
        while (currentNode) {
            pathParts.insert(pathParts.begin(), currentNode->GetNodeName());
            currentNode = currentNode->GetParent();
        }
        
        std::stringstream path;
        for (size_t i = 0; i < pathParts.size(); i++) {
            if (i > 0) path << "/";
            path << pathParts[i];
        }
        
        return path.str();
    }

    // 虚方法默认实现
    std::string CHTLBaseNode::ToString() const {
        std::stringstream ss;
        ss << NodeTypeToString(nodeType);
        if (!nodeName.empty()) {
            ss << ":" << nodeName;
        }
        if (!nodeValue.empty()) {
            ss << "=" << nodeValue;
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLBaseNode::Clone() const {
        auto cloned = std::make_shared<CHTLBaseNode>(nodeType, nodeName, nodeValue);
        cloned->attributes = attributes;
        cloned->startPos = startPos;
        cloned->endPos = endPos;
        cloned->lineNumber = lineNumber;
        cloned->columnNumber = columnNumber;
        
        // 递归克隆子节点
        for (const auto& child : children) {
            auto clonedChild = child->Clone();
            cloned->AddChild(clonedChild);
        }
        
        return cloned;
    }

    bool CHTLBaseNode::Validate() const {
        // 基础验证：检查节点类型是否有效
        return nodeType != CHTLNodeType::BASE || !nodeName.empty();
    }

    void CHTLBaseNode::PrintTree(int indent) const {
        // 打印缩进
        for (int i = 0; i < indent; i++) {
            std::cout << "  ";
        }
        
        // 打印节点信息
        std::cout << "├─ " << NodeTypeToString(nodeType);
        if (!nodeName.empty()) {
            std::cout << " [" << nodeName << "]";
        }
        if (!nodeValue.empty()) {
            std::cout << " = \"" << nodeValue << "\"";
        }
        std::cout << " (行:" << lineNumber << ")" << std::endl;
        
        // 递归打印子节点
        for (const auto& child : children) {
            child->PrintTree(indent + 1);
        }
    }

    void CHTLBaseNode::PrintInfo() const {
        std::cout << "节点类型: " << NodeTypeToString(nodeType) << std::endl;
        std::cout << "节点名称: " << nodeName << std::endl;
        std::cout << "节点值: " << nodeValue << std::endl;
        std::cout << "位置: " << startPos << "-" << endPos << std::endl;
        std::cout << "行号: " << lineNumber << ", 列号: " << columnNumber << std::endl;
        std::cout << "子节点数量: " << children.size() << std::endl;
        std::cout << "属性数量: " << attributes.size() << std::endl;
    }

    // 静态工具方法实现
    std::string CHTLBaseNode::NodeTypeToString(CHTLNodeType type) {
        switch (type) {
            case CHTLNodeType::BASE: return "BASE";
            case CHTLNodeType::ELEMENT: return "ELEMENT";
            case CHTLNodeType::TEXT: return "TEXT";
            case CHTLNodeType::COMMENT: return "COMMENT";
            case CHTLNodeType::STYLE: return "STYLE";
            case CHTLNodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
            case CHTLNodeType::CSS_SELECTOR: return "CSS_SELECTOR";
            case CHTLNodeType::SCRIPT: return "SCRIPT";
            case CHTLNodeType::TEMPLATE: return "TEMPLATE";
            case CHTLNodeType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
            case CHTLNodeType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
            case CHTLNodeType::TEMPLATE_VAR: return "TEMPLATE_VAR";
            case CHTLNodeType::CUSTOM: return "CUSTOM";
            case CHTLNodeType::CUSTOM_STYLE: return "CUSTOM_STYLE";
            case CHTLNodeType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
            case CHTLNodeType::CUSTOM_VAR: return "CUSTOM_VAR";
            case CHTLNodeType::ORIGIN: return "ORIGIN";
            case CHTLNodeType::ORIGIN_HTML: return "ORIGIN_HTML";
            case CHTLNodeType::ORIGIN_STYLE: return "ORIGIN_STYLE";
            case CHTLNodeType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
            case CHTLNodeType::ORIGIN_CUSTOM: return "ORIGIN_CUSTOM";
            case CHTLNodeType::IMPORT: return "IMPORT";
            case CHTLNodeType::CONFIGURATION: return "CONFIGURATION";
            case CHTLNodeType::NAMESPACE: return "NAMESPACE";
            case CHTLNodeType::INHERIT: return "INHERIT";
            case CHTLNodeType::DELETE: return "DELETE";
            case CHTLNodeType::INSERT: return "INSERT";
            case CHTLNodeType::EXCEPT: return "EXCEPT";
            case CHTLNodeType::USE: return "USE";
            case CHTLNodeType::ATTRIBUTE: return "ATTRIBUTE";
            case CHTLNodeType::ATTRIBUTE_VALUE: return "ATTRIBUTE_VALUE";
            default: return "UNKNOWN";
        }
    }

    std::shared_ptr<CHTLBaseNode> CHTLBaseNode::CreateNode(CHTLNodeType type, const std::string& name, const std::string& value) {
        return std::make_shared<CHTLBaseNode>(type, name, value);
    }

    // CHTLNodeFactory实现
    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateElementNode(const std::string& tagName) {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::ELEMENT, tagName);
        return node;
    }

    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateTextNode(const std::string& text) {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::TEXT, "text", text);
        return node;
    }

    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateCommentNode(const std::string& comment) {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::COMMENT, "comment", comment);
        return node;
    }

    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateStyleNode() {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::STYLE, "style");
        return node;
    }

    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateStylePropertyNode(const std::string& property, const std::string& value) {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::STYLE_PROPERTY, property, value);
        return node;
    }

    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateSelectorNode(const std::string& selector) {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::CSS_SELECTOR, selector);
        return node;
    }

    std::shared_ptr<CHTLBaseNode> CHTLNodeFactory::CreateAttributeNode(const std::string& name, const std::string& value) {
        auto node = std::make_shared<CHTLBaseNode>(CHTLNodeType::ATTRIBUTE, name, value);
        return node;
    }

    bool CHTLNodeFactory::IsValidNodeType(CHTLNodeType type) {
        return type != CHTLNodeType::BASE; // BASE类型仅用于基类
    }

    bool CHTLNodeFactory::IsValidNodeName(const std::string& name) {
        if (name.empty()) return false;
        
        // 简单验证：字母开头，字母数字下划线组成
        if (!std::isalpha(name[0]) && name[0] != '_') {
            return false;
        }
        
        for (char c : name) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        
        return true;
    }

} // namespace CHTL