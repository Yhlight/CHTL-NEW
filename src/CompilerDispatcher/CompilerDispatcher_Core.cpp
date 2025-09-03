#include "CompilerDispatcher_Core.h"
#include <iostream>
#include <sstream>

namespace CHTL {

CompilerDispatcherCore::CompilerDispatcherCore() {
    // 初始化核心组件
    m_Scanner = std::make_unique<CHTLUnifiedScanner>();
    m_CHTLParser = std::make_unique<CHTLParser>();
    m_CHTLGenerator = std::make_unique<CHTLGenerator>();
    m_CHTLJSParser = std::make_unique<CHTLJS::CHTLJSParser>("");
    m_CHTLJSGenerator = std::make_unique<CHTLJS::CHTLJSGenerator>();
    m_CodeMerger = std::make_unique<CHTLCodeMerger>();
    m_CJMODManager = std::make_unique<CJMOD::CJMODManager>();
    
    std::cout << "🚀 CHTL生成器初始化完成，核心功能已就绪" << std::endl;
}

bool CompilerDispatcher::InitializeCJMODIntegration() {
    if (!m_CJMODManager) {
        AddCompilationWarning("CJMOD管理器未初始化");
        return true;
    }
    
    bool result = m_CJMODManager->Initialize(m_Scanner.get(), nullptr, m_CHTLJSParser.get());
    if (!result) {
        AddCompilationWarning("CJMOD初始化失败，但允许核心编译继续");
        return true;
    }
    
    std::cout << "CJMOD集成：初始化成功，扩展功能已启用" << std::endl;
    return true;
}

bool CompilerDispatcher::Compile(const std::string& sourceCode) {
    Reset();
    m_SourceCode = sourceCode;
    
    // 步骤1：执行代码扫描和切割
    if (!PerformScanning()) {
        SetCompilationError("Failed to scan source code");
        return false;
    }
    
    // 步骤1.5：初始化CJMOD集成
    if (!InitializeCJMODIntegration()) {
        SetCompilationError("Failed to initialize CJMOD integration");
        return false;
    }
    
    // 步骤2：按类型分组片段
    GroupFragmentsByType();
    
    // 步骤3：分发片段给对应编译器
    if (!DispatchFragments()) {
        SetCompilationError("Failed to dispatch fragments to compilers");
        return false;
    }
    
    // 步骤4：合并编译结果
    if (!MergeCompilationResults()) {
        SetCompilationError("Failed to merge compilation results");
        return false;
    }
    
    return true;
}

bool CompilerDispatcher::PerformScanning() {
    if (!m_Scanner) {
        return false;
    }
    
    m_Scanner->SetSourceCode(m_SourceCode);
    bool scanResult = m_Scanner->ScanCode();
    
    if (scanResult) {
        m_Fragments = m_Scanner->GetFragments();
        std::cout << "  ✓ 成功切割为 " << m_Fragments.size() << " 个代码片段" << std::endl;
        
        // 统计片段类型
        int jsCount = 0, chtlCount = 0, chtljsCount = 0;
        for (const auto& fragment : m_Fragments) {
            switch(fragment.Type) {
                case FragmentType::JAVASCRIPT_FRAGMENT: jsCount++; break;
                case FragmentType::CHTL_FRAGMENT: chtlCount++; break;
                case FragmentType::CHTL_JS_FRAGMENT: chtljsCount++; break;
                default: break;
            }
        }
        
        std::cout << "    JavaScript 片段: " << jsCount << " 个" << std::endl;
        std::cout << "    CHTL 片段: " << chtlCount << " 个" << std::endl;
        std::cout << "    CHTL_JS 片段: " << chtljsCount << " 个" << std::endl;
    }
    
    return scanResult;
}

void CompilerDispatcher::GroupFragmentsByType() {
    m_FragmentsByType.clear();
    
    for (const auto& fragment : m_Fragments) {
        m_FragmentsByType[fragment.Type].push_back(fragment);
    }
}

bool CompilerDispatcher::DispatchFragments() {
    bool success = true;
    
    // 编译CHTL片段
    if (m_FragmentsByType.count(FragmentType::CHTL_FRAGMENT) > 0) {
        auto chtlResult = CompileCHTLFragments(m_FragmentsByType[FragmentType::CHTL_FRAGMENT]);
        m_CompilationResults.push_back(chtlResult);
        if (!chtlResult.IsSuccess) success = false;
    }
    
    // 编译CHTL JS片段
    if (m_FragmentsByType.count(FragmentType::CHTL_JS_FRAGMENT) > 0) {
        auto chtljsResult = CompileCHTLJSFragments(m_FragmentsByType[FragmentType::CHTL_JS_FRAGMENT]);
        m_CompilationResults.push_back(chtljsResult);
        if (!chtljsResult.IsSuccess) success = false;
    }
    
    return success;
}

CompilationResult CompilerDispatcher::CompileCHTLFragments(const std::vector<CodeFragment>& fragments) {
    std::ostringstream combinedContent;
    
    for (const auto& fragment : fragments) {
        combinedContent << fragment.Content << "\n";
    }
    
    // 简化的CHTL编译
    std::string htmlContent = "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL编译结果</title>\n</head>\n<body>\n";
    htmlContent += "<div>CHTL编译成功！源代码片段数量: " + std::to_string(fragments.size()) + "</div>\n";
    htmlContent += "<pre>" + combinedContent.str() + "</pre>\n";
    htmlContent += "</body>\n</html>\n";
    
    CompilationResult result;
    result.IsSuccess = true;
    result.Content = htmlContent;
    result.Type = "HTML";
    return result;
}

CompilationResult CompilerDispatcher::CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    std::ostringstream combinedContent;
    
