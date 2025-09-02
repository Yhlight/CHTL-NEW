#include "JavaScriptCompiler.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// JavaScriptCompilerListener实现
JavaScriptCompilerListener::JavaScriptCompilerListener() {
    // 初始化监听器
}

void JavaScriptCompilerListener::enterProgram(JavaScriptParser::ProgramContext *ctx) {
    m_CompiledJS += "// ANTLR Compiled JavaScript\n";
}

void JavaScriptCompilerListener::exitProgram(JavaScriptParser::ProgramContext *ctx) {
    m_CompiledJS += "\n// End ANTLR Compiled JavaScript";
}

void JavaScriptCompilerListener::enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) {
    // 处理函数声明
    if (ctx->getText().length() > 0) {
        m_CompiledJS += ctx->getText() + "\n";
    }
}

void JavaScriptCompilerListener::enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) {
    // 处理变量声明
    std::string declarationText = ctx->getText();
    
    // 简单的变量声明验证
    if (declarationText.find("var") == std::string::npos && 
        declarationText.find("let") == std::string::npos && 
        declarationText.find("const") == std::string::npos) {
        addWarning("可能的无效变量声明: " + declarationText);
    }
}

void JavaScriptCompilerListener::enterExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) {
    // 处理表达式语句
    if (ctx->getText().length() > 0) {
        m_CompiledJS += ctx->getText() + "\n";
    }
}

void JavaScriptCompilerListener::addError(const std::string& error) {
    m_Errors.push_back(error);
}

void JavaScriptCompilerListener::addWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

// JavaScriptCompiler实现
JavaScriptCompiler::JavaScriptCompiler() 
    : m_MinifyJS(false), m_RemoveComments(false), m_OptimizeVariables(true), m_DeadCodeElimination(true) {
}

JavaScriptCompilationResult JavaScriptCompiler::Compile(const std::string& jsCode) {
    return CompileSingleFragment(jsCode);
}

