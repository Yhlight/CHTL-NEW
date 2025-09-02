#include "CSSCompiler.h"
#include "generated/css3Lexer.h"
#include "generated/css3Parser.h"
#include "generated/css3ParserBaseListener.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CSSCompilerListener实现
CSSCompilerListener::CSSCompilerListener() {
    // 初始化监听器
}

void CSSCompilerListener::enterStylesheet(css3Parser::StylesheetContext *ctx) {
    m_CompiledCSS += "/* 官方ANTLR CSS3编译器 */\n";
}

void CSSCompilerListener::exitStylesheet(css3Parser::StylesheetContext *ctx) {
    m_CompiledCSS += "\n/* 官方ANTLR CSS3编译器结束 */";
}

void CSSCompilerListener::enterRuleset(css3Parser::RulesetContext *ctx) {
    // 处理CSS规则集
    if (ctx->getText().length() > 0) {
        m_CompiledCSS += ctx->getText() + "\n";
    }
}

void CSSCompilerListener::exitRuleset(css3Parser::RulesetContext *ctx) {
    // 规则集处理完成
}

void CSSCompilerListener::enterSelector(css3Parser::SelectorContext *ctx) {
    // 处理选择器
    std::string selectorText = ctx->getText();
    
    // 简单的选择器优化
    if (selectorText.find("  ") != std::string::npos) {
        addWarning("选择器包含多余空格: " + selectorText);
    }
}

void CSSCompilerListener::enterDeclaration(css3Parser::DeclarationContext *ctx) {
    // 处理CSS声明
    std::string declarationText = ctx->getText();
    
    // 简单的声明验证
    if (declarationText.find(":") == std::string::npos) {
        addError("无效的CSS声明: " + declarationText);
    }
}

void CSSCompilerListener::addError(const std::string& error) {
    m_Errors.push_back(error);
}

void CSSCompilerListener::addWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

// CSSCompiler实现
CSSCompiler::CSSCompiler() 
    : m_MinifyCSS(false), m_RemoveComments(false), m_OptimizeSelectors(true), m_MergeRules(true) {
}

CSSCompilationResult CSSCompiler::Compile(const std::string& cssCode) {
    return CompileSingleFragment(cssCode);
}

CSSCompilationResult CSSCompiler::CompileFragments(const std::vector<std::string>& cssFragments) {
    CSSCompilationResult result;
    
    try {
        std::ostringstream combinedCSS;
        
        for (const auto& fragment : cssFragments) {
            auto fragmentResult = CompileSingleFragment(fragment);
            
            if (fragmentResult.IsSuccess) {
                combinedCSS << fragmentResult.OptimizedCSS << "\n";
            }
            else {
                // 收集错误
                result.Errors.insert(result.Errors.end(), 
                                    fragmentResult.Errors.begin(), fragmentResult.Errors.end());
            }
            
            // 收集警告
            result.Warnings.insert(result.Warnings.end(), 
                                 fragmentResult.Warnings.begin(), fragmentResult.Warnings.end());
        }
        
        result.OptimizedCSS = combinedCSS.str();
        result.IsSuccess = result.Errors.empty();
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.Errors.push_back("CSS编译异常: " + std::string(e.what()));
    }
    
    return result;
}

CSSCompilationResult CSSCompiler::CompileSingleFragment(const std::string& cssCode) {
    CSSCompilationResult result;
    
    try {
        // 初始化ANTLR组件
        initializeANTLR(cssCode);
        
        // 执行解析
        bool parseSuccess = parseCSS();
        
        if (parseSuccess) {
            // 获取监听器结果
            result.OptimizedCSS = m_Listener->getCompiledCSS();
            result.Errors = m_Listener->getErrors();
            result.Warnings = m_Listener->getWarnings();
            
            // 如果没有解析错误，应用优化
            if (result.Errors.empty()) {
                result.OptimizedCSS = ApplyCSSOptimizations(result.OptimizedCSS);
                result.IsSuccess = true;
            }
        }
        else {
            result.IsSuccess = false;
            result.Errors.push_back("CSS语法解析失败");
        }
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.Errors.push_back("CSS编译异常: " + std::string(e.what()));
    }
    
    return result;
}

bool CSSCompiler::ValidateCSS(const std::string& cssCode) {
    try {
        initializeANTLR(cssCode);
        return parseCSS() && m_Listener->getErrors().empty();
    }
    catch (const std::exception&) {
        return false;
    }
}

std::string CSSCompiler::OptimizeCSS(const std::string& cssCode) {
    return ApplyCSSOptimizations(cssCode);
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
    
    // 重置ANTLR组件
    m_InputStream.reset();
    m_Lexer.reset();
    m_TokenStream.reset();
    m_Parser.reset();
    m_Listener.reset();
}

void CSSCompiler::initializeANTLR(const std::string& cssCode) {
    // 创建输入流
    m_InputStream = std::make_unique<antlr4::ANTLRInputStream>(cssCode);
    
    // 创建词法分析器
    m_Lexer = std::make_unique<CSSLexer>(m_InputStream.get());
    
    // 创建令牌流
    m_TokenStream = std::make_unique<antlr4::CommonTokenStream>(m_Lexer.get());
    
    // 创建解析器
    m_Parser = std::make_unique<CSSParser>(m_TokenStream.get());
    
    // 创建监听器
    m_Listener = std::make_unique<CSSCompilerListener>();
}

bool CSSCompiler::parseCSS() {
    try {
        // 解析CSS样式表
        auto tree = m_Parser->stylesheet();
        
        // 遍历语法树
        antlr4::tree::ParseTreeWalker walker;
        walker.walk(m_Listener.get(), tree);
        
        return true;
    }
    catch (const std::exception& e) {
        AddError("CSS解析异常: " + std::string(e.what()));
        return false;
    }
}

std::string CSSCompiler::ApplyCSSOptimizations(const std::string& cssCode) {
    std::string optimized = cssCode;
    
    // 应用各种优化
    if (m_RemoveComments) {
        optimized = std::regex_replace(optimized, std::regex(R"(/\*.*?\*/)"), "");
    }
    
    if (m_OptimizeSelectors) {
        optimized = OptimizeSelectors(optimized);
    }
    
    if (m_MergeRules) {
        optimized = MergeDuplicateRules(optimized);
    }
    
    if (m_MinifyCSS) {
        optimized = MinifyCSS(optimized);
    }
    
    return optimized;
}

std::string CSSCompiler::MinifyCSS(const std::string& cssCode) {
    std::string minified = cssCode;
    
    // 移除多余空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除大括号周围的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*\{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除冒号周围的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*:\s*)"), ":");
    
    // 移除逗号后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*,\s*)"), ",");
    
    return minified;
}

std::string CSSCompiler::MergeDuplicateRules(const std::string& cssCode) {
    // 简化的重复规则合并
    return cssCode; // TODO: 实现复杂的规则合并逻辑
}

std::string CSSCompiler::OptimizeSelectors(const std::string& cssCode) {
    std::string optimized = cssCode;
    
    // 移除多余的通用选择器
    optimized = std::regex_replace(optimized, std::regex(R"(\*\s+)"), "");
    
    // 优化子选择器
    optimized = std::regex_replace(optimized, std::regex(R"(\s*>\s*)"), ">");
    
    return optimized;
}

void CSSCompiler::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

void CSSCompiler::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

} // namespace CHTL