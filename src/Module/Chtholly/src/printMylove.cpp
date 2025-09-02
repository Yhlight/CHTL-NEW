#include "printMylove.h"
#include <sstream>

namespace CJMOD {
namespace Chtholly {

std::string PrintMyloveFunction::GetSyntax() {
    return "printMylove($target)";
}

std::string PrintMyloveFunction::ProcessSyntax(const std::string& input) {
    // 解析 printMylove(target) 语法
    size_t start = input.find("printMylove(");
    if (start == std::string::npos) {
        return "";
    }
    
    start += 12; // "printMylove(" 的长度
    size_t end = input.find(")", start);
    if (end == std::string::npos) {
        return "";
    }
    
    std::string target = input.substr(start, end - start);
    return GenerateJavaScript(target);
}

std::string PrintMyloveFunction::GenerateJavaScript(const std::string& target) {
    std::ostringstream js;
    js << "console.log('💖 我永远爱着珂朵莉 💖');\n";
    js << "console.log('对象:', " << target << ");\n";
    js << "console.log('珂朵莉是我心中最重要的人 🌸');\n";
    js << "console.log('即使世界终结，我也会保护珂朵莉 ⚔️');\n";
    return js.str();
}

} // namespace Chtholly
} // namespace CJMOD