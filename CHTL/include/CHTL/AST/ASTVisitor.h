#ifndef CHTL_CHTL_AST_ASTVISITOR_H
#define CHTL_CHTL_AST_ASTVISITOR_H

namespace CHTL {

// 前向声明
class ASTNode;

// 前向声明所有节点类型
class ProgramNode;
class IdentifierNode;
class StringLiteralNode;
class ElementNode;
class TextNode;
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
class StyleRuleNode;
class StylePropertyNode;
class AttributeNode;
class SpecializationNode;
class DeleteNode;
class InsertNode;
class ElementMatchNode;
class CommentNode;

/**
 * AST访问者接口
 * 使用访问者模式遍历和处理AST
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 访问各种节点类型
    virtual void VisitProgram(ProgramNode* node) = 0;
    virtual void VisitIdentifier(IdentifierNode* node) = 0;
    virtual void VisitStringLiteral(StringLiteralNode* node) = 0;
    virtual void VisitElement(ElementNode* node) = 0;
    virtual void VisitText(TextNode* node) = 0;
    virtual void VisitTemplateStyle(TemplateStyleNode* node) = 0;
    virtual void VisitTemplateElement(TemplateElementNode* node) = 0;
    virtual void VisitTemplateVar(TemplateVarNode* node) = 0;
    virtual void VisitCustomStyle(CustomStyleNode* node) = 0;
    virtual void VisitCustomElement(CustomElementNode* node) = 0;
    virtual void VisitCustomVar(CustomVarNode* node) = 0;
    virtual void VisitOrigin(OriginNode* node) = 0;
    virtual void VisitImport(ImportNode* node) = 0;
    virtual void VisitNamespace(NamespaceNode* node) = 0;
    virtual void VisitConfiguration(ConfigurationNode* node) = 0;
    virtual void VisitLocalStyle(LocalStyleNode* node) = 0;
    virtual void VisitLocalScript(LocalScriptNode* node) = 0;
    virtual void VisitStyleRule(StyleRuleNode* node) = 0;
    virtual void VisitStyleProperty(StylePropertyNode* node) = 0;
    virtual void VisitAttribute(AttributeNode* node) = 0;
    virtual void VisitSpecialization(SpecializationNode* node) = 0;
    virtual void VisitDelete(DeleteNode* node) = 0;
    virtual void VisitInsert(InsertNode* node) = 0;
    virtual void VisitElementMatch(ElementMatchNode* node) = 0;
    virtual void VisitComment(CommentNode* node) = 0;
};

/**
 * 默认AST访问者实现
 * 提供默认的遍历行为
 */
class DefaultASTVisitor : public ASTVisitor {
public:
    void VisitProgram(ProgramNode* node) override;
    void VisitIdentifier(IdentifierNode* node) override {}
    void VisitStringLiteral(StringLiteralNode* node) override {}
    void VisitElement(ElementNode* node) override;
    void VisitText(TextNode* node) override {}
    void VisitTemplateStyle(TemplateStyleNode* node) override;
    void VisitTemplateElement(TemplateElementNode* node) override;
    void VisitTemplateVar(TemplateVarNode* node) override;
    void VisitCustomStyle(CustomStyleNode* node) override;
    void VisitCustomElement(CustomElementNode* node) override;
    void VisitCustomVar(CustomVarNode* node) override;
    void VisitOrigin(OriginNode* node) override;
    void VisitImport(ImportNode* node) override;
    void VisitNamespace(NamespaceNode* node) override;
    void VisitConfiguration(ConfigurationNode* node) override;
    void VisitLocalStyle(LocalStyleNode* node) override;
    void VisitLocalScript(LocalScriptNode* node) override;
    void VisitStyleRule(StyleRuleNode* node) override;
    void VisitStyleProperty(StylePropertyNode* node) override;
    void VisitAttribute(AttributeNode* node) override;
    void VisitSpecialization(SpecializationNode* node) override;
    void VisitDelete(DeleteNode* node) override;
    void VisitInsert(InsertNode* node) override;
    void VisitComment(CommentNode* node) override {}
    
protected:
    // 遍历子节点的辅助方法
    void VisitChildren(ASTNode* node);
};

} // namespace CHTL

#endif // CHTL_CHTL_AST_ASTVISITOR_H