#include "CHTLCodeMerger.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLCodeMerger::CHTLCodeMerger() : m_MergeCounter(0) {
    // 设置默认合并选项
    m_Options = MergeOptions();
}

void CHTLCodeMerger::AddHTMLFragment(const std::string& content, const std::string& sourceFile, 
                                    size_t line, size_t column) {
    MergerCodeFragment fragment(CodeFragmentType::HTML_FRAGMENT, content, sourceFile, line, column);
    AddFragmentMetadata(fragment, "merge_order", std::to_string(m_MergeCounter++));
    m_HTMLFragments.push_back(fragment);
}

void CHTLCodeMerger::AddCSSFragment(const std::string& content, const std::string& sourceFile, 
                                   size_t line, size_t column) {
    MergerCodeFragment fragment(CodeFragmentType::CSS_FRAGMENT, content, sourceFile, line, column);
    AddFragmentMetadata(fragment, "merge_order", std::to_string(m_MergeCounter++));
    AddFragmentMetadata(fragment, "validated", ValidateCSSFragment(content) ? "true" : "false");
    m_CSSFragments.push_back(fragment);
}

void CHTLCodeMerger::AddJavaScriptFragment(const std::string& content, const std::string& sourceFile, 
                                          size_t line, size_t column) {
    MergerCodeFragment fragment(CodeFragmentType::JAVASCRIPT_FRAGMENT, content, sourceFile, line, column);
    AddFragmentMetadata(fragment, "merge_order", std::to_string(m_MergeCounter++));
    AddFragmentMetadata(fragment, "validated", ValidateJavaScriptFragment(content) ? "true" : "false");
    m_JavaScriptFragments.push_back(fragment);
}

void CHTLCodeMerger::AddFragments(const std::vector<MergerCodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        switch (fragment.Type) {
            case CodeFragmentType::HTML_FRAGMENT:
                m_HTMLFragments.push_back(fragment);
                break;
            case CodeFragmentType::CSS_FRAGMENT:
                m_CSSFragments.push_back(fragment);
                break;
            case CodeFragmentType::JAVASCRIPT_FRAGMENT:
                m_JavaScriptFragments.push_back(fragment);
                break;
            default:
                // CHTL和CHTL JS片段需要先由对应编译器处理
                break;
        }
    }
}

CodeMergeResult CHTLCodeMerger::MergeCode() {
    CodeMergeResult result;
    
    try {
        // 验证片段完整性
        if (!ValidateFragments()) {
            result.IsSuccess = false;
            result.Errors.push_back("片段验证失败");
            return result;
        }
        
        // 1. 合并各类型片段为完整代码
        std::string mergedHTML = MergeHTMLFragments();
        std::string mergedCSS = MergeCSSFragments();
        std::string mergedJavaScript = MergeJavaScriptFragments();
        
        // 2. 根据合并策略生成最终结果
        switch (m_Options.Strategy) {
            case MergeStrategy::INLINE_MERGE:
                result = InlineMergeStrategy(mergedHTML, mergedCSS, mergedJavaScript);
                break;
            case MergeStrategy::SEPARATE_FILES:
                result = SeparateFilesMergeStrategy(mergedHTML, mergedCSS, mergedJavaScript);
                break;
            case MergeStrategy::HYBRID_MERGE:
                result = HybridMergeStrategy(mergedHTML, mergedCSS, mergedJavaScript);
                break;
        }
        
        // 3. 设置合并后的完整代码
        result.MergedHTML = mergedHTML;
        result.MergedCSS = mergedCSS;
        result.MergedJavaScript = mergedJavaScript;
        result.IsSuccess = true;
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.Errors.push_back("代码合并异常: " + std::string(e.what()));
    }
    
    return result;
}

std::string CHTLCodeMerger::MergeCSSFragments() {
    if (m_CSSFragments.empty()) {
        return "";
    }
    
    std::ostringstream mergedCSS;
    
    // 添加合并头部注释
    mergedCSS << "/* CHTL合并CSS - 生成自 " << m_CSSFragments.size() << " 个片段 */\n\n";
    
    // 优化片段顺序（如果启用）
    auto fragments = m_CSSFragments;
    if (m_Options.OptimizeOrder) {
        fragments = OptimizeFragmentOrder(fragments);
    }
    
    // 合并所有CSS片段
    for (const auto& fragment : fragments) {
        if (!fragment.Content.empty()) {
            // 添加源信息注释
            if (!fragment.SourceFile.empty()) {
                mergedCSS << "/* 源文件: " << fragment.SourceFile;
                if (fragment.LineNumber > 0) {
                    mergedCSS << " 行:" << fragment.LineNumber;
                }
                mergedCSS << " */\n";
            }
            
            // 添加片段内容
            mergedCSS << fragment.Content;
            
            // 确保片段以换行结束
            if (!fragment.Content.empty() && fragment.Content.back() != '\n') {
                mergedCSS << "\n";
            }
            
            mergedCSS << "\n";
        }
    }
    
    mergedCSS << "/* CSS合并完成 */\n";
    
    return mergedCSS.str();
}

