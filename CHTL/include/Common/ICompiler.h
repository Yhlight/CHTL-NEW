#ifndef CHTL_COMMON_ICOMPILER_H
#define CHTL_COMMON_ICOMPILER_H

#include <string>
#include <memory>
#include "Common/CodeFragment.h"

namespace CHTL {

/**
 * 编译器接口
 * 所有编译器都必须实现此接口
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /**
     * 编译代码片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    virtual std::string Compile(const CodeFragmentPtr& fragment) = 0;
    
    /**
     * 设置模块目录
     * @param dir 模块目录路径
     */
    virtual void SetModuleDirectory(const std::string& dir) = 0;
    
    /**
     * 设置调试模式
     * @param enable 是否启用调试模式
     */
    virtual void SetDebugMode(bool enable) = 0;
    
    /**
     * 获取编译器名称
     * @return 编译器名称
     */
    virtual std::string GetCompilerName() const = 0;
    
    /**
     * 重置编译器状态
     */
    virtual void Reset() = 0;
};

} // namespace CHTL

#endif // CHTL_COMMON_ICOMPILER_H