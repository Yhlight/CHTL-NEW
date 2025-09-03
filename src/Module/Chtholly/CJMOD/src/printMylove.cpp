/**
 * 珂朵莉模块CJMOD部分 - printMylove功能
 * 使用真正的CJMOD API实现
 * 严格按照CJMOD.md规范
 */

#include "printMylove.h"
#include "../../../CHTL JS/CJMODSystem/CJMODCore.h"

namespace Chtholly {

void printMylove::registerCJMODAPI() {
    // 使用CJMOD API创建强大的语法扩展
    
    // 1. 使用Syntax::analyze创建语法模式
    CJMOD::Arg urlPattern = CJMOD::Syntax::analyze("url: $!_");  // url: 必须无序占位符
    CJMOD::Arg modePattern = CJMOD::Syntax::analyze("mode: $?_"); // mode: 可选无序占位符
    
    // 2. 绑定处理函数
    urlPattern.bind("$!_", [](const std::string& value) {
        // 验证URL格式
        if (value.find("http") == 0) {
            return value;
        }
        return "https://default.chtl.example.com";
    });
    
    modePattern.bind("$?_", [](const std::string& value) {
        // 处理模式参数
        if (value.empty()) {
            return "auto";  // 默认模式
        }
        return value;
    });
    
    // 3. 创建CHTL JS函数，天然支持vir
    CJMOD::CHTLJSFunction printMyLoveFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "printMyLove {url: $!_, mode: $?_}"
    );
    
    std::cout << "🌸 珂朵莉模块：printMyLove CJMOD API注册完成" << std::endl;
    std::cout << "   ✅ 支持虚对象vir: " << (printMyLoveFunc.supportsVir() ? "是" : "否") << std::endl;
}

void printMylove::demonstrateCJMODPower() {
    std::cout << "\n🔥 演示CJMOD强大功能：" << std::endl;
    
    // 演示CJMOD的语法分析能力
    CJMOD::Arg mathPattern = CJMOD::Syntax::analyze("$ ** $");
    std::cout << "📋 语法分析 '$ ** $'：";
    mathPattern.print();
    
    // 演示CJMOD的扫描能力
    CJMOD::Arg scanResult = CJMOD::CJMODScanner::scan(mathPattern, "**");
    std::cout << "📋 扫描结果：";
    scanResult.print();
    
    // 演示CJMOD的转换能力
    mathPattern.fillValue(scanResult);
    mathPattern.transform("pow(" + mathPattern[0].getValue() + ", " + mathPattern[2].getValue() + ")");
    
    // 演示CJMOD的导出能力
    std::string jsCode = CJMOD::CJMODGenerator::exportResult(mathPattern);
    std::cout << "📋 生成的JS代码: " << jsCode << std::endl;
    
    // 演示CJMOD的语法检测能力
    std::cout << "\n🔍 CJMOD语法检测能力：" << std::endl;
    std::cout << "   isObject('{name: \"CHTL\"}'): " << (CJMOD::Syntax::isObject("{name: \"CHTL\"}") ? "是" : "否") << std::endl;
    std::cout << "   isFunction('function test(){}'): " << (CJMOD::Syntax::isFunction("function test(){}") ? "是" : "否") << std::endl;
    std::cout << "   isArray('[1,2,3]'): " << (CJMOD::Syntax::isArray("[1,2,3]") ? "是" : "否") << std::endl;
    std::cout << "   isCHTLJSFunction('test {a: $!, b: $?}'): " << (CJMOD::Syntax::isCHTLJSFunction("test {a: $!, b: $?}") ? "是" : "否") << std::endl;
}

std::string printMylove::generateLoveMessage(const std::string& url, const std::string& mode) {
    // 使用CJMOD API处理参数
    CJMOD::Arg urlArg = CJMOD::Syntax::analyze("$!_");
    CJMOD::Arg modeArg = CJMOD::Syntax::analyze("$?_");
    
    // 填充值
    urlArg[0].setValue(url);
    modeArg[0].setValue(mode.empty() ? "auto" : mode);
    
    // 生成爱的消息
    std::ostringstream message;
    message << "💕 珂朵莉的爱意传递：\n";
    message << "   🌐 URL: " << urlArg[0].getValue() << "\n";
    message << "   ⚙️  模式: " << modeArg[0].getValue() << "\n";
    message << "   🌸 消息: 愿你的世界充满温暖与幸福\n";
    
    return message.str();
}

} // namespace Chtholly