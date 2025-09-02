#include "CHTLJS/CHTLJSCompiler.h"
#include "Common/Logger.h"

namespace CHTL {

CHTLJSCompiler::CHTLJSCompiler() 
    : debugMode(false) {
    InitializeComponents();
    boundaryChecker = std::make_shared<SyntaxBoundaryChecker>();
    // 设置为CHTL JS上下文
    boundaryChecker->GetConstraint()->EnterContext(SyntaxConstraint::Context::CHTLJS);
}

CHTLJSCompiler::~CHTLJSCompiler() = default;

void CHTLJSCompiler::InitializeComponents() {
    context = std::make_shared<JS::CompileContext>();
    lexer = std::make_unique<JS::Lexer>();
    parser = std::make_unique<JS::Parser>();
    generator = std::make_unique<JS::Generator>();
}

std::string CHTLJSCompiler::Compile(const CodeFragmentPtr& fragment) {
    if (!fragment || fragment->IsEmpty()) {
        return "";
    }
    
    LOG_INFO("CHTL JS编译器处理片段，类型: " + fragment->GetTypeString());
    
    try {
        // 设置源文件信息
        context->SetCurrentFile(fragment->sourcePath);
        
        // 预处理
        std::string processedCode = PreprocessFragment(fragment->content);
        
        // TODO: 实现完整的编译流程
        
        // 后处理
        std::string result = PostprocessResult(processedCode);
        
        return result;
        
    } catch (const std::exception& e) {
        LOG_ERROR("CHTL JS编译错误: " + std::string(e.what()));
        throw;
    }
}

void CHTLJSCompiler::SetModuleDirectory(const std::string& dir) {
    moduleDirectory = dir;
    context->SetModuleDirectory(dir);
}

void CHTLJSCompiler::SetDebugMode(bool enable) {
    debugMode = enable;
    context->SetDebugMode(enable);
}

void CHTLJSCompiler::Reset() {
    context->Reset();
}

void CHTLJSCompiler::SetSourceFile(const std::string& file) {
    sourceFile = file;
    context->SetCurrentFile(file);
}

std::string CHTLJSCompiler::PreprocessFragment(const std::string& code) {
    // TODO: 实现CHTL JS语法预处理
    return code;
}

std::string CHTLJSCompiler::PostprocessResult(const std::string& jsCode) {
    // TODO: 实现后处理
    return jsCode;
}

std::string CHTLJSCompiler::GenerateModuleLoader() {
    // TODO: 生成AMD模块加载器
    return "";
}

std::string CHTLJSCompiler::GenerateEventDelegationManager() {
    // TODO: 生成事件委托管理器
    return "";
}

std::string CHTLJSCompiler::GenerateSelectorCache() {
    // TODO: 生成选择器缓存
    return "";
}

void CHTLJSCompiler::EnableSelectorCache(bool enable) {
    context->SetEnableSelectorCache(enable);
}

void CHTLJSCompiler::EnableEventDelegation(bool enable) {
    context->SetEnableEventDelegation(enable);
}

void CHTLJSCompiler::EnableAnimationOptimization(bool enable) {
    context->SetEnableAnimationOptimization(enable);
}

JS::CompileContext::CodeStats CHTLJSCompiler::GetCompileStats() const {
    return context->GetStats();
}

} // namespace CHTL