#pragma once

#include <string>

namespace CHTL {
namespace CJMOD {
namespace Chtholly {

/**
 * util...then表达式CJMOD扩展头文件
 * 严格按照CJMOD.md API规范定义
 */
class UtilThenExtension {
public:
    /**
     * 处理util...then表达式语法
     * @param syntaxContent 语法内容
     * @return 生成的JavaScript代码
     */
    static std::string ProcessUtilThen(const std::string& syntaxContent);
};

} // namespace Chtholly
} // namespace CJMOD
} // namespace CHTL