#include "CHTL/CHTLCompiler.h"
#include "CHTL/Lexer/Lexer.h"
#include "CHTL/Parser/Parser.h"
#include "CHTL/Generator/Generator.h"
#include "Common/Logger.h"

namespace CHTL {

CHTLCompiler::CHTLCompiler() 
    : debugMode(false), hasHTML5Declaration(false) {
    InitializeComponents();
}

CHTLCompiler::~CHTLCompiler() = default;

void CHTLCompiler::InitializeComponents() {
    context = std::make_shared<CompileContext>();
    lexer = std::make_unique<Lexer>("", "");
    parser = std::make_unique<Parser>(context);
    generator = std::make_unique<Generator>(context);
}

std::string CHTLCompiler::Compile(const CodeFragmentPtr& fragment) {
    if (!fragment || fragment->IsEmpty()) {
        return "";
    }
    
    LOG_INFO("CHTL编译器处理片段，类型: " + fragment->GetTypeString());
    
    try {
        // 设置源文件信息
        context->SetCurrentFile(fragment->sourcePath);
        
        // 预处理
        std::string processedCode = PreprocessFragment(fragment->content);
        
        // 词法分析
        lexer->SetSource(processedCode, fragment->sourcePath);
        lexer->Tokenize();
        
        // 语法分析
        auto ast = parser->Parse(lexer->GetTokens());
        
        // 检查HTML5声明
        if (context->UseHTML5()) {
            hasHTML5Declaration = true;
        }
        
        // 代码生成
        generator->Generate(ast, lastResult);
        
        // 后处理
        PostprocessResult(lastResult);
        
        return lastResult.htmlContent;
        
    } catch (const std::exception& e) {
        LOG_ERROR("CHTL编译错误: " + std::string(e.what()));
        throw;
    }
}

void CHTLCompiler::SetModuleDirectory(const std::string& dir) {
    moduleDirectory = dir;
    context->SetModuleDirectory(dir);
}

void CHTLCompiler::SetDebugMode(bool enable) {
    debugMode = enable;
    context->SetDebugMode(enable);
    Logger::SetDebugMode(enable);
}

void CHTLCompiler::Reset() {
    context->Reset();
    lexer->Reset();
    parser->Reset();
    hasHTML5Declaration = false;
    lastResult.Clear();
}

void CHTLCompiler::SetSourceFile(const std::string& file) {
    sourceFile = file;
    context->SetCurrentFile(file);
}

std::string CHTLCompiler::PreprocessFragment(const std::string& code) {
    // TODO: 实现预处理逻辑
    return code;
}

void CHTLCompiler::PostprocessResult(CompileResult& result) {
    // TODO: 实现后处理逻辑
}

} // namespace CHTL