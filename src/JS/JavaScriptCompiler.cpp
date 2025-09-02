#include "JavaScriptCompiler.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

JavaScriptCompiler::JavaScriptCompiler() 
    : m_MinifyJS(false), m_RemoveComments(false), 
      m_OptimizeVariables(false), m_RemoveDeadCode(false) {
    InitializeANTLR();
}

JavaScriptCompilationResult JavaScriptCompiler::Compile(const std::string& jsCode) {
    JavaScriptCompilationResult result;
    Reset();
    
    try {
        // 1. 使用ANTLR解析JavaScript语法树（暂时简化实现）
        if (!ParseJavaScriptTree(jsCode)) {
            result.IsSuccess = false;
            result.Errors = m_Errors;
            return result;
        }
        
        // 2. 验证JavaScript语法
        if (!ValidateJavaScript(jsCode)) {
            AddWarning("JavaScript语法验证发现问题");
        }
        
        // 3. 应用JavaScript优化
        std::string optimizedJS = ApplyJavaScriptOptimizations(jsCode);
        
        // 4. 生成源映射（如果需要）
        // TODO: 实现源映射生成
        
        result.OptimizedJS = optimizedJS;
        result.IsSuccess = true;
        result.Warnings = m_Warnings;
        result.Errors = m_Errors;
        
    }
    catch (const std::exception& e) {
        AddError("JavaScript编译异常: " + std::string(e.what()));
        result.IsSuccess = false;
        result.Errors = m_Errors;
    }
    
    return result;
}

JavaScriptCompilationResult JavaScriptCompiler::CompileFragments(const std::vector<std::string>& jsFragments) {
    JavaScriptCompilationResult result;
    
    std::ostringstream combinedJS;
    
    // 合并所有JavaScript片段
    for (size_t i = 0; i < jsFragments.size(); ++i) {
        const auto& fragment = jsFragments[i];
        
        // 编译单个片段
        auto fragmentResult = Compile(fragment);
        
        if (!fragmentResult.IsSuccess) {
            AddError("JavaScript片段 " + std::to_string(i+1) + " 编译失败");
            result.IsSuccess = false;
        }
        else {
            combinedJS << fragmentResult.OptimizedJS << "\n";
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
        result.OptimizedJS = combinedJS.str();
        
        // 对合并后的JavaScript进行最终优化
        if (m_RemoveDeadCode) {
            result.OptimizedJS = RemoveDeadCode(result.OptimizedJS);
        }
    }
    
    return result;
}

bool JavaScriptCompiler::ValidateJavaScript(const std::string& jsCode) {
    // 基础JavaScript语法验证（简化实现，等待ANTLR集成）
    
    if (jsCode.empty()) {
        return true; // 空JavaScript是有效的
    }
    
    // 检查基本的JavaScript结构
    return ValidateJavaScriptStructure(jsCode);
}

bool JavaScriptCompiler::ValidateJavaScriptStructure(const std::string& jsCode) {
    // 检查括号匹配
    int parentheses = 0, braces = 0, brackets = 0;
    bool inString = false;
    bool inComment = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < jsCode.length(); ++i) {
        char ch = jsCode[i];
        
        // 处理字符串
        if (!inComment && (ch == '"' || ch == '\'')) {
            if (!inString) {
                inString = true;
                stringChar = ch;
            }
            else if (ch == stringChar) {
                inString = false;
                stringChar = '\0';
            }
        }
        
        // 处理注释
        if (!inString && i < jsCode.length() - 1) {
            if (ch == '/' && jsCode[i+1] == '/') {
                inComment = true;
            }
            else if (ch == '/' && jsCode[i+1] == '*') {
                inComment = true;
            }
            else if (inComment && ch == '*' && jsCode[i+1] == '/') {
                inComment = false;
                ++i; // 跳过下一个字符
                continue;
            }
        }
        
        if (ch == '\n') {
            inComment = false; // 单行注释结束
        }
        
        // 检查括号（不在字符串或注释中）
        if (!inString && !inComment) {
            switch (ch) {
                case '(':
                    parentheses++;
                    break;
                case ')':
                    parentheses--;
                    if (parentheses < 0) {
                        AddError("JavaScript语法错误：不匹配的圆括号");
                        return false;
                    }
                    break;
                case '{':
                    braces++;
                    break;
                case '}':
                    braces--;
                    if (braces < 0) {
                        AddError("JavaScript语法错误：不匹配的大括号");
                        return false;
                    }
                    break;
                case '[':
                    brackets++;
                    break;
                case ']':
                    brackets--;
                    if (brackets < 0) {
                        AddError("JavaScript语法错误：不匹配的方括号");
                        return false;
                    }
                    break;
            }
        }
    }
    
    // 检查最终括号匹配
    if (parentheses != 0) {
        AddError("JavaScript语法错误：圆括号不匹配");
        return false;
    }
    if (braces != 0) {
        AddError("JavaScript语法错误：大括号不匹配");
        return false;
    }
    if (brackets != 0) {
        AddError("JavaScript语法错误：方括号不匹配");
        return false;
    }
    
    return true;
}

