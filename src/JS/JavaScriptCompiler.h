#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// ANTLR生成的头文件
#include "generated/JavaScriptLexer.h"
#include "generated/JavaScriptParser.h"
#include "generated/JavaScriptParserBaseListener.h"

// ANTLR运行时
#include "../../thirdparty/antlr/include/antlr4-runtime.h"

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
 * JavaScript监听器
 * 用于遍历JavaScript AST并生成优化后的JavaScript
 */
class JavaScriptCompilerListener : public JavaScriptParserBaseListener {
private:
    std::string m_CompiledJS;                           // 编译结果
    std::vector<std::string> m_Errors;                  // 错误列表
    std::vector<std::string> m_Warnings;               // 警告列表

public:
    /**
     * 构造函数
     */
    JavaScriptCompilerListener();
    
    /**
     * 析构函数
     */
    ~JavaScriptCompilerListener() override = default;
    
    /**
     * 进入程序
     */
    void enterProgram(JavaScriptParser::ProgramContext *ctx) override;
    
    /**
     * 退出程序
     */
    void exitProgram(JavaScriptParser::ProgramContext *ctx) override;
    
    /**
     * 进入函数声明
     */
    void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override;
    
    /**
     * 进入变量声明
     */
    void enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) override;
    
    /**
     * 进入表达式语句
     */
    void enterExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) override;
    
    /**
     * 获取编译结果
     */
    const std::string& getCompiledJS() const { return m_CompiledJS; }
    
    /**
     * 获取错误列表
     */
    const std::vector<std::string>& getErrors() const { return m_Errors; }
    
    /**
     * 获取警告列表
     */
    const std::vector<std::string>& getWarnings() const { return m_Warnings; }

private:
    /**
     * 添加错误
     */
    void addError(const std::string& error);
    
    /**
     * 添加警告
     */
    void addWarning(const std::string& warning);
};

/**
 * JavaScript编译器（ANTLR实现）
 * 处理CHTL中的JavaScript片段
 */
class JavaScriptCompiler {
private:
    std::unique_ptr<antlr4::ANTLRInputStream> m_InputStream;
    std::unique_ptr<JavaScriptLexer> m_Lexer;
    std::unique_ptr<antlr4::CommonTokenStream> m_TokenStream;
    std::unique_ptr<JavaScriptParser> m_Parser;
    std::unique_ptr<JavaScriptCompilerListener> m_Listener;
    
    // JavaScript优化选项
    bool m_MinifyJS;                       // 是否压缩JavaScript
    bool m_RemoveComments;                 // 是否移除注释
    bool m_OptimizeVariables;              // 是否优化变量
    bool m_DeadCodeElimination;            // 是否移除死代码
    
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
     * 编译单个JavaScript片段
     * @param jsCode JavaScript代码
     * @return 编译结果
     */
    JavaScriptCompilationResult CompileSingleFragment(const std::string& jsCode);
    
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
     * @param optimizeVariables 是否优化变量
     * @param deadCodeElimination 是否移除死代码
     */
    void SetCompilationOptions(bool minify = false, bool removeComments = false, 
                              bool optimizeVariables = true, bool deadCodeElimination = true);
    
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
     * @param jsCode JavaScript代码
     */
    void initializeANTLR(const std::string& jsCode);
    
    /**
     * 执行JavaScript解析
     * @return 解析是否成功
     */
    bool parseJavaScript();
    
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