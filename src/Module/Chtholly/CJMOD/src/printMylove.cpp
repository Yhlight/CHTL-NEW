/**
 * 珂朵莉模块CJMOD部分 - printMylove功能
 * 使用真正的CJMOD API实现爱意传递功能
 * 严格按照CJMOD.md规范，展示CJMOD的极致强大
 */

#include "printMylove.h"
#include "../../../CHTL JS/CJMODSystem/CJMODCore.h"
#include "../../../CHTL JS/CJMODSystem/CHTLJSFunction.h"
#include <iostream>

namespace Chtholly {

void printMylove::registerCJMODAPI() {
    std::cout << "🌸 珂朵莉模块：注册printMylove CJMOD API" << std::endl;
    
    // ==========================================
    // 1. 使用CJMOD强大的语法分析创建爱意表达语法
    // ==========================================
    
    // 爱意传递语法模式
    CJMOD::Arg lovePattern = CJMOD::Syntax::analyze("love $!_ to $!_ with $?_ forever $?_");
    
    // 绑定爱意处理函数
    lovePattern.bind("$!_", [](const std::string& loveType) {
        // 必须无序参数：爱意类型
        if (loveType.empty()) return std::string("eternal_love");
        return "pure_" + loveType;
    });
    
    lovePattern.bind("$!_", [](const std::string& target) {
        // 必须无序参数：爱意目标
        if (target.empty()) return std::string("this_beautiful_world");
        return target;
    });
    
    lovePattern.bind("$?_", [](const std::string& method) {
        // 可选无序参数：表达方式
        return method.empty() ? "gentle_whisper" : method;
    });
    
    lovePattern.bind("$?_", [](const std::string& duration) {
        // 可选无序参数：持续时间
        return duration.empty() ? "eternity" : duration;
    });
    
    // ==========================================
    // 2. 创建天然支持vir的CHTL JS函数
    // ==========================================
    
    CJMOD::CHTLJSFunction printMyLoveFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "printMyLove {url: $!_, mode: $?_, intensity: $?_, message: $?_, callback: $?_}"
    );
    
    CJMOD::CHTLJSFunction expressLoveFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "expressLove {target: $!_, emotion: $!_, method: $?_, duration: $?_}"
    );
    
    CJMOD::CHTLJSFunction sendWishesFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "sendWishes {wishes: ..., recipient: $!_, delivery: $?_}"
    );
    
    std::cout << "   ✅ " << printMyLoveFunc.getFunctionName() << " 创建成功（天然支持vir: " << (printMyLoveFunc.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ " << expressLoveFunc.getFunctionName() << " 创建成功（天然支持vir: " << (expressLoveFunc.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ " << sendWishesFunc.getFunctionName() << " 创建成功（天然支持vir: " << (sendWishesFunc.supportsVir() ? "是" : "否") << "）" << std::endl;
    
    // ==========================================
    // 3. 手动绑定更多虚对象支持
    // ==========================================
    
    // 为符合CHTL JS函数语法的函数手动绑定vir支持
    std::vector<std::string> loveFunctions = {
        "shareHappiness {joy: $!_, recipients: ..., method: $?_}",
        "protectPrecious {treasure: $!_, protection: $!_, guardian: $?_}",
        "createMemories {moments: ..., preservation: $!_, sharing: $?_}",
        "spreadWarmth {warmth: $!_, range: $?_, intensity: $?_}"
    };
    
    for (const auto& func : loveFunctions) {
        if (CJMOD::Syntax::isCHTLJSFunction(func)) {
            std::string funcName = func.substr(0, func.find(' '));
            CJMOD::CHTLJSFunction::bindVirtualObject(funcName);
            
            std::cout << "   🌟 " << funcName << " 现在支持vir: " 
                      << (CJMOD::CHTLJSFunction::supportsVirtualObject(funcName) ? "是" : "否") << std::endl;
        }
    }
}

