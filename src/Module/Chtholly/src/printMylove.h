#pragma once
#include <string>

namespace CJMOD {
namespace Chtholly {

/**
 * printMylove CJMOD函数
 * 语法: printMylove(对象)
 * 功能: 在控制台输出对珂朵莉的爱意表达
 */
class PrintMyloveFunction {
public:
    static std::string GetSyntax();
    static std::string ProcessSyntax(const std::string& input);
    static std::string GenerateJavaScript(const std::string& target);
};

} // namespace Chtholly
} // namespace CJMOD