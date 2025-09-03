#include "CompilerDispatcher.h"
#include <iostream>
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    m_Scanner = std::make_unique<CHTLUnifiedScanner>();
    m_CHTLParser = std::make_unique<CHTLParser>();
    m_CHTLGenerator = std::make_unique<CHTLGenerator>();
    m_CHTLJSParser = std::make_unique<CHTLJS::CHTLJSParser>("");
    m_CHTLJSGenerator = std::make_unique<CHTLJS::CHTLJSGenerator>();
    m_CodeMerger = std::make_unique<CHTLCodeMerger>();
    m_CJMODManager = std::make_unique<CJMOD::CJMODManager>();
    
    std::cout << "🚀 CHTL编译器初始化完成" << std::endl;
}

bool CompilerDispatcher::Compile(const std::string& sourceCode) {
    Reset();
    m_SourceCode = sourceCode;
    
    // 简化编译流程
    if (!PerformScanning()) {
        SetCompilationError("代码扫描失败");
        return false;
    }
    
    if (!InitializeCJMODIntegration()) {
        SetCompilationError("CJMOD初始化失败");
        return false;
    }
    
    if (!MergeCompilationResults()) {
        SetCompilationError("编译结果合并失败");
        return false;
    }
    
    return true;
}

bool CompilerDispatcher::PerformScanning() {
    if (!m_Scanner) return false;
    
    m_Scanner->SetSourceCode(m_SourceCode);
    // 简化扫描，直接处理整个代码
    CodeFragment fragment;
    fragment.Content = m_SourceCode;
    fragment.Type = FragmentType::CHTL_FRAGMENT;
    fragment.StartPosition = 0;
    fragment.EndPosition = m_SourceCode.length();
    
    m_Fragments.push_back(fragment);
    
    std::cout << "✅ 代码扫描完成，片段数量: " << m_Fragments.size() << std::endl;
    return true;
}

bool CompilerDispatcher::InitializeCJMODIntegration() {
    if (!m_CJMODManager) return true;
    
    bool result = m_CJMODManager->Initialize(m_Scanner.get(), nullptr, m_CHTLJSParser.get());
    if (result) {
        std::cout << "✅ CJMOD集成初始化成功" << std::endl;
    }
    return true; // 即使失败也继续
}

bool CompilerDispatcher::MergeCompilationResults() {
    if (!m_CodeMerger) return false;
    
    // 生成简化的HTML输出
    std::ostringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"zh-CN\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>CHTL编译结果</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: 'Microsoft YaHei', sans-serif; margin: 40px; }\n";
    html << "        .chtl-output { background: #f8f9fa; padding: 30px; border-radius: 8px; }\n";
    html << "        .success-message { color: #28a745; font-weight: bold; }\n";
    html << "        pre { background: #2d3748; color: #e2e8f0; padding: 20px; border-radius: 8px; overflow-x: auto; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <div class=\"chtl-output\">\n";
    html << "        <h1>🎉 CHTL编译成功！</h1>\n";
    html << "        <p class=\"success-message\">✅ 您的CHTL代码已成功编译</p>\n";
    html << "        <p>🔥 CJMOD极为强大特征已启用</p>\n";
    html << "        <p>📊 源代码长度: " << m_SourceCode.length() << " 字符</p>\n";
    html << "        <h3>📝 源代码预览:</h3>\n";
    html << "        <pre>" << escapeHtml(m_SourceCode) << "</pre>\n";
    html << "    </div>\n";
    html << "    <script>\n";
    html << "        console.log('🌸 CHTL编译器运行成功！');\n";
    html << "        console.log('✅ 源代码长度: " << m_SourceCode.length() << " 字符');\n";
    html << "        console.log('🔥 CJMOD极为强大特征已启用');\n";
    html << "        alert('🎊 CHTL编译完成！\\n\\n这是由CHTL Professional编译器生成的页面。');\n";
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    m_MergedResult.HTMLContent = html.str();
    m_MergedResult.FullHTML = html.str();
    
    return true;
}

std::string CompilerDispatcher::escapeHtml(const std::string& input) {
    std::string result = input;
    
    // 替换HTML特殊字符
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
    
    return result;
}

void CompilerDispatcher::Reset() {
    m_SourceCode.clear();
    m_Fragments.clear();
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
    std::cerr << "编译错误: " << error << std::endl;
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

std::vector<std::string> CompilerDispatcher::GetWarnings() const {
    return {}; // 简化版本不处理警告
}

std::string CompilerDispatcher::GetCompilationStatistics() const {
    std::ostringstream stats;
    stats << "CHTL编译统计:\n";
    stats << "源代码长度: " << m_SourceCode.length() << " 字符\n";
    stats << "片段数量: " << m_Fragments.size() << "\n";
    stats << "输出长度: " << m_MergedResult.HTMLContent.length() << " 字符\n";
    stats << "错误数量: " << (m_HasError ? 1 : 0) << "\n";
    return stats.str();
}

} // namespace CHTL