void printMylove::demonstrateCJMODPower() {
    std::cout << "\n🔥 演示CJMOD强大功能：" << std::endl;
    
    // ==========================================
    // 演示完整的CJMOD工作流程
    // ==========================================
    
    // 1. 语法分析
    CJMOD::Arg loveExpression = CJMOD::Syntax::analyze("send $!_ to $!_ with $?_ intensity $?_");
    std::cout << "📋 爱意表达语法分析：";
    loveExpression.print();
    
    // 2. 绑定处理函数
    loveExpression.bind("$!_", [](const std::string& content) {
        return content.empty() ? "eternal_love" : content;
    });
    
    loveExpression.bind("$?_", [](const std::string& option) {
        return option.empty() ? "gentle" : option;
    });
    
    // 3. 使用双指针扫描法扫描
    CJMOD::Arg scanResult = CJMOD::CJMODScanner::scan(loveExpression, "to");
    std::cout << "📋 双指针扫描结果：";
    scanResult.print();
    
    // 4. 填充值
    loveExpression.fillValue(scanResult);
    std::cout << "📋 填充后的值：" << std::endl;
    for (size_t i = 0; i < loveExpression.size(); ++i) {
        std::cout << "   arg[" << i << "].value: " << loveExpression[i].getValue() << std::endl;
    }
    
    // 5. 转换为珂朵莉风格的爱意表达
    loveExpression.transform(
        "console.log('🌸 珂朵莉轻声说道：我要将 ' + " + loveExpression[0].getValue() + 
        " + ' 传递给 ' + " + loveExpression[1].getValue() + 
        " + '，用 ' + " + loveExpression[2].getValue() + 
        " + ' 的方式，以 ' + " + loveExpression[3].getValue() + " + ' 的强度。');"
    );
    
    // 6. 导出最终的爱意代码
    std::string loveCode = CJMOD::CJMODGenerator::exportResult(loveExpression);
    std::cout << "📋 生成的爱意代码: " << loveCode << std::endl;
    
    // ==========================================
    // 演示CJMOD的语法检测强大能力
    // ==========================================
    
    std::cout << "\n🔍 CJMOD语法检测能力：" << std::endl;
    
    // 检测各种JavaScript结构
    std::cout << "   检测爱意对象: " << (CJMOD::Syntax::isObject("{love: true, target: 'world', intensity: 'maximum'}") ? "✅" : "❌") << std::endl;
    std::cout << "   检测爱意函数: " << (CJMOD::Syntax::isFunction("function spreadLove() { return happiness; }") ? "✅" : "❌") << std::endl;
    std::cout << "   检测回忆数组: " << (CJMOD::Syntax::isArray("['first_meeting', 'precious_moments', 'eternal_promise']") ? "✅" : "❌") << std::endl;
    std::cout << "   检测CHTL JS爱意函数: " << (CJMOD::Syntax::isCHTLJSFunction("expressLove {emotion: $!, target: $!, method: $?}") ? "✅" : "❌") << std::endl;
}

std::string printMylove::generateLoveMessage(const std::string& url, const std::string& mode) {
    // 使用CJMOD API生成个性化的爱意消息
    
    // 1. 创建URL处理语法
    CJMOD::Arg urlPattern = CJMOD::Syntax::analyze("connect to $!_ via $?_");
    
    // 2. 绑定URL处理逻辑
    urlPattern.bind("$!_", [](const std::string& targetUrl) {
        // 验证和美化URL
        if (targetUrl.find("http") == 0) {
            return targetUrl;
        }
        return "https://love.chtl.example.com/" + targetUrl;
    });
    
    urlPattern.bind("$?_", [](const std::string& protocol) {
        return protocol.empty() ? "secure_love_protocol" : protocol;
    });
    
    // 3. 填充实际值
    urlPattern[0].setValue(url);
    urlPattern[1].setValue(mode);
    
    // 4. 转换为爱意连接代码
    urlPattern.transform(
        "console.log('🌸 珂朵莉建立爱意连接：'); " +
        "console.log('💕 目标: ' + " + urlPattern[0].getValue() + "); " +
        "console.log('🔗 方式: ' + " + urlPattern[1].getValue() + "); " +
        "console.log('💖 状态: 连接成功，爱意传递中...');"
    );
    
    // 5. 导出最终的爱意消息
    std::string loveMessage = CJMOD::CJMODGenerator::exportResult(urlPattern);
    
    // 6. 添加珂朵莉的个人风格
    std::ostringstream personalizedMessage;
    personalizedMessage << "// 🌸 珂朵莉的爱意传递系统\n";
    personalizedMessage << loveMessage << "\n";
    personalizedMessage << "console.log('🌸 珂朵莉轻声说道：愿你的世界充满温暖与幸福...');\n";
    personalizedMessage << "console.log('💝 这份爱意将永远守护着你。');\n";
    
    return personalizedMessage.str();
}

