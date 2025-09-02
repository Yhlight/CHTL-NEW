#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CHTL {
namespace Core {

/**
 * 组件状态枚举
 */
enum class ComponentState {
    UNINITIALIZED,  // 未初始化
    INITIALIZING,   // 初始化中
    INITIALIZED,    // 已初始化
    RUNNING,        // 运行中
    STOPPED,        // 已停止
    ERROR_STATE     // 错误状态
};

/**
 * 组件接口基类
 * 所有CHTL组件都应该继承此接口
 */
class IComponent {
public:
    /**
     * 虚析构函数
     */
    virtual ~IComponent() = default;
    
    /**
     * 初始化组件
     * @return 是否成功
     */
    virtual bool Initialize() = 0;
    
    /**
     * 启动组件
     * @return 是否成功
     */
    virtual bool Start() = 0;
    
    /**
     * 停止组件
     * @return 是否成功
     */
    virtual bool Stop() = 0;
    
    /**
     * 重置组件
     */
    virtual void Reset() = 0;
    
    /**
     * 获取组件名称
     * @return 组件名称
     */
    virtual std::string GetComponentName() const = 0;
    
    /**
     * 获取组件版本
     * @return 组件版本
     */
    virtual std::string GetComponentVersion() const = 0;
    
    /**
     * 获取组件状态
     * @return 组件状态
     */
    virtual ComponentState GetState() const = 0;
    
    /**
     * 获取组件依赖
     * @return 依赖组件名称列表
     */
    virtual std::vector<std::string> GetDependencies() const = 0;
    
    /**
     * 检查组件健康状态
     * @return 是否健康
     */
    virtual bool IsHealthy() const = 0;
    
    /**
     * 获取组件配置
     * @return 配置映射
     */
    virtual std::unordered_map<std::string, std::string> GetConfiguration() const = 0;
    
    /**
     * 设置组件配置
     * @param config 配置映射
     * @return 是否成功
     */
    virtual bool SetConfiguration(const std::unordered_map<std::string, std::string>& config) = 0;
};

/**
 * 编译器组件接口
 * 专门用于编译器类组件
 */
class ICompilerComponent : public IComponent {
public:
    /**
     * 编译接口
     * @param input 输入内容
     * @return 编译结果
     */
    virtual std::string Compile(const std::string& input) = 0;
    
    /**
     * 验证输入
     * @param input 输入内容
     * @return 是否有效
     */
    virtual bool ValidateInput(const std::string& input) = 0;
    
    /**
     * 获取支持的文件扩展名
     * @return 扩展名列表
     */
    virtual std::vector<std::string> GetSupportedExtensions() const = 0;
    
    /**
     * 获取编译统计
     * @return 统计信息
     */
    virtual std::unordered_map<std::string, size_t> GetCompilationStats() const = 0;
};

/**
 * 扫描器组件接口
 */
class IScannerComponent : public IComponent {
public:
    /**
     * 扫描接口
     * @param source 源代码
     * @return 是否成功
     */
    virtual bool Scan(const std::string& source) = 0;
    
    /**
     * 获取扫描结果
     * @return 扫描结果
     */
    virtual std::vector<std::string> GetScanResults() const = 0;
    
    /**
     * 获取扫描统计
     * @return 统计信息
     */
    virtual std::unordered_map<std::string, size_t> GetScanStats() const = 0;
};

/**
 * 解析器组件接口
 */
class IParserComponent : public IComponent {
public:
    /**
     * 解析接口
     * @param tokens Token列表
     * @return 解析结果
     */
    virtual std::string Parse(const std::vector<std::string>& tokens) = 0;
    
    /**
     * 验证语法
     * @param tokens Token列表
     * @return 是否有效
     */
    virtual bool ValidateSyntax(const std::vector<std::string>& tokens) = 0;
    
    /**
     * 获取解析统计
     * @return 统计信息
     */
    virtual std::unordered_map<std::string, size_t> GetParseStats() const = 0;
};

/**
 * 生成器组件接口
 */
class IGeneratorComponent : public IComponent {
public:
    /**
     * 生成代码
     * @param ast AST树
     * @return 生成的代码
     */
    virtual std::string Generate(const std::string& ast) = 0;
    
    /**
     * 优化代码
     * @param code 原始代码
     * @return 优化后的代码
     */
    virtual std::string Optimize(const std::string& code) = 0;
    
    /**
     * 获取生成统计
     * @return 统计信息
     */
    virtual std::unordered_map<std::string, size_t> GetGenerateStats() const = 0;
};

} // namespace Core
} // namespace CHTL