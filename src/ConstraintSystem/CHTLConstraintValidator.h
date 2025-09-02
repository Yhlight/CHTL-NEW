#pragma once

#include "../CHTL/CHTLNode/CHTLBaseNode.h"
#include "../CHTL JS/CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace CHTL {

/**
 * 约束类型枚举
 * 定义不同类型的语法约束
 */
enum class ConstraintType {
    PRECISE_CONSTRAINT,     // 精准约束：某语法不能出现在某位置
    TYPE_CONSTRAINT,        // 类型约束：某类型语法的使用限制
    GLOBAL_CONSTRAINT       // 全局约束：全局范围的语法限制
};

/**
 * 约束规则结构
 * 定义具体的约束规则
 */
struct ConstraintRule {
    ConstraintType Type;                            // 约束类型
    std::string SourceSyntax;                       // 源语法（被约束的）
    std::string TargetContext;                      // 目标上下文（约束范围）
    std::string Description;                        // 约束描述
    bool IsEnabled;                                 // 是否启用
    
    ConstraintRule(ConstraintType type, const std::string& source, 
                  const std::string& target, const std::string& desc)
        : Type(type), SourceSyntax(source), TargetContext(target), 
          Description(desc), IsEnabled(true) {}
};

/**
 * 约束验证结果
 */
struct ConstraintValidationResult {
    bool IsValid;                                   // 是否通过验证
    std::vector<std::string> Violations;           // 违反的约束
    std::vector<std::string> Warnings;             // 警告信息
    
    ConstraintValidationResult() : IsValid(true) {}
};

/**
 * CHTL约束验证器
 * 负责验证CHTL和CHTL JS语法的边界和使用约束
 * 严格按照CHTL语法文档中的except约束规则
 */
class CHTLConstraintValidator {
private:
    std::vector<ConstraintRule> m_PreciseConstraints;       // 精准约束列表
    std::vector<ConstraintRule> m_TypeConstraints;          // 类型约束列表
    std::vector<ConstraintRule> m_GlobalConstraints;        // 全局约束列表
    
    // CHTL和CHTL JS语法边界定义
    std::unordered_set<std::string> m_CHTLOnlySyntax;       // 仅CHTL语法
    std::unordered_set<std::string> m_CHTLJSOnlySyntax;     // 仅CHTL JS语法
    std::unordered_set<std::string> m_SharedSyntax;         // 共享语法
    
    // 局部script特殊约束（属于CHTL但支持部分CHTL JS语法）
    std::unordered_set<std::string> m_LocalScriptAllowedCHTLSyntax;     // 局部script允许的CHTL语法
    std::unordered_set<std::string> m_LocalScriptAllowedCHTLJSSyntax;   // 局部script允许的CHTL JS语法
    
    // 上下文约束映射
    std::unordered_map<std::string, std::vector<std::string>> m_ContextConstraints;

public:
    /**
     * 构造函数
     */
    CHTLConstraintValidator();
    
    /**
     * 析构函数
     */
    ~CHTLConstraintValidator() = default;
    
    /**
     * 验证CHTL节点约束
     * @param node CHTL节点
     * @param context 上下文信息
     * @return 验证结果
     */
    ConstraintValidationResult ValidateCHTLNode(CHTLBaseNode* node, const std::string& context = "");
    
    /**
     * 验证CHTL JS节点约束
     * @param node CHTL JS节点
     * @param context 上下文信息
     * @return 验证结果
     */
    ConstraintValidationResult ValidateCHTLJSNode(CHTLJS::CHTLJSBaseNode* node, const std::string& context = "");
    
    /**
     * 验证语法边界
     * @param syntax 语法名称
     * @param targetLanguage 目标语言（CHTL/CHTL_JS）
     * @param context 上下文
     * @return 是否符合边界约束
     */
    bool ValidateSyntaxBoundary(const std::string& syntax, const std::string& targetLanguage, const std::string& context = "");
    
    /**
     * 验证局部script约束（特殊处理）
     * @param scriptContent 脚本内容
     * @return 验证结果
     */
    ConstraintValidationResult ValidateLocalScriptConstraints(const std::string& scriptContent);
    
