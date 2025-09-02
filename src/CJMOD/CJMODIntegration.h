#pragma once

#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "CJMODSyntax.h"
#include "CJMODScanner.h"
#include "CJMODGenerator.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CJMOD {

// 前向声明
class CJMODExtension;

/**
 * CJMOD编译器集成接口
 * 负责CJMOD与CHTL JS编译器的深度集成
 */
class CJMODCompilerIntegration {
public:
    /**
     * 代码片段处理回调函数类型
     * @param fragment 原始代码片段
     * @return 处理后的代码片段
     */
    using FragmentProcessor = std::function<std::string(const std::string& fragment)>;
    
    /**
     * 扫描器集成回调函数类型
     * @param scanner 统一扫描器引用
     * @param fragmentIndex 片段索引
     * @return 是否需要CJMOD处理
     */
    using ScannerCallback = std::function<bool(CHTL::CHTLUnifiedScanner& scanner, size_t fragmentIndex)>;

private:
    // 注册的CJMOD扩展
    std::unordered_map<std::string, std::unique_ptr<CJMODExtension>> m_Extensions;
    
    // 编译器引用
    CHTL::CHTLUnifiedScanner* m_Scanner;
    CHTLJS::CHTLJSLexer* m_CHTLJSLexer;
    CHTLJS::CHTLJSParser* m_CHTLJSParser;
    
    // 处理状态
    bool m_IsProcessing;
    std::vector<std::string> m_ProcessedFragments;

public:
    /**
     * 构造函数
     */
    CJMODCompilerIntegration();
    
    /**
     * 析构函数
     */
    ~CJMODCompilerIntegration() = default;
    
    /**
     * 设置编译器引用
     * @param scanner 统一扫描器
     * @param lexer CHTL JS词法分析器
     * @param parser CHTL JS解析器
     */
    void SetCompilerReferences(CHTL::CHTLUnifiedScanner* scanner,
                              CHTLJS::CHTLJSLexer* lexer,
                              CHTLJS::CHTLJSParser* parser);
    
    /**
     * 注册CJMOD扩展
     * @param extensionName 扩展名称
     * @param extension CJMOD扩展
     */
    void RegisterExtension(const std::string& extensionName, std::unique_ptr<CJMODExtension> extension);
    
    /**
     * 处理代码片段 (核心方法)
     * @param fragment 原始代码片段
     * @param fragmentType 片段类型
     * @return 处理后的代码片段
     */
    std::string ProcessCodeFragment(const std::string& fragment, const std::string& fragmentType);
    
    /**
     * 检查片段是否需要CJMOD处理
     * @param fragment 代码片段
     * @return 是否需要处理
     */
    bool RequiresCJMODProcessing(const std::string& fragment);
    
    /**
     * 获取真实代码片段 (从扫描器)
     * @param fragmentIndex 片段索引
     * @return 真实代码片段
     */
    std::string GetRealCodeFragment(size_t fragmentIndex);
    
    /**
     * 返回处理后的代码片段 (到扫描器)
     * @param fragmentIndex 片段索引
     * @param processedFragment 处理后的片段
     * @return 是否成功
     */
    bool ReturnProcessedFragment(size_t fragmentIndex, const std::string& processedFragment);
    
    /**
     * 扫描器集成钩子
     * @param callback 回调函数
     */
    void SetScannerHook(ScannerCallback callback);
    
    /**
     * 处理CHTL JS语法扩展
     * @param chtljsCode CHTL JS代码
     * @return 扩展后的代码
     */
    std::string ProcessCHTLJSExtensions(const std::string& chtljsCode);
    
    /**
     * 重置集成状态
     */
    void Reset();

private:
    /**
     * 分析代码片段中的CJMOD语法
     * @param fragment 代码片段
     * @return CJMOD语法列表
     */
    std::vector<std::string> AnalyzeCJMODSyntax(const std::string& fragment);
    
    /**
     * 应用CJMOD扩展
     * @param fragment 原始片段
     * @param extensions 扩展列表
     * @return 处理后的片段
     */
    std::string ApplyCJMODExtensions(const std::string& fragment, const std::vector<std::string>& extensions);
};

/**
 * CJMOD扩展基类
 * 所有CJMOD扩展都必须继承此类
 */
class CJMODExtension {
protected:
    std::string m_ExtensionName;
    std::string m_Version;
    std::vector<std::string> m_SupportedSyntax;

public:
    /**
     * 构造函数
     * @param name 扩展名称
     * @param version 版本
     */
    CJMODExtension(const std::string& name, const std::string& version)
        : m_ExtensionName(name), m_Version(version) {}
    
    /**
     * 虚析构函数
     */
    virtual ~CJMODExtension() = default;
    
