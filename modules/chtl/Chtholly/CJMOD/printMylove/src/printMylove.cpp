#include "printMylove.h"
#include <iostream>
#include <sstream>

namespace CHTL {
namespace CJMOD {

const char* PrintMyLove::ASCII_CHARS = " .:-=+*#%@";
const char* PrintMyLove::PIXEL_CHARS = "█▓▒░ ";

PrintMyLove::PrintMyLove() {}

PrintMyLove::~PrintMyLove() {}

void PrintMyLove::Register() {
    // 创建CHTL JS函数
    CHTLJSFunction::CreateCHTLJSFunction("printMylove", "printMylove {url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_}");
    
    // 分析语法
    Arg args = Syntax::analyze("printMylove {url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_}");
    
    // 绑定url参数（必须）
    args.bind("url", [](const std::string& value) {
        return value;
    });
    
    // 绑定mode参数（可选，默认ASCII）
    args.bind("mode", [](const std::string& value) {
        if (value.empty()) return std::string("ASCII");
        return value;
    });
    
    // 绑定width参数（可选）
    args.bind("width", [](const std::string& value) {
        if (value.empty()) return std::string("auto");
        return value;
    });
    
    // 绑定height参数（可选）
    args.bind("height", [](const std::string& value) {
        if (value.empty()) return std::string("auto");
        return value;
    });
    
    // 绑定scale参数（可选，默认1.0）
    args.bind("scale", [](const std::string& value) {
        if (value.empty()) return std::string("1.0");
        return value;
    });
    
    // 设置转换函数
    args.transform([](const std::vector<AtomArg>& atomArgs) -> std::string {
        std::stringstream js;
        
        // 生成JavaScript代码
        js << "(function() {\n";
        js << "  const img = new Image();\n";
        js << "  img.crossOrigin = 'anonymous';\n";
        js << "  img.src = " << atomArgs[0].value << ";\n";
        js << "  \n";
        js << "  img.onload = function() {\n";
        js << "    const canvas = document.createElement('canvas');\n";
        js << "    const ctx = canvas.getContext('2d');\n";
        js << "    \n";
        
        // 处理尺寸
        js << "    let width = " << atomArgs[2].value << ";\n";
        js << "    let height = " << atomArgs[3].value << ";\n";
        js << "    const scale = " << atomArgs[4].value << ";\n";
        js << "    \n";
        js << "    if (width === 'auto') width = img.width * scale;\n";
        js << "    if (height === 'auto') height = img.height * scale;\n";
        js << "    \n";
        js << "    canvas.width = width;\n";
        js << "    canvas.height = height;\n";
        js << "    ctx.drawImage(img, 0, 0, width, height);\n";
        js << "    \n";
        js << "    const imageData = ctx.getImageData(0, 0, width, height);\n";
        js << "    const pixels = imageData.data;\n";
        js << "    let result = '';\n";
        js << "    \n";
        
        // ASCII或Pixel模式
        js << "    const mode = " << atomArgs[1].value << ";\n";
        js << "    const chars = mode === 'ASCII' ? ' .:-=+*#%@' : '█▓▒░ ';\n";
        js << "    \n";
        js << "    for (let y = 0; y < height; y++) {\n";
        js << "      for (let x = 0; x < width; x++) {\n";
        js << "        const i = (y * width + x) * 4;\n";
        js << "        const r = pixels[i];\n";
        js << "        const g = pixels[i + 1];\n";
        js << "        const b = pixels[i + 2];\n";
        js << "        const brightness = (r + g + b) / 3;\n";
        js << "        const charIndex = Math.floor((brightness / 255) * (chars.length - 1));\n";
        js << "        result += chars[charIndex];\n";
        js << "      }\n";
        js << "      result += '\\n';\n";
        js << "    }\n";
        js << "    \n";
        js << "    console.log(result);\n";
        js << "    return result;\n";
        js << "  };\n";
        js << "})()";
        
        return js.str();
    });
    
    // 导出结果
    CJMODGenerator::exportResult(args);
}

} // namespace CJMOD
} // namespace CHTL