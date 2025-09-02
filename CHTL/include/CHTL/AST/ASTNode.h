#ifndef CHTL_CHTL_AST_ASTNODE_H
#define CHTL_CHTL_AST_ASTNODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 前向声明
class ASTVisitor;

/**
 * AST节点类型
 */
enum class ASTNodeType {
    // 根节点
    Program,
    
    // 声明节点
    TemplateStyle,
    TemplateElement,
    TemplateVar,
    CustomStyle,
    CustomElement,
    CustomVar,
    Origin,
    Import,
    Namespace,
    Configuration,
    
    // 元素节点
    Element,
    Text,
    Attribute,
    LocalStyle,
    LocalScript,
    
    // 样式节点
    StyleRule,
    StyleProperty,
    StyleSelector,
    
    // 表达式节点
    Identifier,
    StringLiteral,
    NumberLiteral,
    UnquotedLiteral,
    FunctionCall,
    MemberAccess,
    
    // 特例化节点
    Specialization,
    Delete,
    Insert,
    ElementMatch,
    
    // 其他
    Comment,
    Use,
    From,
    Except
};

/**
 * AST节点基类
 */
class ASTNode {
protected:
    ASTNodeType nodeType;
    size_t line;
    size_t column;
    std::vector<std::shared_ptr<ASTNode>> children;
    
public:
    ASTNode(ASTNodeType type, size_t l = 0, size_t c = 0)
        : nodeType(type), line(l), column(c) {}
    
    virtual ~ASTNode() = default;
    
    // 获取节点类型
    ASTNodeType GetType() const { return nodeType; }
    
    // 获取位置信息
    size_t GetLine() const { return line; }
    size_t GetColumn() const { return column; }
    
    // 子节点操作
    void AddChild(std::shared_ptr<ASTNode> child) {
        if (child) {
            children.push_back(child);
        }
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& GetChildren() const {
        return children;
    }
    
    std::vector<std::shared_ptr<ASTNode>>& GetChildren() {
        return children;
    }
    
    // 访问者模式
    virtual void Accept(ASTVisitor* visitor) = 0;
    
    // 获取节点类型字符串
    virtual std::string GetNodeTypeString() const;
    
    // 转换为字符串（用于调试）
    virtual std::string ToString() const = 0;
};

/**
 * 程序根节点
 */
class ProgramNode : public ASTNode {
private:
    std::string filename;
    bool useHTML5;
    
public:
    ProgramNode(const std::string& file = "")
        : ASTNode(ASTNodeType::Program), filename(file), useHTML5(false) {}
    
    void SetUseHTML5(bool use) { useHTML5 = use; }
    bool UseHTML5() const { return useHTML5; }
    
    const std::string& GetFilename() const { return filename; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
};

/**
 * 标识符节点
 */
class IdentifierNode : public ASTNode {
private:
    std::string name;
    
public:
    IdentifierNode(const std::string& n, size_t l = 0, size_t c = 0)
        : ASTNode(ASTNodeType::Identifier, l, c), name(n) {}
    
    const std::string& GetName() const { return name; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
};

/**
 * 字符串字面量节点
 */
class StringLiteralNode : public ASTNode {
private:
    std::string value;
    
public:
    StringLiteralNode(const std::string& v, size_t l = 0, size_t c = 0)
        : ASTNode(ASTNodeType::StringLiteral, l, c), value(v) {}
    
    const std::string& GetValue() const { return value; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
};

/**
 * 属性节点
 */
class AttributeNode : public ASTNode {
private:
    std::string attributeName;
    std::string attributeValue;
    
public:
    AttributeNode(const std::string& name = "", const std::string& value = "") 
        : ASTNode(ASTNodeType::Attribute), attributeName(name), attributeValue(value) {}
    
    const std::string& GetName() const { return attributeName; }
    const std::string& GetValue() const { return attributeValue; }
    
    void SetName(const std::string& name) { attributeName = name; }
    void SetValue(const std::string& value) { attributeValue = value; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "AttributeNode(" + attributeName + "=\"" + attributeValue + "\")"; 
    }
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
private:
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributeNodes;
    std::unordered_map<std::string, std::string> attributes; // 保留用于快速查找
    std::string id;
    std::string className;
    
public:
    ElementNode(const std::string& tag, size_t l = 0, size_t c = 0)
        : ASTNode(ASTNodeType::Element, l, c), tagName(tag) {}
    
    const std::string& GetTagName() const { return tagName; }
    
    void AddAttributeNode(std::shared_ptr<AttributeNode> attrNode) {
        if (attrNode) {
            attributeNodes.push_back(attrNode);
            const std::string& name = attrNode->GetName();
            const std::string& value = attrNode->GetValue();
            attributes[name] = value;
            if (name == "id") {
                id = value;
            } else if (name == "class") {
                className = value;
            }
        }
    }
    
    void SetAttribute(const std::string& name, const std::string& value) {
        auto attrNode = std::make_shared<AttributeNode>(name, value);
        AddAttributeNode(attrNode);
    }
    
    std::string GetAttribute(const std::string& name) const {
        auto it = attributes.find(name);
        return it != attributes.end() ? it->second : "";
    }
    
    const std::unordered_map<std::string, std::string>& GetAttributes() const {
        return attributes;
    }
    
    const std::vector<std::shared_ptr<AttributeNode>>& GetAttributeNodes() const {
        return attributeNodes;
    }
    
    const std::string& GetId() const { return id; }
    const std::string& GetClassName() const { return className; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
};

/**
 * 文本节点
 */
class TextNode : public ASTNode {
private:
    std::string content;
    
public:
    TextNode(const std::string& text = "", size_t l = 0, size_t c = 0)
        : ASTNode(ASTNodeType::Text, l, c), content(text) {}
    
    const std::string& GetContent() const { return content; }
    void SetContent(const std::string& text) { content = text; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
};

// 声明其他节点类（在后续文件中实现）
class TemplateStyleNode;
class TemplateElementNode;
class TemplateVarNode;
class CustomStyleNode;
class CustomElementNode;
class CustomVarNode;
class OriginNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class LocalStyleNode;
class LocalScriptNode;

} // namespace CHTL

#endif // CHTL_CHTL_AST_ASTNODE_H