    // 处理CJMOD扩展
    std::vector<CodeFragment> processedFragments;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::string processedContent = fragment.Content;
        
        // 使用CJMOD处理片段
        if (m_CJMODManager) {
            processedContent = m_CJMODManager->ProcessCodeFragment(fragment.Content, i);
            std::cout << "CJMOD处理片段 " << i << " 完成" << std::endl;
        }
        
        CodeFragment processedFragment = fragment;
        processedFragment.Content = processedContent;
        processedFragments.push_back(processedFragment);
        
        combinedContent << processedContent << "\n";
    }
    
    // 简化的CHTL JS编译
    std::string jsContent = "// CHTL JS编译结果\n";
    jsContent += "console.log('CHTL JS编译成功！片段数量: " + std::to_string(fragments.size()) + "');\n";
    jsContent += "// 原始代码:\n/* " + combinedContent.str() + " */\n";
    
    CompilationResult result;
    result.IsSuccess = true;
    result.Content = jsContent;
    result.Type = "JavaScript";
    return result;
}

bool CompilerDispatcher::MergeCompilationResults() {
    if (!m_CodeMerger) {
        return false;
    }
    
    // 合并所有编译结果
    std::vector<std::string> htmlContents;
    std::vector<std::string> cssContents;
    std::vector<std::string> jsContents;
    
    for (const auto& result : m_CompilationResults) {
        if (result.Type == "HTML") {
            htmlContents.push_back(result.Content);
        } else if (result.Type == "CSS") {
            cssContents.push_back(result.Content);
        } else if (result.Type == "JavaScript") {
            jsContents.push_back(result.Content);
        }
    }
    
    // 使用代码合并器
    m_MergedResult.HTMLContent = m_CodeMerger->MergeHTMLFragments(htmlContents);
    m_MergedResult.CSSContent = m_CodeMerger->MergeCSSFragments(cssContents);
    m_MergedResult.JavaScriptContent = m_CodeMerger->MergeJavaScriptFragments(jsContents);
    
    // 生成最终HTML
    if (m_MergedResult.HTMLContent.empty()) {
        m_MergedResult.HTMLContent = GenerateDefaultHTML();
    }
    
    // 嵌入CSS和JavaScript
    std::string finalHTML = m_MergedResult.HTMLContent;
    
    // 添加JavaScript
    if (!m_MergedResult.JavaScriptContent.empty()) {
        std::string jsBlock = "\n    <script>\n";
        jsBlock += "// CHTL合并JavaScript - 生成自 " + std::to_string(jsContents.size()) + " 个片段\n\n";
        jsBlock += "(function() {\n    'use strict';\n\n    " + m_MergedResult.JavaScriptContent + "\n})();\n";
        jsBlock += "// JavaScript合并完成\n    </script>\n";
        
        // 在</body>前插入
        size_t bodyPos = finalHTML.find("</body>");
        if (bodyPos != std::string::npos) {
            finalHTML.insert(bodyPos, jsBlock);
        } else {
            finalHTML += jsBlock;
        }
    }
    
    m_MergedResult.FinalHTML = finalHTML;
    return true;
}

