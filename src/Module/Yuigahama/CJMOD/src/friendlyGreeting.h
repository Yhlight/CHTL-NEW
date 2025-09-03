#pragma once

#include <string>
#include <vector>

namespace CJMOD {
namespace Yuigahama {

/**
 * 由比滨友好问候功能
 */

class FriendlyGreetingProcessor {
public:
    static std::string ProcessGreeting(const std::string& config);
    static std::string GenerateGreetingResponse(const std::string& input);
    static std::vector<std::string> GetGreetingTemplates();
};

} // namespace Yuigahama
} // namespace CJMOD