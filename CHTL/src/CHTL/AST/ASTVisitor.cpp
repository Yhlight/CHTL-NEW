#include "CHTL/AST/ASTVisitor.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"

namespace CHTL {

void DefaultASTVisitor::VisitChildren(ASTNode* node) {
    if (!node) return;
    
    for (auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
}

void DefaultASTVisitor::VisitProgram(ProgramNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitElement(ElementNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitTemplateStyle(TemplateStyleNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitTemplateElement(TemplateElementNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitTemplateVar(TemplateVarNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitCustomStyle(CustomStyleNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitCustomElement(CustomElementNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitCustomVar(CustomVarNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitOrigin(OriginNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitImport(ImportNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitNamespace(NamespaceNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitConfiguration(ConfigurationNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitLocalStyle(LocalStyleNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitLocalScript(LocalScriptNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitStyleRule(StyleRuleNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitStyleProperty(StylePropertyNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitAttribute(AttributeNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitSpecialization(SpecializationNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitDelete(DeleteNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitInsert(InsertNode* node) {
    VisitChildren(node);
}

} // namespace CHTL