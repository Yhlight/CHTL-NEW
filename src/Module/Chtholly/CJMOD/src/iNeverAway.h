#pragma once

#include <string>
#include <functional>
#include <unordered_map>

namespace CJMOD {
namespace Chtholly {

/**
 * iNeverAway功能头文件
 * 永恒守护功能，创建标记函数组，支持状态区分
 */

struct GuardianConfig {
    std::string target;
    std::string promise;
    std::string threat;
    int duration;
    bool forever;
};

class INeverAwayProcessor {
public:
    static std::string ProcessINeverAway(const std::string& config);
    static std::string GenerateGuardianCode(const GuardianConfig& config);
    static GuardianConfig ParseConfig(const std::string& configStr);
    static std::string CreateVirtualGuardian(const std::string& name, const GuardianConfig& config);
};

} // namespace Chtholly
} // namespace CJMOD