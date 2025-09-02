#include "CHTL/AST/TemplateNodes.h"
#include "CHTL/AST/ASTVisitor.h"

namespace CHTL {

void TemplateStyleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplateStyle(this);
}

void TemplateElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplateElement(this);
}

void TemplateVarNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplateVar(this);
}

void CustomStyleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustomStyle(this);
}

void CustomElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustomElement(this);
}

void CustomVarNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustomVar(this);
}

void OriginNode::Accept(ASTVisitor* visitor) {
    visitor->VisitOrigin(this);
}



void NamespaceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitNamespace(this);
}

void DeleteNode::Accept(ASTVisitor* visitor) {
    // TODO: 实现DeleteNode的visitor模式
}

void InsertNode::Accept(ASTVisitor* visitor) {
    // TODO: 实现InsertNode的visitor模式
}

void ElementMatchNode::Accept(ASTVisitor* visitor) {
    // TODO: 实现ElementMatchNode的visitor模式
}

void ExceptNode::Accept(ASTVisitor* visitor) {
    // visitor->VisitExcept(this);
    // TODO: 当ASTVisitor添加VisitExcept方法时启用
}

void ConfigurationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitConfiguration(this);
}

void LocalStyleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitLocalStyle(this);
}

void LocalScriptNode::Accept(ASTVisitor* visitor) {
    visitor->VisitLocalScript(this);
}

void ImportNode::Accept(ASTVisitor* visitor) {
    // visitor->VisitImport(this);
    // TODO: 当ASTVisitor添加VisitImport方法时启用
}

void StyleRuleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleRule(this);
}

void StylePropertyNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleProperty(this);
}

void AttributeNode::Accept(ASTVisitor* visitor) {
    visitor->VisitAttribute(this);
}

void SpecializationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitSpecialization(this);
}



void CommentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitComment(this);
}

} // namespace CHTL