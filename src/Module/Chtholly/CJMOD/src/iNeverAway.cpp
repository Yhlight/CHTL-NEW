/**
 * 珂朵莉模块CJMOD部分 - iNeverAway功能
 * 使用真正的CJMOD API实现永恒守护功能
 * 严格按照CJMOD.md规范
 */

#include "iNeverAway.h"
#include "../../../CHTL JS/CJMODSystem/CJMODCore.h"
#include "../../../CHTL JS/CJMODSystem/CHTLJSFunction.h"

namespace Chtholly {

void iNeverAway::initializeCJMODAPI() {
    std::cout << "🌸 珂朵莉模块：初始化iNeverAway CJMOD API" << std::endl;
    
    // 1. 创建永恒守护语法 - 使用CJMOD强大的语法分析
    CJMOD::Arg guardianPattern = CJMOD::Syntax::analyze("guardian $!_ forever $?_");
    
    // 2. 绑定强大的处理函数
    guardianPattern.bind("$!_", [](const std::string& target) {
        // 必须无序参数：守护目标
        if (target.empty()) {
            return std::string("world");  // 默认守护世界
        }
        return target;
    });
    
    guardianPattern.bind("$?_", [](const std::string& duration) {
        // 可选无序参数：守护时长
        if (duration.empty()) {
            return std::string("eternal");  // 默认永恒
        }
        return duration;
    });
    
    // 3. 创建CHTL JS函数，天然支持vir虚对象
    CJMOD::CHTLJSFunction guardianFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "iNeverAway {target: $!_, duration: $?_, promise: $?_}"
    );
    
    std::cout << "   ✅ 创建守护函数: " << guardianFunc.getFunctionName() << std::endl;
    std::cout << "   ✅ 天然支持vir: " << (guardianFunc.supportsVir() ? "是" : "否") << std::endl;
    
    // 4. 创建记忆保护语法
    CJMOD::Arg memoryPattern = CJMOD::Syntax::analyze("protect $ from $ until $");
    
    memoryPattern.bind("$", [](const std::string& memory) {
        return "珍贵的回忆：" + memory;
    });
    
    // 5. 创建时间循环语法 - 使用不定参数
    CJMOD::Arg timeLoopPattern = CJMOD::Syntax::analyze("timeLoop ...");
    
    timeLoopPattern.bind("...", [](const std::string& moments) {
        return "时间循环中的瞬间：" + moments;
    });
    
    std::cout << "   🔥 iNeverAway CJMOD API初始化完成" << std::endl;
}

void iNeverAway::demonstrateGuardianPower() {
    std::cout << "\n🛡️  演示永恒守护功能：" << std::endl;
    
    // 使用CJMOD API创建守护语法
    CJMOD::Arg guardianSyntax = CJMOD::Syntax::analyze("$ will be protected by $ forever");
    
    // 绑定守护逻辑
    guardianSyntax.bind("$", [](const std::string& target) {
        return target.empty() ? "这个世界" : target;
    });
    
    // 使用CJMODScanner的强大扫描能力
    CJMOD::Arg scanResult = CJMOD::CJMODScanner::scan(guardianSyntax, "protected");
    
    // 填充扫描结果
    guardianSyntax.fillValue(scanResult);
    
    // 转换为守护誓言
    guardianSyntax.transform(
        "console.log('🌸 珂朵莉的守护誓言：' + " + 
        guardianSyntax[0].getValue() + " + ' 将永远被 ' + " + 
        guardianSyntax[1].getValue() + " + ' 守护');"
    );
    
    // 导出最终的守护代码
    std::string guardianCode = CJMOD::CJMODGenerator::exportResult(guardianSyntax);
    
    std::cout << "   🌸 生成的守护代码: " << guardianCode << std::endl;
}

void iNeverAway::createMemoryProtection() {
    std::cout << "\n💎 创建记忆保护机制：" << std::endl;
    
    // 使用CJMOD的语法检测功能
    std::string memoryCode = "{memories: ['happy', 'precious', 'eternal'], protection: true}";
    
    if (CJMOD::Syntax::isObject(memoryCode)) {
        std::cout << "   ✅ 检测到有效的记忆对象" << std::endl;
        
        // 创建记忆保护函数
        CJMOD::CHTLJSFunction memoryFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
            "protectMemory {memories: $!_, protection: $!_, guardian: $?_}"
        );
        
        std::cout << "   🛡️  记忆保护函数创建完成: " << memoryFunc.getFunctionName() << std::endl;
        std::cout << "   💎 支持虚对象: " << (memoryFunc.supportsVir() ? "是" : "否") << std::endl;
    }
    
    // 使用CJMOD的数组检测
    std::string memoriesArray = "['first_meeting', 'precious_moments', 'eternal_promise']";
    
    if (CJMOD::Syntax::isArray(memoriesArray)) {
        std::cout << "   ✅ 检测到记忆数组，启动保护机制" << std::endl;
    }
}

std::string iNeverAway::generateEternalPromise(const std::string& target, const std::string& promise) {
    // 使用CJMOD API生成永恒誓言
    
    CJMOD::Arg promisePattern = CJMOD::Syntax::analyze("I $!_ promise to $!_ forever");
    
    // 填充誓言内容
    promisePattern[0].setValue(target.empty() ? "珂朵莉" : target);
    promisePattern[1].setValue(promise.empty() ? "守护这个世界" : promise);
    
    // 转换为完整誓言
    promisePattern.transform(
        "\"🌸 \" + " + promisePattern[0].getValue() + " + \" 发誓：我承诺 \" + " +
        promisePattern[1].getValue() + " + \"，直到永远。这是我不会离开的誓言。\""
    );
    
    // 导出誓言代码
    std::string promiseCode = CJMOD::CJMODGenerator::exportResult(promisePattern);
    
    return promiseCode;
}

void iNeverAway::bindVirtualObjectSupport() {
    std::cout << "\n🌟 绑定虚对象支持：" << std::endl;
    
    // 为符合CHTL JS函数语法的函数绑定虚对象支持
    std::vector<std::string> functions = {
        "eternalGuard {target: $!_, duration: $?_}",
        "protectWorld {method: $!_, intensity: $?_}",
        "preserveMemory {memory: $!_, protection: $!_}",
        "timeLoop {moments: ...}"
    };
    
    for (const auto& func : functions) {
        if (CJMOD::Syntax::isCHTLJSFunction(func)) {
            std::string funcName = func.substr(0, func.find(' '));
            CJMOD::CHTLJSFunction::bindVirtualObject(funcName);
            
            std::cout << "   ✅ " << funcName << " 现在支持vir: " 
                      << (CJMOD::CHTLJSFunction::supportsVirtualObject(funcName) ? "是" : "否") << std::endl;
        }
    }
}

} // namespace Chtholly