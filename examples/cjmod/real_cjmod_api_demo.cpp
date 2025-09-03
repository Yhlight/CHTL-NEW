/**
 * 真正的CJMOD API演示
 * 严格按照CJMOD.md实现的API使用示例
 */

#include "../../src/CHTL JS/CJMODSystem/CJMODSyntax.h"
#include "../../src/CHTL JS/CJMODSystem/CJMODScanner.h"
#include "../../src/CHTL JS/CJMODSystem/CJMODGenerator.h"
#include "../../src/CHTL JS/CJMODSystem/CHTLJSFunction.h"
#include <iostream>

using namespace CJMOD;

int main() {
    std::cout << "🔥 CJMOD API完整演示（严格按照CJMOD.md）" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // 按照CJMOD.md的使用案例
    std::cout << "📋 步骤1: 语法分析" << std::endl;
    Arg args = Syntax::analyze("$ ** $");  // 语法分析
    args.print();  // 输出-> ["$", "**", "$"]
    
    std::cout << "\n📋 步骤2: 绑定函数" << std::endl;
    args.bind("$", [](const std::string& value) {
        return value;
    });
    
    args.bind("**", [](const std::string& value) {
        return value;
    });
    
    args.bind("$", [](const std::string& value) {
        return value;
    });
    
    std::cout << "\n📋 步骤3: 扫描（使用双指针扫描法或前置截取）" << std::endl;
    Arg result = CJMODScanner::scan(args, "**");
    result.print();  // 输出-> ["3", "**", "4"]
    
    std::cout << "\n📋 步骤4: 填充值" << std::endl;
    args.fillValue(result);
    std::cout << "arg[0].value: " << args[0].getValue() << std::endl;  // 输出-> 3
    std::cout << "arg[1].value: " << args[1].getValue() << std::endl;  // 输出-> **
    std::cout << "arg[2].value: " << args[2].getValue() << std::endl;  // 输出-> 4
    
    std::cout << "\n📋 步骤5: 转换" << std::endl;
    args.transform("pow(" + args[0].getValue() + ", " + args[2].getValue() + ")");
    
    std::cout << "\n📋 步骤6: 导出结果" << std::endl;
    std::string jsCode = CJMODGenerator::exportResult(args);
    std::cout << "生成的JS代码: " << jsCode << std::endl;
    
    std::cout << "\n🌸 CHTL JS函数演示" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // 使用CHTLJSFunction
    std::cout << "📋 创建CHTL JS函数" << std::endl;
    CHTLJSFunction func = CHTLJSFunction::CreateCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");
    
    std::cout << "函数名: " << func.getFunctionName() << std::endl;
    std::cout << "支持vir: " << (func.supportsVir() ? "是" : "否") << std::endl;
    
    std::cout << "\n📋 手动绑定虚对象" << std::endl;
    // 对于符合CHTL JS函数语法的函数，手动绑定虚对象支持
    if (Syntax::isCHTLJSFunction("printMyLove {url: $!_, mode: $?_}")) {
        CHTLJSFunction::bindVirtualObject("printMyLove");
        std::cout << "printMyLove现在支持vir: " 
                  << (CHTLJSFunction::supportsVirtualObject("printMyLove") ? "是" : "否") << std::endl;
    }
    
    std::cout << "\n🎉 CJMOD API演示完成！" << std::endl;
    std::cout << "这展示了CJMOD API的强大功能：" << std::endl;
    std::cout << "✅ Syntax::analyze - 语法分析" << std::endl;
    std::cout << "✅ Arg::bind - 函数绑定" << std::endl;
    std::cout << "✅ CJMODScanner::scan - 双指针/前置截取扫描" << std::endl;
    std::cout << "✅ Arg::fillValue - 值填充" << std::endl;
    std::cout << "✅ Arg::transform - 代码转换" << std::endl;
    std::cout << "✅ CJMODGenerator::exportResult - 结果导出" << std::endl;
    std::cout << "✅ CHTLJSFunction::CreateCHTLJSFunction - CHTL JS函数创建" << std::endl;
    std::cout << "✅ CHTLJSFunction::bindVirtualObject - 虚对象绑定" << std::endl;
    
    return 0;
}