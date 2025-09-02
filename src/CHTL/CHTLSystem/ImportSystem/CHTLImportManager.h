#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

/**
 * 导入类型枚举
 * 定义不同类型的导入
 */
enum class ImportType {
    CHTL_FILE,          // .chtl文件
    HTML_FILE,          // .html文件
    CSS_FILE,           // .css文件
    JS_FILE,            // .js文件
    CJJS_FILE,          // .cjjs文件（CHTL JS）
    CMOD_MODULE,        // CMOD模块
    CJMOD_MODULE,       // CJMOD模块
    OFFICIAL_MODULE     // 官方模块（chtl::前缀）
};

/**
 * 导入项结构
 * 表示单个导入项的信息
 */
struct ImportItem {
    ImportType Type;                            // 导入类型
    std::string OriginalPath;                   // 原始路径
    std::string ResolvedPath;                   // 解析后的完整路径
    std::string AsName;                         // as别名
    std::string Namespace;                      // 命名空间
    bool IsWildcard;                            // 是否为通配符导入
    std::vector<std::string> WildcardMatches;   // 通配符匹配的文件
    bool IsResolved;                            // 是否已解析
    
    ImportItem() : Type(ImportType::CHTL_FILE), IsWildcard(false), IsResolved(false) {}
};

/**
 * 模块搜索路径配置
 */
struct ModuleSearchPath {
    std::string BasePath;                       // 基础路径
    std::vector<std::string> SubPaths;          // 子路径列表
    bool IsRecursive;                           // 是否递归搜索
    
    ModuleSearchPath(const std::string& base, bool recursive = false) 
        : BasePath(base), IsRecursive(recursive) {}
};

/**
 * CMOD模块信息
 */
struct CMODInfo {
    std::string ModuleName;                     // 模块名称
    std::string Version;                        // 版本
    std::string Author;                         // 作者
    std::string Description;                    // 描述
    std::vector<std::string> Dependencies;      // 依赖模块
    std::unordered_map<std::string, std::string> ExportTable; // 导出表
    std::vector<std::string> SubModules;        // 子模块
    
    CMODInfo() = default;
};

/**
 * CJMOD模块信息
 */
struct CJMODInfo {
    std::string ModuleName;                     // 模块名称
    std::string Version;                        // 版本
    std::string APIVersion;                     // API版本
    std::vector<std::string> CHTLJSExtensions;  // CHTL JS扩展
    std::unordered_map<std::string, std::string> SyntaxDefinitions; // 语法定义
    
    CJMODInfo() = default;
};

/**
 * 导入解析结果
 */
struct ImportResolutionResult {
    bool IsSuccess;                             // 是否成功
    std::vector<ImportItem> ResolvedImports;    // 解析后的导入项
    std::vector<std::string> Errors;           // 错误信息
    std::vector<std::string> Warnings;         // 警告信息
    
    ImportResolutionResult() : IsSuccess(false) {}
};

/**
 * CHTL导入管理器
 * 负责处理所有类型的导入：CHTL、HTML、CSS、JS、CJJS、CMOD、CJMOD
 * 严格按照CHTL语法文档和完善选择器自动化与引用规则文档
 */
class CHTLImportManager {
private:
    std::vector<ModuleSearchPath> m_SearchPaths;        // 模块搜索路径
    std::unordered_map<std::string, ImportItem> m_ImportCache; // 导入缓存
    std::unordered_map<std::string, CMODInfo> m_CMODRegistry;  // CMOD注册表
    std::unordered_map<std::string, CJMODInfo> m_CJMODRegistry; // CJMOD注册表
    
    // 官方模块前缀映射
    std::unordered_map<std::string, std::string> m_OfficialModulePrefixes;
    
    // 命名空间管理
    std::unordered_set<std::string> m_RegisteredNamespaces;     // 已注册的命名空间
    std::unordered_map<std::string, std::string> m_NamespaceAliases; // 命名空间别名
    
    // 错误和警告
    std::vector<std::string> m_Errors;          // 错误列表
    std::vector<std::string> m_Warnings;       // 警告列表

public:
    /**
     * 构造函数
     */
    CHTLImportManager();
    
    /**
     * 析构函数
     */
    ~CHTLImportManager() = default;
    
    /**
     * 解析导入语句
     * @param importStatement 导入语句
     * @return 解析结果
     */
    ImportResolutionResult ResolveImport(const std::string& importStatement);
    
    /**
     * 批量解析导入
     * @param importStatements 导入语句列表
     * @return 解析结果
     */
    ImportResolutionResult ResolveImports(const std::vector<std::string>& importStatements);
    