std::string CHTLCodeMerger::MergeJavaScriptFragments() {
    if (m_JavaScriptFragments.empty()) {
        return "";
    }
    
    std::ostringstream mergedJS;
    
    // 添加合并头部注释
    mergedJS << "// CHTL合并JavaScript - 生成自 " << m_JavaScriptFragments.size() << " 个片段\n\n";
    
    // 添加严格模式（如果适用）
    mergedJS << "(function() {\n";
    mergedJS << "    'use strict';\n\n";
    
    // 优化片段顺序（如果启用）
    auto fragments = m_JavaScriptFragments;
    if (m_Options.OptimizeOrder) {
        fragments = OptimizeFragmentOrder(fragments);
    }
    
    // 合并所有JavaScript片段
    for (const auto& fragment : fragments) {
        if (!fragment.Content.empty()) {
            // 添加源信息注释
            if (!fragment.SourceFile.empty()) {
                mergedJS << "    // 源文件: " << fragment.SourceFile;
                if (fragment.LineNumber > 0) {
                    mergedJS << " 行:" << fragment.LineNumber;
                }
                mergedJS << "\n";
            }
            
            // 缩进片段内容
            std::string indentedContent = fragment.Content;
            std::regex linePattern(R"(^(.*)$)", std::regex_constants::multiline);
            indentedContent = std::regex_replace(indentedContent, linePattern, "    $1");
            
            mergedJS << indentedContent;
            
            // 确保片段以换行结束
            if (!fragment.Content.empty() && fragment.Content.back() != '\n') {
                mergedJS << "\n";
            }
            
            mergedJS << "\n";
        }
    }
    
    mergedJS << "})();\n";
    mergedJS << "// JavaScript合并完成\n";
    
    return mergedJS.str();
}

std::string CHTLCodeMerger::MergeHTMLFragments() {
    if (m_HTMLFragments.empty()) {
        return "";
    }
    
    std::ostringstream mergedHTML;
    
    // 优化片段顺序（如果启用）
    auto fragments = m_HTMLFragments;
    if (m_Options.OptimizeOrder) {
        fragments = OptimizeFragmentOrder(fragments);
    }
    
    // 合并所有HTML片段
    for (const auto& fragment : fragments) {
        if (!fragment.Content.empty()) {
            mergedHTML << fragment.Content;
            
            // 确保片段以换行结束
            if (!fragment.Content.empty() && fragment.Content.back() != '\n') {
                mergedHTML << "\n";
            }
        }
    }
    
    return mergedHTML.str();
}

std::string CHTLCodeMerger::GenerateCompleteHTML(const std::string& htmlContent, 
                                                 const std::string& cssContent, 
                                                 const std::string& jsContent) {
    std::ostringstream completeHTML;
    
    completeHTML << "<!DOCTYPE html>\n";
    completeHTML << "<html lang=\"zh-CN\">\n";
    completeHTML << "<head>\n";
    completeHTML << "    <meta charset=\"UTF-8\">\n";
    completeHTML << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    completeHTML << "    <title>CHTL生成页面</title>\n";
    
    // 根据合并策略添加CSS
    if (m_Options.Strategy == MergeStrategy::INLINE_MERGE || 
        m_Options.Strategy == MergeStrategy::HYBRID_MERGE) {
        if (!cssContent.empty()) {
            completeHTML << "    <style>\n";
            completeHTML << cssContent;
            completeHTML << "    </style>\n";
        }
    }
    else if (m_Options.Strategy == MergeStrategy::SEPARATE_FILES) {
        if (!cssContent.empty()) {
            completeHTML << "    <link rel=\"stylesheet\" href=\"" << m_Options.CSSFileName << "\">\n";
        }
    }
    
    completeHTML << "</head>\n";
    completeHTML << "<body>\n";
    
    // 添加HTML内容
    completeHTML << htmlContent;
    
    // 根据合并策略添加JavaScript
    if (m_Options.Strategy == MergeStrategy::INLINE_MERGE || 
        m_Options.Strategy == MergeStrategy::HYBRID_MERGE) {
        if (!jsContent.empty()) {
            completeHTML << "    <script>\n";
            completeHTML << jsContent;
            completeHTML << "    </script>\n";
        }
    }
    else if (m_Options.Strategy == MergeStrategy::SEPARATE_FILES) {
        if (!jsContent.empty()) {
            completeHTML << "    <script src=\"" << m_Options.JavaScriptFileName << "\"></script>\n";
        }
    }
    
    completeHTML << "</body>\n";
    completeHTML << "</html>\n";
    
    return completeHTML.str();
}

