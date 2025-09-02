#include "JavaScript/JavaScriptCompiler.h"
#include "JavaScript/JavaScriptErrorListener.h"
#include "JavaScript/JavaScriptVisitor.h"
#include "Common/Logger.h"

// ANTLR头文件
#include "antlr4-runtime.h"
#include "JavaScriptLexer.h"
#include "JavaScriptParser.h"

#include <memory>
#include <regex>

namespace CHTL {

JavaScriptCompiler::JavaScriptCompiler() 
    : debugMode(false), enableMinification(false) {
    LOG_DEBUG("初始化JavaScript编译器");
}

JavaScriptCompiler::~JavaScriptCompiler() = default;

std::string JavaScriptCompiler::Compile(const CodeFragmentPtr& fragment) {
    if (!fragment || fragment->IsEmpty()) {
        return "";
    }
    
    LOG_DEBUG("JavaScript编译器处理片段，行: " + std::to_string(fragment->startLine) + 
              "-" + std::to_string(fragment->endLine));
    
    try {
        // 预处理JavaScript代码
        std::string processedCode = PreprocessJavaScript(fragment->content);
        
        // 创建ANTLR输入流
        antlr4::ANTLRInputStream input(processedCode);
        
        // 创建词法分析器
        JavaScriptLexer lexer(&input);
        
        // 创建错误监听器
        JavaScriptErrorListener errorListener;
        lexer.removeErrorListeners();
        lexer.addErrorListener(&errorListener);
        
        // 创建Token流
        antlr4::CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        JavaScriptParser parser(&tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(&errorListener);
        
        // 解析JavaScript
        JavaScriptParser::ProgramContext* tree = nullptr;
        
        try {
            tree = parser.program();
        } catch (const std::exception& e) {
            HandleJavaScriptError("解析JavaScript时发生异常: " + std::string(e.what()));
            return fragment->content; // 返回原始内容
        }
        
        // 检查是否有错误
        if (errorListener.HasErrors()) {
            for (const auto& error : errorListener.GetErrors()) {
                HandleJavaScriptError(error);
            }
            // 有错误但仍尝试处理
        }
        
        // 处理解析树
        if (tree) {
            return ProcessParseTree(tree);
        }
        
        // 如果解析失败，返回原始内容
        return fragment->content;
        
    } catch (const std::exception& e) {
        LOG_ERROR("JavaScript编译失败: " + std::string(e.what()));
        return fragment->content;
    }
}

std::string JavaScriptCompiler::PreprocessJavaScript(const std::string& code) {
    // 这里可以添加对特殊JavaScript语法的预处理
    // 例如：处理ES6+语法、TypeScript类型注解等
    
    std::string processed = code;
    
    // 示例：移除TypeScript类型注解（简化处理）
    // 实际实现需要更复杂的处理
    std::regex typeAnnotation(R"(:\s*\w+(?:<[^>]+>)?(?:\[\])?(?:\s*[|&]\s*\w+)*)");
    processed = std::regex_replace(processed, typeAnnotation, "");
    
    return processed;
}

std::string JavaScriptCompiler::ProcessParseTree(antlr4::tree::ParseTree* tree) {
    if (!tree) {
        return "";
    }
    
    try {
        // 创建JavaScript访问器
        JavaScriptVisitor visitor(!enableMinification, enableMinification);
        
        // 遍历解析树
        antlr4::tree::ParseTreeWalker walker;
        walker.walk(&visitor, tree);
        
        // 获取处理后的JavaScript
        std::string result = visitor.GetOutput();
        
        if (debugMode) {
            LOG_DEBUG("处理后的JavaScript:\n" + result);
        }
        
        // 如果启用了压缩，可以进行额外的优化
        if (enableMinification) {
            // 移除多余的空白和注释
            result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
            result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        LOG_ERROR("处理JavaScript解析树失败: " + std::string(e.what()));
        return "";
    }
}

void JavaScriptCompiler::HandleJavaScriptError(const std::string& error) {
    LOG_ERROR(error);
    
    // 可以在这里添加更多的错误处理逻辑
    // 例如：收集错误信息、生成错误报告等
}

void JavaScriptCompiler::Reset() {
    // 重置编译器状态
    LOG_DEBUG("重置JavaScript编译器");
}

} // namespace CHTL