void printMylove::showcaseAdvancedFeatures() {
    std::cout << "\n🌟 展示CJMOD高级特性：" << std::endl;
    
    // ==========================================
    // 高级占位符组合使用
    // ==========================================
    
    // 复杂的爱意表达语法
    CJMOD::Arg complexLove = CJMOD::Syntax::analyze("express $!_ with $_ and ... to $!_ forever $?_");
    
    // 绑定复杂的处理逻辑
    complexLove.bind("$!_", [](const std::string& emotion) {
        return emotion.empty() ? "deepest_love" : "precious_" + emotion;
    });
    
    complexLove.bind("$_", [](const std::string& method) {
        return "method:" + (method.empty() ? "gentle_touch" : method);
    });
    
    complexLove.bind("...", [](const std::string& expressions) {
        return "expressions:[" + expressions + "]";
    });
    
    // 使用前置截取法扫描复杂语法
    CJMOD::Arg complexScanResult = CJMOD::CJMODScanner::scan(complexLove, "to");
    
    // 展示扫描结果
    std::cout << "📋 复杂语法扫描结果：";
    complexScanResult.print();
    
    // ==========================================
    // 创建珂朵莉专属的CHTL JS函数库
    // ==========================================
    
    std::vector<CJMOD::CHTLJSFunction> chthollyFunctions = {
        CJMOD::CHTLJSFunction::CreateCHTLJSFunction("whisperLove {message: $!_, target: $!_, emotion: $?_}"),
        CJMOD::CHTLJSFunction::CreateCHTLJSFunction("protectWorld {method: $!_, scope: $?_, duration: $?_}"),
        CJMOD::CHTLJSFunction::CreateCHTLJSFunction("shareHappiness {joy: $!_, recipients: ..., intensity: $?_}"),
        CJMOD::CHTLJSFunction::CreateCHTLJSFunction("preserveMemories {memories: ..., protection: $!_, sharing: $?_}"),
        CJMOD::CHTLJSFunction::CreateCHTLJSFunction("createMiracle {wish: $!_, power: $?_, belief: $?_}")
    };
    
    std::cout << "📚 珂朵莉专属CHTL JS函数库：" << std::endl;
    for (const auto& func : chthollyFunctions) {
        std::cout << "   🌸 " << func.getFunctionName() 
                  << "（天然支持vir: " << (func.supportsVir() ? "是" : "否") << "）" << std::endl;
    }
    
    std::cout << "🔥 printMylove CJMOD API注册完成，展现了CJMOD的极致强大！" << std::endl;
}

void printMylove::demonstrateAllPlaceholderTypes() {
    std::cout << "\n🎯 演示所有CJMOD占位符类型：" << std::endl;
    
    // ==========================================
    // 展示CJMOD支持的所有占位符类型
    // ==========================================
    
    struct PlaceholderDemo {
        std::string name;
        std::string pattern;
        std::string description;
    };
    
    std::vector<PlaceholderDemo> placeholders = {
        {"基本占位符", "send $ to $", "最基础的占位符"},
        {"可选占位符", "love $? with $?", "可选的参数"},
        {"必须占位符", "protect $! from $!", "必须提供的参数"},
        {"无序占位符", "create {joy: $_, peace: $_}", "无序的参数"},
        {"必须无序占位符", "express {love: $!_, target: $!_}", "必须的无序参数"},
        {"不定参数占位符", "gather ... together", "任意数量的参数"},
        {"组合占位符", "miracle {wish: $!_, power: $?, support: ..., method: $_}", "多种占位符组合"}
    };
    
    for (const auto& demo : placeholders) {
        std::cout << "   📝 " << demo.name << ": " << demo.pattern << std::endl;
        std::cout << "      " << demo.description << std::endl;
        
        // 使用CJMOD API分析每种占位符
        CJMOD::Arg pattern = CJMOD::Syntax::analyze(demo.pattern);
        std::cout << "      解析结果: ";
        pattern.print();
        std::cout << std::endl;
    }
}

