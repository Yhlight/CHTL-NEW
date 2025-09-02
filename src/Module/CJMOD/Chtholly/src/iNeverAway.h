#pragma once

#include <string>

namespace CHTL {
namespace CJMOD {
namespace Chtholly {

/**
 * iNeverAway CJMOD扩展头文件
 * 严格按照CJMOD.md API规范定义
 */
class INeverAwayExtension {
public:
    /**
     * 处理iNeverAway语法
     * @param syntaxContent 语法内容
     * @return 生成的JavaScript代码
     */
    static std::string ProcessINeverAway(const std::string& syntaxContent);
};

} // namespace Chtholly
} // namespace CJMOD
} // namespace CHTL