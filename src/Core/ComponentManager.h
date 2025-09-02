#pragma once

#include "ComponentInterface.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <mutex>

namespace CHTL {
namespace Core {

/**
 * 组件工厂函数类型
 */
using ComponentFactory = std::function<std::unique_ptr<IComponent>()>;

/**
 * 组件依赖信息
 */
struct ComponentDependency {
    std::string ComponentName;
    std::string RequiredVersion;
    bool IsOptional;
    
    ComponentDependency(const std::string& name, const std::string& version = "", bool optional = false)
        : ComponentName(name), RequiredVersion(version), IsOptional(optional) {}
};

/**
 * 组件注册信息
 */
struct ComponentRegistration {
    std::string Name;
    std::string Version;
    std::string Description;
    ComponentFactory Factory;
    std::vector<ComponentDependency> Dependencies;
    
    // 默认构造函数
    ComponentRegistration() = default;
    
    ComponentRegistration(const std::string& name, const std::string& version, 
                         const std::string& desc, ComponentFactory factory)
        : Name(name), Version(version), Description(desc), Factory(factory) {}
};

/**
 * 组件管理器
 * 负责组件的注册、创建、生命周期管理和依赖解析
 */
class ComponentManager {
private:
    std::unordered_map<std::string, ComponentRegistration> m_RegisteredComponents;
    std::unordered_map<std::string, std::unique_ptr<IComponent>> m_ActiveComponents;
    std::unordered_map<std::string, ComponentState> m_ComponentStates;
    mutable std::mutex m_Mutex;
    bool m_IsInitialized;

public:
    /**
     * 构造函数
     */
    ComponentManager();
    
    /**
     * 析构函数
     */
    ~ComponentManager();
    
    /**
     * 初始化组件管理器
     * @return 是否成功
     */
    bool Initialize();
    
    /**
     * 注册组件
     * @param registration 组件注册信息
     * @return 是否成功
     */
    bool RegisterComponent(const ComponentRegistration& registration);
    
    /**
     * 注册组件（便捷方法）
     * @param name 组件名称
     * @param version 版本
     * @param description 描述
     * @param factory 工厂函数
     * @return 是否成功
     */
    template<typename T>
    bool RegisterComponent(const std::string& name, const std::string& version, 
                          const std::string& description) {
        ComponentFactory factory = []() -> std::unique_ptr<IComponent> {
            return std::make_unique<T>();
        };
        
        ComponentRegistration reg(name, version, description, factory);
        return RegisterComponent(reg);
    }
    
    /**
     * 创建组件实例
     * @param componentName 组件名称
     * @return 组件实例
     */
    std::unique_ptr<IComponent> CreateComponent(const std::string& componentName);
    
    /**
     * 获取组件实例
     * @param componentName 组件名称
     * @return 组件实例指针
     */
    IComponent* GetComponent(const std::string& componentName);
    
    /**
     * 启动组件
     * @param componentName 组件名称
     * @return 是否成功
     */
    bool StartComponent(const std::string& componentName);
    
    /**
     * 停止组件
     * @param componentName 组件名称
     * @return 是否成功
     */
    bool StopComponent(const std::string& componentName);
    
    /**
     * 启动所有组件
     * @return 启动成功的组件数量
     */
    size_t StartAllComponents();
    
    /**
     * 停止所有组件
     */
    void StopAllComponents();
    
    /**
     * 检查依赖关系
     * @param componentName 组件名称
     * @return 依赖是否满足
     */
    bool CheckDependencies(const std::string& componentName);
    
    /**
     * 解析依赖顺序
     * @return 按依赖顺序排列的组件名称列表
     */
    std::vector<std::string> ResolveDependencyOrder();
    
    /**
     * 获取已注册组件列表
     * @return 组件名称列表
     */
    std::vector<std::string> GetRegisteredComponents() const;
    
    /**
     * 获取活动组件列表
     * @return 组件名称列表
     */
    std::vector<std::string> GetActiveComponents() const;
    