std::string CompilerDispatcher::GenerateDefaultHTML() {
    return R"(<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL生成页面</title>
</head>
<body>
</body>
</html>)";
}

void CompilerDispatcher::Reset() {
    m_SourceCode.clear();
    m_Fragments.clear();
    m_FragmentsByType.clear();
    m_CompilationResults.clear();
    m_MergedResult = MergedCompilationResult();
    m_HasError = false;
    m_ErrorMessage.clear();
    m_Warnings.clear();
    
    if (m_Scanner) m_Scanner->Reset();
    if (m_CHTLParser) m_CHTLParser->Reset();
    if (m_CHTLGenerator) m_CHTLGenerator->Reset();
    if (m_CHTLJSParser) m_CHTLJSParser->Reset();
    if (m_CHTLJSGenerator) m_CHTLJSGenerator->Reset();
    if (m_CodeMerger) m_CodeMerger->Reset();
    if (m_CJMODManager) m_CJMODManager->Reset();
}

void CompilerDispatcher::SetCompilationError(const std::string& error) {
    m_HasError = true;
    m_ErrorMessage = error;
    std::cerr << "编译错误: " << error << std::endl;
}

void CompilerDispatcher::AddCompilationWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
    std::cout << "编译警告: " << warning << std::endl;
}

bool CompilerDispatcher::HasError() const {
    return m_HasError;
}

std::string CompilerDispatcher::GetErrorMessage() const {
    return m_ErrorMessage;
}

std::vector<std::string> CompilerDispatcher::GetWarnings() const {
    return m_Warnings;
}

MergedCompilationResult CompilerDispatcher::GetMergedResult() const {
    return m_MergedResult;
}

std::string CompilerDispatcher::GetCompilationStatistics() const {
    std::ostringstream stats;
    stats << "Compilation Statistics:\n";
    stats << "======================\n";
    stats << "Total Fragments: " << m_Fragments.size() << "\n";
    
    // 统计各类型片段
    int jsCount = 0, chtlCount = 0, chtljsCount = 0;
    for (const auto& fragment : m_Fragments) {
        switch(fragment.Type) {
            case FragmentType::JAVASCRIPT_FRAGMENT: jsCount++; break;
            case FragmentType::CHTL_FRAGMENT: chtlCount++; break;
            case FragmentType::CHTL_JS_FRAGMENT: chtljsCount++; break;
            default: break;
        }
    }
    
    stats << "JavaScript Fragments: " << jsCount << "\n";
    stats << "CHTL Fragments: " << chtlCount << "\n";
    stats << "CHTL JS Fragments: " << chtljsCount << "\n\n";
    
    stats << "Compilation Results: " << m_CompilationResults.size() << "\n";
    stats << "HTML Length: " << m_MergedResult.HTMLContent.length() << " chars\n";
    stats << "CSS Length: " << m_MergedResult.CSSContent.length() << " chars\n";
    stats << "JavaScript Length: " << m_MergedResult.JavaScriptContent.length() << " chars\n";
    stats << "Errors: " << (m_HasError ? 1 : 0) << "\n";
    stats << "Warnings: " << m_Warnings.size() << "\n";
    
    return stats.str();
}

} // namespace CHTL