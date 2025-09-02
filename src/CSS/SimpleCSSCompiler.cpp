#include "SimpleCSSCompiler.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

SimpleCSSCompiler::SimpleCSSCompiler() 
    : m_EnableOptimization(true), m_EnableMinification(false) {
}

SimpleCSSCompilationResult SimpleCSSCompiler::CompileFragments(const std::vector<std::string>& cssFragments) {
    SimpleCSSCompilationResult result;
    
    // 合并所有CSS片段
    std::ostringstream combinedCSS;
    for (const auto& fragment : cssFragments) {
        combinedCSS << fragment << "\n";
    }
    
    return Compile(combinedCSS.str());
}

SimpleCSSCompilationResult SimpleCSSCompiler::Compile(const std::string& cssContent) {
    SimpleCSSCompilationResult result;
    
    // 重置状态
    m_Warnings.clear();
    m_Errors.clear();
    
    try {
        // 验证CSS语法
        if (!ValidateCSS(cssContent)) {
            result.IsSuccess = false;
            result.Errors = m_Errors;
            result.Warnings = m_Warnings;
            return result;
        }
        
        std::string processedCSS = cssContent;
        
        // 优化CSS
        if (m_EnableOptimization) {
            processedCSS = OptimizeCSS(processedCSS);
        }
        
        // 压缩CSS
        if (m_EnableMinification) {
            processedCSS = MinifyCSS(processedCSS);
        }
        
        result.OptimizedCSS = processedCSS;
        result.IsSuccess = true;
        result.Warnings = m_Warnings;
        result.Errors = m_Errors;
        
    } catch (const std::exception& e) {
        AddError("CSS编译异常: " + std::string(e.what()));
        result.IsSuccess = false;
        result.Errors = m_Errors;
    }
    
    return result;
}

void SimpleCSSCompiler::Reset() {
    m_Warnings.clear();
    m_Errors.clear();
}

std::string SimpleCSSCompiler::OptimizeCSS(const std::string& css) {
    std::string optimized = css;
    
    // 移除多余的空白字符
    optimized = std::regex_replace(optimized, std::regex(R"(\s+)"), " ");
    
    // 移除注释
    optimized = std::regex_replace(optimized, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除空规则
    optimized = std::regex_replace(optimized, std::regex(R"([^{}]*\{\s*\})"), "");
    
    // 合并相同选择器
    // TODO: 实现更复杂的选择器合并逻辑
    
    AddWarning("CSS优化完成，移除了注释和空白");
    
    return optimized;
}

std::string SimpleCSSCompiler::MinifyCSS(const std::string& css) {
    std::string minified = css;
    
    // 移除所有不必要的空白
    minified = std::regex_replace(minified, std::regex(R"(\s*{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*}\s*)"), "}");
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    minified = std::regex_replace(minified, std::regex(R"(\s*:\s*)"), ":");
    minified = std::regex_replace(minified, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除行末分号（最后一个属性）
    minified = std::regex_replace(minified, std::regex(R"(;\s*})"), "}");
    
    // 移除首尾空白
    minified = std::regex_replace(minified, std::regex(R"(^\s+|\s+$)"), "");
    
    AddWarning("CSS压缩完成，减少了 " + std::to_string(css.length() - minified.length()) + " 个字符");
    
    return minified;
}

bool SimpleCSSCompiler::ValidateCSS(const std::string& css) {
    // 基本CSS语法验证
    
    // 检查括号匹配
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : css) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
        
        if (braceCount < 0 || parenCount < 0 || bracketCount < 0) {
            AddError("CSS语法错误：括号不匹配");
            return false;
        }
    }
    
    if (braceCount != 0) {
        AddError("CSS语法错误：大括号不匹配，缺少 " + std::to_string(braceCount) + " 个闭合括号");
        return false;
    }
    
    if (parenCount != 0) {
        AddError("CSS语法错误：圆括号不匹配");
        return false;
    }
    
    if (bracketCount != 0) {
        AddError("CSS语法错误：方括号不匹配");
        return false;
    }
    
    // 检查基本CSS规则格式
    std::regex cssRulePattern(R"([^{}]*\{[^{}]*\})");
    std::sregex_iterator iter(css.begin(), css.end(), cssRulePattern);
    std::sregex_iterator end;
    
    bool hasValidRules = false;
    for (; iter != end; ++iter) {
        hasValidRules = true;
        std::string rule = iter->str();
        
        // 检查规则是否包含属性
        if (rule.find(':') == std::string::npos) {
            AddWarning("CSS规则可能缺少属性: " + rule.substr(0, 50) + "...");
        }
    }
    
    if (!hasValidRules && !css.empty()) {
        AddWarning("未检测到有效的CSS规则");
    }
    
    return true;
}

void SimpleCSSCompiler::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

void SimpleCSSCompiler::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

} // namespace CHTL