#ifndef CHTL_JAVASCRIPT_JAVASCRIPTCOMPILER_H
#define CHTL_JAVASCRIPT_JAVASCRIPTCOMPILER_H

#include <string>
#include <memory>
#include <sstream>
#include "Common/ICompiler.h"

// 前向声明ANTLR类
namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
    namespace tree {
        class ParseTree;
    }
}

// 前向声明JavaScript解析器类
class JavaScriptLexer;
class JavaScriptParser;

namespace CHTL {

/**
 * JavaScript编译器
 * 使用ANTLR解析和处理JavaScript代码
 */
class JavaScriptCompiler : public ICompiler {
private:
    std::string moduleDirectory;
    bool debugMode;
    bool enableMinification;  // 是否启用代码压缩
    
    /**
     * 处理JavaScript解析树
     */
    std::string ProcessParseTree(antlr4::tree::ParseTree* tree);
    
    /**
     * 处理JavaScript错误
     */
    void HandleJavaScriptError(const std::string& error);
    
    /**
     * 预处理JavaScript代码（处理特殊语法）
     */
    std::string PreprocessJavaScript(const std::string& code);
    
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler() override;
    
    // 实现ICompiler接口
    std::string Compile(const CodeFragmentPtr& fragment) override;
    void SetModuleDirectory(const std::string& dir) override { moduleDirectory = dir; }
    void SetDebugMode(bool enable) override { debugMode = enable; }
    std::string GetCompilerName() const override { return "JavaScript Compiler"; }
    void Reset() override;
    
    // JavaScript编译器特有方法
    void SetMinification(bool enable) { enableMinification = enable; }
    bool IsMinificationEnabled() const { return enableMinification; }
};

} // namespace CHTL

#endif // CHTL_JAVASCRIPT_JAVASCRIPTCOMPILER_H