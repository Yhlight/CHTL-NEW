#ifndef CHTL_CORE_CONSTRAINTCHECKER_H
#define CHTL_CORE_CONSTRAINTCHECKER_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"

namespace CHTL {

/**
 * 约束检查器
 * 负责检查except约束是否被违反
 */
class ConstraintChecker {
private:
    // 当前活动的约束
    struct Constraint {
        enum Type {
            Precise,      // 精确约束（特定元素/模板）
            TypeBased     // 类型约束（@Html, [Custom], [Template]等）
        };
        
        Type type;
        std::string target;  // 对于精确约束是元素名，对于类型约束是类型名
        bool isFullQualified;  // 是否是全缀名
        
        Constraint(Type t, const std::string& tgt, bool fq = false) 
            : type(t), target(tgt), isFullQualified(fq) {}
    };
    
    std::vector<Constraint> currentConstraints;
    
    /**
     * 检查节点是否违反约束
     */
    bool IsNodeViolatingConstraint(const std::shared_ptr<ASTNode>& node, const Constraint& constraint);
    
    /**
     * 获取节点的类型名称（用于类型约束检查）
     */
    std::string GetNodeTypeName(const std::shared_ptr<ASTNode>& node);

public:
    /**
     * 添加约束
     */
    void AddConstraint(const ExceptNode* exceptNode);
    
    /**
     * 清除所有约束
     */
    void ClearConstraints();
    
    /**
     * 检查节点是否违反当前约束
     * @return 如果违反约束返回true，否则返回false
     */
    bool CheckNode(const std::shared_ptr<ASTNode>& node, std::string& errorMessage);
    
    /**
     * 进入新的作用域（保存当前约束）
     */
    void EnterScope();
    
    /**
     * 退出作用域（恢复之前的约束）
     */
    void ExitScope();
    
private:
    // 作用域栈，用于保存每个作用域的约束
    std::vector<std::vector<Constraint>> scopeStack;
};

} // namespace CHTL

#endif // CHTL_CORE_CONSTRAINTCHECKER_H