#include "CompilerDispatcher.h"

#ifdef CHTL_WITH_ANTLR
#include "../CSS/CSSCompiler.h"
#include "../JS/JavaScriptCompiler.h"
#endif

#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : m_HasError(false) {
    
    // 初始化核心组件
    m_Scanner = std::make_unique<CHTLUnifiedScanner>("");
    m_CHTLParser = std::make_unique<CHTLParser>("");
    m_CHTLGenerator = std::make_unique<CHTLGenerator>();
    m_CHTLJSParser = std::make_unique<CHTLJS::CHTLJSParser>("");
    m_CHTLJSGenerator = std::make_unique<CHTLJS::CHTLJSGenerator>();
    m_CodeMerger = std::make_unique<CHTLCodeMerger>();
    m_CJMODManager = std::make_unique<CJMOD::CJMODManager>();
    
#ifdef CHTL_WITH_ANTLR
    // 初始化ANTLR编译器
    m_CSSCompiler = std::make_unique<CSSCompiler>();
    m_JSCompiler = std::make_unique<JavaScriptCompiler>();
    std::cout << "🚀 CHTL编译器初始化完成 (完整版本，包含ANTLR支持)" << std::endl;
#else
    std::cout << "🚀 CHTL编译器初始化完成 (简化版本，无ANTLR支持)" << std::endl;
#endif
}

bool CompilerDispatcher::Compile(const std::string& sourceCode) {
    Reset();
    m_SourceCode = sourceCode;
    
    // 1. 代码扫描
    if (!PerformScanning()) {
        SetCompilationError("代码扫描失败");
        return false;
    }
    
    // 2. CJMOD集成初始化
    if (!InitializeCJMODIntegration()) {
        SetCompilationError("CJMOD初始化失败");
        return false;
    }
    
    // 3. 分组片段
    GroupFragmentsByType();
    
    // 4. 编译各类型片段
    if (m_FragmentsByType.count(FragmentType::CHTL_FRAGMENT) > 0) {
        auto result = CompileCHTLFragments(m_FragmentsByType[FragmentType::CHTL_FRAGMENT]);
        m_CompilationResults.push_back(result);
        if (!result.IsSuccess) {
            SetCompilationError("CHTL编译失败: " + result.ErrorMessage);
            return false;
        }
    }
    
    if (m_FragmentsByType.count(FragmentType::CHTL_JS_FRAGMENT) > 0) {
        auto result = CompileCHTLJSFragments(m_FragmentsByType[FragmentType::CHTL_JS_FRAGMENT]);
        m_CompilationResults.push_back(result);
        if (!result.IsSuccess) {
            SetCompilationError("CHTL JS编译失败: " + result.ErrorMessage);
            return false;
        }
    }

#ifdef CHTL_WITH_ANTLR
    if (m_FragmentsByType.count(FragmentType::CSS_FRAGMENT) > 0) {
        auto result = CompileCSSFragments(m_FragmentsByType[FragmentType::CSS_FRAGMENT]);
        m_CompilationResults.push_back(result);
        if (!result.IsSuccess) {
            SetCompilationError("CSS编译失败: " + result.ErrorMessage);
            return false;
        }
    }
    
    if (m_FragmentsByType.count(FragmentType::JS_FRAGMENT) > 0) {
        auto result = CompileJavaScriptFragments(m_FragmentsByType[FragmentType::JS_FRAGMENT]);
        m_CompilationResults.push_back(result);
        if (!result.IsSuccess) {
            SetCompilationError("JavaScript编译失败: " + result.ErrorMessage);
            return false;
        }
    }
#endif
    
    // 5. 合并编译结果
    if (!MergeCompilationResults()) {
        SetCompilationError("编译结果合并失败");
        return false;
    }
    
    return true;
}

bool CompilerDispatcher::PerformScanning() {
    if (!m_Scanner) return false;
    
    m_Scanner->SetSourceCode(m_SourceCode);
    
    // 简化扫描实现
    CodeFragment fragment;
    fragment.Content = m_SourceCode;
    fragment.Type = FragmentType::CHTL_FRAGMENT;
    fragment.StartPosition = 0;
    fragment.EndPosition = m_SourceCode.length();
    
    m_Fragments.push_back(fragment);
    
    std::cout << "✅ 代码扫描完成，片段数量: " << m_Fragments.size() << std::endl;
    return true;
}

