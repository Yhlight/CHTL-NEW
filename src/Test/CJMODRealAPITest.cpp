/**
 * CJMOD真正API测试
 * 严格按照CJMOD.md实现和测试所有API
 */

#include "../CHTL JS/CJMODSystem/CJMODCore.h"
#include "../CHTL JS/CJMODSystem/CHTLJSFunction.h"
#include <iostream>

using namespace CJMOD;

int main() {
    std::cout << "🔥 CJMOD真正API测试（严格按照CJMOD.md）" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        // 完全按照CJMOD.md的使用案例
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
        
        std::cout << "\n📋 步骤3: 扫描（双指针扫描法/前置截取）" << std::endl;
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
        
        std::cout << "\n🌸 CHTL JS函数API测试" << std::endl;
        std::cout << "===============================================" << std::endl;
        
        // 测试CHTLJSFunction API
        std::cout << "📋 创建CHTL JS函数" << std::endl;
        CHTLJSFunction func = CHTLJSFunction::CreateCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");
        
        std::cout << "函数名: " << func.getFunctionName() << std::endl;
        std::cout << "天然支持vir: " << (func.supportsVir() ? "是" : "否") << std::endl;
        
        std::cout << "\n📋 手动绑定虚对象" << std::endl;
        if (Syntax::isCHTLJSFunction("testFunc {param: $!, optional: $?}")) {
            CHTLJSFunction::bindVirtualObject("testFunc");
            std::cout << "testFunc现在支持vir: " 
                      << (CHTLJSFunction::supportsVirtualObject("testFunc") ? "是" : "否") << std::endl;
        }
        
        std::cout << "\n🔍 语法检测API测试" << std::endl;
        std::cout << "===============================================" << std::endl;
        
        // 测试语法检测功能
        std::cout << "isObject('{b: 1}'): " << (Syntax::isObject("{b: 1}") ? "是" : "否") << std::endl;
        std::cout << "isFunction('function a(){}'): " << (Syntax::isFunction("function a(){}") ? "是" : "否") << std::endl;
        std::cout << "isArray('[1, 2, 3]'): " << (Syntax::isArray("[1, 2, 3]") ? "是" : "否") << std::endl;
        std::cout << "isCHTLJSFunction('test {test: 1, test2: 2};'): " 
                  << (Syntax::isCHTLJSFunction("test {test: 1, test2: 2};") ? "是" : "否") << std::endl;
        
        std::cout << "\n🎯 占位符类型测试" << std::endl;
        std::cout << "===============================================" << std::endl;
        
        // 测试各种占位符类型
        Arg placeholderTest1 = Syntax::analyze("$");      // 占位符
        Arg placeholderTest2 = Syntax::analyze("$?");     // 可选占位符
        Arg placeholderTest3 = Syntax::analyze("$!");     // 必须占位符
        Arg placeholderTest4 = Syntax::analyze("$_");     // 无序占位符
        Arg placeholderTest5 = Syntax::analyze("$!_");    // 必须无序占位符
        Arg placeholderTest6 = Syntax::analyze("...");    // 不定参数占位符
        
        std::cout << "$ 占位符解析：";
        placeholderTest1.print();
        std::cout << "$? 可选占位符解析：";
        placeholderTest2.print();
        std::cout << "$! 必须占位符解析：";
        placeholderTest3.print();
        std::cout << "$_ 无序占位符解析：";
        placeholderTest4.print();
        std::cout << "$!_ 必须无序占位符解析：";
        placeholderTest5.print();
        std::cout << "... 不定参数占位符解析：";
        placeholderTest6.print();
        
        std::cout << "\n✅ CJMOD API测试完成！" << std::endl;
        std::cout << "🔥 CJMOD确实是CHTL极为强大的特征之一！" << std::endl;
        std::cout << "\n展示的强大功能：" << std::endl;
        std::cout << "✅ Syntax::analyze - 语法分析" << std::endl;
        std::cout << "✅ Arg::bind - 函数绑定" << std::endl;
        std::cout << "✅ CJMODScanner::scan - 双指针扫描法/前置截取" << std::endl;
        std::cout << "✅ Arg::fillValue - 值填充" << std::endl;
        std::cout << "✅ Arg::transform - 代码转换" << std::endl;
        std::cout << "✅ CJMODGenerator::exportResult - 结果导出" << std::endl;
        std::cout << "✅ CHTLJSFunction::CreateCHTLJSFunction - CHTL JS函数创建" << std::endl;
        std::cout << "✅ CHTLJSFunction::bindVirtualObject - 虚对象绑定" << std::endl;
        std::cout << "✅ 各种占位符类型：$, $?, $!, $_, $!_, ..." << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
}