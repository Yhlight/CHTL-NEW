#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * CMOD模块信息
 */
struct CMODModuleInfo {
    std::string Name;           // 模块名称
    std::string Version;        // 版本
    std::string Description;    // 描述
    std::string Author;         // 作者
    std::string License;        // 许可证
    std::vector<std::string> Dependencies; // 依赖模块
    std::unordered_map<std::string, std::string> Metadata; // 元数据
};

/**
 * CMOD导出项
 */
struct CMODExportItem {
    std::string Name;           // 导出名称
    std::string Type;           // 类型 (Template, Custom, Style, Script等)
    std::string Path;           // 文件路径
    std::string FullQualifiedName; // 完全限定名
    bool IsPublic;              // 是否公开
};

/**
 * CMOD模块
 */
class CMODModule {
private:
    CMODModuleInfo m_Info;
    std::string m_ModulePath;
    std::string m_SrcPath;
    std::string m_InfoPath;
    std::vector<CMODExportItem> m_Exports;
    std::vector<std::unique_ptr<CMODModule>> m_Submodules;
    bool m_IsLoaded;

public:
    /**
     * 构造函数
     * @param modulePath 模块路径
     */
    explicit CMODModule(const std::string& modulePath);
    
    /**
     * 析构函数
     */
    ~CMODModule() = default;
    
    /**
     * 加载模块
     * @return 是否成功
     */
    bool Load();
    
    /**
     * 卸载模块
     */
    void Unload();
    
    /**
     * 获取模块信息
     * @return 模块信息
     */
    const CMODModuleInfo& GetInfo() const { return m_Info; }
    
    /**
     * 获取导出项
     * @return 导出项列表
     */
    const std::vector<CMODExportItem>& GetExports() const { return m_Exports; }
    
    /**
     * 查找导出项
     * @param name 导出项名称
     * @return 导出项指针，未找到返回nullptr
     */
    const CMODExportItem* FindExport(const std::string& name) const;
    
    /**
     * 获取子模块
     * @return 子模块列表
     */
    const std::vector<std::unique_ptr<CMODModule>>& GetSubmodules() const { return m_Submodules; }
    
    /**
     * 查找子模块
     * @param name 子模块名称
     * @return 子模块指针，未找到返回nullptr
     */
    CMODModule* FindSubmodule(const std::string& name) const;
    
    /**
     * 检查是否已加载
     * @return 是否已加载
     */
    bool IsLoaded() const { return m_IsLoaded; }
    
    /**
     * 获取模块路径
     * @return 模块路径
     */
    const std::string& GetModulePath() const { return m_ModulePath; }

private:
    /**
     * 解析模块信息文件
     * @return 是否成功
     */
    bool ParseModuleInfo();
    
    /**
     * 解析导出表
     * @return 是否成功
     */
    bool ParseExportTable();
    
    /**
     * 加载子模块
     * @return 是否成功
     */
    bool LoadSubmodules();
    
    /**
     * 验证模块结构
     * @return 是否有效
     */
    bool ValidateModuleStructure();
};

/**
 * CMOD管理器
 * 负责CMOD模块的加载、管理和查找
 */
class CMODManager {
private:
    std::unordered_map<std::string, std::unique_ptr<CMODModule>> m_LoadedModules;
    std::vector<std::string> m_ModuleSearchPaths;
    std::unordered_map<std::string, std::string> m_ModuleAliases;
    bool m_IsInitialized;

public:
    /**
     * 构造函数
     */
    CMODManager();
    
    /**
     * 析构函数
     */
    ~CMODManager() = default;
    
    /**
     * 初始化管理器
     * @return 是否成功
     */
    bool Initialize();
    
    /**
     * 添加模块搜索路径
     * @param path 搜索路径
     */
    void AddSearchPath(const std::string& path);
    
    /**
     * 加载模块
     * @param moduleName 模块名称
     * @param modulePath 模块路径（可选）
     * @return 是否成功
     */
    bool LoadModule(const std::string& moduleName, const std::string& modulePath = "");
    
    /**
     * 卸载模块
     * @param moduleName 模块名称
     * @return 是否成功
     */
    bool UnloadModule(const std::string& moduleName);
    
    /**
     * 查找模块
     * @param moduleName 模块名称
     * @return 模块指针，未找到返回nullptr
     */
    CMODModule* FindModule(const std::string& moduleName) const;
    
    /**
     * 解析模块路径
     * @param moduleName 模块名称
     * @return 模块路径，未找到返回空字符串
     */
    std::string ResolveModulePath(const std::string& moduleName) const;
    
    /**
     * 获取已加载的模块列表
     * @return 模块名称列表
     */
    std::vector<std::string> GetLoadedModules() const;
    
    /**
     * 检查模块是否已加载
     * @param moduleName 模块名称
     * @return 是否已加载
     */
    bool IsModuleLoaded(const std::string& moduleName) const;
    
    /**
     * 设置模块别名
     * @param alias 别名
     * @param moduleName 模块名称
     */
    void SetModuleAlias(const std::string& alias, const std::string& moduleName);
    
    /**
     * 获取模块别名
     * @param alias 别名
     * @return 模块名称，未找到返回空字符串
     */
    std::string GetModuleByAlias(const std::string& alias) const;
    
    /**
     * 清理所有模块
     */
    void Clear();
    
    /**
     * 生成模块报告
     * @return 模块报告字符串
     */
    std::string GenerateModuleReport() const;

private:
    /**
     * 扫描搜索路径中的模块
     * @return 找到的模块路径列表
     */
    std::vector<std::string> ScanModulesInSearchPaths() const;
    
    /**
     * 验证模块路径
     * @param path 模块路径
     * @return 是否有效
     */
    bool ValidateModulePath(const std::string& path) const;
};

} // namespace CHTL