void CHTLCodeMerger::SetMergeOptions(const MergeOptions& options) {
    m_Options = options;
}

void CHTLCodeMerger::SetCurrentSourceFile(const std::string& sourceFile) {
    m_CurrentSourceFile = sourceFile;
}

std::unordered_map<std::string, size_t> CHTLCodeMerger::GetFragmentStatistics() const {
    std::unordered_map<std::string, size_t> stats;
    
    stats["HTML"] = m_HTMLFragments.size();
    stats["CSS"] = m_CSSFragments.size();
    stats["JavaScript"] = m_JavaScriptFragments.size();
    stats["Total"] = m_HTMLFragments.size() + m_CSSFragments.size() + m_JavaScriptFragments.size();
    
    return stats;
}

void CHTLCodeMerger::ClearFragments() {
    m_HTMLFragments.clear();
    m_CSSFragments.clear();
    m_JavaScriptFragments.clear();
    m_MergeCounter = 0;
}

void CHTLCodeMerger::Reset() {
    ClearFragments();
    m_CurrentSourceFile.clear();
    m_Options = MergeOptions();
}

bool CHTLCodeMerger::ValidateFragments() const {
    // 验证所有片段的有效性
    
    // 检查CSS片段
    for (const auto& fragment : m_CSSFragments) {
        if (!ValidateCSSFragment(fragment.Content)) {
            return false;
        }
    }
    
    // 检查JavaScript片段
    for (const auto& fragment : m_JavaScriptFragments) {
        if (!ValidateJavaScriptFragment(fragment.Content)) {
            return false;
        }
    }
    
    return true;
}

std::vector<MergerCodeFragment> CHTLCodeMerger::OptimizeFragmentOrder(const std::vector<MergerCodeFragment>& fragments) {
    auto optimized = fragments;
    
    // 按行号排序（保持原始顺序）
    std::sort(optimized.begin(), optimized.end(), 
              [](const MergerCodeFragment& a, const MergerCodeFragment& b) {
                  if (a.SourceFile != b.SourceFile) {
                      return a.SourceFile < b.SourceFile;
                  }
                  return a.LineNumber < b.LineNumber;
              });
    
    return optimized;
}

std::string CHTLCodeMerger::MergeSameTypeFragments(const std::vector<MergerCodeFragment>& fragments) {
    std::ostringstream merged;
    
    for (const auto& fragment : fragments) {
        merged << fragment.Content;
        if (!fragment.Content.empty() && fragment.Content.back() != '\n') {
            merged << "\n";
        }
    }
    
    return merged.str();
}

std::string CHTLCodeMerger::GenerateSourceMap(const std::vector<MergerCodeFragment>& originalFragments, 
                                              const std::string& mergedContent) {
    if (!m_Options.GenerateSourceMap) {
        return "";
    }
    
    std::ostringstream sourceMap;
    
    sourceMap << "{\n";
    sourceMap << "  \"version\": 3,\n";
    sourceMap << "  \"sources\": [";
    
    // 收集所有源文件
    std::vector<std::string> sourceFiles;
    for (const auto& fragment : originalFragments) {
        if (!fragment.SourceFile.empty() && 
            std::find(sourceFiles.begin(), sourceFiles.end(), fragment.SourceFile) == sourceFiles.end()) {
            sourceFiles.push_back(fragment.SourceFile);
        }
    }
    
    for (size_t i = 0; i < sourceFiles.size(); ++i) {
        if (i > 0) sourceMap << ", ";
        sourceMap << "\"" << sourceFiles[i] << "\"";
    }
    
    sourceMap << "],\n";
    sourceMap << "  \"mappings\": \"AAAA\",\n";  // 简化的映射
    sourceMap << "  \"names\": []\n";
    sourceMap << "}\n";
    
    return sourceMap.str();
}

