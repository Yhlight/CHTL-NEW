#pragma once
#include <string>

namespace CJMOD {
namespace Chtholly {

/**
 * util...then CJMOD表达式
 * 语法: util 表达式 then 回调
 * 功能: 珂朵莉风格的异步处理
 */
class UtilThenExpression {
public:
    static std::string GetSyntax();
    static std::string ProcessSyntax(const std::string& input);
    static std::string GenerateJavaScript(const std::string& expression, const std::string& callback);
};

} // namespace Chtholly
} // namespace CJMOD