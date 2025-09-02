#include "CHTL/Core/ConstraintChecker.h"

namespace CHTL {

void ConstraintChecker::AddConstraint(const ExceptNode* exceptNode) {
    if (!exceptNode) return;
    
    for (const auto& constraint : exceptNode->GetConstraints()) {
        Constraint::Type type = Constraint::Precise;
        std::string targetName = constraint.value;
        bool isFullQualified = false;
        
        // 判断约束类型
        if (constraint.type == ExceptNode::ConstraintType::Type) {
            type = Constraint::TypeBased;
            
            // 如果有修饰符，添加到目标名称
            if (!constraint.modifier.empty()) {
                targetName = constraint.modifier + " " + targetName;
                isFullQualified = true;
            }
        }
        
        currentConstraints.emplace_back(type, targetName, isFullQualified);
        LOG_DEBUG("添加约束: " + targetName);
    }
}

void ConstraintChecker::ClearConstraints() {
    currentConstraints.clear();
}

bool ConstraintChecker::CheckNode(const std::shared_ptr<ASTNode>& node, std::string& errorMessage) {
    if (!node) return false;
    
    for (const auto& constraint : currentConstraints) {
        if (IsNodeViolatingConstraint(node, constraint)) {
            errorMessage = "违反约束: 不允许使用 " + constraint.target;
            return true;
        }
    }
    
    return false;
}

void ConstraintChecker::EnterScope() {
    scopeStack.push_back(currentConstraints);
}

void ConstraintChecker::ExitScope() {
    if (!scopeStack.empty()) {
        currentConstraints = scopeStack.back();
        scopeStack.pop_back();
    }
}

bool ConstraintChecker::IsNodeViolatingConstraint(const std::shared_ptr<ASTNode>& node, const Constraint& constraint) {
    if (!node) return false;
    
    if (constraint.type == Constraint::Precise) {
        // 精确约束
        if (node->GetType() == ASTNodeType::Element) {
            auto element = std::static_pointer_cast<ElementNode>(node);
            return element->GetTagName() == constraint.target;
        } else if (node->GetType() == ASTNodeType::CustomElement) {
            // CustomElement节点没有名称，精确约束不适用
            return false;
        } else if (node->GetType() == ASTNodeType::TemplateElement) {
            auto tmpl = std::static_pointer_cast<TemplateElementNode>(node);
            return tmpl->GetName() == constraint.target;
        }
    } else {
        // 类型约束
        std::string nodeTypeName = GetNodeTypeName(node);
        
        if (constraint.target == "@Html") {
            // 禁止HTML元素
            return node->GetType() == ASTNodeType::Element;
        } else if (constraint.target == "[Custom]") {
            // 禁止自定义类型
            if (constraint.isFullQualified) {
                // 根据具体的自定义类型判断
                if (nodeTypeName == "[Custom] @Element") {
                    return node->GetType() == ASTNodeType::CustomElement;
                } else if (nodeTypeName == "[Custom] @Style") {
                    return node->GetType() == ASTNodeType::CustomStyle;
                } else if (nodeTypeName == "[Custom] @Var") {
                    return node->GetType() == ASTNodeType::CustomVar;
                }
            } else {
                // 禁止所有自定义类型
                return node->GetType() == ASTNodeType::CustomElement ||
                       node->GetType() == ASTNodeType::CustomStyle ||
                       node->GetType() == ASTNodeType::CustomVar;
            }
        } else if (constraint.target == "[Template]") {
            // 禁止模板类型
            if (constraint.isFullQualified) {
                // 根据具体的模板类型判断
                if (nodeTypeName == "[Template] @Element") {
                    return node->GetType() == ASTNodeType::TemplateElement;
                } else if (nodeTypeName == "[Template] @Style") {
                    return node->GetType() == ASTNodeType::TemplateStyle;
                } else if (nodeTypeName == "[Template] @Var") {
                    return node->GetType() == ASTNodeType::TemplateVar;
                }
            } else {
                // 禁止所有模板类型
                return node->GetType() == ASTNodeType::TemplateElement ||
                       node->GetType() == ASTNodeType::TemplateStyle ||
                       node->GetType() == ASTNodeType::TemplateVar;
            }
        }
    }
    
    return false;
}

std::string ConstraintChecker::GetNodeTypeName(const std::shared_ptr<ASTNode>& node) {
    if (!node) return "";
    
    switch (node->GetType()) {
        case ASTNodeType::Element:
            return "@Html";
        case ASTNodeType::CustomElement:
            return "[Custom] @Element";
        case ASTNodeType::CustomStyle:
            return "[Custom] @Style";
        case ASTNodeType::CustomVar:
            return "[Custom] @Var";
        case ASTNodeType::TemplateElement:
            return "[Template] @Element";
        case ASTNodeType::TemplateStyle:
            return "[Template] @Style";
        case ASTNodeType::TemplateVar:
            return "[Template] @Var";
        default:
            return "";
    }
}

} // namespace CHTL