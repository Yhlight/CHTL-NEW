#include "CSS/CSSCompiler.h"
#include "CSS/CSSErrorListener.h"
#include "CSS/CSSVisitor.h"
#include "Common/Logger.h"

// ANTLR头文件
#include "antlr4-runtime.h"
#include "css3Lexer.h"
#include "css3Parser.h"

#include <memory>

namespace CHTL {

CSSCompiler::CSSCompiler() : debugMode(false) {
    LOG_DEBUG("初始化CSS编译器");
}

CSSCompiler::~CSSCompiler() = default;

std::string CSSCompiler::Compile(const CodeFragmentPtr& fragment) {
    if (!fragment || fragment->IsEmpty()) {
        return "";
    }
    
    LOG_DEBUG("CSS编译器处理片段，行: " + std::to_string(fragment->startLine) + 
              "-" + std::to_string(fragment->endLine));
    
    try {
        // 创建ANTLR输入流
        antlr4::ANTLRInputStream input(fragment->content);
        
        // 创建词法分析器
        css3Lexer lexer(&input);
        
        // 创建错误监听器
        CSSErrorListener errorListener;
        lexer.removeErrorListeners();
        lexer.addErrorListener(&errorListener);
        
        // 创建Token流
        antlr4::CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        css3Parser parser(&tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(&errorListener);
        
        // 解析CSS
        css3Parser::StylesheetContext* tree = nullptr;
        
        try {
            tree = parser.stylesheet();
        } catch (const std::exception& e) {
            HandleCSSError("解析CSS时发生异常: " + std::string(e.what()));
            return fragment->content; // 返回原始内容
        }
        
        // 检查是否有错误
        if (errorListener.HasErrors()) {
            for (const auto& error : errorListener.GetErrors()) {
                HandleCSSError(error);
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
        LOG_ERROR("CSS编译失败: " + std::string(e.what()));
        return fragment->content;
    }
}

std::string CSSCompiler::ProcessParseTree(antlr4::tree::ParseTree* tree) {
    if (!tree) {
        return "";
    }
    
    try {
        // 创建CSS访问器
        CSSVisitor visitor(true); // 启用美化打印
        
        // 遍历解析树
        antlr4::tree::ParseTreeWalker walker;
        walker.walk(&visitor, tree);
        
        // 获取处理后的CSS
        std::string result = visitor.GetOutput();
        
        if (debugMode) {
            LOG_DEBUG("处理后的CSS:\n" + result);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        LOG_ERROR("处理CSS解析树失败: " + std::string(e.what()));
        return "";
    }
}

void CSSCompiler::HandleCSSError(const std::string& error) {
    LOG_ERROR(error);
    
    // 可以在这里添加更多的错误处理逻辑
    // 例如：收集错误信息、生成错误报告等
}

void CSSCompiler::Reset() {
    // 重置编译器状态
    LOG_DEBUG("重置CSS编译器");
}

} // namespace CHTL