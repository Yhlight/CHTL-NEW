#ifndef CHTL_CHTL_CORE_COMPILERESULT_H
#define CHTL_CHTL_CORE_COMPILERESULT_H

#include <string>

namespace CHTL {

/**
 * 编译结果结构
 */
struct CompileResult {
    std::string htmlContent;     // HTML内容
    std::string globalStyles;    // 全局样式
    std::string globalScripts;   // 全局脚本
    std::string headContent;     // <head>标签内容
    
    void Clear() {
        htmlContent.clear();
        globalStyles.clear();
        globalScripts.clear();
        headContent.clear();
    }
};

} // namespace CHTL

#endif // CHTL_CHTL_CORE_COMPILERESULT_H