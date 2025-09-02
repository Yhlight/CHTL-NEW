#pragma once

#include "ComponentInterface.h"
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <mutex>

namespace CHTL {
namespace Core {

/**
 * 服务定位器
 * 提供组件的依赖注入和服务定位功能
 */
class ServiceLocator {
private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponent>> m_Services;
    std::unordered_map<std::string, std::type_index> m_ServiceNames;
    mutable std::mutex m_Mutex;
    static std::unique_ptr<ServiceLocator> s_Instance;

public:
    /**
     * 构造函数
     */
    ServiceLocator() = default;
    
    /**
     * 析构函数
     */
    ~ServiceLocator() = default;
    
    /**
     * 禁用拷贝构造和赋值
     */
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
    
    /**
     * 注册服务
     * @param service 服务实例
     * @param name 服务名称（可选）
     */
    template<typename T>
    void RegisterService(std::unique_ptr<T> service, const std::string& name = "") {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        std::type_index typeIndex(typeid(T));
        m_Services[typeIndex] = std::move(service);
        
        if (!name.empty()) {
            m_ServiceNames[name] = typeIndex;
        }
        
        std::cout << "📦 服务已注册: " << typeid(T).name() << std::endl;
    }
    
    /**
     * 获取服务
     * @return 服务指针
     */
    template<typename T>
    T* GetService() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        std::type_index typeIndex(typeid(T));
        auto it = m_Services.find(typeIndex);
        
        if (it != m_Services.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        
        return nullptr;
    }
    
    /**
     * 根据名称获取服务
     * @param name 服务名称
     * @return 服务指针
     */
    IComponent* GetService(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        auto nameIt = m_ServiceNames.find(name);
        if (nameIt != m_ServiceNames.end()) {
            auto serviceIt = m_Services.find(nameIt->second);
            if (serviceIt != m_Services.end()) {
                return serviceIt->second.get();
            }
        }
        
        return nullptr;
    }
    
    /**
     * 检查服务是否存在
     * @return 是否存在
     */
    template<typename T>
    bool HasService() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        std::type_index typeIndex(typeid(T));
        return m_Services.find(typeIndex) != m_Services.end();
    }
    
    /**
     * 检查服务是否存在（根据名称）
     * @param name 服务名称
     * @return 是否存在
     */
    bool HasService(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_ServiceNames.find(name) != m_ServiceNames.end();
    }
    
    /**
     * 移除服务
     */
    template<typename T>
    bool RemoveService() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        std::type_index typeIndex(typeid(T));
        auto it = m_Services.find(typeIndex);
        
        if (it != m_Services.end()) {
            // 停止服务
            if (it->second) {
                it->second->Stop();
            }
            
            m_Services.erase(it);
            
            // 移除名称映射
            for (auto nameIt = m_ServiceNames.begin(); nameIt != m_ServiceNames.end(); ) {
                if (nameIt->second == typeIndex) {
                    nameIt = m_ServiceNames.erase(nameIt);
                } else {
                    ++nameIt;
                }
            }
            
            std::cout << "📤 服务已移除: " << typeid(T).name() << std::endl;
            return true;
        }
        
        return false;
    }
    
    /**
     * 清除所有服务
     */
    void Clear() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        // 停止所有服务
        for (auto& pair : m_Services) {
            if (pair.second) {
                pair.second->Stop();
            }
        }
        
        m_Services.clear();
        m_ServiceNames.clear();
        
        std::cout << "🧹 所有服务已清除" << std::endl;
    }
    
    /**
     * 获取已注册服务数量
     * @return 服务数量
     */
    size_t GetServiceCount() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Services.size();
    }
    
    /**
     * 生成服务报告
     * @return 服务报告字符串
     */
    std::string GenerateServiceReport() const;
    
    /**
     * 获取单例实例
     * @return 服务定位器实例
     */
    static ServiceLocator& GetInstance() {
        static ServiceLocator instance;
        return instance;
    }
};

/**
 * 服务注册宏
 */
#define REGISTER_SERVICE(ServiceClass, ServiceName) \
    ServiceLocator::GetInstance().RegisterService<ServiceClass>(std::make_unique<ServiceClass>(), ServiceName)

/**
 * 获取服务宏
 */
#define GET_SERVICE(ServiceClass) \
    ServiceLocator::GetInstance().GetService<ServiceClass>()

/**
 * 依赖注入辅助类
 */
template<typename T>
class ServiceProvider {
private:
    static T* s_CachedService;

public:
    /**
     * 获取服务实例
     * @return 服务指针
     */
    static T* Get() {
        if (!s_CachedService) {
            s_CachedService = ServiceLocator::GetInstance().GetService<T>();
        }
        return s_CachedService;
    }
    
    /**
     * 刷新缓存
     */
    static void Refresh() {
        s_CachedService = nullptr;
    }
    
    /**
     * 检查服务是否可用
     * @return 是否可用
     */
    static bool IsAvailable() {
        return Get() != nullptr;
    }
};

template<typename T>
T* ServiceProvider<T>::s_CachedService = nullptr;

} // namespace Core
} // namespace CHTL