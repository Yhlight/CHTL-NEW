#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * JavaScript编译结果
 */
struct JavaScriptCompilationResult {
    std::string OptimizedJS;               // 优化后的JavaScript
    std::string SourceMap;                 // 源映射
    std::vector<std::string> Warnings;    // 警告信息
    std::vector<std::string> Errors;      // 错误信息
    bool IsSuccess;                        // 是否成功
    
    JavaScriptCompilationResult() : IsSuccess(false) {}
};

/**
 * JavaScript编译器（基于ANTLR4）
 * 负责JavaScript语法分析、优化和验证
 */
class JavaScriptCompiler {
private:
    // ANTLR相关（待集成）
    // std::unique_ptr<JavaScriptLexer> m_AntlrLexer;
    // std::unique_ptr<JavaScriptParser> m_AntlrParser;
    
    // JavaScript优化选项
    bool m_MinifyJS;                       // 是否压缩JavaScript
    bool m_RemoveComments;                 // 是否移除注释
    bool m_OptimizeVariables;              // 是否优化变量名
    bool m_RemoveDeadCode;                 // 是否移除死代码
    
    // 错误和警告
    std::vector<std::string> m_Errors;
    std::vector<std::string> m_Warnings;

public:
    /**
     * 构造函数
     */
    JavaScriptCompiler();
    
    /**
     * 析构函数
     */
    ~JavaScriptCompiler() = default;
    
    /**
     * 编译JavaScript代码
     * @param jsCode JavaScript源代码
     * @return 编译结果
     */
    JavaScriptCompilationResult Compile(const std::string& jsCode);
    
    /**
     * 批量编译JavaScript片段
     * @param jsFragments JavaScript片段列表
     * @return 编译结果
     */
    JavaScriptCompilationResult CompileFragments(const std::vector<std::string>& jsFragments);
    
    /**
     * 验证JavaScript语法
     * @param jsCode JavaScript代码
     * @return 是否有效
     */
    bool ValidateJavaScript(const std::string& jsCode);
    
    /**
     * 优化JavaScript代码
     * @param jsCode 原始JavaScript
     * @return 优化后的JavaScript
     */
    std::string OptimizeJavaScript(const std::string& jsCode);
    
    /**
     * 设置编译选项
     * @param minify 是否压缩
     * @param removeComments 是否移除注释
     * @param optimizeVariables 是否优化变量名
     * @param removeDeadCode 是否移除死代码
     */
    void SetCompilationOptions(bool minify = false, bool removeComments = false, 
                              bool optimizeVariables = false, bool removeDeadCode = false);
    
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
     * 解析JavaScript语法树
     * @param jsCode JavaScript代码
     * @return 是否成功
     */
    bool ParseJavaScriptTree(const std::string& jsCode);
    
    /**
     * 应用JavaScript优化规则
     * @param jsCode 原始JavaScript
     * @return 优化后的JavaScript
     */
    std::string ApplyJavaScriptOptimizations(const std::string& jsCode);
    
    /**
     * 压缩JavaScript代码
     * @param jsCode JavaScript代码
     * @return 压缩后的JavaScript
     */
    std::string MinifyJavaScript(const std::string& jsCode);
    
    /**
     * 优化变量名
     * @param jsCode JavaScript代码
     * @return 优化后的JavaScript
     */
    std::string OptimizeVariableNames(const std::string& jsCode);
    
    /**
     * 移除死代码
     * @param jsCode JavaScript代码
     * @return 优化后的JavaScript
     */
    std::string RemoveDeadCode(const std::string& jsCode);
    
    /**
     * 验证JavaScript语法结构
     * @param jsCode JavaScript代码
     * @return 是否有效
     */
    bool ValidateJavaScriptStructure(const std::string& jsCode);
    
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