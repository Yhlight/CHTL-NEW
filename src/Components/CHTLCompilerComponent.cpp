#include "CHTLCompilerComponent.h"
#include "../Core/ServiceLocator.h"
#include <iostream>
#include <regex>

namespace CHTL {
namespace Components {

// CHTLCompilerComponent实现
CHTLCompilerComponent::CHTLCompilerComponent() 
    : Core::CompilerComponentBase("CHTLCompiler", "1.0.0") {
    
    AddSupportedExtension("chtl");
    AddDependency("UnifiedScanner");
    AddDependency("ErrorHandler");
}

bool CHTLCompilerComponent::Initialize() {
    if (!Core::CompilerComponentBase::Initialize()) {
        return false;
    }
    
    return InitializeSubComponents();
}

bool CHTLCompilerComponent::InitializeSubComponents() {
    try {
        // 初始化CHTL编译器子组件
        m_Lexer = std::make_unique<CHTLLexer>("");
        m_Parser = std::make_unique<CHTLParser>("");
        m_Generator = std::make_unique<CHTLGenerator>();
        
        std::cout << "✅ CHTL编译器子组件已初始化" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ CHTL编译器子组件初始化失败: " << e.what() << std::endl;
        return false;
    }
}

void CHTLCompilerComponent::Reset() {
    Core::CompilerComponentBase::Reset();
    
    if (m_Lexer) {
        m_Lexer->Reset();
    }
    if (m_Parser) {
        m_Parser->Reset();
    }
    if (m_Generator) {
        m_Generator->Reset();
    }
}

bool CHTLCompilerComponent::ValidateInput(const std::string& input) {
    if (input.empty()) {
        return false;
    }
    
    return ValidateCHTLSyntax(input);
}

std::string CHTLCompilerComponent::DoCompile(const std::string& input) {
    if (!m_Lexer || !m_Parser || !m_Generator) {
        return "";
    }
    
    try {
        // 词法分析
        m_Lexer->SetSourceCode(input);
        auto lexResult = m_Lexer->Tokenize();
        
        if (!lexResult.IsSuccess) {
            GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::LEXICAL_ERROR, 
                                      "CHTL词法分析失败: " + lexResult.ErrorMessage);
            return "";
        }
        
        // 语法解析
        m_Parser->SetSourceCode(input);
        auto parseResult = m_Parser->Parse();
        
        if (!parseResult.IsSuccess) {
            GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR,
                                      "CHTL语法解析失败: " + parseResult.ErrorMessage);
            return "";
        }
        
        // 代码生成
        auto generateResult = m_Generator->Generate(std::move(parseResult.RootNode));
        
        if (!generateResult.IsSuccess) {
            GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::GENERATION_ERROR,
                                      "CHTL代码生成失败: " + generateResult.ErrorMessage);
            return "";
        }
        
        return generateResult.HTMLContent;
        
    } catch (const std::exception& e) {
        GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::SYSTEM_ERROR,
                                  "CHTL编译异常: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLCompilerComponent::DoOptimize(const std::string& code) {
    // CHTL代码优化
    std::string optimized = code;
    
    // 移除多余的空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+)"), " ");
    
    // 移除空的HTML标签
    optimized = std::regex_replace(optimized, std::regex(R"(<(\w+)[^>]*>\s*</\1>)"), "");
    
    return optimized;
}

std::string CHTLCompilerComponent::DoMinify(const std::string& code) {
    // CHTL代码压缩
    std::string minified = code;
    
    // 移除HTML注释
    minified = std::regex_replace(minified, std::regex(R"(<!--.*?-->)"), "");
    
    // 压缩空白
    minified = std::regex_replace(minified, std::regex(R"(\s*>\s*)"), ">");
    minified = std::regex_replace(minified, std::regex(R"(\s*<\s*)"), "<");
    
    return minified;
}

bool CHTLCompilerComponent::ValidateCHTLSyntax(const std::string& input) {
    // 基本CHTL语法验证
    
    // 检查括号匹配
    int braceCount = 0;
    int bracketCount = 0;
    
    for (char c : input) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
        
        if (braceCount < 0 || bracketCount < 0) {
            return false;
        }
    }
    
    return braceCount == 0 && bracketCount == 0;
}

// CHTLJSCompilerComponent实现
CHTLJSCompilerComponent::CHTLJSCompilerComponent() 
    : Core::CompilerComponentBase("CHTLJSCompiler", "1.0.0") {
    
    AddSupportedExtension("cjjs");
    AddDependency("UnifiedScanner");
    AddDependency("ErrorHandler");
    AddDependency("CJMODManager");
}

bool CHTLJSCompilerComponent::Initialize() {
    if (!Core::CompilerComponentBase::Initialize()) {
        return false;
    }
    
    return InitializeSubComponents();
}

bool CHTLJSCompilerComponent::InitializeSubComponents() {
    try {
        // 初始化CHTL JS编译器子组件
        m_Lexer = std::make_unique<CHTLJS::CHTLJSLexer>("");
        m_Parser = std::make_unique<CHTLJS::CHTLJSParser>("");
        m_Generator = std::make_unique<CHTLJS::CHTLJSGenerator>();
        
        std::cout << "✅ CHTL JS编译器子组件已初始化" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ CHTL JS编译器子组件初始化失败: " << e.what() << std::endl;
        return false;
    }
}