CodeMergeResult CHTLCodeMerger::InlineMergeStrategy(const std::string& htmlContent, 
                                                   const std::string& cssContent, 
                                                   const std::string& jsContent) {
    CodeMergeResult result;
    
    // 内联合并：所有CSS和JavaScript都嵌入HTML中
    result.FullHTML = GenerateCompleteHTML(htmlContent, cssContent, jsContent);
    result.MergedHTML = htmlContent;
    result.MergedCSS = cssContent;
    result.MergedJavaScript = jsContent;
    result.IsSuccess = true;
    
            // 生成源映射
        if (m_Options.GenerateSourceMap) {
            std::vector<MergerCodeFragment> allFragments;
            allFragments.insert(allFragments.end(), m_HTMLFragments.begin(), m_HTMLFragments.end());
            allFragments.insert(allFragments.end(), m_CSSFragments.begin(), m_CSSFragments.end());
            allFragments.insert(allFragments.end(), m_JavaScriptFragments.begin(), m_JavaScriptFragments.end());
        
        // 这里可以为HTML、CSS、JavaScript分别生成源映射
        // 简化实现，只生成一个总的源映射
        result.Warnings.push_back("源映射生成：内联模式");
    }
    
    return result;
}

CodeMergeResult CHTLCodeMerger::SeparateFilesMergeStrategy(const std::string& htmlContent, 
                                                          const std::string& cssContent, 
                                                          const std::string& jsContent) {
    CodeMergeResult result;
    
    // 分离文件合并：CSS和JavaScript保存为独立文件
    result.FullHTML = GenerateCompleteHTML(htmlContent, "", "");  // HTML中只包含链接
    result.MergedHTML = htmlContent;
    result.MergedCSS = cssContent;
    result.MergedJavaScript = jsContent;
    result.IsSuccess = true;
    
    // 添加文件生成提示
    if (!cssContent.empty()) {
        result.Warnings.push_back("需要生成CSS文件: " + m_Options.CSSFileName);
    }
    if (!jsContent.empty()) {
        result.Warnings.push_back("需要生成JavaScript文件: " + m_Options.JavaScriptFileName);
    }
    
    return result;
}

CodeMergeResult CHTLCodeMerger::HybridMergeStrategy(const std::string& htmlContent, 
                                                   const std::string& cssContent, 
                                                   const std::string& jsContent) {
    CodeMergeResult result;
    
    // 混合合并：关键CSS内联，其他CSS和JavaScript分离
    std::string criticalCSS;
    std::string nonCriticalCSS;
    
    // 简单的关键CSS检测（实际应用中可以更复杂）
    std::vector<std::string> criticalSelectors = {
        "body", "html", ".container", ".header", ".main", ".footer"
    };
    
    std::istringstream cssStream(cssContent);
    std::string line;
    bool isCritical = false;
    
    while (std::getline(cssStream, line)) {
        // 检查是否为关键CSS
        isCritical = false;
        for (const auto& selector : criticalSelectors) {
            if (line.find(selector) != std::string::npos) {
                isCritical = true;
                break;
            }
        }
        
        if (isCritical) {
            criticalCSS += line + "\n";
        }
        else {
            nonCriticalCSS += line + "\n";
        }
    }
    
    // 生成混合HTML
    result.FullHTML = GenerateCompleteHTML(htmlContent, criticalCSS, "");
    
    // 添加非关键CSS链接
    if (!nonCriticalCSS.empty()) {
        size_t headPos = result.FullHTML.find("</head>");
        if (headPos != std::string::npos) {
            std::string cssLink = "    <link rel=\"stylesheet\" href=\"" + m_Options.CSSFileName + "\">\n";
            result.FullHTML.insert(headPos, cssLink);
        }
    }
    
    // 添加JavaScript链接
    if (!jsContent.empty()) {
        size_t bodyPos = result.FullHTML.find("</body>");
        if (bodyPos != std::string::npos) {
            std::string jsLink = "    <script src=\"" + m_Options.JavaScriptFileName + "\"></script>\n";
            result.FullHTML.insert(bodyPos, jsLink);
        }
    }
    
    result.MergedHTML = htmlContent;
    result.MergedCSS = nonCriticalCSS;  // 只有非关键CSS需要单独文件
    result.MergedJavaScript = jsContent;
    result.IsSuccess = true;
    
    result.Warnings.push_back("混合策略：关键CSS内联，其他CSS/JS分离");
    
    return result;
}

