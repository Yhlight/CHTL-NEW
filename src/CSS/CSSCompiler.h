#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// ANTLR生成的头文件
#include "generated/CSSLexer.h"
#include "generated/CSSParser.h"
#include "generated/CSSBaseListener.h"

// ANTLR运行时
#include "../../thirdparty/antlr/include/antlr4-runtime.h"

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
 * CSS监听器
 * 用于遍历CSS AST并生成优化后的CSS
 */
class CSSCompilerListener : public CSSBaseListener {
private:
    std::string m_CompiledCSS;                          // 编译结果
    std::vector<std::string> m_Errors;                  // 错误列表
    std::vector<std::string> m_Warnings;               // 警告列表

public:
    /**
     * 构造函数
     */
    CSSCompilerListener();
    
    /**
     * 析构函数
     */
    ~CSSCompilerListener() override = default;
    
    /**
     * 进入样式表
     */
    void enterStylesheet(CSSParser::StylesheetContext *ctx) override;
    
    /**
     * 退出样式表
     */
    void exitStylesheet(CSSParser::StylesheetContext *ctx) override;
    
    /**
     * 进入CSS规则
     */
    void enterCssRule(CSSParser::CssRuleContext *ctx) override;
    
    /**
     * 退出CSS规则
     */
    void exitCssRule(CSSParser::CssRuleContext *ctx) override;
    
    /**
     * 进入选择器
     */
    void enterSelector(CSSParser::SelectorContext *ctx) override;
    
    /**
     * 进入声明
     */
    void enterDeclaration(CSSParser::DeclarationContext *ctx) override;
    
    /**
     * 获取编译结果
     */
    const std::string& getCompiledCSS() const { return m_CompiledCSS; }
    
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
 * CSS编译器（ANTLR实现）
 * 处理CHTL中的CSS片段
 */
class CSSCompiler {
private:
    std::unique_ptr<antlr4::ANTLRInputStream> m_InputStream;
    std::unique_ptr<CSSLexer> m_Lexer;
    std::unique_ptr<antlr4::CommonTokenStream> m_TokenStream;
    std::unique_ptr<CSSParser> m_Parser;
    std::unique_ptr<CSSCompilerListener> m_Listener;
    
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
     * 编译单个CSS片段
     * @param cssCode CSS代码
     * @return 编译结果
     */
    CSSCompilationResult CompileSingleFragment(const std::string& cssCode);
    
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
     * @param cssCode CSS代码
     */
    void initializeANTLR(const std::string& cssCode);
    
    /**
     * 执行CSS解析
     * @return 解析是否成功
     */
    bool parseCSS();
    
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