#include "utilThen.h"
#include <sstream>
#include <regex>

namespace CJMOD {
namespace Chtholly {

std::string UtilThenExpression::GetSyntax() {
    return "util $expression then $callback";
}

std::string UtilThenExpression::ProcessSyntax(const std::string& input) {
    // 使用正则表达式解析 util ... then ... 语法
    std::regex pattern(R"(util\s+(.+?)\s+then\s+(.+))");
    std::smatch matches;
    
    if (std::regex_search(input, matches, pattern)) {
        std::string expression = matches[1].str();
        std::string callback = matches[2].str();
        return GenerateJavaScript(expression, callback);
    }
    
    return "";
}

std::string UtilThenExpression::GenerateJavaScript(const std::string& expression, const std::string& callback) {
    std::ostringstream js;
    js << "// 珂朵莉的util...then异步处理\n";
    js << "const chthollyUtil = {\n";
    js << "    async process(expr, callback) {\n";
    js << "        console.log('🌸 珂朵莉开始处理:', expr);\n";
    js << "        \n";
    js << "        try {\n";
    js << "            // 执行表达式\n";
    js << "            const result = await Promise.resolve(" << expression << ");\n";
    js << "            \n";
    js << "            console.log('✨ 珂朵莉处理完成:', result);\n";
    js << "            \n";
    js << "            // 执行回调\n";
    js << "            if (typeof callback === 'function') {\n";
    js << "                callback(result);\n";
    js << "            } else {\n";
    js << "                console.log('💖 珂朵莉说:', callback);\n";
    js << "            }\n";
    js << "            \n";
    js << "            return result;\n";
    js << "        } catch (error) {\n";
    js << "            console.error('💔 珂朵莉遇到了问题:', error);\n";
    js << "            console.log('但是珂朵莉不会放弃！会继续努力的 🌟');\n";
    js << "        }\n";
    js << "    }\n";
    js << "};\n";
    js << "\n";
    js << "// 执行珂朵莉的util...then\n";
    js << "chthollyUtil.process(\n";
    js << "    " << expression << ",\n";
    js << "    " << callback << "\n";
    js << ");\n";
    return js.str();
}

} // namespace Chtholly
} // namespace CJMOD