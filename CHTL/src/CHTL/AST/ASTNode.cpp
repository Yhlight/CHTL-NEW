#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/ASTVisitor.h"

namespace CHTL {

std::string ASTNode::GetNodeTypeString() const {
    switch (nodeType) {
        case ASTNodeType::Program: return "Program";
        case ASTNodeType::Element: return "Element";
        case ASTNodeType::Text: return "Text";
        case ASTNodeType::Identifier: return "Identifier";
        case ASTNodeType::StringLiteral: return "StringLiteral";
        default: return "Unknown";
    }
}

void ProgramNode::Accept(ASTVisitor* visitor) {
    visitor->VisitProgram(this);
}

std::string ProgramNode::ToString() const {
    return "ProgramNode(" + filename + ")";
}

void IdentifierNode::Accept(ASTVisitor* visitor) {
    visitor->VisitIdentifier(this);
}

std::string IdentifierNode::ToString() const {
    return "IdentifierNode(" + name + ")";
}

void StringLiteralNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStringLiteral(this);
}

std::string StringLiteralNode::ToString() const {
    return "StringLiteralNode(\"" + value + "\")";
}

void ElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitElement(this);
}

std::string ElementNode::ToString() const {
    return "ElementNode(<" + tagName + ">)";
}

void TextNode::Accept(ASTVisitor* visitor) {
    visitor->VisitText(this);
}

std::string TextNode::ToString() const {
    return "TextNode(\"" + content + "\")";
}

} // namespace CHTL