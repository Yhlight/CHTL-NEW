#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

/**
 * 导入类型枚举
 */
enum class ImportType {
    HTML_FILE,              // @Html 文件导入
    STYLE_FILE,             // @Style 文件导入
    JAVASCRIPT_FILE,        // @JavaScript 文件导入
    CHTL_FILE,              // @Chtl 文件导入
    CJMOD_FILE,             // @CJmod 文件导入
    TEMPLATE_STYLE,         // [Template] @Style 导入
    TEMPLATE_ELEMENT,       // [Template] @Element 导入
    TEMPLATE_VAR,           // [Template] @Var 导入
    CUSTOM_STYLE,           // [Custom] @Style 导入
    CUSTOM_ELEMENT,         // [Custom] @Element 导入
    CUSTOM_VAR,             // [Custom] @Var 导入
    ORIGIN_HTML,            // [Origin] @Html 导入
    ORIGIN_STYLE,           // [Origin] @Style 导入
    ORIGIN_JAVASCRIPT,      // [Origin] @JavaScript 导入
    ORIGIN_CUSTOM,          // [Origin] @CustomType 导入
    CONFIG_NAMED,           // @Config 导入
    CONFIG_FULL,            // [Configuration] @Config 导入
    BATCH_ALL,              // 批量导入所有 (.*)
    BATCH_CMOD,             // 批量导入CMOD (*.cmod)
    BATCH_CHTL              // 批量导入CHTL (*.chtl)
};

/**
 * 导入信息结构
 */
struct ImportInfo {
    ImportType Type;                        // 导入类型
    std::string SourcePath;                 // 源路径
    std::string ResolvedPath;               // 解析后的实际路径
    std::string Alias;                      // 别名 (as语法)
    std::string TargetName;                 // 目标名称 (具体导入项)
    std::string Namespace;                  // 命名空间
    bool HasAlias = false;                  // 是否有别名
    bool IsResolved = false;                // 是否已解析路径
    std::vector<std::string> ResolvedFiles; // 解析后的文件列表 (批量导入)
};

/**
 * 路径搜索策略
 */
enum class SearchStrategy {
    OFFICIAL_MODULES_FIRST,     // 官方模块优先
    LOCAL_MODULES_FIRST,        // 本地模块优先
    CURRENT_DIR_ONLY,           // 仅当前目录
    ABSOLUTE_PATH_ONLY          // 仅绝对路径
};

/**
 * 导入解析器
 * 实现完整的CHTL导入系统
 * 支持所有导入类型、路径搜索策略、循环依赖检测等
 */
class ImportResolver {
private:
    // 路径配置
    std::string m_OfficialModulesPath;              // 官方模块路径
    std::string m_CurrentDirectory;                 // 当前目录
    std::string m_LocalModulesPath;                 // 本地模块路径
    
    // 导入状态
    std::vector<ImportInfo> m_ImportList;           // 导入列表
    std::unordered_set<std::string> m_ImportedFiles; // 已导入文件 (避免重复)
    std::unordered_set<std::string> m_ProcessingFiles; // 正在处理的文件 (避免循环)
    
    // 路径映射缓存
    std::unordered_map<std::string, std::string> m_PathCache;
    
    // 模块结构检测
    bool m_HasClassifiedStructure = false;          // 是否有分类结构 (CMOD/CJMOD文件夹)

public:
    /**
     * 构造函数
     */
    ImportResolver();
    
    /**
     * 析构函数
     */
    ~ImportResolver() = default;
    
    /**
     * 设置路径配置
     * @param officialModulesPath 官方模块路径
     * @param currentDirectory 当前目录
     * @param localModulesPath 本地模块路径
     */
    void SetPathConfiguration(const std::string& officialModulesPath,
                             const std::string& currentDirectory,
                             const std::string& localModulesPath = "");
    
    /**
     * 解析导入语句
     * @param importStatement 导入语句
     * @return 导入信息
     */
    ImportInfo ParseImportStatement(const std::string& importStatement);
    
    /**
     * 解析导入类型
     * @param typeExpression 类型表达式
     * @return 导入类型和目标名称
     */
    std::pair<ImportType, std::string> ParseImportType(const std::string& typeExpression);
    
    /**
     * 解析导入路径
     * @param pathExpression 路径表达式
     * @param importType 导入类型
     * @return 解析后的路径信息
     */
    std::vector<std::string> ResolveImportPath(const std::string& pathExpression, ImportType importType);
    
    /**
     * 处理批量导入
     * @param basePath 基础路径
     * @param pattern 匹配模式 (.*, *.cmod, *.chtl)
     * @return 匹配的文件列表
     */
    std::vector<std::string> ProcessBatchImport(const std::string& basePath, const std::string& pattern);
    
    /**
     * 检查循环依赖
     * @param filePath 文件路径
     * @return 是否存在循环依赖
     */
    bool CheckCircularDependency(const std::string& filePath);
    
    /**
     * 检查重复导入
     * @param filePath 文件路径
     * @return 是否已导入
     */
    bool CheckDuplicateImport(const std::string& filePath);
    
    /**
     * 处理路径多表达方式
     * @param path1 路径表达式1
     * @param path2 路径表达式2
     * @return 是否是同一路径
     */
    bool IsSamePath(const std::string& path1, const std::string& path2);
    
    /**
     * 搜索文件
     * @param fileName 文件名
     * @param fileExtensions 文件扩展名列表
     * @param strategy 搜索策略
     * @return 找到的文件路径
     */
    std::string SearchFile(const std::string& fileName,
                          const std::vector<std::string>& fileExtensions,
                          SearchStrategy strategy = SearchStrategy::OFFICIAL_MODULES_FIRST);
    
    /**
     * 检测模块目录结构
     * @param modulePath 模块路径
     * @return 是否有分类结构
     */
    bool DetectModuleStructure(const std::string& modulePath);
    
    /**
     * 处理官方模块前缀
     * @param moduleName 模块名称
     * @return 是否是官方模块前缀
     */
    bool IsOfficialModulePrefix(const std::string& moduleName);
    
    /**
     * 获取导入统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetImportStatistics();
    
    /**
     * 验证所有导入
     * @return 验证结果
     */
    bool ValidateAllImports();
    
    /**
     * 获取导入列表
     * @return 导入信息列表
     */
    const std::vector<ImportInfo>& GetImportList() const { return m_ImportList; }
    
    /**
     * 清理所有导入
     */
    void Clear();

private:
    /**
     * 规范化路径
     * @param path 原始路径
     * @return 规范化后的路径
     */
    std::string NormalizePath(const std::string& path);
    
    /**
     * 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool FileExists(const std::string& filePath);
    
    /**
     * 获取文件扩展名
     * @param fileName 文件名
     * @return 扩展名
     */
    std::string GetFileExtension(const std::string& fileName);
    
    /**
     * 构建搜索路径列表
     * @param strategy 搜索策略
     * @return 搜索路径列表
     */
    std::vector<std::string> BuildSearchPaths(SearchStrategy strategy);
    
    /**
     * 解析批量导入模式
     * @param pattern 模式字符串
     * @return 解析后的扩展名列表
     */
    std::vector<std::string> ParseBatchPattern(const std::string& pattern);
};

} // namespace CHTL