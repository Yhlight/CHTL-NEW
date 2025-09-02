#include "iNeverAway.h"
#include <sstream>

namespace CJMOD {
namespace Chtholly {

std::string INeverAwayFunction::GetSyntax() {
    return "iNeverAway()";
}

std::string INeverAwayFunction::ProcessSyntax(const std::string& input) {
    if (input.find("iNeverAway()") != std::string::npos) {
        return GenerateJavaScript();
    }
    return "";
}

std::string INeverAwayFunction::GenerateJavaScript() {
    std::ostringstream js;
    js << "// 珂朵莉的永不消失守护\n";
    js << "const chthollyGuardian = {\n";
    js << "    isActive: true,\n";
    js << "    message: '我永远不会离开珂朵莉身边 💫',\n";
    js << "    \n";
    js << "    activate() {\n";
    js << "        if (this.isActive) {\n";
    js << "            console.log('🛡️ 珂朵莉的守护已激活');\n";
    js << "            console.log(this.message);\n";
    js << "            this.createProtection();\n";
    js << "        }\n";
    js << "    },\n";
    js << "    \n";
    js << "    createProtection() {\n";
    js << "        const protection = document.createElement('div');\n";
    js << "        protection.style.cssText = `\n";
    js << "            position: fixed;\n";
    js << "            top: 10px;\n";
    js << "            right: 10px;\n";
    js << "            background: linear-gradient(45deg, #ff9a9e, #fecfef);\n";
    js << "            color: white;\n";
    js << "            padding: 10px 15px;\n";
    js << "            border-radius: 25px;\n";
    js << "            font-size: 14px;\n";
    js << "            box-shadow: 0 4px 15px rgba(0,0,0,0.2);\n";
    js << "            z-index: 9999;\n";
    js << "            animation: chthollyGlow 2s infinite alternate;\n";
    js << "        `;\n";
    js << "        protection.textContent = '珂朵莉守护中... 🌸';\n";
    js << "        document.body.appendChild(protection);\n";
    js << "        \n";
    js << "        // 添加动画样式\n";
    js << "        const style = document.createElement('style');\n";
    js << "        style.textContent = `\n";
    js << "            @keyframes chthollyGlow {\n";
    js << "                from { box-shadow: 0 4px 15px rgba(255,154,158,0.4); }\n";
    js << "                to { box-shadow: 0 4px 25px rgba(254,207,239,0.6); }\n";
    js << "            }\n";
    js << "        `;\n";
    js << "        document.head.appendChild(style);\n";
    js << "    }\n";
    js << "};\n";
    js << "\n";
    js << "chthollyGuardian.activate();\n";
    return js.str();
}

} // namespace Chtholly
} // namespace CJMOD