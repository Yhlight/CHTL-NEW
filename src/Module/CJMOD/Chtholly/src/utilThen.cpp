// 珂朵莉util...then CJMOD扩展实现
// 严格按照CJMOD.md文档的API规范实现

#include "utilThen.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODSyntax.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODScanner.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODGenerator.h"

namespace CHTL {
namespace CJMOD {
namespace Chtholly {

/**
 * util...then表达式CJMOD扩展实现
 * 严格按照语法文档定义：util 表达式 -> change { 条件变化时 } -> then { 条件完成时 }
 */
class UtilThenExtension {
public:
    /**
     * 处理util...then表达式语法
     * 语法格式：util 表达式 -> change { ... } -> then { ... }
     * 或：util 表达式 -> change 语句; -> then 语句;
     */
    static std::string ProcessUtilThen(const std::string& syntaxContent) {
        // 使用CJMOD API进行语法分析
        Arg args = Syntax::analyze("util $! -> change $? -> then $?");
        
        // 绑定参数处理函数
        args.bind("$!", [](const std::string& value) {
            return value; // 条件表达式，必需
        });
        
        args.bind("$?", [](const std::string& value) {
            return value; // change和then的内容，可选
        });
        
        // 扫描语法片段
        Arg result = CJMODScanner::scan(args, syntaxContent);
        args.fillValue(result);
        
        // 生成JavaScript代码
        std::string jsCode = R"(
(function() {
    console.log('🔗 珂朵莉util...then表达式启动');
    
    const condition = () => )" + args[0].value + R"(;
    const changeHandler = () => {
        )" + args[1].value + R"(
    };
    const thenHandler = () => {
        )" + args[2].value + R"(
    };
    
    let lastValue = condition();
    let isMonitoring = true;
    
    // 条件监听器
    const monitor = {
        start: function() {
            isMonitoring = true;
            this.check();
        },
        
        stop: function() {
            isMonitoring = false;
        },
        
        check: function() {
            if (!isMonitoring) return;
            
            try {
                const currentValue = condition();
                
                // 检查条件是否发生变化
                if (currentValue !== lastValue) {
                    console.log('🔄 珂朵莉检测到条件变化');
                    changeHandler();
                    lastValue = currentValue;
                    
                    // 如果条件为真，执行then
                    if (currentValue) {
                        console.log('✅ 珂朵莉条件完成');
                        thenHandler();
                    }
                }
            } catch (e) {
                console.error('❌ 珂朵莉util...then监听异常:', e);
            }
            
            // 继续监听
            if (isMonitoring) {
                setTimeout(() => this.check(), 100);
            }
        }
    };
    
    // 启动监听
    monitor.start();
    
    // 导出控制接口
    window.ChthollyUtilThen = window.ChthollyUtilThen || [];
    window.ChthollyUtilThen.push(monitor);
    
    console.log('💖 珂朵莉util...then表达式已启动监听');
    
    return monitor;
})()
        )";
        
        // 使用CJMOD生成器导出结果
        args.transform(jsCode);
        CJMODGenerator::exportResult(args);
        
        return jsCode;
    }
};

} // namespace Chtholly
} // namespace CJMOD
} // namespace CHTL