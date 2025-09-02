#include "ComponentManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace CHTL {
namespace Core {

// 内部辅助函数
std::string GetStateString(ComponentState state) {
    switch (state) {
        case ComponentState::UNINITIALIZED: return "未初始化";
        case ComponentState::INITIALIZING: return "初始化中";
        case ComponentState::INITIALIZED: return "已初始化";
        case ComponentState::RUNNING: return "运行中";
        case ComponentState::STOPPED: return "已停止";
        case ComponentState::ERROR_STATE: return "错误状态";
        default: return "未知状态";
    }
}

// ComponentManager实现
ComponentManager::ComponentManager() : m_IsInitialized(false) {
}

ComponentManager::~ComponentManager() {
    StopAllComponents();
}

bool ComponentManager::Initialize() {
    if (m_IsInitialized) {
        return true;
    }
    
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_IsInitialized = true;
    
    std::cout << "🔧 组件管理器已初始化" << std::endl;
    return true;
}

bool ComponentManager::RegisterComponent(const ComponentRegistration& registration) {
    if (!ValidateRegistration(registration)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    // 检查是否已注册
    if (m_RegisteredComponents.find(registration.Name) != m_RegisteredComponents.end()) {
        std::cerr << "组件已存在: " << registration.Name << std::endl;
        return false;
    }
    
    m_RegisteredComponents[registration.Name] = registration;
    m_ComponentStates[registration.Name] = ComponentState::UNINITIALIZED;
    
    std::cout << "📦 组件已注册: " << registration.Name << " v" << registration.Version << std::endl;
    return true;
}

std::unique_ptr<IComponent> ComponentManager::CreateComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    auto it = m_RegisteredComponents.find(componentName);
    if (it == m_RegisteredComponents.end()) {
        std::cerr << "组件未注册: " << componentName << std::endl;
        return nullptr;
    }
    
    try {
        auto component = it->second.Factory();
        if (component) {
            std::cout << "🏗️ 组件已创建: " << componentName << std::endl;
        }
        return component;
    } catch (const std::exception& e) {
        std::cerr << "组件创建失败: " << componentName << " - " << e.what() << std::endl;
        return nullptr;
    }
}

IComponent* ComponentManager::GetComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    auto it = m_ActiveComponents.find(componentName);
    if (it != m_ActiveComponents.end()) {
        return it->second.get();
    }
    
    // 如果组件不存在，尝试创建
    auto component = CreateComponent(componentName);
    if (component) {
        IComponent* ptr = component.get();
        m_ActiveComponents[componentName] = std::move(component);
        return ptr;
    }
    
    return nullptr;
}

bool ComponentManager::StartComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    // 检查依赖
    if (!CheckDependencies(componentName)) {
        std::cerr << "组件依赖不满足: " << componentName << std::endl;
        return false;
    }
    
    auto component = GetComponent(componentName);
    if (!component) {
        return false;
    }
    
    SetComponentState(componentName, ComponentState::INITIALIZING);
    
    try {
        if (component->Initialize()) {
            SetComponentState(componentName, ComponentState::INITIALIZED);
            
            if (component->Start()) {
                SetComponentState(componentName, ComponentState::RUNNING);
                std::cout << "🚀 组件已启动: " << componentName << std::endl;
                return true;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "组件启动失败: " << componentName << " - " << e.what() << std::endl;
    }
    
    SetComponentState(componentName, ComponentState::ERROR_STATE);
    return false;
}

bool ComponentManager::StopComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    auto component = GetComponent(componentName);
    if (!component) {
        return false;
    }
    
    try {
        if (component->Stop()) {
            SetComponentState(componentName, ComponentState::STOPPED);
            std::cout << "⏹️ 组件已停止: " << componentName << std::endl;
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "组件停止失败: " << componentName << " - " << e.what() << std::endl;
    }
    
    return false;
}

size_t ComponentManager::StartAllComponents() {
    auto startOrder = ResolveDependencyOrder();
    size_t startedCount = 0;
    
    for (const auto& componentName : startOrder) {
        if (StartComponent(componentName)) {
            startedCount++;
        }
    }
    
    std::cout << "🎯 已启动 " << startedCount << "/" << startOrder.size() << " 个组件" << std::endl;
    return startedCount;
}

void ComponentManager::StopAllComponents() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    // 按相反顺序停止组件
    auto stopOrder = ResolveDependencyOrder();
    std::reverse(stopOrder.begin(), stopOrder.end());
    
    for (const auto& componentName : stopOrder) {
        auto it = m_ActiveComponents.find(componentName);
        if (it != m_ActiveComponents.end()) {
            try {
                it->second->Stop();
                SetComponentState(componentName, ComponentState::STOPPED);
            } catch (...) {
                // 忽略停止异常
            }
        }
    }
    
    m_ActiveComponents.clear();
    std::cout << "⏹️ 所有组件已停止" << std::endl;
}

