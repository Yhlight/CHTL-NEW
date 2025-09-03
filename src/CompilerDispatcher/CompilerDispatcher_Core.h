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

/**
 * 编译结果
 */
struct CompilationResult {
    bool IsSuccess = false;
    std::string Content;
    std::string Type;
    std::string ErrorMessage;
};

/**
 * 合并编译结果
 */
struct MergedCompilationResult {
    std::string HTMLContent;
    std::string CSSContent;
    std::string JavaScriptContent;
    std::string FullHTML;
};

/**
 * 编译器调度器 (核心版本)
 * 协调CHTL和CHTL JS编译器，不依赖ANTLR
 */
class CompilerDispatcherCore {
private:
    std::unique_ptr<CHTLUnifiedScanner> m_Scanner;
    std::unique_ptr<CHTLParser> m_CHTLParser;
    std::unique_ptr<CHTLGenerator> m_CHTLGenerator;
    std::unique_ptr<CHTLJS::CHTLJSParser> m_CHTLJSParser;
    std::unique_ptr<CHTLJS::CHTLJSGenerator> m_CHTLJSGenerator;
    std::unique_ptr<CHTLCodeMerger> m_CodeMerger;
    std::unique_ptr<CJMOD::CJMODManager> m_CJMODManager;
    
    std::string m_SourceCode;
    std::vector<CodeFragment> m_Fragments;
    std::unordered_map<FragmentType, std::vector<CodeFragment>> m_FragmentsByType;
    std::vector<CompilationResult> m_CompilationResults;
    MergedCompilationResult m_MergedResult;
    
    bool m_HasError = false;
    std::string m_ErrorMessage;
    std::vector<std::string> m_Warnings;

public:
    /**
     * 构造函数
     */
    CompilerDispatcherCore();
    
    /**
     * 析构函数
     */
    ~CompilerDispatcherCore() = default;
    
    /**
     * 编译源代码
     * @param sourceCode 源代码
     * @return 是否成功
     */
    bool Compile(const std::string& sourceCode);
    
    /**
     * 重置编译器状态
     */
    void Reset();
    
    /**
     * 获取合并结果
     * @return 合并结果引用
     */
    const MergedCompilationResult& GetMergedResult() const;
    
    /**
     * 是否有错误
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
     * @return 警告列表
     */
    std::vector<std::string> GetWarnings() const;
    
    /**
     * 获取编译统计信息
     * @return 统计信息字符串
     */
    std::string GetCompilationStatistics() const;

private:
    /**
     * 初始化CJMOD集成
     * @return 是否成功
     */
    bool InitializeCJMODIntegration();
    
    /**
     * 执行代码扫描
     * @return 是否成功
     */
    bool PerformScanning();
    
    /**
     * 按类型分组片段
     */
    void GroupFragmentsByType();
    
    /**
     * 分发片段给编译器
     * @return 是否成功
     */
    bool DispatchFragments();
    
    /**
     * 编译CHTL片段
     * @param fragments 片段列表
     * @return 编译结果
     */
    CompilationResult CompileCHTLFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译CHTL JS片段
     * @param fragments 片段列表
     * @return 编译结果
     */
    CompilationResult CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 合并编译结果
     * @return 是否成功
     */
    bool MergeCompilationResults();
    
    /**
     * 生成默认HTML
     * @return 默认HTML内容
     */
    std::string GenerateDefaultHTML();
    
    /**
     * 设置编译错误
     * @param error 错误信息
     */
    void SetCompilationError(const std::string& error);
    
    /**
     * 添加编译警告
     * @param warning 警告信息
     */
    void AddCompilationWarning(const std::string& warning);
};

} // namespace CHTL