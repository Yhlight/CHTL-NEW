#pragma once

#include "BaseNode.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace CHTL {

    // 导入类型枚举
    enum class CHTLImportType {
        IMPORT_HTML,            // @Html 导入
        IMPORT_STYLE,           // @Style 导入  
        IMPORT_JAVASCRIPT,      // @JavaScript 导入
        IMPORT_CHTL,            // @Chtl 导入
        IMPORT_CJMOD,           // @CJmod 导入
        IMPORT_CONFIG,          // @Config 导入
        IMPORT_TEMPLATE,        // [Template] 导入
        IMPORT_CUSTOM,          // [Custom] 导入
        IMPORT_ORIGIN           // [Origin] 导入
    };

    // 导入目标类型枚举
    enum class CHTLImportTargetType {
        TARGET_FILE,            // 导入文件
        TARGET_SPECIFIC_ITEM,   // 导入特定项目
        TARGET_ALL_TYPE,        // 导入某类型的所有项目
        TARGET_ALL_ITEMS        // 导入所有项目
    };

    // 路径类型枚举
    enum class CHTLPathType {
        FILENAME_NO_EXT,        // 文件名（不带后缀）
        FILENAME_WITH_EXT,      // 具体文件名（带后缀）
        SPECIFIC_PATH_WITH_FILE, // 具体路径（含文件信息）
        SPECIFIC_PATH_NO_FILE,  // 具体路径（不含文件信息）
        WILDCARD_PATH           // 通配符路径
    };

    // 导入节点基类
    // 专门处理CHTL导入系统的AST节点
    // 严格按照语法文档798-880行和规则文件27-78行实现
    class CHTLImportNode : public CHTLBaseNode {
    private:
        CHTLImportType importType;                              // 导入类型
        CHTLImportTargetType targetType;                       // 导入目标类型
        std::string importPath;                                 // 导入路径
        std::string aliasName;                                  // 别名（as语法）
        std::string itemName;                                   // 项目名称（精确导入）
        std::string prefix;                                     // 前缀（[Custom], [Template]等）
        bool hasAs;                                            // 是否有as语法
        bool isFullQualified;                                  // 是否使用全缀名
        bool supportsUnquotedString;                           // 是否支持无修饰字符串
        
        // 路径处理
        CHTLPathType pathType;                                  // 路径类型
        std::vector<std::string> resolvedPaths;                // 解析后的路径列表
        bool isWildcard;                                       // 是否为通配符导入

    public:
        // 构造函数
        CHTLImportNode(CHTLImportType type, const std::string& path);
        ~CHTLImportNode() override = default;

        // 导入类型管理
        CHTLImportType GetImportType() const;                   // 获取导入类型
        void SetImportType(CHTLImportType type);                // 设置导入类型
        
        // 导入目标类型管理
        CHTLImportTargetType GetTargetType() const;             // 获取导入目标类型
        void SetTargetType(CHTLImportTargetType type);          // 设置导入目标类型
        
        // 路径管理
        const std::string& GetImportPath() const;               // 获取导入路径
        void SetImportPath(const std::string& path);            // 设置导入路径
        CHTLPathType GetPathType() const;                      // 获取路径类型
        void SetPathType(CHTLPathType type);                   // 设置路径类型
        
        // 别名管理
        const std::string& GetAliasName() const;                // 获取别名
        void SetAliasName(const std::string& alias);            // 设置别名
        bool HasAs() const;                                     // 是否有as语法
        void SetHasAs(bool hasAsValue);                         // 设置是否有as
        
        // 项目名称管理（精确导入）
        const std::string& GetItemName() const;                 // 获取项目名称
        void SetItemName(const std::string& name);              // 设置项目名称
        
        // 前缀管理
        const std::string& GetPrefix() const;                   // 获取前缀
        void SetPrefix(const std::string& prefixValue);         // 设置前缀
        bool IsFullQualified() const;                          // 是否使用全缀名
        void SetFullQualified(bool qualified);                 // 设置全缀名
        
        // 无修饰字符串支持
        bool SupportsUnquotedString() const;                   // 是否支持无修饰字符串
        void SetSupportsUnquotedString(bool supports);         // 设置无修饰字符串支持
        
        // 通配符管理
        bool IsWildcard() const;                               // 是否为通配符导入
        void SetWildcard(bool wildcard);                      // 设置通配符
        const std::vector<std::string>& GetResolvedPaths() const; // 获取解析后的路径
        void AddResolvedPath(const std::string& path);         // 添加解析后的路径
        
        // as语法规则处理
        bool ShouldSkipImport() const;                         // 是否应该跳过导入
        bool ShouldCreateNamedOriginNode() const;             // 是否应该创建带名原始嵌入节点
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 静态工具方法
        static std::string ImportTypeToString(CHTLImportType type);         // 导入类型转字符串
        static std::string TargetTypeToString(CHTLImportTargetType type);   // 目标类型转字符串
        static std::string PathTypeToString(CHTLPathType type);             // 路径类型转字符串
    };

    // 路径解析器类
    // 处理复杂的路径搜索策略
    class CHTLPathResolver {
    private:
        static std::vector<std::string> officialModulePaths;    // 官方模块路径
        static std::vector<std::string> currentModulePaths;     // 当前模块路径
        static std::vector<std::string> currentDirectoryPaths;  // 当前目录路径

    public:
        // 路径搜索方法
        static std::vector<std::string> ResolveHtmlPath(const std::string& path);        // 解析HTML文件路径
        static std::vector<std::string> ResolveStylePath(const std::string& path);       // 解析Style文件路径
        static std::vector<std::string> ResolveJavaScriptPath(const std::string& path);  // 解析JavaScript文件路径
        static std::vector<std::string> ResolveChtlPath(const std::string& path);        // 解析Chtl路径
        static std::vector<std::string> ResolveCJmodPath(const std::string& path);       // 解析CJmod路径
        
        // 路径类型判断
        static CHTLPathType AnalyzePathType(const std::string& path);      // 分析路径类型
        static bool IsFilenameNoExt(const std::string& path);              // 是否为不带后缀文件名
        static bool IsFilenameWithExt(const std::string& path);            // 是否为带后缀文件名
        static bool IsSpecificPathWithFile(const std::string& path);       // 是否为含文件信息的路径
        static bool IsWildcardPath(const std::string& path);               // 是否为通配符路径
        
        // 通配符处理
        static std::vector<std::string> ExpandWildcardPath(const std::string& wildcardPath); // 展开通配符路径
        static std::string ConvertDotSlashPath(const std::string& path);   // 转换.为/的路径
        
        // 搜索路径配置
        static void SetOfficialModulePaths(const std::vector<std::string>& paths);       // 设置官方模块路径
        static void SetCurrentModulePaths(const std::vector<std::string>& paths);        // 设置当前模块路径
        static void SetCurrentDirectoryPaths(const std::vector<std::string>& paths);     // 设置当前目录路径
        
        // 文件类型优先级
        static std::vector<std::string> GetChtlFilePriority();             // 获取Chtl文件优先级
        static std::vector<std::string> GetCJmodFilePriority();            // 获取CJmod文件优先级
        
        // 验证方法
        static bool ValidatePath(const std::string& path);                 // 验证路径是否有效
        static bool PathExists(const std::string& path);                   // 路径是否存在
    };

    // 循环依赖检测器
    // 修正同一路径多表达方式、循环依赖及重复导入问题
    class CHTLCircularDependencyDetector {
    private:
        static std::unordered_set<std::string> importedPaths;              // 已导入路径
        static std::unordered_map<std::string, std::string> pathAliases;   // 路径别名映射
        static std::vector<std::string> importStack;                       // 导入栈

    public:
        // 循环依赖检测
        static bool HasCircularDependency(const std::string& path);        // 是否有循环依赖
        static void PushImport(const std::string& path);                   // 压入导入
        static void PopImport();                                           // 弹出导入
        
        // 重复导入检测
        static bool IsAlreadyImported(const std::string& path);            // 是否已导入
        static void MarkAsImported(const std::string& path);               // 标记为已导入
        
        // 路径别名管理
        static void RegisterPathAlias(const std::string& alias, const std::string& realPath); // 注册路径别名
        static std::string ResolvePath(const std::string& path);           // 解析路径
        
        // 清理方法
        static void ClearImportHistory();                                  // 清空导入历史
        static void Reset();                                               // 重置检测器
        
        // 调试方法
        static void PrintImportStack();                                    // 打印导入栈
        static void PrintImportHistory();                                  // 打印导入历史
    };

    // 导入节点工厂类
    class CHTLImportNodeFactory {
    public:
        // 创建基础文件导入节点
        static std::shared_ptr<CHTLImportNode> CreateHtmlImportNode(const std::string& path, const std::string& alias);
        static std::shared_ptr<CHTLImportNode> CreateStyleImportNode(const std::string& path, const std::string& alias);
        static std::shared_ptr<CHTLImportNode> CreateJavaScriptImportNode(const std::string& path, const std::string& alias);
        
        // 创建模块导入节点
        static std::shared_ptr<CHTLImportNode> CreateChtlImportNode(const std::string& path);
        static std::shared_ptr<CHTLImportNode> CreateCJmodImportNode(const std::string& path);
        
        // 创建精确导入节点
        static std::shared_ptr<CHTLImportNode> CreateCustomImportNode(const std::string& type, const std::string& name, const std::string& path);
        static std::shared_ptr<CHTLImportNode> CreateTemplateImportNode(const std::string& type, const std::string& name, const std::string& path);
        static std::shared_ptr<CHTLImportNode> CreateOriginImportNode(const std::string& type, const std::string& name, const std::string& path);
        
        // 创建批量导入节点
        static std::shared_ptr<CHTLImportNode> CreateBatchTemplateImportNode(const std::string& path);
        static std::shared_ptr<CHTLImportNode> CreateBatchCustomImportNode(const std::string& path);
        static std::shared_ptr<CHTLImportNode> CreateBatchOriginImportNode(const std::string& path);
        
        // 创建配置导入节点
        static std::shared_ptr<CHTLImportNode> CreateConfigImportNode(const std::string& configName, const std::string& path);
        
        // 解析方法
        static CHTLImportType ParseImportType(const std::string& importText);
        static CHTLImportTargetType ParseTargetType(const std::string& targetText);
        static std::shared_ptr<CHTLImportNode> ParseImport(const std::string& importText);
        
        // 验证方法
        static bool IsValidImportPath(const std::string& path);            // 验证导入路径
        static bool IsValidAliasName(const std::string& alias);            // 验证别名
        static bool RequiresAs(CHTLImportType type);                       // 是否需要as语法
    };

} // namespace CHTL