#include "ServiceLocator.h"
#include <sstream>
#include <iostream>

// 前向声明
namespace CHTL {
namespace Core {
    std::string GetStateString(ComponentState state);
}
}

namespace CHTL {
namespace Core {

std::string ServiceLocator::GenerateServiceReport() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    oss << "║                              🔧 服务定位器报告 🔧                                          ║" << std::endl;
    oss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    oss << "\n📊 服务统计:" << std::endl;
    oss << "  已注册服务: " << m_Services.size() << " 个" << std::endl;
    oss << "  命名服务: " << m_ServiceNames.size() << " 个" << std::endl;
    
    if (!m_Services.empty()) {
        oss << "\n📋 服务详情:" << std::endl;
        oss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
        
        for (const auto& pair : m_Services) {
            const auto& service = pair.second;
            if (service) {
                oss << "\n🔧 服务: " << service->GetComponentName() << std::endl;
                oss << "  版本: " << service->GetComponentVersion() << std::endl;
                oss << "  状态: " << GetStateString(service->GetState()) << std::endl;
                oss << "  健康: " << (service->IsHealthy() ? "✅ 健康" : "❌ 异常") << std::endl;
                
                auto deps = service->GetDependencies();
                if (!deps.empty()) {
                    oss << "  依赖: ";
                    for (size_t i = 0; i < deps.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << deps[i];
                    }
                    oss << std::endl;
                }
            }
        }
        
        // 显示命名服务映射
        if (!m_ServiceNames.empty()) {
            oss << "\n📝 命名服务映射:" << std::endl;
            for (const auto& namePair : m_ServiceNames) {
                oss << "  " << namePair.first << " -> 类型索引" << std::endl;
            }
        }
    }
    
    return oss.str();
}



} // namespace Core
} // namespace CHTL