#include "printMylove.h"
#include <sstream>
#include <regex>

namespace CJMOD {
namespace Chtholly {

std::string PrintMyloveProcessor::ProcessPrintMylove(const std::string& config) {
    auto parsedConfig = ParseConfig(config);
    return GenerateCharacterArt(parsedConfig);
}

std::string PrintMyloveProcessor::GenerateCharacterArt(const PrintMyloveConfig& config) {
    std::ostringstream art;
    
    if (config.mode == "ASCII") {
        // ASCII艺术生成
        art << "\n";
        art << "    ♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥\n";
        art << "    ♥                                   ♥\n";
        art << "    ♥     " << config.message << "           ♥\n";
        art << "    ♥                                   ♥\n";
        art << "    ♥           -> " << config.target << "              ♥\n";
        art << "    ♥                                   ♥\n";
        art << "    ♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥\n";
    } else {
        // Pixel模式
        art << "\n";
        art << "💝💝💝💝💝💝💝💝💝💝💝💝💝💝💝\n";
        art << "💝                                               💝\n";
        art << "💝     " << config.message << "           💝\n";
        art << "💝                                               💝\n";
        art << "💝           -> " << config.target << "              💝\n";
        art << "💝                                               💝\n";
        art << "💝💝💝💝💝💝💝💝💝💝💝💝💝💝💝\n";
    }
    
    return art.str();
}

PrintMyloveConfig PrintMyloveProcessor::ParseConfig(const std::string& configStr) {
    PrintMyloveConfig config;
    
    // 设置默认值
    config.target = "world";
    config.message = "我永远爱你";
    config.mode = "ASCII";
    config.width = 80;
    config.height = 40;
    config.scale = 1.0;
    
    // 简单的配置解析
    std::regex targetRegex(R"(target:\s*['"](.*?)['"])");
    std::regex messageRegex(R"(message:\s*['"](.*?)['"])");
    std::regex modeRegex(R"(mode:\s*['"](.*?)['"])");
    
    std::smatch match;
    
    if (std::regex_search(configStr, match, targetRegex)) {
        config.target = match[1].str();
    }
    
    if (std::regex_search(configStr, match, messageRegex)) {
        config.message = match[1].str();
    }
    
    if (std::regex_search(configStr, match, modeRegex)) {
        config.mode = match[1].str();
    }
    
    return config;
}

} // namespace Chtholly
} // namespace CJMOD