    /**
     * 处理代码片段 (纯虚函数)
     * @param fragment 原始代码片段
     * @param context 上下文信息
     * @return 处理后的代码片段
     */
    virtual std::string ProcessFragment(const std::string& fragment, 
                                       const std::unordered_map<std::string, std::string>& context) = 0;
    
    /**
     * 检查是否支持该语法
     * @param syntax 语法字符串
     * @return 是否支持
     */
    virtual bool SupportsSyntax(const std::string& syntax) = 0;
    
    /**
     * 获取扩展信息
     * @return 扩展信息
     */
    virtual std::unordered_map<std::string, std::string> GetExtensionInfo() = 0;
    
    /**
     * 初始化扩展
     * @return 是否成功
     */
    virtual bool Initialize() = 0;
    
    /**
     * 清理扩展
     */
    virtual void Cleanup() = 0;
    
    // Getter方法
    const std::string& GetName() const { return m_ExtensionName; }
    const std::string& GetVersion() const { return m_Version; }
    const std::vector<std::string>& GetSupportedSyntax() const { return m_SupportedSyntax; }
};

/**
 * 珂朵莉CJMOD扩展
 * 实现printMylove、iNeverAway等特殊功能
 */
class ChthollyExtension : public CJMODExtension {
public:
    /**
     * 构造函数
     */
    ChthollyExtension() : CJMODExtension("Chtholly", "1.0.0") {
        m_SupportedSyntax = {"printMylove", "iNeverAway", "util...then"};
    }
    
    /**
     * 处理代码片段
     */
    std::string ProcessFragment(const std::string& fragment, 
                               const std::unordered_map<std::string, std::string>& context) override;
    
    /**
     * 检查语法支持
     */
    bool SupportsSyntax(const std::string& syntax) override;
    
    /**
     * 获取扩展信息
     */
    std::unordered_map<std::string, std::string> GetExtensionInfo() override;
    
    /**
     * 初始化扩展
     */
    bool Initialize() override;
    
    /**
     * 清理扩展
     */
    void Cleanup() override;

private:
    /**
     * 处理printMylove语法
     * @param fragment 代码片段
     * @return 处理后的代码
     */
    std::string ProcessPrintMylove(const std::string& fragment);
    
    /**
     * 处理iNeverAway语法
     * @param fragment 代码片段
     * @return 处理后的代码
     */
    std::string ProcessINeverAway(const std::string& fragment);
    
    /**
     * 处理util...then语法
     * @param fragment 代码片段
     * @return 处理后的代码
     */
    std::string ProcessUtilThen(const std::string& fragment);
};

/**
 * CJMOD管理器
 * 负责CJMOD扩展的生命周期管理和编译器集成
 */
class CJMODManager {
private:
    std::unique_ptr<CJMODCompilerIntegration> m_Integration;
    std::vector<std::unique_ptr<CJMODExtension>> m_LoadedExtensions;
    bool m_IsInitialized;

public:
    /**
     * 构造函数
     */
    CJMODManager();
    
    /**
     * 析构函数
     */
    ~CJMODManager() = default;
    
    /**
     * 初始化CJMOD管理器
     * @param scanner 统一扫描器
     * @param lexer CHTL JS词法分析器
     * @param parser CHTL JS解析器
     * @return 是否成功
     */
    bool Initialize(CHTL::CHTLUnifiedScanner* scanner,
                   CHTLJS::CHTLJSLexer* lexer,
                   CHTLJS::CHTLJSParser* parser);
    
    /**
     * 加载CJMOD扩展
     * @param extensionPath 扩展路径
     * @return 是否成功
     */
    bool LoadExtension(const std::string& extensionPath);
    
    /**
     * 注册内置扩展
     */
    void RegisterBuiltinExtensions();
    
    /**
     * 处理代码片段 (编译器调用接口)
     * @param fragment 原始代码片段
     * @param fragmentIndex 片段索引
     * @return 处理后的代码片段
     */
    std::string ProcessCodeFragment(const std::string& fragment, size_t fragmentIndex);
    
    /**
     * 获取集成接口
     * @return CJMOD编译器集成接口
     */
    CJMODCompilerIntegration* GetIntegration() { return m_Integration.get(); }
    
    /**
     * 检查是否已初始化
     * @return 是否已初始化
     */
    bool IsInitialized() const { return m_IsInitialized; }
    
    /**
     * 获取已加载的扩展数量
     * @return 扩展数量
     */
    size_t GetLoadedExtensionCount() const { return m_LoadedExtensions.size(); }
    
    /**
     * 重置管理器
     */
    void Reset();
};

} // namespace CJMOD