    /**
     * 添加精准约束
     * @param sourceSyntax 源语法
     * @param targetContext 目标上下文
     * @param description 描述
     */
    void AddPreciseConstraint(const std::string& sourceSyntax, const std::string& targetContext, const std::string& description);
    
    /**
     * 添加类型约束
     * @param syntaxType 语法类型
     * @param constraint 约束描述
     * @param description 描述
     */
    void AddTypeConstraint(const std::string& syntaxType, const std::string& constraint, const std::string& description);
    
    /**
     * 添加全局约束
     * @param syntax 语法
     * @param constraint 约束描述
     * @param description 描述
     */
    void AddGlobalConstraint(const std::string& syntax, const std::string& constraint, const std::string& description);
    
    /**
     * 检查CHTL语法合法性
     * @param syntax 语法名称
     * @param context 上下文
     * @return 是否合法
     */
    bool IsCHTLSyntaxValid(const std::string& syntax, const std::string& context = "");
    
    /**
     * 检查CHTL JS语法合法性
     * @param syntax 语法名称
     * @param context 上下文
     * @return 是否合法
     */
    bool IsCHTLJSSyntaxValid(const std::string& syntax, const std::string& context = "");
    
    /**
     * 获取语法归属
     * @param syntax 语法名称
     * @return 归属语言（CHTL/CHTL_JS/SHARED/UNKNOWN）
     */
    std::string GetSyntaxOwnership(const std::string& syntax);
    
    /**
     * 重置约束验证器
     */
    void Reset();
    
    /**
     * 获取所有约束规则
     * @return 约束规则列表
     */
    std::vector<ConstraintRule> GetAllConstraints() const;

private:
    /**
     * 初始化默认约束规则
     */
    void InitializeDefaultConstraints();
    
    /**
     * 初始化CHTL语法集合
     */
    void InitializeCHTLSyntaxSets();
    
    /**
     * 初始化CHTL JS语法集合
     */
    void InitializeCHTLJSSyntaxSets();
    
    /**
     * 初始化局部script特殊约束
     */
    void InitializeLocalScriptConstraints();
    
    /**
     * 验证单个约束规则
     * @param rule 约束规则
     * @param syntax 语法
     * @param context 上下文
     * @return 是否违反约束
     */
    bool CheckConstraintRule(const ConstraintRule& rule, const std::string& syntax, const std::string& context);
    
    /**
     * 分析语法上下文
     * @param node 节点
     * @return 上下文信息
     */
    std::string AnalyzeNodeContext(CHTLBaseNode* node);
    
    /**
     * 分析CHTL JS语法上下文
     * @param node CHTL JS节点
     * @return 上下文信息
     */
    std::string AnalyzeCHTLJSNodeContext(CHTLJS::CHTLJSBaseNode* node);
    
    /**
     * 检查模板约束
     * @param templateType 模板类型
     * @param content 模板内容
     * @return 是否符合约束
     */
    bool CheckTemplateConstraints(const std::string& templateType, const std::string& content);
    
    /**
     * 检查自定义约束
     * @param customType 自定义类型
     * @param content 自定义内容
     * @return 是否符合约束
     */
    bool CheckCustomConstraints(const std::string& customType, const std::string& content);
    
    /**
     * 检查导入约束
     * @param importType 导入类型
     * @param importPath 导入路径
     * @return 是否符合约束
     */
    bool CheckImportConstraints(const std::string& importType, const std::string& importPath);
    
    /**
     * 检查命名空间约束
     * @param namespaceName 命名空间名称
     * @param context 上下文
     * @return 是否符合约束
     */
    bool CheckNamespaceConstraints(const std::string& namespaceName, const std::string& context);
    
    /**
     * 格式化约束违反信息
     * @param rule 约束规则
     * @param syntax 违反的语法
     * @param context 违反的上下文
     * @return 格式化的违反信息
     */
    std::string FormatConstraintViolation(const ConstraintRule& rule, const std::string& syntax, const std::string& context);
};

} // namespace CHTL