void CHTLCodeMerger::AddFragmentMetadata(MergerCodeFragment& fragment, const std::string& key, const std::string& value) {
    fragment.Metadata[key] = value;
}

bool CHTLCodeMerger::ValidateCSSFragment(const std::string& cssContent) const {
    if (cssContent.empty()) {
        return true;  // 空片段是有效的
    }
    
    // 简单的CSS片段验证
    // 检查大括号匹配
    int braceCount = 0;
    for (char ch : cssContent) {
        if (ch == '{') braceCount++;
        else if (ch == '}') braceCount--;
        
        if (braceCount < 0) {
            return false;  // 大括号不匹配
        }
    }
    
    return braceCount == 0;  // 所有大括号都匹配
}

bool CHTLCodeMerger::ValidateJavaScriptFragment(const std::string& jsContent) const {
    if (jsContent.empty()) {
        return true;  // 空片段是有效的
    }
    
    // 简单的JavaScript片段验证
    // 检查大括号匹配
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char ch : jsContent) {
        if (ch == '{') braceCount++;
        else if (ch == '}') braceCount--;
        else if (ch == '(') parenCount++;
        else if (ch == ')') parenCount--;
        else if (ch == '[') bracketCount++;
        else if (ch == ']') bracketCount--;
        
        if (braceCount < 0 || parenCount < 0 || bracketCount < 0) {
            return false;  // 括号不匹配
        }
    }
    
    return braceCount == 0 && parenCount == 0 && bracketCount == 0;  // 所有括号都匹配
}

// CodeMergerFactory实现
std::unique_ptr<CHTLCodeMerger> CodeMergerFactory::CreateInlineMerger() {
    auto merger = std::make_unique<CHTLCodeMerger>();
    
    MergeOptions options;
    options.Strategy = MergeStrategy::INLINE_MERGE;
    options.MinifyCSS = false;
    options.MinifyJavaScript = false;
    options.RemoveComments = false;
    options.OptimizeOrder = true;
    
    merger->SetMergeOptions(options);
    return merger;
}

std::unique_ptr<CHTLCodeMerger> CodeMergerFactory::CreateSeparateFilesMerger(
    const std::string& cssFileName, const std::string& jsFileName) {
    
    auto merger = std::make_unique<CHTLCodeMerger>();
    
    MergeOptions options;
    options.Strategy = MergeStrategy::SEPARATE_FILES;
    options.CSSFileName = cssFileName;
    options.JavaScriptFileName = jsFileName;
    options.MinifyCSS = true;
    options.MinifyJavaScript = true;
    options.RemoveComments = true;
    options.OptimizeOrder = true;
    options.GenerateSourceMap = true;
    
    merger->SetMergeOptions(options);
    return merger;
}

std::unique_ptr<CHTLCodeMerger> CodeMergerFactory::CreateHybridMerger(size_t criticalCSSThreshold) {
    auto merger = std::make_unique<CHTLCodeMerger>();
    
    MergeOptions options;
    options.Strategy = MergeStrategy::HYBRID_MERGE;
    options.MinifyCSS = true;
    options.MinifyJavaScript = true;
    options.RemoveComments = true;
    options.OptimizeOrder = true;
    options.GenerateSourceMap = true;
    
    merger->SetMergeOptions(options);
    return merger;
}

std::unique_ptr<CHTLCodeMerger> CodeMergerFactory::CreateProductionMerger() {
    auto merger = std::make_unique<CHTLCodeMerger>();
    
    MergeOptions options;
    options.Strategy = MergeStrategy::HYBRID_MERGE;
    options.MinifyCSS = true;
    options.MinifyJavaScript = true;
    options.RemoveComments = true;
    options.OptimizeOrder = true;
    options.GenerateSourceMap = true;
    
    merger->SetMergeOptions(options);
    return merger;
}

std::unique_ptr<CHTLCodeMerger> CodeMergerFactory::CreateDevelopmentMerger() {
    auto merger = std::make_unique<CHTLCodeMerger>();
    
    MergeOptions options;
    options.Strategy = MergeStrategy::INLINE_MERGE;
    options.MinifyCSS = false;
    options.MinifyJavaScript = false;
    options.RemoveComments = false;
    options.OptimizeOrder = false;
    options.GenerateSourceMap = true;
    
    merger->SetMergeOptions(options);
    return merger;
}

} // namespace CHTL