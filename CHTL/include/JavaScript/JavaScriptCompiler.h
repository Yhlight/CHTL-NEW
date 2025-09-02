#ifndef CHTL_JAVASCRIPT_JAVASCRIPTCOMPILER_H
#define CHTL_JAVASCRIPT_JAVASCRIPTCOMPILER_H

#include <string>
#include <memory>
#include "Common/ICompiler.h"

namespace CHTL {

/**
 * JavaScript编译器
 * 使用ANTLR解析和处理JavaScript代码
 */
class JavaScriptCompiler : public ICompiler {
private:
    std::string moduleDirectory;
    bool debugMode;
    
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler() override;
    
    // 实现ICompiler接口
    std::string Compile(const CodeFragmentPtr& fragment) override;
    void SetModuleDirectory(const std::string& dir) override { moduleDirectory = dir; }
    void SetDebugMode(bool enable) override { debugMode = enable; }
    std::string GetCompilerName() const override { return "JavaScript Compiler"; }
    void Reset() override;
};

} // namespace CHTL

#endif // CHTL_JAVASCRIPT_JAVASCRIPTCOMPILER_H