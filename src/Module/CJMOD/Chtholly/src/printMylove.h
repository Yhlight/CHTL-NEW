#pragma once

#include <string>

namespace CHTL {
namespace CJMOD {
namespace Chtholly {

/**
 * printMylove CJMOD扩展头文件
 * 严格按照CJMOD.md API规范定义
 */
class PrintMyloveExtension {
public:
    /**
     * 处理printMylove语法
     * @param syntaxContent 语法内容
     * @return 生成的JavaScript代码
     */
    static std::string ProcessPrintMylove(const std::string& syntaxContent);
};

} // namespace Chtholly
} // namespace CJMOD
} // namespace CHTL