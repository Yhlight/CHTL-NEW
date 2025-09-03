#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

/**
 * 约束类型枚举
 */
enum class ConstraintType {
    PRECISE_CONSTRAINT,     // 精确约束 (具体元素/对象)
    TYPE_CONSTRAINT,        // 类型约束 (@Html, [Template], [Custom])
    GLOBAL_CONSTRAINT       // 全局约束 (命名空间级别)
};

/**
 * 约束范围枚举
 */
enum class ConstraintScope {
    ELEMENT_SCOPE,          // 元素范围
    NAMESPACE_SCOPE,        // 命名空间范围
    FILE_SCOPE,             // 文件范围
    GLOBAL_SCOPE            // 全局范围
};

/**
 * 约束信息结构
 */
struct ConstraintInfo {
    ConstraintType Type;                        // 约束类型
    ConstraintScope Scope;                      // 约束范围
    std::string Target;                         // 约束目标
    std::string Context;                        // 约束上下文
    std::string Expression;                     // 约束表达式
    std::vector<std::string> Parameters;       // 约束参数
    bool IsActive = true;                       // 是否激活
    std::string SourceLocation;                // 来源位置
};

/**
 * 约束冲突信息
 */
struct ConstraintConflict {
    std::string ConflictTarget;                 // 冲突目标
    std::vector<ConstraintInfo> ConflictingConstraints; // 冲突的约束
    std::string Resolution;                     // 解决方案
    std::string ConflictReason;                 // 冲突原因
};

/**
 * 高级约束验证器
 * 实现完整的CHTL约束系统
 * 支持精确约束、类型约束、全局约束等所有约束特性
 */
class AdvancedConstraintValidator {
private:
    // 约束存储
    std::vector<ConstraintInfo> m_ElementConstraints;       // 元素级约束
    std::vector<ConstraintInfo> m_NamespaceConstraints;     // 命名空间级约束
    std::vector<ConstraintInfo> m_GlobalConstraints;        // 全局约束
    
    // 约束上下文
    std::string m_CurrentNamespace;                         // 当前命名空间
    std::string m_CurrentElement;                           // 当前元素
    std::string m_CurrentFile;                              // 当前文件
    
    // 约束冲突检测
    std::vector<ConstraintConflict> m_Conflicts;            // 约束冲突列表
    
    // 验证缓存
    std::unordered_map<std::string, bool> m_ValidationCache;

public:
    /**
     * 构造函数
     */
    AdvancedConstraintValidator();
    
    /**
     * 析构函数
     */
    ~AdvancedConstraintValidator() = default;
    
    /**
     * 添加精确约束
     * @param target 约束目标 (如: span, [Custom] @Element Box)
     * @param context 约束上下文
     * @param scope 约束范围
     */
    void AddPreciseConstraint(const std::string& target,
                             const std::string& context,
                             ConstraintScope scope = ConstraintScope::ELEMENT_SCOPE);
    
    /**
     * 添加类型约束
     * @param targetType 约束类型 (如: @Html, [Template], [Custom])
     * @param context 约束上下文
     * @param scope 约束范围
     */
    void AddTypeConstraint(const std::string& targetType,
                          const std::string& context,
                          ConstraintScope scope = ConstraintScope::ELEMENT_SCOPE);
    
    /**
     * 添加全局约束
     * @param targetType 约束类型
     * @param namespaceName 命名空间名称
     */
    void AddGlobalConstraint(const std::string& targetType,
                            const std::string& namespaceName);
    
    /**
     * 验证元素使用
     * @param elementName 元素名称
     * @param elementType 元素类型
     * @param context 使用上下文
     * @return 是否允许使用
     */
    bool ValidateElementUsage(const std::string& elementName,
                             const std::string& elementType,
                             const std::string& context);
    
    /**
     * 验证模板使用
     * @param templateName 模板名称
     * @param templateType 模板类型
     * @param context 使用上下文
     * @return 是否允许使用
     */
    bool ValidateTemplateUsage(const std::string& templateName,
                              const std::string& templateType,
                              const std::string& context);
    
    /**
     * 验证自定义使用
     * @param customName 自定义名称
     * @param customType 自定义类型
     * @param context 使用上下文
     * @return 是否允许使用
     */
    bool ValidateCustomUsage(const std::string& customName,
                            const std::string& customType,
                            const std::string& context);
    
    /**
     * 验证原始嵌入使用
     * @param originType 原始嵌入类型
     * @param context 使用上下文
     * @return 是否允许使用
     */
    bool ValidateOriginUsage(const std::string& originType,
                            const std::string& context);
    
    /**
     * 解析约束表达式
     * @param constraintExpression 约束表达式
     * @return 解析后的约束目标列表
     */
    std::vector<std::string> ParseConstraintExpression(const std::string& constraintExpression);
    
    /**
     * 检测约束冲突
     * @return 冲突列表
     */
    std::vector<ConstraintConflict> DetectConstraintConflicts();
    
    /**
     * 解决约束冲突
     * @param conflict 冲突信息
     * @param resolution 解决方案
     */
    void ResolveConstraintConflict(const ConstraintConflict& conflict, 
                                  const std::string& resolution);
    
    /**
     * 设置当前上下文
     * @param namespaceName 命名空间
     * @param elementName 元素名称
     * @param fileName 文件名称
     */
    void SetCurrentContext(const std::string& namespaceName,
                          const std::string& elementName,
                          const std::string& fileName);
    
    /**
     * 获取约束统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetConstraintStatistics();
    
    /**
     * 验证所有约束的一致性
     * @return 验证结果
     */
    bool ValidateConstraintConsistency();
    
    /**
     * 清理所有约束
     */
    void Clear();

private:
    /**
     * 解析约束目标
     * @param target 约束目标字符串
     * @return 解析后的目标信息
     */
    std::pair<std::string, std::string> ParseConstraintTarget(const std::string& target);
    
    /**
     * 检查约束是否适用
     * @param constraint 约束信息
     * @param usage 使用信息
     * @return 是否适用
     */
    bool IsConstraintApplicable(const ConstraintInfo& constraint,
                               const std::string& usage);
    
    /**
     * 匹配约束模式
     * @param pattern 约束模式
     * @param target 目标
     * @return 是否匹配
     */
    bool MatchConstraintPattern(const std::string& pattern, const std::string& target);
    
    /**
     * 生成约束键
     * @param target 目标
     * @param context 上下文
     * @return 约束键
     */
    std::string GenerateConstraintKey(const std::string& target, const std::string& context);
    
    /**
     * 检查约束层次结构
     * @param constraint 约束信息
     * @param currentScope 当前范围
     * @return 是否在约束范围内
     */
    bool IsInConstraintScope(const ConstraintInfo& constraint, ConstraintScope currentScope);
};

} // namespace CHTL