// 珂朵莉iNeverAway CJMOD扩展实现
// 严格按照CJMOD.md文档的API规范实现

#include "iNeverAway.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODSyntax.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODScanner.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODGenerator.h"

namespace CHTL {
namespace CJMOD {
namespace Chtholly {

/**
 * iNeverAway CJMOD扩展实现
 * 严格按照语法文档定义：创建标记函数组，支持键定义和状态区分，与虚对象共用
 */
class INeverAwayExtension {
public:
    /**
     * 处理iNeverAway语法
     * 语法格式：vir Test = iNeverAway { Void<A>: function(int, int) {...}, Void<B>: function(int, int) {...} }
     */
    static std::string ProcessINeverAway(const std::string& syntaxContent) {
        // 使用CJMOD API进行语法分析
        Arg args = Syntax::analyze("vir $! = iNeverAway { $... }");
        
        // 绑定参数处理函数
        args.bind("$!", [](const std::string& value) {
            return value; // 虚对象名称，必需
        });
        
        args.bind("$...", [](const std::string& value) {
            return value; // 函数定义内容，可变参数
        });
        
        // 扫描语法片段
        Arg result = CJMODScanner::scan(args, syntaxContent);
        args.fillValue(result);
        
        // 生成JavaScript代码
        std::string jsCode = R"(
(function() {
    console.log('💫 我永远不会离开你身边');
    console.log('🛡️ 妖精兵器，永恒守护');
    
    // 创建全局函数注册表
    window.ChthollyGuardian = window.ChthollyGuardian || {
        functions: {},
        virtualObjects: {},
        isActive: true,
        protectionLevel: 'maximum'
    };
    
    // 解析函数定义
    const functionDefs = {)" + args[1].value + R"(};
    const virObjectName = ')" + args[0].value + R"(';
    
    // 创建虚对象映射
    const virtualObject = {};
    
    // 处理函数定义
    for (const [key, func] of Object.entries(functionDefs)) {
        // 生成全局函数名
        const globalFuncName = 'ChthollyVir_' + virObjectName + '_' + key.replace(/[<>]/g, '_');
        
        // 注册全局函数
        window[globalFuncName] = func;
        window.ChthollyGuardian.functions[globalFuncName] = func;
        
        // 创建虚对象引用
        if (key.includes('<')) {
            // 处理状态区分的同名键
            const baseName = key.split('<')[0];
            const state = key.match(/<(.+)>/)[1];
            
            if (!virtualObject[baseName]) {
                virtualObject[baseName] = {};
            }
            virtualObject[baseName][state] = globalFuncName;
        } else {
            virtualObject[key] = globalFuncName;
        }
    }
    
    // 注册虚对象
    window.ChthollyGuardian.virtualObjects[virObjectName] = virtualObject;
    
    // 创建虚对象访问器
    const virProxy = new Proxy({}, {
        get: function(target, prop) {
            if (typeof prop === 'string' && prop.includes('<') && prop.includes('>')) {
                // 处理状态访问 Test->Void<A>()
                const baseName = prop.split('<')[0];
                const state = prop.match(/<(.+)>/)[1];
                
                if (virtualObject[baseName] && virtualObject[baseName][state]) {
                    return window[virtualObject[baseName][state]];
                }
            } else if (virtualObject[prop]) {
                // 处理普通访问
                if (typeof virtualObject[prop] === 'string') {
                    return window[virtualObject[prop]];
                } else {
                    return virtualObject[prop];
                }
            }
            return undefined;
        }
    });
    
    // 导出虚对象
    window[virObjectName] = virProxy;
    
    console.log('💖 虚对象 ' + virObjectName + ' 已创建，永恒守护启动');
    
    return virProxy;
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