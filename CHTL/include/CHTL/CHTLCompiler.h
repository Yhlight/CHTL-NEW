#ifndef CHTL_CHTL_CHTLCOMPILER_H
#define CHTL_CHTL_CHTLCOMPILER_H

#include <string>
#include <memory>
#include "Common/ICompiler.h"
#include "CHTL/Core/Context.h"
#include "CHTL/Core/CompileResult.h"

namespace CHTL {

// 前向声明
class Lexer;
class Parser;
class Generator;

/**
 * CHTL编译器
 * 负责编译CHTL代码片段
 */
class CHTLCompiler : public ICompiler {
private:
    std::shared_ptr<CompileContext> context;    // 编译上下文
    std::unique_ptr<Lexer> lexer;              // 词法分析器
    std::unique_ptr<Parser> parser;             // 语法分析器
    std::unique_ptr<Generator> generator;       // 代码生成器
    
    std::string moduleDirectory;                // 模块目录
    std::string sourceFile;                     // 源文件路径
    bool debugMode;                             // 调试模式
    bool hasHTML5Declaration;                   // 是否有HTML5声明
    
    CompileResult lastResult;                   // 最后一次编译结果
    
    /**
     * 初始化编译器组件
     */
    void InitializeComponents();
    
    /**
     * 预处理代码片段
     */
    std::string PreprocessFragment(const std::string& code);
    
    /**
     * 后处理编译结果
     */
    void PostprocessResult(CompileResult& result);
    
public:
    CHTLCompiler();
    ~CHTLCompiler() override;
    
    // 实现ICompiler接口
    std::string Compile(const CodeFragmentPtr& fragment) override;
    void SetModuleDirectory(const std::string& dir) override;
    void SetDebugMode(bool enable) override;
    std::string GetCompilerName() const override { return "CHTL Compiler"; }
    void Reset() override;
    
    // CHTL特有方法
    void SetSourceFile(const std::string& file);
    const std::string& GetSourceFile() const { return sourceFile; }
    
    bool HasHTML5Declaration() const { return hasHTML5Declaration; }
    const CompileResult& GetLastCompileResult() const { return lastResult; }
    
    // 获取编译上下文（供其他组件使用）
    std::shared_ptr<CompileContext> GetContext() const { return context; }
};

} // namespace CHTL

#endif // CHTL_CHTL_CHTLCOMPILER_H