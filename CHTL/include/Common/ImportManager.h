#ifndef CHTL_COMMON_IMPORTMANAGER_H
#define CHTL_COMMON_IMPORTMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <filesystem>

namespace CHTL {

/**
 * 导入信息
 */
struct ImportInfo {
    std::string originalPath;      // 原始导入路径
    std::string resolvedPath;      // 解析后的绝对路径
    std::string canonicalPath;     // 规范化路径（用于去重）
    std::string asName;           // as别名
    std::string itemName;         // 导入的具体项
    int line;                     // 导入语句所在行
    int column;                   // 导入语句所在列
    
    enum Type {
        File,         // 文件导入
        Module,       // 模块导入
        CMOD,         // CMOD模块
        CJMOD,        // CJMOD模块
        Style,        // 样式导入
        JavaScript    // JS导入
    } type;
    
    ImportInfo() : line(-1), column(-1), type(File) {}
};

/**
 * 导入依赖图节点
 */
struct DependencyNode {
    std::string path;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    bool visited;
    bool inStack;
    
    DependencyNode() : visited(false), inStack(false) {}
};

/**
 * 导入管理器
 * 负责管理所有导入相关的功能
 */
class ImportManager {
private:
    // 已导入的文件集合（规范化路径 -> ImportInfo）
    std::unordered_map<std::string, ImportInfo> importedFiles;
    
    // 导入栈（用于检测循环依赖）
    std::vector<std::string> importStack;
    
    // 依赖图
    std::unordered_map<std::string, DependencyNode> dependencyGraph;
    
    // 搜索路径
    std::vector<std::string> searchPaths;
    
    // 模块别名映射
    std::unordered_map<std::string, std::string> moduleAliases;
    
    // 配置选项
    bool allowDuplicateImports;
    bool useCanonicalPaths;
    bool cacheResolvedPaths;
    
    // 路径缓存
    std::unordered_map<std::string, std::string> pathCache;
    
    /**
     * 规范化路径
     */
    std::string CanonicalizePath(const std::string& path);
    
    /**
     * 检查文件是否存在
     */
    bool FileExists(const std::string& path);
    
    /**
     * 获取文件扩展名
     */
    std::string GetFileExtension(const std::string& path);
    
    /**
     * 检测循环依赖（DFS）
     */
    bool DetectCycleDFS(const std::string& node, std::vector<std::string>& cycle);
    
public:
    ImportManager();
    ~ImportManager() = default;
    
    /**
     * 解析导入路径
     * @param importPath 导入路径
     * @param currentFile 当前文件路径
     * @return 解析后的绝对路径
     */
    std::string ResolvePath(const std::string& importPath, const std::string& currentFile);
    
    /**
     * 添加搜索路径
     */
    void AddSearchPath(const std::string& path);
    
    /**
     * 设置搜索路径列表
     */
    void SetSearchPaths(const std::vector<std::string>& paths);
    
    /**
     * 获取搜索路径列表
     */
    const std::vector<std::string>& GetSearchPaths() const { return searchPaths; }
    
    /**
     * 注册模块别名
     */
    void RegisterModuleAlias(const std::string& alias, const std::string& path);
    
    /**
     * 检查是否已导入
     */
    bool IsImported(const std::string& path) const;
    
    /**
     * 标记文件已导入
     */
    void MarkImported(const ImportInfo& info);
    
    /**
     * 获取导入信息
     */
    const ImportInfo* GetImportInfo(const std::string& path) const;
    
    /**
     * 进入导入（压栈）
     */
    void EnterImport(const std::string& path);
    
    /**
     * 退出导入（出栈）
     */
    void ExitImport();
    
    /**
     * 检查是否在导入循环中
     */
    bool IsInImportCycle(const std::string& path) const;
    
    /**
     * 获取当前导入栈
     */
    const std::vector<std::string>& GetImportStack() const { return importStack; }
    
    /**
     * 添加依赖关系
     */
    void AddDependency(const std::string& from, const std::string& to);
    
    /**
     * 检测所有循环依赖
     */
    std::vector<std::vector<std::string>> DetectCycles();
    
    /**
     * 获取文件的所有依赖
     */
    std::vector<std::string> GetDependencies(const std::string& path) const;
    
    /**
     * 获取依赖于指定文件的所有文件
     */
    std::vector<std::string> GetDependents(const std::string& path) const;
    
    /**
     * 拓扑排序（获取正确的编译顺序）
     */
    std::vector<std::string> TopologicalSort();
    
    /**
     * 生成导入报告
     */
    std::string GenerateImportReport() const;
    
    /**
     * 生成依赖图（DOT格式）
     */
    std::string GenerateDependencyGraph() const;
    
    /**
     * 清除所有状态
     */
    void Clear();
    
    /**
     * 配置选项
     */
    void SetAllowDuplicateImports(bool allow) { allowDuplicateImports = allow; }
    void SetUseCanonicalPaths(bool use) { useCanonicalPaths = use; }
    void SetCacheResolvedPaths(bool cache) { cacheResolvedPaths = cache; }
};

} // namespace CHTL

#endif // CHTL_COMMON_IMPORTMANAGER_H