void CHTLJSCompilerComponent::Reset() {
    Core::CompilerComponentBase::Reset();
    
    if (m_Lexer) {
        m_Lexer->Reset();
    }
    if (m_Parser) {
        m_Parser->Reset();
    }
    if (m_Generator) {
        m_Generator->Reset();
    }
}

bool CHTLJSCompilerComponent::ValidateInput(const std::string& input) {
    if (input.empty()) {
        return false;
    }
    
    return ValidateCHTLJSSyntax(input);
}

std::string CHTLJSCompilerComponent::DoCompile(const std::string& input) {
    if (!m_Lexer || !m_Parser || !m_Generator) {
        return "";
    }
    
    try {
        // 词法分析
        m_Lexer->SetSourceCode(input);
        auto lexResult = m_Lexer->Tokenize();
        
        if (!lexResult.IsSuccess) {
            GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::LEXICAL_ERROR,
                                      "CHTL JS词法分析失败: " + lexResult.ErrorMessage);
            return "";
        }
        
        // 语法解析
        m_Parser->SetSourceCode(input);
        auto parseResult = m_Parser->Parse();
        
        if (!parseResult.IsSuccess) {
            GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR,
                                      "CHTL JS语法解析失败: " + parseResult.ErrorMessage);
            return "";
        }
        
        // 代码生成
        auto generateResult = m_Generator->Generate(std::move(parseResult.RootNode));
        
        if (!generateResult.IsSuccess) {
            GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::GENERATION_ERROR,
                                      "CHTL JS代码生成失败: " + generateResult.ErrorMessage);
            return "";
        }
        
        return generateResult.JavaScriptContent;
        
    } catch (const std::exception& e) {
        GetErrorHandler()->AddError(ErrorLevel::ERROR, ErrorType::SYSTEM_ERROR,
                                  "CHTL JS编译异常: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLJSCompilerComponent::DoOptimize(const std::string& code) {
    // CHTL JS代码优化
    std::string optimized = code;
    
    // 移除多余的空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+)"), " ");
    
    // 优化增强选择器
    optimized = std::regex_replace(optimized, std::regex(R"(\{\{([^}]+)\}\})"), "document.querySelector('$1')");
    
    return optimized;
}

std::string CHTLJSCompilerComponent::DoMinify(const std::string& code) {
    // CHTL JS代码压缩
    std::string minified = code;
    
    // 移除JavaScript注释
    minified = std::regex_replace(minified, std::regex(R"(//.*$)"), "", std::regex_constants::format_default);
    minified = std::regex_replace(minified, std::regex(R"(/\*.*?\*/)"), "");
    
    // 压缩空白
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    minified = std::regex_replace(minified, std::regex(R"(\s*\{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*\}\s*)"), "}");
    
    return minified;
}

bool CHTLJSCompilerComponent::ValidateCHTLJSSyntax(const std::string& input) {
    // 基本CHTL JS语法验证
    
    // 检查是否包含CHTL JS特有语法
    std::vector<std::regex> chtljsPatterns = {
        std::regex(R"(module\s*\{)"),           // module块
        std::regex(R"(script\s*\{)"),           // script块
        std::regex(R"(\{\{[^}]+\}\})"),         // 增强选择器
        std::regex(R"(vir\s+\w+)"),             // 虚对象
        std::regex(R"(\w+\s*->\s*\w+)"),        // 箭头操作符
        std::regex(R"(&->\s*\w+)"),             // 事件绑定操作符
    };
    
    for (const auto& pattern : chtljsPatterns) {
        if (std::regex_search(input, pattern)) {
            return true; // 包含CHTL JS语法
        }
    }
    
    // 如果不包含CHTL JS特有语法，检查是否为有效的JavaScript
    return input.find_first_not_of(" \t\n\r") != std::string::npos;
}

// UnifiedScannerComponent实现
UnifiedScannerComponent::UnifiedScannerComponent() 
    : Core::ScannerComponentBase("UnifiedScanner", "1.0.0") {
}

bool UnifiedScannerComponent::Initialize() {
    if (!Core::ScannerComponentBase::Initialize()) {
        return false;
    }
    
    try {
        m_Scanner = std::make_unique<CHTLUnifiedScanner>("");
        std::cout << "✅ 统一扫描器组件已初始化" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 统一扫描器组件初始化失败: " << e.what() << std::endl;
        return false;
    }
}

void UnifiedScannerComponent::Reset() {
    Core::ScannerComponentBase::Reset();
    
    if (m_Scanner) {
        m_Scanner->Reset();
    }
}

bool UnifiedScannerComponent::DoScan(const std::string& source) {
    if (!m_Scanner) {
        return false;
    }
    
    try {
        m_Scanner->SetSourceCode(source);
        bool result = m_Scanner->Scan();
        
        if (result) {
            // 更新扫描结果
            auto fragments = m_Scanner->GetFragments();
            m_ScanResults.clear();
            
            for (const auto& fragment : fragments) {
                AddScanResult(fragment.Content);
            }
            
            UpdateScanStats("fragment_count", fragments.size());
            UpdateScanStats("source_size", source.size());
        }
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "扫描异常: " << e.what() << std::endl;
        return false;
    }
}

std::vector<CodeFragment> UnifiedScannerComponent::GetFragments() const {
    if (m_Scanner) {
        return m_Scanner->GetFragments();
    }
    return {};
}

} // namespace Components
} // namespace CHTL