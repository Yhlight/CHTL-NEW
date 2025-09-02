#include "CSSCompiler.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

CSSCompiler::CSSCompiler() 
    : m_MinifyCSS(false), m_RemoveComments(false), 
      m_OptimizeSelectors(true), m_MergeRules(true) {
    InitializeANTLR();
}

CSSCompilationResult CSSCompiler::Compile(const std::string& cssCode) {
    CSSCompilationResult result;
    Reset();
    
    try {
        // 1. 使用ANTLR解析CSS语法树（暂时简化实现）
        if (!ParseCSSTree(cssCode)) {
            result.IsSuccess = false;
            result.Errors = m_Errors;
            return result;
        }
        
        // 2. 验证CSS语法
        if (!ValidateCSS(cssCode)) {
            AddWarning("CSS语法验证发现问题");
        }
        
        // 3. 应用CSS优化
        std::string optimizedCSS = ApplyCSSOptimizations(cssCode);
        
        // 4. 生成源映射（如果需要）
        // TODO: 实现源映射生成
        
        result.OptimizedCSS = optimizedCSS;
        result.IsSuccess = true;
        result.Warnings = m_Warnings;
        result.Errors = m_Errors;
        
    }
    catch (const std::exception& e) {
        AddError("CSS编译异常: " + std::string(e.what()));
        result.IsSuccess = false;
        result.Errors = m_Errors;
    }
    
    return result;
}

CSSCompilationResult CSSCompiler::CompileFragments(const std::vector<std::string>& cssFragments) {
    CSSCompilationResult result;
    
    std::ostringstream combinedCSS;
    
    // 合并所有CSS片段
    for (size_t i = 0; i < cssFragments.size(); ++i) {
        const auto& fragment = cssFragments[i];
        
        // 编译单个片段
        auto fragmentResult = Compile(fragment);
        
        if (!fragmentResult.IsSuccess) {
            AddError("CSS片段 " + std::to_string(i+1) + " 编译失败");
            result.IsSuccess = false;
        }
        else {
            combinedCSS << fragmentResult.OptimizedCSS << "\n";
        }
        
        // 收集警告和错误
        result.Warnings.insert(result.Warnings.end(), 
                              fragmentResult.Warnings.begin(), 
                              fragmentResult.Warnings.end());
        result.Errors.insert(result.Errors.end(), 
                            fragmentResult.Errors.begin(), 
                            fragmentResult.Errors.end());
    }
    
    if (result.IsSuccess) {
        result.OptimizedCSS = combinedCSS.str();
        
        // 对合并后的CSS进行最终优化
        if (m_MergeRules) {
            result.OptimizedCSS = MergeDuplicateRules(result.OptimizedCSS);
        }
    }
    
    return result;
}

bool CSSCompiler::ValidateCSS(const std::string& cssCode) {
    // 基础CSS语法验证（简化实现，等待ANTLR集成）
    
    // 检查基本的CSS结构
    std::regex cssRulePattern(R"([^{}]*\{[^{}]*\})");
    std::sregex_iterator iter(cssCode.begin(), cssCode.end(), cssRulePattern);
    std::sregex_iterator end;
    
    bool hasValidRules = false;
    
    for (; iter != end; ++iter) {
        hasValidRules = true;
        std::string rule = iter->str();
        
        // 检查选择器部分
        size_t bracePos = rule.find('{');
        if (bracePos != std::string::npos) {
            std::string selector = rule.substr(0, bracePos);
            
            // 简单的选择器验证
            if (selector.empty() || selector.find_first_not_of(" \t\n\r") == std::string::npos) {
                AddWarning("发现空选择器");
            }
        }
        
        // 检查属性部分
        size_t startBrace = rule.find('{');
        size_t endBrace = rule.find('}');
        if (startBrace != std::string::npos && endBrace != std::string::npos) {
            std::string properties = rule.substr(startBrace + 1, endBrace - startBrace - 1);
            
            // 检查属性格式
            if (!properties.empty()) {
                std::regex propertyPattern(R"([^:]+:[^;]+;?)");
                std::sregex_iterator propIter(properties.begin(), properties.end(), propertyPattern);
                
                if (propIter == std::sregex_iterator()) {
                    AddWarning("CSS属性格式可能有问题: " + properties);
                }
            }
        }
    }
    
    return hasValidRules || cssCode.empty();
}

std::string CSSCompiler::OptimizeCSS(const std::string& cssCode) {
    return ApplyCSSOptimizations(cssCode);
}

