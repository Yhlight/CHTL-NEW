// 珂朵莉printMylove CJMOD扩展实现
// 严格按照CJMOD.md文档的API规范实现

#include "printMylove.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODSyntax.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODScanner.h"
#include "../../../../CHTL JS/CJMODSystem/CJMODGenerator.h"

namespace CHTL {
namespace CJMOD {
namespace Chtholly {

/**
 * printMylove CJMOD扩展实现
 * 严格按照语法文档定义：将图片转换为字符像素块或ASCII形式，输出到控制台
 */
class PrintMyloveExtension {
public:
    /**
     * 处理printMylove语法
     * 语法格式：printMylove { url: , mode: , width: , height: , scale: }
     */
    static std::string ProcessPrintMylove(const std::string& syntaxContent) {
        // 使用CJMOD API进行语法分析
        Arg args = Syntax::analyze("printMylove { url: $!, mode: $?, width: $?, height: $?, scale: $? }");
        
        // 绑定参数处理函数
        args.bind("$!", [](const std::string& value) {
            return value; // url参数，必需
        });
        
        args.bind("$?", [](const std::string& value) {
            return value.empty() ? "ASCII" : value; // 可选参数，默认ASCII
        });
        
        // 扫描语法片段
        Arg result = CJMODScanner::scan(args, syntaxContent);
        args.fillValue(result);
        
        // 生成JavaScript代码
        std::string jsCode = R"(
(function() {
    console.log('💖 珂朵莉的爱的表达');
    console.log('🌸 世界上最幸福的女孩');
    
    const config = {
        url: ')" + args[0].value + R"(',
        mode: ')" + args[1].value + R"(',
        width: )" + (args[2].value.empty() ? "80" : args[2].value) + R"(,
        height: )" + (args[3].value.empty() ? "40" : args[3].value) + R"(,
        scale: )" + (args[4].value.empty() ? "1.0" : args[4].value) + R"(
    };
    
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const img = new Image();
    
    img.onload = function() {
        canvas.width = config.width;
        canvas.height = config.height;
        ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
        
        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
        const pixels = imageData.data;
        let result = '';
        
        if (config.mode === 'ASCII') {
            const chars = ' .:-=+*#%@';
            for (let y = 0; y < canvas.height; y++) {
                for (let x = 0; x < canvas.width; x++) {
                    const offset = (y * canvas.width + x) * 4;
                    const r = pixels[offset];
                    const g = pixels[offset + 1];
                    const b = pixels[offset + 2];
                    const brightness = (r + g + b) / 3;
                    const charIndex = Math.floor((brightness / 255) * (chars.length - 1));
                    result += chars[charIndex];
                }
                result += '\n';
            }
        } else {
            // Pixel模式
            for (let y = 0; y < canvas.height; y++) {
                for (let x = 0; x < canvas.width; x++) {
                    const offset = (y * canvas.width + x) * 4;
                    const r = pixels[offset];
                    const g = pixels[offset + 1];
                    const b = pixels[offset + 2];
                    const brightness = (r + g + b) / 3;
                    result += brightness > 128 ? '██' : '  ';
                }
                result += '\n';
            }
        }
        
        console.log('🎨 珂朵莉的' + config.mode + '艺术:');
        console.log(result);
    };
    
    img.src = config.url;
    return '💖 Chtholly Love Forever 💖';
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