/**
 * 珂朵莉模块CJMOD部分 - utilThen工具功能
 * 使用CJMOD API实现强大的工具链功能
 * 严格按照CJMOD.md规范
 */

#include "utilThen.h"
#include "../../../CHTL JS/CJMODSystem/CJMODCore.h"
#include "../../../CHTL JS/CJMODSystem/CHTLJSFunction.h"

namespace Chtholly {

void utilThen::setupCJMODUtilities() {
    std::cout << "🔧 珂朵莉模块：设置utilThen CJMOD工具" << std::endl;
    
    // 1. 创建链式操作语法 - 使用CJMOD强大的占位符系统
    CJMOD::Arg chainPattern = CJMOD::Syntax::analyze("$ then $ then $");
    
    // 绑定链式处理函数
    chainPattern.bind("$", [](const std::string& operation) {
        return "await " + operation;
    });
    
    chainPattern.bind("then", [](const std::string& connector) {
        return ".then";
    });
    
    // 2. 创建条件执行语法 - 使用可选和必须占位符
    CJMOD::Arg conditionalPattern = CJMOD::Syntax::analyze("if $! then $!_ else $?_");
    
    conditionalPattern.bind("$!", [](const std::string& condition) {
        return "(" + condition + ")";
    });
    
    conditionalPattern.bind("$!_", [](const std::string& thenAction) {
        return thenAction.empty() ? "console.log('条件为真')" : thenAction;
    });
    
    conditionalPattern.bind("$?_", [](const std::string& elseAction) {
        return elseAction.empty() ? "console.log('条件为假')" : elseAction;
    });
    
    // 3. 创建批处理语法 - 使用不定参数
    CJMOD::Arg batchPattern = CJMOD::Syntax::analyze("batch ...");
    
    batchPattern.bind("...", [](const std::string& operations) {
        return "[" + operations + "].forEach(op => op())";
    });
    
    // 4. 创建CHTL JS工具函数，天然支持vir
    CJMOD::CHTLJSFunction thenFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "utilThen {operation: $!_, callback: $?_, error: $?_}"
    );
    
    CJMOD::CHTLJSFunction chainFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "chainOperations {operations: ..., final: $?_}"
    );
    
    CJMOD::CHTLJSFunction conditionalFunc = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "conditionalExecute {condition: $!, then: $!_, else: $?_}"
    );
    
    std::cout << "   ✅ utilThen函数创建: " << thenFunc.getFunctionName() << "（支持vir: " << (thenFunc.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ chainOperations函数创建: " << chainFunc.getFunctionName() << "（支持vir: " << (chainFunc.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ conditionalExecute函数创建: " << conditionalFunc.getFunctionName() << "（支持vir: " << (conditionalFunc.supportsVir() ? "是" : "否") << "）" << std::endl;
}

void utilThen::demonstrateChainOperations() {
    std::cout << "\n⛓️  演示链式操作功能：" << std::endl;
    
    // 使用CJMOD API创建复杂的链式操作
    CJMOD::Arg chainSyntax = CJMOD::Syntax::analyze("$ -> $ -> $ -> $");
    
    // 绑定每个步骤
    chainSyntax.bind("$", [](const std::string& step) {
        return step.empty() ? "defaultStep" : step;
    });
    
    chainSyntax.bind("->", [](const std::string& arrow) {
        return ".then";
    });
    
    // 使用CJMODScanner扫描链式结构
    CJMOD::Arg scanResult = CJMOD::CJMODScanner::scan(chainSyntax, "->");
    
    // 填充扫描结果
    chainSyntax.fillValue(scanResult);
    
    // 转换为JavaScript链式调用
    chainSyntax.transform(
        chainSyntax[0].getValue() + "()" +
        chainSyntax[1].getValue() + "(" + chainSyntax[2].getValue() + ")" +
        chainSyntax[3].getValue() + "(" + chainSyntax[4].getValue() + ")" +
        chainSyntax[5].getValue() + "(" + chainSyntax[6].getValue() + ")"
    );
    
    // 导出链式操作代码
    std::string chainCode = CJMOD::CJMODGenerator::exportResult(chainSyntax);
    
    std::cout << "   🔗 生成的链式代码: " << chainCode << std::endl;
}

void utilThen::demonstrateConditionalLogic() {
    std::cout << "\n🤔 演示条件逻辑功能：" << std::endl;
    
    // 创建复杂的条件语法
    CJMOD::Arg ifThenElse = CJMOD::Syntax::analyze("if $! then $!_ else $?_ finally $?_");
    
    // 绑定条件处理
    ifThenElse.bind("$!", [](const std::string& condition) {
        return condition.empty() ? "true" : condition;
    });
    
    ifThenElse.bind("$!_", [](const std::string& thenBlock) {
        return thenBlock.empty() ? "console.log('Then executed')" : thenBlock;
    });
    
    ifThenElse.bind("$?_", [](const std::string& elseBlock) {
        return elseBlock.empty() ? "console.log('Else executed')" : elseBlock;
    });
    
    // 转换为JavaScript条件语句
    ifThenElse.transform(
        "if " + ifThenElse[0].getValue() + " { " +
        ifThenElse[1].getValue() + " } else { " +
        ifThenElse[2].getValue() + " } finally { " +
        ifThenElse[3].getValue() + " }"
    );
    
    std::string conditionalCode = CJMOD::CJMODGenerator::exportResult(ifThenElse);
    
    std::cout << "   🧠 生成的条件代码: " << conditionalCode << std::endl;
}

void utilThen::createAdvancedUtilities() {
    std::cout << "\n🚀 创建高级工具功能：" << std::endl;
    
    // 使用CJMOD创建多种高级工具
    
    // 1. 异步工具
    CJMOD::CHTLJSFunction asyncUtil = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "asyncThen {promise: $!_, success: $?_, error: $?_, finally: $?_}"
    );
    
    // 2. 数据处理工具
    CJMOD::CHTLJSFunction dataUtil = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "processData {input: $!_, transform: $?_, validate: $?_, output: $?_}"
    );
    
    // 3. 事件工具
    CJMOD::CHTLJSFunction eventUtil = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "handleEvent {target: $!_, events: ..., options: $?_}"
    );
    
    // 4. 动画工具
    CJMOD::CHTLJSFunction animateUtil = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
        "animateThen {element: $!_, animation: $!_, duration: $?_, easing: $?_, complete: $?_}"
    );
    
    std::cout << "   ✅ asyncThen: " << asyncUtil.getFunctionName() << "（vir支持: " << (asyncUtil.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ processData: " << dataUtil.getFunctionName() << "（vir支持: " << (dataUtil.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ handleEvent: " << eventUtil.getFunctionName() << "（vir支持: " << (eventUtil.supportsVir() ? "是" : "否") << "）" << std::endl;
    std::cout << "   ✅ animateThen: " << animateUtil.getFunctionName() << "（vir支持: " << (animateUtil.supportsVir() ? "是" : "否") << "）" << std::endl;
    
    // 手动绑定更多虚对象支持
    CJMOD::CHTLJSFunction::bindVirtualObject("thenChain");
    CJMOD::CHTLJSFunction::bindVirtualObject("conditionalFlow");
    CJMOD::CHTLJSFunction::bindVirtualObject("batchProcess");
    
    std::cout << "   🌟 所有工具函数现在都支持虚对象vir！" << std::endl;
}

} // namespace Chtholly