std::string CSSCompiler::ApplyCSSOptimizations(const std::string& cssCode) {
    std::string optimized = cssCode;
    
    // 1. 移除注释（如果启用）
    if (m_RemoveComments) {
        std::regex commentPattern(R"(/\*.*?\*/)");
        optimized = std::regex_replace(optimized, commentPattern, "");
    }
    
    // 2. 优化选择器（如果启用）
    if (m_OptimizeSelectors) {
        optimized = OptimizeSelectors(optimized);
    }
    
    // 3. 压缩CSS（如果启用）
    if (m_MinifyCSS) {
        optimized = MinifyCSS(optimized);
    }
    
    return optimized;
}

std::string CSSCompiler::MinifyCSS(const std::string& cssCode) {
    std::string minified = cssCode;
    
    // 移除多余的空白
    std::regex excessWhitespace(R"(\s+)");
    minified = std::regex_replace(minified, excessWhitespace, " ");
    
    // 移除不必要的分号和空格
    std::regex unnecessaryChars(R"(\s*;\s*\})");
    minified = std::regex_replace(minified, unnecessaryChars, "}");
    
    std::regex spaceAroundBraces(R"(\s*\{\s*)");
    minified = std::regex_replace(minified, spaceAroundBraces, "{");
    
    std::regex spaceAroundColons(R"(\s*:\s*)");
    minified = std::regex_replace(minified, spaceAroundColons, ":");
    
    // 移除行首行尾空白
    std::regex leadingTrailingSpace(R"(^\s+|\s+$)");
    minified = std::regex_replace(minified, leadingTrailingSpace, "");
    
    return minified;
}

std::string CSSCompiler::MergeDuplicateRules(const std::string& cssCode) {
    // 简化的规则合并实现
    std::unordered_map<std::string, std::string> selectorProperties;
    
    std::regex cssRulePattern(R"(([^{}]+)\{([^{}]*)\})");
    std::sregex_iterator iter(cssCode.begin(), cssCode.end(), cssRulePattern);
    std::sregex_iterator end;
    
    // 收集所有规则
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string selector = match[1].str();
        std::string properties = match[2].str();
        
        // 清理选择器
        selector.erase(0, selector.find_first_not_of(" \t\n\r"));
        selector.erase(selector.find_last_not_of(" \t\n\r") + 1);
        
        // 合并相同选择器的属性
        if (selectorProperties.find(selector) != selectorProperties.end()) {
            selectorProperties[selector] += " " + properties;
        }
        else {
            selectorProperties[selector] = properties;
        }
    }
    
    // 重新构建CSS
    std::ostringstream mergedCSS;
    for (const auto& rule : selectorProperties) {
        mergedCSS << rule.first << " { " << rule.second << " }\n";
    }
    
    return mergedCSS.str();
}

std::string CSSCompiler::OptimizeSelectors(const std::string& cssCode) {
    // 简化的选择器优化
    std::string optimized = cssCode;
    
    // 移除重复的空格
    std::regex multipleSpaces(R"(\s+)");
    optimized = std::regex_replace(optimized, multipleSpaces, " ");
    
    // 优化后代选择器
    std::regex descendantOptimize(R"(\s+>\s+)");
    optimized = std::regex_replace(optimized, descendantOptimize, ">");
    
    return optimized;
}

void CSSCompiler::SetCompilationOptions(bool minify, bool removeComments, 
                                        bool optimizeSelectors, bool mergeRules) {
    m_MinifyCSS = minify;
    m_RemoveComments = removeComments;
    m_OptimizeSelectors = optimizeSelectors;
    m_MergeRules = mergeRules;
}

void CSSCompiler::Reset() {
    m_Errors.clear();
    m_Warnings.clear();
}

void CSSCompiler::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

void CSSCompiler::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

// 暂时的简化实现（等待ANTLR集成）
void CSSCompiler::InitializeANTLR() {
    // TODO: 初始化ANTLR4组件
    // 需要CSS.g4语法文件和生成的C++代码
    AddWarning("ANTLR4集成待实现，当前使用简化的CSS处理");
}

bool CSSCompiler::ParseCSSTree(const std::string& cssCode) {
    // TODO: 使用ANTLR解析CSS语法树
    // 暂时使用简化的验证
    
    if (cssCode.empty()) {
        return true; // 空CSS是有效的
    }
    
    // 基础语法检查
    int braceCount = 0;
    for (char ch : cssCode) {
        if (ch == '{') braceCount++;
        else if (ch == '}') braceCount--;
        
        if (braceCount < 0) {
            AddError("CSS语法错误：不匹配的大括号");
            return false;
        }
    }
    
    if (braceCount != 0) {
        AddError("CSS语法错误：大括号不匹配");
        return false;
    }
    
    return true;
}

} // namespace CHTL