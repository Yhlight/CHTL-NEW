#include "Dispatcher/CompilerDispatcher.h"
#include "CHTL/CHTLCompiler.h"
#include "CHTLJS/CHTLJSCompiler.h"
#include "CSS/CSSCompiler.h"
#include "JavaScript/JavaScriptCompiler.h"
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(const std::string& modulePath, bool debug)
    : moduleDir(modulePath), debugMode(debug) {
    Logger::SetDebugMode(debug);
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::InitializeCompilers() {
    LOG_INFO("初始化编译器调度器");
    
    // 创建各个编译器实例
    chtlCompiler = std::make_unique<CHTLCompiler>();
    chtljsCompiler = std::make_unique<CHTLJSCompiler>();
    cssCompiler = std::make_unique<CSSCompiler>();
    jsCompiler = std::make_unique<JavaScriptCompiler>();
    
    // 设置模块目录和调试模式
    chtlCompiler->SetModuleDirectory(moduleDir);
    chtlCompiler->SetDebugMode(debugMode);
    
    chtljsCompiler->SetModuleDirectory(moduleDir);
    chtljsCompiler->SetDebugMode(debugMode);
    
    cssCompiler->SetModuleDirectory(moduleDir);
    cssCompiler->SetDebugMode(debugMode);
    
    jsCompiler->SetModuleDirectory(moduleDir);
    jsCompiler->SetDebugMode(debugMode);
}

std::string CompilerDispatcher::Dispatch(const CodeFragmentList& fragments, 
                                        const std::string& sourceFile) {
    LOG_INFO("开始分发代码片段，源文件: " + sourceFile);
    
    CompileResult result;
    
    // 重置所有编译器状态
    chtlCompiler->Reset();
    chtljsCompiler->Reset();
    cssCompiler->Reset();
    jsCompiler->Reset();
    
    // 设置源文件路径
    chtlCompiler->SetSourceFile(sourceFile);
    chtljsCompiler->SetSourceFile(sourceFile);
    
    // 处理每个代码片段
    for (const auto& fragment : fragments) {
        if (fragment->IsEmpty()) {
            continue;
        }
        
        LOG_DEBUG("处理片段: 类型=" + fragment->GetTypeString() + 
                 ", 行=" + std::to_string(fragment->startLine) + "-" + 
                 std::to_string(fragment->endLine));
        
        switch (fragment->type) {
            case FragmentType::CHTL:
                ProcessCHTLFragment(fragment, result);
                break;
                
            case FragmentType::CHTLJS:
                ProcessCHTLJSFragment(fragment, result);
                break;
                
            case FragmentType::CSS:
                ProcessCSSFragment(fragment, result);
                break;
                
            case FragmentType::JavaScript:
                ProcessJavaScriptFragment(fragment, result);
                break;
                
            default:
                LOG_WARNING("未知的片段类型: " + fragment->GetTypeString());
                break;
        }
    }
    
    // 合并所有结果
    return MergeResults(result);
}

void CompilerDispatcher::ProcessCHTLFragment(const CodeFragmentPtr& fragment, 
                                            CompileResult& result) {
    try {
        auto compiledResult = chtlCompiler->Compile(fragment);
        
        // CHTL编译器返回的是一个结构化结果
        // 需要解析并分配到不同的部分
        auto chtlResult = chtlCompiler->GetLastCompileResult();
        
        result.html += chtlResult.htmlContent;
        result.globalStyles += chtlResult.globalStyles;
        result.globalScripts += chtlResult.globalScripts;
        result.headContent += chtlResult.headContent;
        
    } catch (const std::exception& e) {
        LOG_ERROR("CHTL编译错误: " + std::string(e.what()));
        throw;
    }
}

void CompilerDispatcher::ProcessCHTLJSFragment(const CodeFragmentPtr& fragment, 
                                               CompileResult& result) {
    try {
        auto compiledResult = chtljsCompiler->Compile(fragment);
        
        // CHTL JS编译结果添加到全局脚本
        result.globalScripts += compiledResult;
        
    } catch (const std::exception& e) {
        LOG_ERROR("CHTL JS编译错误: " + std::string(e.what()));
        throw;
    }
}

void CompilerDispatcher::ProcessCSSFragment(const CodeFragmentPtr& fragment, 
                                           CompileResult& result) {
    try {
        auto compiledResult = cssCompiler->Compile(fragment);
        
        // CSS编译结果添加到全局样式
        result.globalStyles += compiledResult;
        
    } catch (const std::exception& e) {
        LOG_ERROR("CSS编译错误: " + std::string(e.what()));
        throw;
    }
}

void CompilerDispatcher::ProcessJavaScriptFragment(const CodeFragmentPtr& fragment, 
                                                  CompileResult& result) {
    try {
        auto compiledResult = jsCompiler->Compile(fragment);
        
        // JavaScript编译结果添加到全局脚本
        result.globalScripts += compiledResult;
        
    } catch (const std::exception& e) {
        LOG_ERROR("JavaScript编译错误: " + std::string(e.what()));
        throw;
    }
}

std::string CompilerDispatcher::MergeResults(const CompileResult& result) {
    LOG_INFO("合并编译结果");
    
    std::ostringstream output;
    
    // 检查是否需要添加HTML5声明
    bool hasHTML5Declaration = chtlCompiler->HasHTML5Declaration();
    
    if (hasHTML5Declaration) {
        output << "<!DOCTYPE html>\n";
    }
    
    // 检查是否有html标签
    bool hasHtmlTag = result.html.find("<html") != std::string::npos;
    
    if (!hasHtmlTag) {
        // 如果没有html标签，创建一个完整的HTML结构
        output << "<html>\n";
        output << "<head>\n";
        
        // 添加字符集
        output << "    <meta charset=\"UTF-8\">\n";
        
        // 添加head内容
        if (!result.headContent.empty()) {
            output << result.headContent;
        }
        
        // 添加全局样式
        if (!result.globalStyles.empty()) {
            output << "    <style>\n";
            output << result.globalStyles;
            output << "    </style>\n";
        }
        
        output << "</head>\n";
        output << "<body>\n";
        
        // 添加HTML内容
        output << result.html;
        
        // 添加全局脚本
        if (!result.globalScripts.empty()) {
            output << "    <script>\n";
            output << result.globalScripts;
            output << "    </script>\n";
        }
        
        output << "</body>\n";
        output << "</html>";
    } else {
        // 如果已经有html标签，直接输出并在适当位置插入样式和脚本
        // 这需要更复杂的HTML解析和插入逻辑
        // 暂时简单处理
        output << result.html;
        
        // TODO: 实现智能插入样式和脚本的逻辑
    }
    
    return output.str();
}

} // namespace CHTL