std::string JavaScriptCompiler::OptimizeJavaScript(const std::string& jsCode) {
    return ApplyJavaScriptOptimizations(jsCode);
}

std::string JavaScriptCompiler::ApplyJavaScriptOptimizations(const std::string& jsCode) {
    std::string optimized = jsCode;
    
    // 1. 移除注释（如果启用）
    if (m_RemoveComments) {
        // 移除单行注释
        std::regex singleLineComment(R"(//.*?$)");
        optimized = std::regex_replace(optimized, singleLineComment, "");
        
        // 移除多行注释
        std::regex multiLineComment(R"(/\*.*?\*/)");
        optimized = std::regex_replace(optimized, multiLineComment, "");
    }
    
    // 2. 优化变量名（如果启用）
    if (m_OptimizeVariables) {
        optimized = OptimizeVariableNames(optimized);
    }
    
    // 3. 移除死代码（如果启用）
    if (m_RemoveDeadCode) {
        optimized = RemoveDeadCode(optimized);
    }
    
    // 4. 压缩JavaScript（如果启用）
    if (m_MinifyJS) {
        optimized = MinifyJavaScript(optimized);
    }
    
    return optimized;
}

std::string JavaScriptCompiler::MinifyJavaScript(const std::string& jsCode) {
    std::string minified = jsCode;
    
    // 移除多余的空白（保留字符串内的空白）
    bool inString = false;
    char stringChar = '\0';
    std::string result;
    
    for (size_t i = 0; i < minified.length(); ++i) {
        char ch = minified[i];
        
        // 处理字符串
        if (ch == '"' || ch == '\'') {
            if (!inString) {
                inString = true;
                stringChar = ch;
            }
            else if (ch == stringChar) {
                inString = false;
                stringChar = '\0';
            }
        }
        
        // 在字符串外压缩空白
        if (!inString && std::isspace(ch)) {
            if (!result.empty() && !std::isspace(result.back())) {
                result += ' ';
            }
        }
        else {
            result += ch;
        }
    }
    
    // 移除不必要的分号和空格
    std::regex unnecessarySpaces(R"(\s*([{}();,])\s*)");
    result = std::regex_replace(result, unnecessarySpaces, "$1");
    
    return result;
}

std::string JavaScriptCompiler::OptimizeVariableNames(const std::string& jsCode) {
    // 简化的变量名优化（实际需要更复杂的作用域分析）
    std::string optimized = jsCode;
    
    // 暂时不实现变量名优化，因为需要完整的作用域分析
    AddWarning("变量名优化需要完整的作用域分析，暂时跳过");
    
    return optimized;
}

std::string JavaScriptCompiler::RemoveDeadCode(const std::string& jsCode) {
    // 简化的死代码移除
    std::string optimized = jsCode;
    
    // 移除明显的死代码
    std::regex deadCodePatterns[] = {
        std::regex(R"(if\s*\(\s*false\s*\)\s*\{[^{}]*\})"),  // if (false) { ... }
        std::regex(R"(while\s*\(\s*false\s*\)\s*\{[^{}]*\})") // while (false) { ... }
    };
    
    for (const auto& pattern : deadCodePatterns) {
        optimized = std::regex_replace(optimized, pattern, "");
    }
    
    return optimized;
}

void JavaScriptCompiler::SetCompilationOptions(bool minify, bool removeComments, 
                                               bool optimizeVariables, bool removeDeadCode) {
    m_MinifyJS = minify;
    m_RemoveComments = removeComments;
    m_OptimizeVariables = optimizeVariables;
    m_RemoveDeadCode = removeDeadCode;
}

void JavaScriptCompiler::Reset() {
    m_Errors.clear();
    m_Warnings.clear();
}

void JavaScriptCompiler::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

void JavaScriptCompiler::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

// 暂时的简化实现（等待ANTLR集成）
void JavaScriptCompiler::InitializeANTLR() {
    // TODO: 初始化ANTLR4组件
    // 需要JavaScript.g4语法文件和生成的C++代码
    AddWarning("ANTLR4集成待实现，当前使用简化的JavaScript处理");
}

bool JavaScriptCompiler::ParseJavaScriptTree(const std::string& jsCode) {
    // TODO: 使用ANTLR解析JavaScript语法树
    // 暂时使用简化的验证
    
    return ValidateJavaScriptStructure(jsCode);
}

} // namespace CHTL