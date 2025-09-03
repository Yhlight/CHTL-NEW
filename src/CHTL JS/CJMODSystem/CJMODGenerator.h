#pragma once

#include "CJMODSyntax.h"
#include "CJMODScanner.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CJMOD {

/**
 * CJMOD生成结果
 */
struct CJMODGenerationResult {
    std::string GeneratedCode;                          // 生成的代码
    std::string ExtensionCode;                          // 扩展代码
    std::vector<std::string> CHTLJSExtensions;          // CHTL JS扩展列表
    bool IsSuccess;                                     // 是否成功
    std::string ErrorMessage;                           // 错误信息
    
    CJMODGenerationResult() : IsSuccess(false) {}
};

/**
 * CHTL JS函数信息
 */
struct CHTLJSFunctionInfo {
    std::string FunctionName;                           // 函数名称
    std::string FunctionType;                           // 函数类型（listen/delegate/animate）
    std::string FunctionBody;                           // 函数体
    std::vector<std::string> Parameters;                // 参数列表
    std::unordered_map<std::string, std::string> Metadata; // 元数据
    
    CHTLJSFunctionInfo() = default;
};

/**
 * CJMOD生成器接口
 * 提供CJMOD代码生成和CHTL JS扩展功能
 */
class CJMODGenerator {
private:
    std::string m_SourceCode;                           // 源代码
    std::vector<CHTLJSFunctionInfo> m_CHTLJSFunctions;  // CHTL JS函数列表
    std::unordered_map<std::string, std::string> m_ExtensionMappings; // 扩展映射

public:
    /**
     * 构造函数
     * @param sourceCode 源代码
     */
    explicit CJMODGenerator(const std::string& sourceCode);
    
    /**
     * 析构函数
     */
    virtual ~CJMODGenerator() = default;
    
    /**
     * 静态方法 - exportResult() 导出最终JS代码
     * @param args Arg对象 (包含处理后的参数)
     * @return 最终的JavaScript代码
     */
    static std::string exportResult(const Arg& args);
    
    /**
     * 实例方法 - 导出CJMOD结果
     * @param scanResult 扫描结果
     * @return 生成结果
     */
    virtual CJMODGenerationResult exportResult(const CJMODScanResult& scanResult);
    
    /**
     * 生成CHTL JS扩展代码
     * @param extensionName 扩展名称
     * @param extensionCode 扩展代码
     * @return 生成的CHTL JS代码
     */
    std::string generateCHTLJSExtension(const std::string& extensionName, const std::string& extensionCode);
    
    /**
     * 创建CHTL JS函数
     * @param functionType 函数类型
     * @param functionName 函数名称
     * @param functionBody 函数体
     * @return CHTL JS函数信息
     */
    CHTLJSFunctionInfo createCHTLJSFunction(const std::string& functionType, 
                                           const std::string& functionName,
                                           const std::string& functionBody);
    
    /**
     * 绑定虚对象
     * @param virName 虚对象名称
     * @param functionInfo CHTL JS函数信息
     * @return 绑定后的代码
     */
    std::string bindVirtualObject(const std::string& virName, const CHTLJSFunctionInfo& functionInfo);
    
    /**
     * 生成扩展语法定义
     * @param syntaxName 语法名称
     * @param syntaxPattern 语法模式
     * @param replacement 替换模式
     * @return 扩展定义代码
     */
    std::string generateExtensionSyntaxDefinition(const std::string& syntaxName,
                                                  const std::string& syntaxPattern,
                                                  const std::string& replacement);
    
    /**
     * 处理占位符绑定
     * @param template_str 模板字符串
     * @param args 参数映射
     * @return 绑定后的字符串
     */
    std::string processPlaceholderBinding(const std::string& template_str,
                                         const std::unordered_map<std::string, Arg>& args);
    
    /**
     * 导出JavaScript代码
     * @return JavaScript代码
     */
    std::string exportJavaScript();
    
    /**
     * 导出CHTL JS代码
     * @return CHTL JS代码
     */
    std::string exportCHTLJS();
    
    /**
     * 添加扩展映射
     * @param pattern 模式
     * @param replacement 替换
     */
    void addExtensionMapping(const std::string& pattern, const std::string& replacement);
    
    /**
     * 获取CHTL JS函数列表
     * @return 函数列表
     */
    const std::vector<CHTLJSFunctionInfo>& getCHTLJSFunctions() const { return m_CHTLJSFunctions; }
    
    /**
     * 重置生成器
     */
    virtual void reset();

protected:
    /**
     * 生成函数包装器
     * @param functionInfo 函数信息
     * @return 包装器代码
     */
    virtual std::string generateFunctionWrapper(const CHTLJSFunctionInfo& functionInfo);
    
    /**
     * 生成扩展注册代码
     * @return 注册代码
     */
    virtual std::string generateExtensionRegistration();
    
    /**
     * 处理语法转换
     * @param originalSyntax 原始语法
     * @return 转换后的语法
     */
    virtual std::string processSyntaxTransformation(const std::string& originalSyntax);

private:
    /**
     * 初始化默认扩展映射
     */
    void initializeDefaultExtensionMappings();
    
    /**
     * 解析CHTL JS函数体
     * @param functionBody 函数体
     * @return 解析后的函数信息
     */
    CHTLJSFunctionInfo parseCHTLJSFunctionBody(const std::string& functionBody);
    
    /**
     * 生成虚对象绑定代码
     * @param virName 虚对象名称
     * @param functionInfo 函数信息
     * @return 绑定代码
     */
    std::string generateVirtualObjectBinding(const std::string& virName, const CHTLJSFunctionInfo& functionInfo);
};

/**
 * CJMOD函数创建器
 * 专门用于创建和管理CHTL JS函数
 */
class CHTLJSFunction {
public:
    /**
     * 创建CHTL JS函数
     * @param functionType 函数类型（listen/delegate/animate）
     * @param functionName 函数名称
     * @param functionBody 函数体
     * @return CHTL JS函数信息
     */
    static CHTLJSFunctionInfo CreateCHTLJSFunction(const std::string& functionType,
                                                   const std::string& functionName,
                                                   const std::string& functionBody);
    
    /**
     * 绑定虚对象到CHTL JS函数
     * @param virName 虚对象名称
     * @param functionInfo 函数信息
     * @return 绑定代码
     */
    static std::string bindVirtualObject(const std::string& virName, const CHTLJSFunctionInfo& functionInfo);
    
    /**
     * 生成CHTL JS函数包装器
     * @param functionInfo 函数信息
     * @return 包装器代码
     */
    static std::string generateFunctionWrapper(const CHTLJSFunctionInfo& functionInfo);
    
    /**
     * 验证CHTL JS函数语法
     * @param functionCode 函数代码
     * @return 是否有效
     */
    static bool validateCHTLJSFunction(const std::string& functionCode);
};

} // namespace CJMOD