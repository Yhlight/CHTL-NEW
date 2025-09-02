#pragma once

#include "../CHTL/CMODSystem/CMODManager.h"
#include "../Util/FileSystem/FileSystem.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace Module {

/**
 * 官方模块加载器
 * 负责加载和管理src/Module目录下的官方模块源码
 */
class OfficialModuleLoader {
private:
    std::unique_ptr<CMODManager> m_CMODManager;
    std::string m_ModuleSourcePath;
    std::vector<std::string> m_LoadedOfficialModules;
    bool m_IsInitialized;

public:
    /**
     * 构造函数
     */
    OfficialModuleLoader();
    
    /**
     * 析构函数
     */
    ~OfficialModuleLoader() = default;
    
    /**
     * 初始化模块加载器
     * @param moduleSourcePath 模块源码路径，默认为src/Module
     * @return 是否成功
     */
    bool Initialize(const std::string& moduleSourcePath = "src/Module");
    
    /**
     * 扫描并加载所有官方模块
     * @return 加载的模块数量
     */
    size_t LoadAllOfficialModules();
    
    /**
     * 加载指定的官方模块
     * @param moduleName 模块名称
     * @return 是否成功
     */
    bool LoadOfficialModule(const std::string& moduleName);
    
    /**
     * 检查模块是否为官方模块
     * @param moduleName 模块名称
     * @return 是否为官方模块
     */
    bool IsOfficialModule(const std::string& moduleName) const;
    
    /**
     * 获取官方模块列表
     * @return 官方模块名称列表
     */
    std::vector<std::string> GetOfficialModuleList() const;
    
    /**
     * 获取模块信息
     * @param moduleName 模块名称
     * @return 模块信息，未找到返回nullptr
     */
    const CMODModuleInfo* GetModuleInfo(const std::string& moduleName) const;
    
    /**
     * 验证官方模块结构
     * @param modulePath 模块路径
     * @return 验证结果
     */
    bool ValidateOfficialModuleStructure(const std::string& modulePath) const;
    
    /**
     * 生成官方模块报告
     * @return 报告字符串
     */
    std::string GenerateOfficialModuleReport() const;
    
    /**
     * 获取CMOD管理器
     * @return CMOD管理器指针
     */
    CMODManager* GetCMODManager() const { return m_CMODManager.get(); }

private:
    /**
     * 扫描模块源码目录
     * @return 找到的模块路径列表
     */
    std::vector<std::string> ScanModuleSourceDirectory() const;
    
    /**
     * 解析模块类型
     * @param modulePath 模块路径
     * @return 模块类型 (CMOD, CJMOD, CMOD+CJMOD)
     */
    std::string ParseModuleType(const std::string& modulePath) const;
};

/**
 * 模块构建器
 * 用于将src/Module源码构建为可发布的模块
 */
class ModuleBuilder {
public:
    /**
     * 构建单个模块
     * @param sourceModulePath 源模块路径
     * @param outputPath 输出路径
     * @return 是否成功
     */
    static bool BuildModule(const std::string& sourceModulePath, const std::string& outputPath);
    
    /**
     * 构建所有官方模块
     * @param sourceDir 源目录
     * @param outputDir 输出目录
     * @return 构建的模块数量
     */
    static size_t BuildAllOfficialModules(const std::string& sourceDir = "src/Module", 
                                         const std::string& outputDir = "module");
    
    /**
     * 验证模块构建结果
     * @param builtModulePath 构建的模块路径
     * @return 验证结果
     */
    static bool ValidateBuiltModule(const std::string& builtModulePath);
    
    /**
     * 生成模块构建报告
     * @param sourceDir 源目录
     * @param outputDir 输出目录
     * @return 构建报告
     */
    static std::string GenerateBuildReport(const std::string& sourceDir, const std::string& outputDir);

private:
    /**
     * 复制模块文件
     * @param sourcePath 源路径
     * @param destPath 目标路径
     * @return 是否成功
     */
    static bool CopyModuleFiles(const std::string& sourcePath, const std::string& destPath);
    
    /**
     * 处理CJMOD文件
     * @param cjmodPath CJMOD文件路径
     * @param outputPath 输出路径
     * @return 是否成功
     */
    static bool ProcessCJMODFiles(const std::string& cjmodPath, const std::string& outputPath);
};

} // namespace Module
} // namespace CHTL