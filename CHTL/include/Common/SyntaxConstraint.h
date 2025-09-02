#ifndef CHTL_COMMON_SYNTAXCONSTRAINT_H
#define CHTL_COMMON_SYNTAXCONSTRAINT_H

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

namespace CHTL {

/**
 * 语法约束器
 * 用于明确CHTL和CHTL JS的语法边界
 * 确保各自的语法不会混用
 */
class SyntaxConstraint {
public:
    enum class Context {
        CHTL,           // CHTL上下文
        CHTLJS,         // CHTL JS上下文
        Script,         // 局部script块（CHTL内的JavaScript）
        Style,          // 局部style块（CHTL内的CSS）
        Module          // 模块导入块
    };
    
    enum class Feature {
        // CHTL特有功能
        UseStatement,           // use语句
        TemplateDeclaration,    // [Template]声明
        CustomDeclaration,      // [Custom]声明
        OriginEmbed,           // [Origin]嵌入
        ImportStatement,        // [Import]导入
        NamespaceDeclaration,  // [Namespace]命名空间
        ConfigurationGroup,    // [Configuration]配置组
        ExceptConstraint,      // except约束
        LocalStyleBlock,       // style {}块
        LocalScriptBlock,      // script {}块
        CEEquivalence,         // CE等价（: 和 =）
        UnquotedLiteral,      // 无引号字面量
        
        // CHTL JS特有功能
        EnhancedSelector,      // {{选择器}}语法
        ArrowOperator,         // ->操作符
        ListenBlock,          // listen {}块
        DelegateBlock,        // delegate {}块
        AnimateBlock,         // animate {}块
        VirtualObject,        // vir虚对象
        EventBinding,         // &->事件绑定
        ModuleImport,         // module {}导入
        CJMODExtension,       // CJMOD扩展语法
        
        // 共享功能
        ElementDeclaration,   // 元素声明
        AttributeAssignment,  // 属性赋值
        TextNode,            // text {}节点
        Comments,            // 注释
        StringLiteral,       // 字符串字面量
        NumberLiteral,       // 数字字面量
        VariableReference    // 变量引用
    };
    
private:
    Context currentContext;
    std::vector<Context> contextStack;
    std::unordered_set<Feature> allowedFeatures;
    bool strictMode;  // 严格模式：禁止任何跨界语法
    
    /**
     * 更新允许的特性集合
     */
    void UpdateAllowedFeatures();
    
public:
    SyntaxConstraint(bool strict = true);
    ~SyntaxConstraint() = default;
    
    /**
     * 进入新的上下文
     */
    void EnterContext(Context ctx);
    
    /**
     * 退出当前上下文
     */
    void ExitContext();
    
    /**
     * 获取当前上下文
     */
    Context GetCurrentContext() const { return currentContext; }
    
    /**
     * 检查特性是否允许在当前上下文中使用
     */
    bool IsFeatureAllowed(Feature feature) const;
    
    /**
     * 获取特性不允许的错误信息
     */
    std::string GetFeatureError(Feature feature) const;
    
    /**
     * 设置严格模式
     */
    void SetStrictMode(bool strict) { strictMode = strict; }
    
    /**
     * 检查是否在严格模式
     */
    bool IsStrictMode() const { return strictMode; }
    
    /**
     * 获取上下文名称
     */
    static std::string GetContextName(Context ctx);
    
    /**
     * 获取特性名称
     */
    static std::string GetFeatureName(Feature feature);
    
    /**
     * 验证语法使用是否合法
     */
    bool ValidateSyntax(Feature feature, std::string& error);
    
    /**
     * 重置约束器状态
     */
    void Reset();
};

/**
 * 语法边界检查器
 * 用于编译时检查语法边界
 */
class SyntaxBoundaryChecker {
private:
    std::shared_ptr<SyntaxConstraint> constraint;
    std::vector<std::string> violations;  // 违规记录
    
public:
    SyntaxBoundaryChecker();
    ~SyntaxBoundaryChecker() = default;
    
    /**
     * 获取语法约束器
     */
    std::shared_ptr<SyntaxConstraint> GetConstraint() { return constraint; }
    
    /**
     * 记录违规
     */
    void RecordViolation(const std::string& message, int line = -1, int column = -1);
    
    /**
     * 检查是否有违规
     */
    bool HasViolations() const { return !violations.empty(); }
    
    /**
     * 获取所有违规信息
     */
    const std::vector<std::string>& GetViolations() const { return violations; }
    
    /**
     * 清除违规记录
     */
    void ClearViolations() { violations.clear(); }
    
    /**
     * 生成违规报告
     */
    std::string GenerateViolationReport() const;
};

} // namespace CHTL

#endif // CHTL_COMMON_SYNTAXCONSTRAINT_H