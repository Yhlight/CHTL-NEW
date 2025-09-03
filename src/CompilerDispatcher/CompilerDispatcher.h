#pragma once

#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTL JS/CJMODSystem/CJMODIntegration.h"
#include "../CodeMerger/CHTLCodeMerger.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 条件前向声明 - 仅在ANTLR可用时包含
#ifdef CHTL_WITH_ANTLR
class CSSCompiler;
class JavaScriptCompiler;
#endif

/**
 * 编译结果结构
 * 存储单个编译器的编译结果
 */
struct CompilationResult {
    std::string Content;                // 编译后的内容
    std::string Type;                   // 内容类型（HTML/CSS/JS）
    bool IsSuccess;                     // 是否编译成功
    std::string ErrorMessage;           // 错误信息
    std::vector<std::string> Warnings;  // 警告信息
    
    CompilationResult() : IsSuccess(false) {}
    CompilationResult(const std::string& content, const std::string& type)
        : Content(content), Type(type), IsSuccess(true) {}
};

/**
 * 合并编译结果
 * 包含最终的HTML、CSS、JavaScript内容
 */
struct MergedCompilationResult {
    std::string HTMLContent;            // HTML内容
    std::string CSSContent;             // CSS内容  
    std::string JSContent;              // JavaScript内容
    std::string FullHTML;               // 完整的HTML文档
    bool IsSuccess;                     // 是否成功
    std::string ErrorMessage;           // 错误信息
    std::vector<std::string> Warnings;  // 警告信息
    
    MergedCompilationResult() : IsSuccess(false) {}
};

/**
 * CHTL编译器调度器
 * 
 * 负责协调多个编译器的工作，包括：
 * - CHTL编译器（手写）
 * - CHTL JS编译器（手写）
 * - CSS编译器（ANTLR）
 * - JavaScript编译器（ANTLR）
 * 
 * 工作流程：
 * 1. 使用CHTLUnifiedScanner切割代码片段
 * 2. 将片段分发给对应的编译器
 * 3. 收集编译结果并合并
 * 4. 生成最终的HTML输出
 */
class CompilerDispatcher {
private:
    std::unique_ptr<CHTLUnifiedScanner> m_Scanner;          // 统一扫描器
    std::unique_ptr<CHTLParser> m_CHTLParser;              // CHTL解析器
    std::unique_ptr<CHTLGenerator> m_CHTLGenerator;        // CHTL生成器
    std::unique_ptr<CHTLJS::CHTLJSParser> m_CHTLJSParser;  // CHTL JS解析器
    std::unique_ptr<CHTLJS::CHTLJSGenerator> m_CHTLJSGenerator; // CHTL JS生成器
    std::unique_ptr<CHTLCodeMerger> m_CodeMerger;          // 代码合并器（关键组件）
    std::unique_ptr<CJMOD::CJMODManager> m_CJMODManager;   // CJMOD管理器（扩展处理）
    
    // 条件编译的ANTLR编译器
#ifdef CHTL_WITH_ANTLR
    std::unique_ptr<CSSCompiler> m_CSSCompiler;            // CSS编译器（ANTLR）
    std::unique_ptr<JavaScriptCompiler> m_JSCompiler;      // JavaScript编译器（ANTLR）
#endif
    
    std::string m_SourceCode;                              // 源代码
    std::string m_CurrentSourceFile;                       // 当前源文件路径
    std::vector<CodeFragment> m_Fragments;                 // 代码片段
    std::unordered_map<FragmentType, std::vector<CodeFragment>> m_FragmentsByType; // 按类型分组的片段
    
    std::vector<CompilationResult> m_CompilationResults;   // 编译结果列表
    MergedCompilationResult m_MergedResult;                // 合并后的结果
    
    bool m_HasError;                                       // 是否有编译错误
    std::string m_ErrorMessage;                            // 错误信息

public:
    /**
     * 构造函数
     */
    CompilerDispatcher();
    
    /**
     * 析构函数
     */
    ~CompilerDispatcher() = default;
    
    /**
     * 编译CHTL源代码
     * @param sourceCode 源代码
     * @return 是否编译成功
     */
    bool Compile(const std::string& sourceCode);
    
    /**
     * 获取合并后的编译结果
     * @return 合并编译结果
     */
    const MergedCompilationResult& GetMergedResult() const;
    
    /**
     * 获取HTML输出
     * @return HTML字符串
     */
    std::string GetHTMLOutput() const;
    
    /**
     * 获取CSS输出
     * @return CSS字符串
     */
    std::string GetCSSOutput() const;
    
    /**
     * 获取JavaScript输出
     * @return JavaScript字符串
     */
    std::string GetJavaScriptOutput() const;
    
    /**
     * 获取完整的HTML文档
     * @return 完整HTML字符串
     */
    std::string GetFullHTML() const;
    
    /**
     * 检查是否有编译错误
     * @return true表示有错误
     */
    bool HasError() const;
    
    /**
     * 获取错误信息
     * @return 错误信息字符串
     */
    std::string GetErrorMessage() const;
    
    /**
     * 获取警告信息
     * @return 警告信息列表
     */
    std::vector<std::string> GetWarnings() const;
    
    /**
     * 获取编译统计信息
     * @return 统计信息字符串
     */
    std::string GetCompilationStatistics() const;
    
    /**
     * 重置编译器状态
     */
    void Reset();
    
    /**
     * 设置源文件路径
     * @param filePath 源文件路径
     */
    void SetSourceFile(const std::string& filePath);
    
    /**
     * 获取当前源文件路径
     * @return 源文件路径
     */
    std::string GetSourceFile() const;

private:
    /**
     * 执行代码扫描
     * @return 是否成功
     */
    bool PerformScanning();
    
    /**
     * 编译CHTL片段
     * @param fragments CHTL代码片段
     * @return 编译结果
     */
    CompilationResult CompileCHTLFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译CHTL JS片段
     * @param fragments CHTL JS代码片段
     * @return 编译结果
     */
    CompilationResult CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments);

#ifdef CHTL_WITH_ANTLR
    /**
     * 编译CSS片段
     * @param fragments CSS代码片段
     * @return 编译结果
     */
    CompilationResult CompileCSSFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译JavaScript片段
     * @param fragments JavaScript代码片段
     * @return 编译结果
     */
    CompilationResult CompileJavaScriptFragments(const std::vector<CodeFragment>& fragments);
#endif
    
    /**
     * 初始化CJMOD集成
     * @return 是否成功
     */
    bool InitializeCJMODIntegration();
    
    /**
     * 合并编译结果
     * @return 是否成功
     */
    bool MergeCompilationResults();
    
    /**
     * 设置编译错误
     * @param error 错误信息
     */
    void SetCompilationError(const std::string& error);
    
    /**
     * 分组代码片段
     * 将代码片段按类型分组以便批量处理
     */
    void GroupFragmentsByType();
    
    /**
     * 验证编译结果
     * @return 是否所有结果都有效
     */
    bool ValidateCompilationResults();
    
    /**
     * 生成编译统计
     * @return 统计信息
     */
    std::string GenerateStatistics();
    
    /**
     * 直接编译（安全模式）
     * @return 是否成功
     */
    bool CompileDirectly();

    /**
     * HTML转义函数
     * @param input 输入字符串
     * @return 转义后的字符串
     */
    std::string escapeHtml(const std::string& input);

#ifdef CHTL_WITH_ANTLR
    /**
     * HTML转义函数（ANTLR版本）
     * @param input 输入字符串
     * @return 转义后的字符串
     */
    std::string htmlEscape(const std::string& input);
#endif
};

} // namespace CHTL