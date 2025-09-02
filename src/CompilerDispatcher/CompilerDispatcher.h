#pragma once

#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 前向声明
class CSSCompiler;
class JavaScriptCompiler;

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
 * 合并后的编译结果
 * 存储最终的HTML输出结果
 */
struct MergedCompilationResult {
    std::string HTMLContent;            // HTML内容
    std::string CSSContent;             // CSS内容
    std::string JavaScriptContent;      // JavaScript内容
    std::string FullHTML;               // 完整的HTML文档
    bool IsSuccess;                     // 是否编译成功
    std::vector<std::string> Errors;    // 错误列表
    std::vector<std::string> Warnings;  // 警告列表
    
    MergedCompilationResult() : IsSuccess(false) {}
};

/**
 * 编译器调度器
 * 协调CHTLUnifiedScanner和四个编译器的工作
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
    std::unique_ptr<CHTLLexer> m_CHTLCompiler;             // CHTL编译器
    std::unique_ptr<CHTLJS::CHTLJSLexer> m_CHTLJSCompiler;         // CHTL JS编译器
    std::unique_ptr<CSSCompiler> m_CSSCompiler;            // CSS编译器（ANTLR）
    std::unique_ptr<JavaScriptCompiler> m_JSCompiler;      // JavaScript编译器（ANTLR）
    
    std::string m_SourceCode;                              // 源代码
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
    ~CompilerDispatcher();
    
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
     * @return 完整HTML文档字符串
     */
    std::string GetFullHTMLDocument() const;
    
    /**
     * 检查是否有编译错误
     * @return 是否有错误
     */
    bool HasError() const;
    
    /**
     * 获取错误信息
     * @return 错误信息
     */
    std::string GetErrorMessage() const;
    
    /**
     * 获取警告信息
     * @return 警告信息列表
     */
    std::vector<std::string> GetWarnings() const;
    
    /**
     * 重置调度器状态
     */
    void Reset();
    
    /**
     * 获取编译统计信息
     * @return 统计信息字符串
     */
    std::string GetCompilationStatistics() const;

private:
    /**
     * 初始化编译器
     */
    void InitializeCompilers();
    
    /**
     * 执行代码扫描和切割
     * @return 是否成功
     */
    bool PerformScanning();
    
    /**
     * 按类型分组片段
     */
    void GroupFragmentsByType();
    
    /**
     * 分发片段给对应编译器
     * @return 是否成功分发
     */
    bool DispatchFragments();
    
    /**
     * 编译CHTL片段
     * @param fragments CHTL片段列表
     * @return 编译结果
     */
    CompilationResult CompileCHTLFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译CHTL JS片段
     * @param fragments CHTL JS片段列表
     * @return 编译结果
     */
    CompilationResult CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译CSS片段
     * @param fragments CSS片段列表
     * @return 编译结果
     */
    CompilationResult CompileCSSFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译JavaScript片段
     * @param fragments JavaScript片段列表
     * @return 编译结果
     */
    CompilationResult CompileJavaScriptFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 合并编译结果
     * @return 是否成功合并
     */
    bool MergeCompilationResults();
    
    /**
     * 处理局部样式块
     * 局部样式（style）→ CHTL编译器
     * @param styleBlocks 样式块列表
     * @return 处理后的CSS代码
     */
    std::string ProcessLocalStyleBlocks(const std::vector<std::string>& styleBlocks);
    
    /**
     * 处理全局样式块
     * 全局样式（style）→ CSS编译器
     * @param globalStyles 全局样式列表
     * @return 处理后的CSS代码
     */
    std::string ProcessGlobalStyleBlocks(const std::vector<std::string>& globalStyles);
    
    /**
     * 处理脚本块
     * 脚本（script）→ 由CHTL编译器、CHTL JS编译器及JS编译器共同管理
     * @param scriptBlocks 脚本块列表
     * @return 处理后的JavaScript代码
     */
    std::string ProcessScriptBlocks(const std::vector<std::string>& scriptBlocks);
    
    /**
     * 生成HTML文档结构
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @param jsContent JavaScript内容
     * @return 完整HTML文档
     */
    std::string GenerateHTMLDocument(const std::string& htmlContent, 
                                   const std::string& cssContent, 
                                   const std::string& jsContent);
    
    /**
     * 处理自动化类名和ID生成
     * @param chtlResult CHTL编译结果
     * @return 处理后的HTML
     */
    std::string ProcessAutoGeneratedSelectors(const std::string& chtlResult);
    
    /**
     * 处理CHTL JS增强选择器转换
     * @param chtljsResult CHTL JS编译结果
     * @return 转换后的JavaScript代码
     */
    std::string ProcessEnhancedSelectors(const std::string& chtljsResult);
    
    /**
     * 验证编译结果一致性
     * @return 是否一致
     */
    bool ValidateCompilationConsistency();
    
    /**
     * 设置编译错误
     * @param message 错误信息
     */
    void SetCompilationError(const std::string& message);
    
    /**
     * 添加编译警告
     * @param message 警告信息
     */
    void AddCompilationWarning(const std::string& message);
    
    /**
     * 处理UTF-8编码
     * @param content 内容
     * @return 处理后的内容
     */
    std::string ProcessUTF8Encoding(const std::string& content);
    
    /**
     * 检查是否为HTML元素
     * @param name 元素名称
     * @return 是否为HTML元素
     */
    bool IsHTMLElement(const std::string& name);
    
    /**
     * 检查是否包含CHTL JS语法
     * @param block 代码块
     * @return 是否包含CHTL JS语法
     */
    bool ContainsCHTLJSSyntax(const std::string& block);
    
    /**
     * 处理CHTL JS脚本块
     * @param block 脚本块
     * @return 处理后的代码
     */
    std::string ProcessCHTLJSScriptBlock(const std::string& block);
    
    /**
     * 解析上下文引用
     * @return 解析后的选择器
     */
    std::string ResolveContextReference();
    
    /**
     * 验证HTML结构
     * @param html HTML内容
     * @return 是否有效
     */
    bool ValidateHTMLStructure(const std::string& html);
    
    /**
     * 验证CSS内容
     * @param css CSS内容
     * @return 是否有效
     */
    bool ValidateCSSContent(const std::string& css);
    
    /**
     * 验证JavaScript内容
     * @param js JavaScript内容
     * @return 是否有效
     */
    bool ValidateJavaScriptContent(const std::string& js);
    
    /**
     * 验证选择器引用
     * @return 是否有效
     */
    bool ValidateSelectorReferences();
};

} // namespace CHTL