bool ComponentManager::CheckDependencies(const std::string& componentName) {
    auto it = m_RegisteredComponents.find(componentName);
    if (it == m_RegisteredComponents.end()) {
        return false;
    }
    
    for (const auto& dep : it->second.Dependencies) {
        if (!dep.IsOptional) {
            auto state = GetComponentState(dep.ComponentName);
            if (state != ComponentState::RUNNING && state != ComponentState::INITIALIZED) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> ComponentManager::ResolveDependencyOrder() {
    std::vector<std::string> order;
    std::unordered_set<std::string> resolved;
    std::unordered_set<std::string> resolving;
    
    for (const auto& pair : m_RegisteredComponents) {
        if (resolved.find(pair.first) == resolved.end()) {
            ResolveDependencies(pair.first, resolved, resolving, order);
        }
    }
    
    return order;
}

std::vector<std::string> ComponentManager::GetRegisteredComponents() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    std::vector<std::string> components;
    for (const auto& pair : m_RegisteredComponents) {
        components.push_back(pair.first);
    }
    
    return components;
}

std::vector<std::string> ComponentManager::GetActiveComponents() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    std::vector<std::string> components;
    for (const auto& pair : m_ActiveComponents) {
        components.push_back(pair.first);
    }
    
    return components;
}

ComponentState ComponentManager::GetComponentState(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    auto it = m_ComponentStates.find(componentName);
    return (it != m_ComponentStates.end()) ? it->second : ComponentState::UNINITIALIZED;
}

bool ComponentManager::ComponentExists(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_RegisteredComponents.find(componentName) != m_RegisteredComponents.end();
}

bool ComponentManager::UnloadComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    // 先停止组件
    auto component = GetComponent(componentName);
    if (component) {
        component->Stop();
    }
    
    // 从活动组件中移除
    m_ActiveComponents.erase(componentName);
    SetComponentState(componentName, ComponentState::UNINITIALIZED);
    
    std::cout << "📤 组件已卸载: " << componentName << std::endl;
    return true;
}

bool ComponentManager::ReloadComponent(const std::string& componentName) {
    if (UnloadComponent(componentName)) {
        return StartComponent(componentName);
    }
    return false;
}

std::string ComponentManager::GenerateComponentReport() const {
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    oss << "║                              🔧 组件管理器报告 🔧                                          ║" << std::endl;
    oss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    oss << "\n📊 组件统计:" << std::endl;
    oss << "  已注册组件: " << m_RegisteredComponents.size() << " 个" << std::endl;
    oss << "  活动组件: " << m_ActiveComponents.size() << " 个" << std::endl;
    
    if (!m_RegisteredComponents.empty()) {
        oss << "\n📋 注册组件详情:" << std::endl;
        oss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
        
        for (const auto& pair : m_RegisteredComponents) {
            const auto& reg = pair.second;
            auto state = GetComponentState(pair.first);
            
            oss << "\n📦 " << reg.Name << " v" << reg.Version << std::endl;
            oss << "  描述: " << reg.Description << std::endl;
                            oss << "  状态: " << GetStateString(state) << std::endl;
            oss << "  依赖: " << reg.Dependencies.size() << " 个" << std::endl;
            
            if (!reg.Dependencies.empty()) {
                for (const auto& dep : reg.Dependencies) {
                    oss << "    • " << dep.ComponentName;
                    if (!dep.RequiredVersion.empty()) {
                        oss << " v" << dep.RequiredVersion;
                    }
                    if (dep.IsOptional) {
                        oss << " (可选)";
                    }
                    oss << std::endl;
                }
            }
        }
    }
    
    return oss.str();
}

ComponentManager& ComponentManager::GetInstance() {
    static ComponentManager instance;
    return instance;
}

bool ComponentManager::ResolveDependencies(const std::string& componentName,
                                         std::unordered_set<std::string>& resolved,
                                         std::unordered_set<std::string>& resolving,
                                         std::vector<std::string>& order) {
    // 检查循环依赖
    if (resolving.find(componentName) != resolving.end()) {
        std::cerr << "检测到循环依赖: " << componentName << std::endl;
        return false;
    }
    
    // 如果已解析，跳过
    if (resolved.find(componentName) != resolved.end()) {
        return true;
    }
    
    resolving.insert(componentName);
    
    // 解析依赖
    auto it = m_RegisteredComponents.find(componentName);
    if (it != m_RegisteredComponents.end()) {
        for (const auto& dep : it->second.Dependencies) {
            if (!ResolveDependencies(dep.ComponentName, resolved, resolving, order)) {
                return false;
            }
        }
    }
    
    resolving.erase(componentName);
    resolved.insert(componentName);
    order.push_back(componentName);
    
    return true;
}

void ComponentManager::SetComponentState(const std::string& componentName, ComponentState state) {
    m_ComponentStates[componentName] = state;
}

bool ComponentManager::ValidateRegistration(const ComponentRegistration& registration) const {
    if (registration.Name.empty()) {
        std::cerr << "组件名称不能为空" << std::endl;
        return false;
    }
    
    if (!registration.Factory) {
        std::cerr << "组件工厂函数不能为空: " << registration.Name << std::endl;
        return false;
    }
    
    return true;
}

// ComponentBase实现
ComponentBase::ComponentBase(const std::string& name, const std::string& version)
    : m_ComponentName(name), m_ComponentVersion(version), m_State(ComponentState::UNINITIALIZED) {
}

} // namespace Core
} // namespace CHTL