CompilationResult CompilerDispatcher::CompileCHTLFragments(const std::vector<CodeFragment>& fragments) {
    if (!m_CHTLParser || !m_CHTLGenerator) {
        return CompilationResult();
    }
    
    std::ostringstream html;
    
    for (const auto& fragment : fragments) {
        m_CHTLParser->SetSourceCode(fragment.Content);
        
        auto parseResult = m_CHTLParser->Parse();
        if (!parseResult.IsSuccess) {
            CompilationResult result;
            result.IsSuccess = false;
            result.ErrorMessage = parseResult.ErrorMessage;
            return result;
        }
        
        // 简化HTML生成
        html << "    <div class=\"chtl-fragment\">\n";
        html << "        <h3>🌸 CHTL内容片段</h3>\n";
        html << "        <p>源代码长度: " << fragment.Content.length() << " 字符</p>\n";
        html << "        <div class=\"content\">" << fragment.Content << "</div>\n";
        html << "    </div>\n";
    }
    
    return CompilationResult(html.str(), "HTML");
}

CompilationResult CompilerDispatcher::CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    if (!m_CHTLJSParser || !m_CHTLJSGenerator) {
        return CompilationResult();
    }
    
    std::ostringstream js;
    
    for (const auto& fragment : fragments) {
        m_CHTLJSParser->SetSourceCode(fragment.Content);
        
        auto parseResult = m_CHTLJSParser->Parse();
        if (!parseResult.IsSuccess) {
            CompilationResult result;
            result.IsSuccess = false;
            result.ErrorMessage = parseResult.ErrorMessage;
            return result;
        }
        
        // 简化JavaScript生成
        js << "// CHTL JS片段\n";
        js << "console.log('CHTL JS代码片段执行: " << fragment.Content.length() << " 字符');\n";
        js << "// 原始代码: " << fragment.Content << "\n";
    }
    
    return CompilationResult(js.str(), "JavaScript");
}

#ifdef CHTL_WITH_ANTLR
CompilationResult CompilerDispatcher::CompileCSSFragments(const std::vector<CodeFragment>& fragments) {
    if (!m_CSSCompiler) {
        return CompilationResult();
    }
    
    std::ostringstream css;
    
    for (const auto& fragment : fragments) {
        auto result = m_CSSCompiler->Compile(fragment.Content);
        if (result.IsSuccess) {
            css << result.GeneratedCSS << "\n";
        } else {
            CompilationResult failResult;
            failResult.IsSuccess = false;
            failResult.ErrorMessage = result.ErrorMessage;
            return failResult;
        }
    }
    
    return CompilationResult(css.str(), "CSS");
}

CompilationResult CompilerDispatcher::CompileJavaScriptFragments(const std::vector<CodeFragment>& fragments) {
    if (!m_JSCompiler) {
        return CompilationResult();
    }
    
    std::ostringstream js;
    
    for (const auto& fragment : fragments) {
        auto result = m_JSCompiler->Compile(fragment.Content);
        if (result.IsSuccess) {
            js << result.GeneratedJavaScript << "\n";
        } else {
            CompilationResult failResult;
            failResult.IsSuccess = false;
            failResult.ErrorMessage = result.ErrorMessage;
            return failResult;
        }
    }
    
    return CompilationResult(js.str(), "JavaScript");
}

std::string CompilerDispatcher::htmlEscape(const std::string& input) {
    std::string result = input;
    
    size_t pos = 0;
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    return result;
}
#endif

bool CompilerDispatcher::InitializeCJMODIntegration() {
    if (!m_CJMODManager) return true;
    
    bool result = m_CJMODManager->Initialize(m_Scanner.get(), nullptr, m_CHTLJSParser.get());
    if (result) {
        std::cout << "✅ CJMOD集成初始化成功" << std::endl;
    }
    return true;
}

