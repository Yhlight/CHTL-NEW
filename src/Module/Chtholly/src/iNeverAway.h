#pragma once
#include <string>

namespace CJMOD {
namespace Chtholly {

/**
 * iNeverAway CJMOD函数
 * 语法: iNeverAway()
 * 功能: 创建永不消失的守护效果
 */
class INeverAwayFunction {
public:
    static std::string GetSyntax();
    static std::string ProcessSyntax(const std::string& input);
    static std::string GenerateJavaScript();
};

} // namespace Chtholly
} // namespace CJMOD