    /**
     * 获取组件状态
     * @param componentName 组件名称
     * @return 组件状态
     */
    ComponentState GetComponentState(const std::string& componentName) const;
    
    /**
     * 检查组件是否存在
     * @param componentName 组件名称
     * @return 是否存在
     */
    bool ComponentExists(const std::string& componentName) const;
    
    /**
     * 卸载组件
     * @param componentName 组件名称
     * @return 是否成功
     */
    bool UnloadComponent(const std::string& componentName);
    
    /**
     * 重新加载组件
     * @param componentName 组件名称
     * @return 是否成功
     */
    bool ReloadComponent(const std::string& componentName);
    
    /**
     * 生成组件报告
     * @return 组件报告字符串
     */
    std::string GenerateComponentReport() const;
    
    /**
     * 获取单例实例
     * @return 组件管理器实例
     */
    static ComponentManager& GetInstance();

private:
    /**
     * 解析单个组件的依赖
     * @param componentName 组件名称
     * @param resolved 已解析的组件
     * @param resolving 正在解析的组件（用于循环依赖检测）
     * @param order 解析顺序
     * @return 是否成功
     */
    bool ResolveDependencies(const std::string& componentName,
                           std::unordered_set<std::string>& resolved,
                           std::unordered_set<std::string>& resolving,
                           std::vector<std::string>& order);
    
    /**
     * 设置组件状态
     * @param componentName 组件名称
     * @param state 新状态
     */
    void SetComponentState(const std::string& componentName, ComponentState state);
    
    /**
     * 验证组件注册信息
     * @param registration 注册信息
     * @return 是否有效
     */
    bool ValidateRegistration(const ComponentRegistration& registration) const;
};

/**
 * 组件注册宏
 * 用于简化组件注册过程
 */
#define REGISTER_COMPONENT(ComponentClass, Name, Version, Description) \
    static bool _registered_##ComponentClass = ComponentManager::GetInstance().RegisterComponent<ComponentClass>(Name, Version, Description)

/**
 * 组件基类
 * 提供组件接口的默认实现
 */
class ComponentBase : public IComponent {
protected:
    std::string m_ComponentName;
    std::string m_ComponentVersion;
    ComponentState m_State;
    std::unordered_map<std::string, std::string> m_Configuration;
    std::vector<std::string> m_Dependencies;

public:
    /**
     * 构造函数
     */
    ComponentBase(const std::string& name, const std::string& version);
    
    /**
     * 析构函数
     */
    virtual ~ComponentBase() = default;
    
    // IComponent接口实现
    std::string GetComponentName() const override { return m_ComponentName; }
    std::string GetComponentVersion() const override { return m_ComponentVersion; }
    ComponentState GetState() const override { return m_State; }
    std::vector<std::string> GetDependencies() const override { return m_Dependencies; }
    
    std::unordered_map<std::string, std::string> GetConfiguration() const override { 
        return m_Configuration; 
    }
    
    bool SetConfiguration(const std::unordered_map<std::string, std::string>& config) override {
        m_Configuration = config;
        return true;
    }
    
    bool IsHealthy() const override {
        return m_State == ComponentState::INITIALIZED || m_State == ComponentState::RUNNING;
    }

protected:
    /**
     * 设置状态
     * @param state 新状态
     */
    void SetState(ComponentState state) { m_State = state; }
    
    /**
     * 添加依赖
     * @param dependency 依赖组件名称
     */
    void AddDependency(const std::string& dependency) {
        m_Dependencies.push_back(dependency);
    }
    
    /**
     * 设置配置项
     * @param key 配置键
     * @param value 配置值
     */
    void SetConfigValue(const std::string& key, const std::string& value) {
        m_Configuration[key] = value;
    }
    
    /**
     * 获取配置项
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string GetConfigValue(const std::string& key, const std::string& defaultValue = "") const {
        auto it = m_Configuration.find(key);
        return (it != m_Configuration.end()) ? it->second : defaultValue;
    }
};

} // namespace Core
} // namespace CHTL