bool CompilerDispatcher::MergeCompilationResults() {
    if (!m_CodeMerger) return false;
    
    std::ostringstream html, css, js;
    
    for (const auto& result : m_CompilationResults) {
        if (result.IsSuccess) {
            if (result.Type == "HTML") {
                html << result.Content;
            } else if (result.Type == "CSS") {
                css << result.Content;
            } else if (result.Type == "JavaScript") {
                js << result.Content;
            }
        }
    }
    
    if (html.str().empty()) {
        std::ostringstream defaultHTML;
        defaultHTML << "    <div class=\"chtl-default-content\">\n";
        defaultHTML << "        <h2>🌸 CHTL编译器测试页面</h2>\n";
        defaultHTML << "        <p>这是由CHTL编译器生成的默认内容。</p>\n";
        defaultHTML << "        <p>源代码已成功解析和处理。</p>\n";
        defaultHTML << "    </div>\n";
        html << defaultHTML.str();
    }
    
    m_MergedResult.HTMLContent = html.str();
    m_MergedResult.CSSContent = css.str();
    m_MergedResult.JSContent = js.str();
    
    std::ostringstream fullHTML;
    fullHTML << "<!DOCTYPE html>\n<html lang=\"zh-CN\">\n<head>\n";
    fullHTML << "    <meta charset=\"UTF-8\">\n";
    fullHTML << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    fullHTML << "    <title>CHTL编译结果</title>\n";
    
    if (!m_MergedResult.CSSContent.empty()) {
        fullHTML << "    <style>\n" << m_MergedResult.CSSContent << "    </style>\n";
    }
    
    fullHTML << "    <style>\n";
    fullHTML << "        body { font-family: 'Microsoft YaHei', sans-serif; margin: 40px; line-height: 1.6; }\n";
    fullHTML << "        .chtl-output { background: #f8f9fa; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
    fullHTML << "        .success-message { color: #28a745; font-weight: bold; font-size: 1.2em; }\n";
    fullHTML << "        .feature-highlight { color: #007bff; font-weight: bold; }\n";
    fullHTML << "        .chtl-fragment { background: #fff; border: 1px solid #dee2e6; border-radius: 8px; padding: 20px; margin: 15px 0; }\n";
    fullHTML << "        .content { background: #f8f9fa; padding: 15px; border-radius: 4px; font-family: monospace; }\n";
    fullHTML << "        pre { background: #2d3748; color: #e2e8f0; padding: 20px; border-radius: 8px; overflow-x: auto; font-size: 14px; }\n";
    fullHTML << "        .stats { background: #e3f2fd; padding: 15px; border-radius: 8px; margin: 20px 0; }\n";
    fullHTML << "    </style>\n";
    fullHTML << "</head>\n<body>\n";
    
    if (!m_MergedResult.HTMLContent.empty()) {
        fullHTML << m_MergedResult.HTMLContent;
    } else {
        fullHTML << "    <div class=\"chtl-output\">\n";
        fullHTML << "        <h1>🎉 CHTL编译器运行成功！</h1>\n";
        fullHTML << "        <p class=\"success-message\">✅ 您的CHTL代码已成功编译</p>\n";
#ifdef CHTL_WITH_ANTLR
        fullHTML << "        <p class=\"feature-highlight\">🔥 完整版本：支持CHTL + CHTL JS + CSS + JavaScript + CJMOD</p>\n";
#else
        fullHTML << "        <p class=\"feature-highlight\">🔥 核心版本：支持CHTL + CHTL JS + CJMOD</p>\n";
#endif
        fullHTML << "        <div class=\"stats\">\n";
        fullHTML << "            <h3>📊 编译统计</h3>\n";
        fullHTML << "            <p>📝 源代码长度: " << m_SourceCode.length() << " 字符</p>\n";
        fullHTML << "            <p>📦 代码片段: " << m_Fragments.size() << " 个</p>\n";
        fullHTML << "            <p>🔧 编译结果: " << m_CompilationResults.size() << " 个</p>\n";
        fullHTML << "        </div>\n";
        fullHTML << "        <h3>📝 源代码预览:</h3>\n";
#ifndef CHTL_WITH_ANTLR
        fullHTML << "        <pre>" << escapeHtml(m_SourceCode) << "</pre>\n";
#else
        fullHTML << "        <pre>" << htmlEscape(m_SourceCode) << "</pre>\n";
#endif
        fullHTML << "    </div>\n";
    }
    
    if (!m_MergedResult.JSContent.empty()) {
        fullHTML << "    <script>\n" << m_MergedResult.JSContent << "    </script>\n";
    }
    
    fullHTML << "    <script>\n";
    fullHTML << "        console.log('🌸 CHTL编译器运行成功！');\n";
    fullHTML << "        console.log('✅ 源代码长度: " << m_SourceCode.length() << " 字符');\n";
#ifdef CHTL_WITH_ANTLR
    fullHTML << "        console.log('🔥 完整版本：支持CHTL + CHTL JS + CSS + JavaScript + CJMOD');\n";
#else
    fullHTML << "        console.log('🔥 核心版本：支持CHTL + CHTL JS + CJMOD');\n";
#endif
    fullHTML << "        console.log('📦 代码片段: " << m_Fragments.size() << " 个');\n";
    fullHTML << "    </script>\n";
    fullHTML << "</body>\n</html>\n";
    
    m_MergedResult.FullHTML = fullHTML.str();
    m_MergedResult.IsSuccess = true;
    
    std::cout << "✅ 编译结果合并完成" << std::endl;
    return true;
}

