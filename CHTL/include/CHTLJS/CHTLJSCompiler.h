#ifndef CHTL_CHTLJS_CHTLJSCOMPILER_H
#define CHTL_CHTLJS_CHTLJSCOMPILER_H

#include <string>
#include <memory>
#include "Common/ICompiler.h"
#include "CHTLJS/Core/Context.h"
#include "CHTLJS/Lexer/Lexer.h"
#include "CHTLJS/Parser/Parser.h"
#include "CHTLJS/Generator/Generator.h"

namespace CHTL {

/**
 * CHTL JS编译器
 * 负责编译CHTL JS代码片段
 */
class CHTLJSCompiler : public ICompiler {
private:
    std::shared_ptr<JS::CompileContext> context;    // 编译上下文
    std::unique_ptr<JS::Lexer> lexer;              // 词法分析器
    std::unique_ptr<JS::Parser> parser;             // 语法分析器
    std::unique_ptr<JS::Generator> generator;       // 代码生成器
    
    std::string moduleDirectory;                    // 模块目录
    std::string sourceFile;                         // 源文件路径
    bool debugMode;                                 // 调试模式
    
    /**
     * 初始化编译器组件
     */
    void InitializeComponents();
    
    /**
     * 预处理代码片段
     * 处理CHTL JS特有语法
     */
    std::string PreprocessFragment(const std::string& code);
    
    /**
     * 后处理生成的JavaScript代码
     */
    std::string PostprocessResult(const std::string& jsCode);
    
    /**
     * 生成AMD模块加载器代码
     */
    std::string GenerateModuleLoader();
    
    /**
     * 生成事件委托管理器代码
     */
    std::string GenerateEventDelegationManager();
    
    /**
     * 生成选择器缓存代码
     */
    std::string GenerateSelectorCache();
    
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler() override;
    
    // 实现ICompiler接口
    std::string Compile(const CodeFragmentPtr& fragment) override;
    void SetModuleDirectory(const std::string& dir) override;
    void SetDebugMode(bool enable) override;
    std::string GetCompilerName() const override { return "CHTL JS Compiler"; }
    void Reset() override;
    
    // CHTL JS特有方法
    void SetSourceFile(const std::string& file);
    const std::string& GetSourceFile() const { return sourceFile; }
    
    // 获取编译上下文（供其他组件使用）
    std::shared_ptr<JS::CompileContext> GetContext() const { return context; }
    
    // 启用/禁用特性
    void EnableSelectorCache(bool enable);
    void EnableEventDelegation(bool enable);
    void EnableAnimationOptimization(bool enable);
    
    // 获取编译统计信息
    JS::CompileContext::CodeStats GetCompileStats() const;
};

} // namespace CHTL

#endif // CHTL_CHTLJS_CHTLJSCOMPILER_H