    /**
     * 添加模块搜索路径
     * @param path 搜索路径
     * @param recursive 是否递归
     */
    void AddSearchPath(const std::string& path, bool recursive = false);
    
    /**
     * 注册CMOD模块
     * @param modPath CMOD路径
     * @return 是否成功
     */
    bool RegisterCMOD(const std::string& modPath);
    
    /**
     * 注册CJMOD模块
     * @param modPath CJMOD路径
     * @return 是否成功
     */
    bool RegisterCJMOD(const std::string& modPath);
    
    /**
     * 解析CMOD模块信息
     * @param modPath CMOD路径
     * @return CMOD信息
     */
    CMODInfo ParseCMODInfo(const std::string& modPath);
    
    /**
     * 解析CJMOD模块信息
     * @param modPath CJMOD路径
     * @return CJMOD信息
     */
    CJMODInfo ParseCJMODInfo(const std::string& modPath);
    
    /**
     * 获取导入项内容
     * @param importItem 导入项
     * @return 文件内容
     */
    std::string GetImportContent(const ImportItem& importItem);
    
    /**
     * 检查导入依赖
     * @param importItem 导入项
     * @return 依赖列表
     */
    std::vector<ImportItem> GetImportDependencies(const ImportItem& importItem);
    
    /**
     * 重置导入管理器
     */
    void Reset();
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& GetWarnings() const { return m_Warnings; }

private:
    /**
     * 初始化默认搜索路径
     */
    void InitializeDefaultSearchPaths();
    
    /**
     * 初始化官方模块前缀
     */
    void InitializeOfficialModulePrefixes();
    
    /**
     * 解析导入路径
     * @param importPath 导入路径
     * @return 导入项
     */
    ImportItem ParseImportPath(const std::string& importPath);
    
    /**
     * 确定导入类型
     * @param filePath 文件路径
     * @return 导入类型
     */
    ImportType DetermineImportType(const std::string& filePath);
    
    /**
     * 解析文件路径
     * @param originalPath 原始路径
     * @return 解析后的完整路径
     */
    std::string ResolveFilePath(const std::string& originalPath);
    
    /**
     * 处理通配符导入
     * @param wildcardPath 通配符路径
     * @return 匹配的文件列表
     */
    std::vector<std::string> ProcessWildcardImport(const std::string& wildcardPath);
    
    /**
     * 处理官方模块导入
     * @param moduleName 模块名称（chtl::前缀）
     * @return 解析后的路径
     */
    std::string ProcessOfficialModuleImport(const std::string& moduleName);
    
    /**
     * 处理as语法
     * @param importStatement 导入语句
     * @param importItem 导入项
     */
    void ProcessAsSyntax(const std::string& importStatement, ImportItem& importItem);
    
    /**
     * 验证导入路径
     * @param path 路径
     * @return 是否有效
     */
    bool ValidateImportPath(const std::string& path);
    
    /**
     * 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool FileExists(const std::string& filePath);
    
    /**
     * 检查目录是否存在
     * @param dirPath 目录路径
     * @return 是否存在
     */
    bool DirectoryExists(const std::string& dirPath);
    
    /**
     * 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容
     */
    std::string ReadFileContent(const std::string& filePath);
    
    /**
     * 解析CMOD info文件
     * @param infoPath info文件路径
     * @return CMOD信息
     */
    CMODInfo ParseCMODInfoFile(const std::string& infoPath);
    
    /**
     * 解析CJMOD info文件
     * @param infoPath info文件路径
     * @return CJMOD信息
     */
    CJMODInfo ParseCJMODInfoFile(const std::string& infoPath);
    
    /**
     * 处理CMOD导出表
     * @param exportSection 导出段内容
     * @return 导出表映射
     */
    std::unordered_map<std::string, std::string> ProcessCMODExportTable(const std::string& exportSection);
    
    /**
     * 处理CJMOD语法定义
     * @param syntaxSection 语法段内容
     * @return 语法定义映射
     */
    std::unordered_map<std::string, std::string> ProcessCJMODSyntaxDefinitions(const std::string& syntaxSection);
    
    /**
     * 添加错误信息
     * @param error 错误信息
     */
    void AddError(const std::string& error);
    
    /**
     * 添加警告信息
     * @param warning 警告信息
     */
    void AddWarning(const std::string& warning);
    
    /**
     * 格式化导入路径
     * @param path 原始路径
     * @return 格式化后的路径
     */
    std::string FormatImportPath(const std::string& path);
    
    /**
     * 检查循环依赖
     * @param importItem 导入项
     * @param visitedPaths 已访问路径
     * @return 是否有循环依赖
     */
    bool CheckCircularDependency(const ImportItem& importItem, std::unordered_set<std::string>& visitedPaths);
};

} // namespace CHTL