JavaScriptCompilationResult JavaScriptCompiler::CompileFragments(const std::vector<std::string>& jsFragments) {
    JavaScriptCompilationResult result;
    
    try {
        std::ostringstream combinedJS;
        
        for (const auto& fragment : jsFragments) {
            auto fragmentResult = CompileSingleFragment(fragment);
            
            if (fragmentResult.IsSuccess) {
                combinedJS << fragmentResult.OptimizedJS << "\n";
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
        
        result.OptimizedJS = combinedJS.str();
        result.IsSuccess = result.Errors.empty();
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.Errors.push_back("JavaScript编译异常: " + std::string(e.what()));
    }
    
    return result;
}

JavaScriptCompilationResult JavaScriptCompiler::CompileSingleFragment(const std::string& jsCode) {
    JavaScriptCompilationResult result;
    
    try {
        // 初始化ANTLR组件
        initializeANTLR(jsCode);
        
        // 执行解析
        bool parseSuccess = parseJavaScript();
        
        if (parseSuccess) {
            // 获取监听器结果
            result.OptimizedJS = m_Listener->getCompiledJS();
            result.Errors = m_Listener->getErrors();
            result.Warnings = m_Listener->getWarnings();
            
            // 如果没有解析错误，应用优化
            if (result.Errors.empty()) {
                result.OptimizedJS = ApplyJavaScriptOptimizations(result.OptimizedJS);
                result.IsSuccess = true;
            }
        }
        else {
            result.IsSuccess = false;
            result.Errors.push_back("JavaScript语法解析失败");
        }
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.Errors.push_back("JavaScript编译异常: " + std::string(e.what()));
    }
    
    return result;
}

bool JavaScriptCompiler::ValidateJavaScript(const std::string& jsCode) {
    try {
        initializeANTLR(jsCode);
        return parseJavaScript() && m_Listener->getErrors().empty();
    }
    catch (const std::exception&) {
        return false;
    }
}

std::string JavaScriptCompiler::OptimizeJavaScript(const std::string& jsCode) {
    return ApplyJavaScriptOptimizations(jsCode);
}

void JavaScriptCompiler::SetCompilationOptions(bool minify, bool removeComments, 
                                               bool optimizeVariables, bool deadCodeElimination) {
    m_MinifyJS = minify;
    m_RemoveComments = removeComments;
    m_OptimizeVariables = optimizeVariables;
    m_DeadCodeElimination = deadCodeElimination;
}

void JavaScriptCompiler::Reset() {
    m_Errors.clear();
    m_Warnings.clear();
    
    // 重置ANTLR组件
    m_InputStream.reset();
    m_Lexer.reset();
    m_TokenStream.reset();
    m_Parser.reset();
    m_Listener.reset();
}

void JavaScriptCompiler::initializeANTLR(const std::string& jsCode) {
    // 创建输入流
    m_InputStream = std::make_unique<antlr4::ANTLRInputStream>(jsCode);
    
    // 创建词法分析器
    m_Lexer = std::make_unique<JavaScriptLexer>(m_InputStream.get());
    
    // 创建令牌流
    m_TokenStream = std::make_unique<antlr4::CommonTokenStream>(m_Lexer.get());
    
    // 创建解析器
    m_Parser = std::make_unique<JavaScriptParser>(m_TokenStream.get());
    
    // 创建监听器
    m_Listener = std::make_unique<JavaScriptCompilerListener>();
}

bool JavaScriptCompiler::parseJavaScript() {
    try {
        // 解析JavaScript程序
        auto tree = m_Parser->program();
        
        // 遍历语法树
        antlr4::tree::ParseTreeWalker walker;
        walker.walk(m_Listener.get(), tree);
        
        return true;
    }
    catch (const std::exception& e) {
        AddError("JavaScript解析异常: " + std::string(e.what()));
        return false;
    }
}

std::string JavaScriptCompiler::ApplyJavaScriptOptimizations(const std::string& jsCode) {
    std::string optimized = jsCode;
    
    // 应用各种优化
    if (m_RemoveComments) {
        // 移除单行注释
        optimized = std::regex_replace(optimized, std::regex(R"(//.*$)", std::regex_constants::multiline), "");
        // 移除多行注释
        optimized = std::regex_replace(optimized, std::regex(R"(/\*.*?\*/)"), "");
    }
    
    if (m_OptimizeVariables) {
        optimized = OptimizeVariableNames(optimized);
    }
    
    if (m_DeadCodeElimination) {
        optimized = RemoveDeadCode(optimized);
    }
    
    if (m_MinifyJS) {
        optimized = MinifyJavaScript(optimized);
    }
    
    return optimized;
}

std::string JavaScriptCompiler::MinifyJavaScript(const std::string& jsCode) {
    std::string minified = jsCode;
    
    // 移除多余空白（保留必要的空格）
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除大括号周围的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*\{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除操作符周围的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*=\s*)"), "=");
    minified = std::regex_replace(minified, std::regex(R"(\s*\+\s*)"), "+");
    minified = std::regex_replace(minified, std::regex(R"(\s*-\s*)"), "-");
    
    return minified;
}

std::string JavaScriptCompiler::OptimizeVariableNames(const std::string& jsCode) {
    // 简化的变量名优化
    std::string optimized = jsCode;
    
    // 将长变量名替换为短变量名（简化实现）
    optimized = std::regex_replace(optimized, std::regex(R"(\bvariableName\b)"), "v");
    optimized = std::regex_replace(optimized, std::regex(R"(\bfunctionName\b)"), "f");
    optimized = std::regex_replace(optimized, std::regex(R"(\belementName\b)"), "e");
    
    return optimized;
}

std::string JavaScriptCompiler::RemoveDeadCode(const std::string& jsCode) {
    std::string optimized = jsCode;
    
    // 移除永远不会执行的代码
    optimized = std::regex_replace(optimized, std::regex(R"(if\s*\(\s*false\s*\)\s*\{[^}]*\})"), "");
    
    // 移除空的if语句
    optimized = std::regex_replace(optimized, std::regex(R"(if\s*\([^)]+\)\s*\{\s*\})"), "");
    
    return optimized;
}

void JavaScriptCompiler::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

void JavaScriptCompiler::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

} // namespace CHTL