void printMylove::demonstrateScanningMethods() {
    std::cout << "\n🔍 演示CJMOD两种强大扫描方法：" << std::endl;
    
    // ==========================================
    // 双指针扫描法演示
    // ==========================================
    
    std::cout << "1. 双指针扫描法演示：" << std::endl;
    
    CJMOD::Arg doublePointerPattern = CJMOD::Syntax::analyze("$ ** $ + $ * $");
    
    // 使用双指针扫描法处理数学操作符
    CJMOD::Arg doublePointerResult = CJMOD::CJMODScanner::scan(doublePointerPattern, "**");
    std::cout << "   🔍 双指针扫描 '**' 操作符结果: ";
    doublePointerResult.print();
    
    CJMOD::Arg multiplyResult = CJMOD::CJMODScanner::scan(doublePointerPattern, "*");
    std::cout << "   🔍 双指针扫描 '*' 操作符结果: ";
    multiplyResult.print();
    
    // ==========================================
    // 前置截取法演示
    // ==========================================
    
    std::cout << "\n2. 前置截取法演示：" << std::endl;
    
    CJMOD::Arg prefixCutPattern = CJMOD::Syntax::analyze("calculate(a, b, c) then process(result) then output");
    
    // 使用前置截取法处理then关键字
    CJMOD::Arg prefixCutResult = CJMOD::CJMODScanner::scan(prefixCutPattern, "then");
    std::cout << "   ✂️  前置截取 'then' 关键字结果: ";
    prefixCutResult.print();
    
    // ==========================================
    // 智能选择扫描方法演示
    // ==========================================
    
    std::cout << "\n3. 智能选择扫描方法演示：" << std::endl;
    
    CJMOD::Arg smartPattern1 = CJMOD::Syntax::analyze("$ ** $");  // 操作符 -> 双指针扫描
    CJMOD::Arg smartPattern2 = CJMOD::Syntax::analyze("$ then $");  // 关键字 -> 前置截取
    
    CJMOD::Arg smartResult1 = CJMOD::CJMODScanner::scan(smartPattern1);  // 自动选择双指针
    CJMOD::Arg smartResult2 = CJMOD::CJMODScanner::scan(smartPattern2);  // 自动选择前置截取
    
    std::cout << "   🧠 智能选择结果1（双指针）: ";
    smartResult1.print();
    std::cout << "   🧠 智能选择结果2（前置截取）: ";
    smartResult2.print();
}

std::string printMylove::createAdvancedLoveExpression(const std::string& target, const std::string& emotion, const std::string& method) {
    // 使用CJMOD API的所有强大功能创建高级爱意表达
    
    std::ostringstream expression;
    expression << "// 🌸 珂朵莉的高级爱意表达系统（CJMOD API驱动）\n\n";
    
    // 1. 使用语法分析创建表达模式
    CJMOD::Arg expressionPattern = CJMOD::Syntax::analyze("express $!_ to $!_ via $?_ with $?_ intensity");
    
    // 2. 绑定个性化处理
    expressionPattern.bind("$!_", [&](const std::string& param) {
        return param == "emotion" ? emotion : (param == "target" ? target : param);
    });
    
    expressionPattern.bind("$?_", [&](const std::string& param) {
        return param == "method" ? method : "gentle";
    });
    
    // 3. 使用双指针扫描法处理
    CJMOD::Arg scanResult = CJMOD::CJMODScanner::scan(expressionPattern, "to");
    expressionPattern.fillValue(scanResult);
    
    // 4. 转换为完整的爱意表达
    expressionPattern.transform(
        "{\n" +
        "  emotion: '" + expressionPattern[0].getValue() + "',\n" +
        "  target: '" + expressionPattern[1].getValue() + "',\n" +
        "  method: '" + expressionPattern[2].getValue() + "',\n" +
        "  intensity: '" + expressionPattern[3].getValue() + "',\n" +
        "  message: '🌸 珂朵莉的爱意如春风般温暖，如星光般永恒',\n" +
        "  promise: '我会一直守护着这份美好'\n" +
        "}"
    );
    
    // 5. 导出高级表达
    std::string advancedExpression = CJMOD::CJMODGenerator::exportResult(expressionPattern);
    
    expression << "// 使用CJMOD API生成的高级爱意表达\n";
    expression << "const chthollyLoveExpression = " << advancedExpression << ";\n\n";
    expression << "// 使用天然支持vir的函数\n";
    expression << "vir loveResult = printMyLove({\n";
    expression << "    url: '" << target << "',\n";
    expression << "    mode: '" << (method.empty() ? "tender" : method) << "',\n";
    expression << "    intensity: 'maximum',\n";
    expression << "    message: chthollyLoveExpression.message\n";
    expression << "});\n\n";
    expression << "console.log('🌸 珂朵莉的爱意已传递:', loveResult);\n";
    
    return expression.str();
}

} // namespace Chtholly