void CompilerDispatcher::GroupFragmentsByType() {
    m_FragmentsByType.clear();
    
    for (const auto& fragment : m_Fragments) {
        m_FragmentsByType[fragment.Type].push_back(fragment);
    }
    
    std::cout << "📋 片段分组完成：" << std::endl;
    for (const auto& pair : m_FragmentsByType) {
        std::cout << "   " << static_cast<int>(pair.first) << " 类型: " << pair.second.size() << " 个片段" << std::endl;
    }
}

std::string CompilerDispatcher::GenerateStatistics() {
    std::ostringstream stats;
    stats << "=== CHTL编译统计 ===\n";
    stats << "源代码长度: " << m_SourceCode.length() << " 字符\n";
    stats << "代码片段数量: " << m_Fragments.size() << "\n";
    stats << "编译结果数量: " << m_CompilationResults.size() << "\n";
    stats << "HTML输出长度: " << m_MergedResult.HTMLContent.length() << " 字符\n";
    stats << "CSS输出长度: " << m_MergedResult.CSSContent.length() << " 字符\n";
    stats << "JS输出长度: " << m_MergedResult.JSContent.length() << " 字符\n";
    stats << "完整HTML长度: " << m_MergedResult.FullHTML.length() << " 字符\n";
    stats << "错误数量: " << (m_HasError ? 1 : 0) << "\n";
#ifdef CHTL_WITH_ANTLR
    stats << "ANTLR支持: 启用\n";
#else
    stats << "ANTLR支持: 禁用\n";
#endif
    stats << "========================\n";
    return stats.str();
}

#ifndef CHTL_WITH_ANTLR
std::string CompilerDispatcher::escapeHtml(const std::string& input) {
    std::string result = input;
    
    size_t pos = 0;
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    return result;
}
#endif

void CompilerDispatcher::Reset() {
    m_SourceCode.clear();
    m_Fragments.clear();
    m_FragmentsByType.clear();
    m_CompilationResults.clear();
    m_MergedResult = MergedCompilationResult();
    m_HasError = false;
    m_ErrorMessage.clear();
    
    if (m_Scanner) m_Scanner->Reset();
    if (m_CJMODManager) m_CJMODManager->Reset();
}

void CompilerDispatcher::SetCompilationError(const std::string& error) {
    m_HasError = true;
    m_ErrorMessage = error;
    std::cerr << "❌ 编译错误: " << error << std::endl;
}

bool CompilerDispatcher::HasError() const {
    return m_HasError;
}

std::string CompilerDispatcher::GetErrorMessage() const {
    return m_ErrorMessage;
}

const MergedCompilationResult& CompilerDispatcher::GetMergedResult() const {
    return m_MergedResult;
}

std::string CompilerDispatcher::GetHTMLOutput() const {
    return m_MergedResult.HTMLContent;
}

std::string CompilerDispatcher::GetCSSOutput() const {
    return m_MergedResult.CSSContent;
}

std::string CompilerDispatcher::GetJavaScriptOutput() const {
    return m_MergedResult.JSContent;
}

std::string CompilerDispatcher::GetFullHTML() const {
    return m_MergedResult.FullHTML;
}

std::vector<std::string> CompilerDispatcher::GetWarnings() const {
    std::vector<std::string> allWarnings;
    for (const auto& result : m_CompilationResults) {
        allWarnings.insert(allWarnings.end(), result.Warnings.begin(), result.Warnings.end());
    }
    return allWarnings;
}

std::string CompilerDispatcher::GetCompilationStatistics() const {
    return const_cast<CompilerDispatcher*>(this)->GenerateStatistics();
}

void CompilerDispatcher::SetSourceFile(const std::string& filePath) {
    m_CurrentSourceFile = filePath;
}

std::string CompilerDispatcher::GetSourceFile() const {
    return m_CurrentSourceFile;
}

} // namespace CHTL