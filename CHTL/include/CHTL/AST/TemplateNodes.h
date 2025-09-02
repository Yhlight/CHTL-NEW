#ifndef CHTL_CHTL_AST_TEMPLATENODES_H
#define CHTL_CHTL_AST_TEMPLATENODES_H

#include "CHTL/AST/ASTNode.h"

namespace CHTL {

class TemplateStyleNode : public ASTNode {
public:
    TemplateStyleNode() : ASTNode(ASTNodeType::TemplateStyle) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "TemplateStyleNode"; }
};

class TemplateElementNode : public ASTNode {
public:
    TemplateElementNode() : ASTNode(ASTNodeType::TemplateElement) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "TemplateElementNode"; }
};

class TemplateVarNode : public ASTNode {
public:
    TemplateVarNode() : ASTNode(ASTNodeType::TemplateVar) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "TemplateVarNode"; }
};

class CustomStyleNode : public ASTNode {
public:
    CustomStyleNode() : ASTNode(ASTNodeType::CustomStyle) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CustomStyleNode"; }
};

class CustomElementNode : public ASTNode {
public:
    CustomElementNode() : ASTNode(ASTNodeType::CustomElement) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CustomElementNode"; }
};

class CustomVarNode : public ASTNode {
public:
    CustomVarNode() : ASTNode(ASTNodeType::CustomVar) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CustomVarNode"; }
};

class OriginNode : public ASTNode {
public:
    OriginNode() : ASTNode(ASTNodeType::Origin) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "OriginNode"; }
};

class ImportNode : public ASTNode {
public:
    ImportNode() : ASTNode(ASTNodeType::Import) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "ImportNode"; }
};

class NamespaceNode : public ASTNode {
public:
    NamespaceNode() : ASTNode(ASTNodeType::Namespace) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "NamespaceNode"; }
};

class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode() : ASTNode(ASTNodeType::Configuration) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "ConfigurationNode"; }
};

class LocalStyleNode : public ASTNode {
public:
    LocalStyleNode() : ASTNode(ASTNodeType::LocalStyle) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "LocalStyleNode"; }
};

class LocalScriptNode : public ASTNode {
public:
    LocalScriptNode() : ASTNode(ASTNodeType::LocalScript) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "LocalScriptNode"; }
};

class StyleRuleNode : public ASTNode {
public:
    StyleRuleNode() : ASTNode(ASTNodeType::StyleRule) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "StyleRuleNode"; }
};

class StylePropertyNode : public ASTNode {
public:
    StylePropertyNode() : ASTNode(ASTNodeType::StyleProperty) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "StylePropertyNode"; }
};

class AttributeNode : public ASTNode {
public:
    AttributeNode() : ASTNode(ASTNodeType::Attribute) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "AttributeNode"; }
};

class SpecializationNode : public ASTNode {
public:
    SpecializationNode() : ASTNode(ASTNodeType::Specialization) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "SpecializationNode"; }
};

class DeleteNode : public ASTNode {
public:
    DeleteNode() : ASTNode(ASTNodeType::Delete) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "DeleteNode"; }
};

class InsertNode : public ASTNode {
public:
    InsertNode() : ASTNode(ASTNodeType::Insert) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "InsertNode"; }
};

class CommentNode : public ASTNode {
public:
    CommentNode() : ASTNode(ASTNodeType::Comment) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CommentNode"; }
};

} // namespace CHTL

#endif // CHTL_CHTL_AST_TEMPLATENODES_H