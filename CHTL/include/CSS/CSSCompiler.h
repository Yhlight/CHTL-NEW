#ifndef CHTL_CSS_CSSCOMPILER_H
#define CHTL_CSS_CSSCOMPILER_H

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

// 前向声明CSS解析器类
class css3Lexer;
class css3Parser;

namespace CHTL {

/**
 * CSS编译器
 * 使用ANTLR解析和处理CSS代码
 */
class CSSCompiler : public ICompiler {
private:
    std::string moduleDirectory;
    bool debugMode;
    
    /**
     * 处理CSS解析树
     */
    std::string ProcessParseTree(antlr4::tree::ParseTree* tree);
    
    /**
     * 处理CSS错误
     */
    void HandleCSSError(const std::string& error);
    
public:
    CSSCompiler();
    ~CSSCompiler() override;
    
    // 实现ICompiler接口
    std::string Compile(const CodeFragmentPtr& fragment) override;
    void SetModuleDirectory(const std::string& dir) override { moduleDirectory = dir; }
    void SetDebugMode(bool enable) override { debugMode = enable; }
    std::string GetCompilerName() const override { return "CSS Compiler"; }
    void Reset() override;
};

} // namespace CHTL

#endif // CHTL_CSS_CSSCOMPILER_H