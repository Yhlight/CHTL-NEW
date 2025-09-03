#include "friendlyGreeting.h"
#include <random>

namespace CJMOD {
namespace Yuigahama {

std::string FriendlyGreetingProcessor::ProcessGreeting(const std::string& config) {
    return GenerateGreetingResponse(config);
}

std::string FriendlyGreetingProcessor::GenerateGreetingResponse(const std::string& input) {
    auto templates = GetGreetingTemplates();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, templates.size() - 1);
    
    return templates[dis(gen)];
}

std::vector<std::string> FriendlyGreetingProcessor::GetGreetingTemplates() {
    return {
        "嗨！你好呀～ 我是由比滨结衣！",
        "今天天气真好呢～ 一起加油吧！",
        "啊哈哈～ 很高兴见到你！",
        "嗯嗯～ 有什么我可以帮忙的吗？"
    };
}

} // namespace Yuigahama
} // namespace CJMOD