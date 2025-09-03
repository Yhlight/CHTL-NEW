#pragma once

#include <string>
#include <unordered_map>

namespace CJMOD {
namespace Chtholly {

/**
 * printMylove功能头文件
 * 爱的表达功能，将图片转换为字符像素块
 */

struct PrintMyloveConfig {
    std::string url;
    std::string target;
    std::string message;
    std::string mode;     // ASCII或Pixel
    int width;
    int height;
    double scale;
};

class PrintMyloveProcessor {
public:
    static std::string ProcessPrintMylove(const std::string& config);
    static std::string GenerateCharacterArt(const PrintMyloveConfig& config);
    static PrintMyloveConfig ParseConfig(const std::string& configStr);
};

} // namespace Chtholly
} // namespace CJMOD