#ifndef CHTL_CSS_CSSCOMPILER_H
#define CHTL_CSS_CSSCOMPILER_H

#include <string>
#include <memory>
#include "Common/ICompiler.h"

namespace CHTL {

/**
 * CSS编译器
 * 使用ANTLR解析和处理CSS代码
 */
class CSSCompiler : public ICompiler {
private:
    std::string moduleDirectory;
    bool debugMode;
    
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