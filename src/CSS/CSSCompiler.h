#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * CSS编译结果
 */
struct CSSCompilationResult {
    std::string OptimizedCSS;              // 优化后的CSS
    std::string SourceMap;                 // 源映射
    std::vector<std::string> Warnings;    // 警告信息
    std::vector<std::string> Errors;      // 错误信息
    bool IsSuccess;                        // 是否成功
    
    CSSCompilationResult() : IsSuccess(false) {}
};

/**
 * CSS编译器（基于ANTLR4）
 * 负责CSS语法分析、优化和验证
 */
class CSSCompiler {
private:
    // ANTLR相关（待集成）
    // std::unique_ptr<CSSLexer> m_AntlrLexer;
    // std::unique_ptr<CSSParser> m_AntlrParser;
    
    // CSS优化选项
    bool m_MinifyCSS;                      // 是否压缩CSS
    bool m_RemoveComments;                 // 是否移除注释
    bool m_OptimizeSelectors;              // 是否优化选择器
    bool m_MergeRules;                     // 是否合并规则
    
    // 错误和警告
    std::vector<std::string> m_Errors;
    std::vector<std::string> m_Warnings;

public:
    /**
     * 构造函数
     */
    CSSCompiler();
    
    /**
     * 析构函数
     */
    ~CSSCompiler() = default;
    
    /**
     * 编译CSS代码
     * @param cssCode CSS源代码
     * @return 编译结果
     */
    CSSCompilationResult Compile(const std::string& cssCode);
    
    /**
     * 批量编译CSS片段
     * @param cssFragments CSS片段列表
     * @return 编译结果
     */
    CSSCompilationResult CompileFragments(const std::vector<std::string>& cssFragments);
    
    /**
     * 验证CSS语法
     * @param cssCode CSS代码
     * @return 是否有效
     */
    bool ValidateCSS(const std::string& cssCode);
    
    /**
     * 优化CSS代码
     * @param cssCode 原始CSS
     * @return 优化后的CSS
     */
    std::string OptimizeCSS(const std::string& cssCode);
    
    /**
     * 设置编译选项
     * @param minify 是否压缩
     * @param removeComments 是否移除注释
     * @param optimizeSelectors 是否优化选择器
     * @param mergeRules 是否合并规则
     */
    void SetCompilationOptions(bool minify = false, bool removeComments = false, 
                              bool optimizeSelectors = true, bool mergeRules = true);
    
    /**
     * 重置编译器
     */
    void Reset();
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& GetWarnings() const { return m_Warnings; }

private:
    /**
     * 初始化ANTLR组件
     */
    void InitializeANTLR();
    
    /**
     * 解析CSS语法树
     * @param cssCode CSS代码
     * @return 是否成功
     */
    bool ParseCSSTree(const std::string& cssCode);
    
    /**
     * 应用CSS优化规则
     * @param cssCode 原始CSS
     * @return 优化后的CSS
     */
    std::string ApplyCSSOptimizations(const std::string& cssCode);
    
    /**
     * 压缩CSS代码
     * @param cssCode CSS代码
     * @return 压缩后的CSS
     */
    std::string MinifyCSS(const std::string& cssCode);
    
    /**
     * 合并重复的CSS规则
     * @param cssCode CSS代码
     * @return 合并后的CSS
     */
    std::string MergeDuplicateRules(const std::string& cssCode);
    
    /**
     * 优化选择器
     * @param cssCode CSS代码
     * @return 优化后的CSS
     */
    std::string OptimizeSelectors(const std::string& cssCode);
    
    /**
     * 添加错误信息
     * @param error 错误信息
     */
    void AddError(const std::string& error);
    
    /**
     * 添加警告信息
     * @param warning 警告信息
     */
    void AddWarning(const std::string& warning);
};

} // namespace CHTL