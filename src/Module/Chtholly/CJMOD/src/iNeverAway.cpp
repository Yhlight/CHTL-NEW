#include "iNeverAway.h"
#include <sstream>
#include <regex>

namespace CJMOD {
namespace Chtholly {

std::string INeverAwayProcessor::ProcessINeverAway(const std::string& config) {
    auto parsedConfig = ParseConfig(config);
    return GenerateGuardianCode(parsedConfig);
}

std::string INeverAwayProcessor::GenerateGuardianCode(const GuardianConfig& config) {
    std::ostringstream code;
    
    code << "// 珂朵莉永恒守护系统\n";
    code << "const EternalGuardian = {\n";
    code << "    Promise: function(target, duration) {\n";
    code << "        console.log(`💝 永恒的承诺: 我将永远守护${target}`);\n";
    code << "        return new Promise(resolve => {\n";
    code << "            setTimeout(() => {\n";
    code << "                console.log(`⭐ 守护时间: ${duration}ms 完成`);\n";
    code << "                resolve(`守护${target}的承诺已实现`);\n";
    code << "            }, duration);\n";
    code << "        });\n";
    code << "    },\n";
    code << "    \n";
    code << "    Protect: function(target, threat) {\n";
    code << "        console.log(`🛡️ 保护${target}免受${threat}的伤害`);\n";
    code << "        return {\n";
    code << "            status: 'protected',\n";
    code << "            target: target,\n";
    code << "            threat: threat,\n";
    code << "            guardian: '珂朵莉',\n";
    code << "            timestamp: Date.now()\n";
    code << "        };\n";
    code << "    }\n";
    code << "};\n";
    
    return code.str();
}

std::string INeverAwayProcessor::CreateVirtualGuardian(const std::string& name, const GuardianConfig& config) {
    std::ostringstream vir;
    
    vir << "vir " << name << " = iNeverAway {\n";
    vir << "    Void<Promise>: function(target, duration) {\n";
    vir << "        return EternalGuardian.Promise(target, duration);\n";
    vir << "    },\n";
    vir << "    \n";
    vir << "    Void<Protect>: function(target, threat) {\n";
    vir << "        return EternalGuardian.Protect(target, threat);\n";
    vir << "    },\n";
    vir << "    \n";
    vir << "    Void: {\n";
    vir << "        console.log('💫 永恒守护系统已激活');\n";
    vir << "        return 'guardian_active';\n";
    vir << "    }\n";
    vir << "};\n";
    
    return vir.str();
}

GuardianConfig INeverAwayProcessor::ParseConfig(const std::string& configStr) {
    GuardianConfig config;
    
    // 设置默认值
    config.target = "重要的人";
    config.promise = "我永远不会离开";
    config.threat = "未知威胁";
    config.duration = 5000;
    config.forever = true;
    
    // 简单的配置解析
    std::regex targetRegex(R"(target:\s*['"](.*?)['"])");
    std::regex promiseRegex(R"(promise:\s*['"](.*?)['"])");
    std::regex threatRegex(R"(threat:\s*['"](.*?)['"])");
    std::regex durationRegex(R"(duration:\s*(\d+))");
    std::regex foreverRegex(R"(forever:\s*(true|false))");
    
    std::smatch match;
    
    if (std::regex_search(configStr, match, targetRegex)) {
        config.target = match[1].str();
    }
    
    if (std::regex_search(configStr, match, promiseRegex)) {
        config.promise = match[1].str();
    }
    
    if (std::regex_search(configStr, match, threatRegex)) {
        config.threat = match[1].str();
    }
    
    if (std::regex_search(configStr, match, durationRegex)) {
        config.duration = std::stoi(match[1].str());
    }
    
    if (std::regex_search(configStr, match, foreverRegex)) {
        config.forever = (match[1].str() == "true");
    }
    
    return config;
}

} // namespace Chtholly
} // namespace CJMOD