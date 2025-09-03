#pragma once

#include "CJMODCore.h"
#include <string>
#include <unordered_map>

namespace CJMOD {

/**
 * CHTL JS函数类
 * 用于CJMOD API的接口
 * 严格按照CJMOD.md实现
 */
class CHTLJSFunction {
private:
    std::string m_FunctionName;                 // 函数名
    std::string m_Pattern;                      // 函数模式
    Arg m_Args;                                // 参数对象
    bool m_SupportsVirtualObject;              // 是否支持虚对象
    
    static std::unordered_map<std::string, bool> s_VirtualObjectBindings; // 虚对象绑定映射

public:
    /**
     * 默认构造函数
     */
    CHTLJSFunction() = default;
    
    /**
     * 构造函数
     * @param functionName 函数名
     * @param pattern 函数模式
     */
    CHTLJSFunction(const std::string& functionName, const std::string& pattern);
    
    /**
     * 创建CHTL JS函数
     * 封装了原始API构建语法的流程，能够快速构建CHTL JS函数
     * 这些CHTL JS函数天然支持虚对象vir以及无修饰字符串
     * @param pattern 函数模式，如 "printMyLove {url: $!_, mode: $?_}"
     * @return 创建的CHTL JS函数
     */
    static CHTLJSFunction CreateCHTLJSFunction(const std::string& pattern);
    
    /**
     * 绑定虚对象vir
     * 对于不使用CreateCHTLJSFunction创建的，但是符合CHTL JS函数的语法
     * 可以使用bindVirtualObject手动绑定虚对象vir，获得虚对象的支持
     * @param functionName 函数名称
     */
    static void bindVirtualObject(const std::string& functionName);
    
    /**
     * 检查函数是否支持虚对象
     * @param functionName 函数名
     * @return 是否支持虚对象
     */
    static bool supportsVirtualObject(const std::string& functionName);
    
    /**
     * 获取函数名
     * @return 函数名
     */
    const std::string& getFunctionName() const { return m_FunctionName; }
    
    /**
     * 获取函数模式
     * @return 函数模式
     */
    const std::string& getPattern() const { return m_Pattern; }
    
    /**
     * 获取参数对象
     * @return 参数对象
     */
    const Arg& getArgs() const { return m_Args; }
    
    /**
     * 是否支持虚对象
     * @return 是否支持虚对象
     */
    bool supportsVir() const { return m_SupportsVirtualObject; }

private:
    /**
     * 解析函数模式
     * @param pattern 模式字符串
     */
    void parsePattern(const std::string& pattern);
    
    /**
     * 提取函数名
     * @param pattern 模式字符串
     * @return 函数名
     */
    static std::string extractFunctionName(const std::string& pattern);
    
    /**
     * 提取参数模式
     * @param pattern 模式字符串
     * @return 参数模式
     */
    static std::string extractArgsPattern(const std::string& pattern);
};

} // namespace CJMOD