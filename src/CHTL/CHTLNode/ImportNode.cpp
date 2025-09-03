#include "ImportNode.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>

namespace CHTL {

    // 静态成员初始化
    std::vector<std::string> CHTLPathResolver::officialModulePaths;
    std::vector<std::string> CHTLPathResolver::currentModulePaths;
    std::vector<std::string> CHTLPathResolver::currentDirectoryPaths;
    
    std::unordered_set<std::string> CHTLCircularDependencyDetector::importedPaths;
    std::unordered_map<std::string, std::string> CHTLCircularDependencyDetector::pathAliases;
    std::vector<std::string> CHTLCircularDependencyDetector::importStack;

    // CHTLImportNode实现
    CHTLImportNode::CHTLImportNode(CHTLImportType type, const std::string& path) 
        : CHTLBaseNode(CHTLNodeType::IMPORT, "import"), 
          importType(type), targetType(CHTLImportTargetType::TARGET_FILE), 
          importPath(path), hasAs(false), isFullQualified(false), 
          supportsUnquotedString(true), pathType(CHTLPathType::FILENAME_NO_EXT), 
          isWildcard(false) {
        
        // 分析路径类型
        pathType = CHTLPathResolver::AnalyzePathType(path);
        isWildcard = CHTLPathResolver::IsWildcardPath(path);
    }

    // 导入类型管理实现
    CHTLImportType CHTLImportNode::GetImportType() const {
        return importType;
    }

    void CHTLImportNode::SetImportType(CHTLImportType type) {
        importType = type;
    }

    // 导入目标类型管理实现
    CHTLImportTargetType CHTLImportNode::GetTargetType() const {
        return targetType;
    }

    void CHTLImportNode::SetTargetType(CHTLImportTargetType type) {
        targetType = type;
    }

    // 路径管理实现
    const std::string& CHTLImportNode::GetImportPath() const {
        return importPath;
    }

    void CHTLImportNode::SetImportPath(const std::string& path) {
        importPath = path;
        pathType = CHTLPathResolver::AnalyzePathType(path);
        isWildcard = CHTLPathResolver::IsWildcardPath(path);
    }

    CHTLPathType CHTLImportNode::GetPathType() const {
        return pathType;
    }

    void CHTLImportNode::SetPathType(CHTLPathType type) {
        pathType = type;
    }

    // 别名管理实现
    const std::string& CHTLImportNode::GetAliasName() const {
        return aliasName;
    }

    void CHTLImportNode::SetAliasName(const std::string& alias) {
        aliasName = alias;
        hasAs = !alias.empty();
    }

    bool CHTLImportNode::HasAs() const {
        return hasAs;
    }

    void CHTLImportNode::SetHasAs(bool hasAsValue) {
        hasAs = hasAsValue;
        if (!hasAs) {
            aliasName.clear();
        }
    }

    // 项目名称管理实现
    const std::string& CHTLImportNode::GetItemName() const {
        return itemName;
    }

    void CHTLImportNode::SetItemName(const std::string& name) {
        itemName = name;
    }

    // 前缀管理实现
    const std::string& CHTLImportNode::GetPrefix() const {
        return prefix;
    }

    void CHTLImportNode::SetPrefix(const std::string& prefixValue) {
        prefix = prefixValue;
        isFullQualified = !prefixValue.empty();
    }

    bool CHTLImportNode::IsFullQualified() const {
        return isFullQualified;
    }

    void CHTLImportNode::SetFullQualified(bool qualified) {
        isFullQualified = qualified;
    }

    // 无修饰字符串支持实现
    bool CHTLImportNode::SupportsUnquotedString() const {
        return supportsUnquotedString;
    }

    void CHTLImportNode::SetSupportsUnquotedString(bool supports) {
        supportsUnquotedString = supports;
    }

    // 通配符管理实现
    bool CHTLImportNode::IsWildcard() const {
        return isWildcard;
    }

    void CHTLImportNode::SetWildcard(bool wildcard) {
        isWildcard = wildcard;
    }

    const std::vector<std::string>& CHTLImportNode::GetResolvedPaths() const {
        return resolvedPaths;
    }

    void CHTLImportNode::AddResolvedPath(const std::string& path) {
        resolvedPaths.push_back(path);
    }

    // as语法规则处理实现
    bool CHTLImportNode::ShouldSkipImport() const {
        // 严格按照规则文件：@Html/@Style/@JavaScript无as时直接跳过
        if (importType == CHTLImportType::IMPORT_HTML ||
            importType == CHTLImportType::IMPORT_STYLE ||
            importType == CHTLImportType::IMPORT_JAVASCRIPT) {
            return !hasAs;
        }
        return false;
    }

    bool CHTLImportNode::ShouldCreateNamedOriginNode() const {
        // 有as时，创建对应类型的带名原始嵌入节点
        if (importType == CHTLImportType::IMPORT_HTML ||
            importType == CHTLImportType::IMPORT_STYLE ||
            importType == CHTLImportType::IMPORT_JAVASCRIPT) {
            return hasAs;
        }
        return false;
    }

    // 重写虚方法实现
    std::string CHTLImportNode::ToString() const {
        std::stringstream ss;
        ss << "Import[" << ImportTypeToString(importType) << "]";
        if (!prefix.empty()) ss << " prefix:\"" << prefix << "\"";
        if (!itemName.empty()) ss << " item:\"" << itemName << "\"";
        ss << " path:\"" << importPath << "\"";
        if (hasAs) ss << " as:\"" << aliasName << "\"";
        if (isWildcard) ss << " (wildcard)";
        if (supportsUnquotedString) ss << " (unquoted)";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLImportNode::Clone() const {
        auto cloned = std::make_shared<CHTLImportNode>(importType, importPath);
        
        cloned->targetType = targetType;
        cloned->aliasName = aliasName;
        cloned->itemName = itemName;
        cloned->prefix = prefix;
        cloned->hasAs = hasAs;
        cloned->isFullQualified = isFullQualified;
        cloned->supportsUnquotedString = supportsUnquotedString;
        cloned->pathType = pathType;
        cloned->resolvedPaths = resolvedPaths;
        cloned->isWildcard = isWildcard;
        
        return cloned;
    }

    bool CHTLImportNode::Validate() const {
        // 验证导入节点
        if (importPath.empty()) return false;
        
        // 验证as语法规则
        if (CHTLImportNodeFactory::RequiresAs(importType) && !hasAs) {
            return false; // @Html/@Style/@JavaScript必须有as
        }
        
        // 验证路径类型
        if (pathType == CHTLPathType::SPECIFIC_PATH_NO_FILE) {
            return false; // 不含文件信息的路径应该报错
        }
        
        return CHTLBaseNode::Validate();
    }

    // 静态工具方法实现
    std::string CHTLImportNode::ImportTypeToString(CHTLImportType type) {
        switch (type) {
            case CHTLImportType::IMPORT_HTML: return "IMPORT_HTML";
            case CHTLImportType::IMPORT_STYLE: return "IMPORT_STYLE";
            case CHTLImportType::IMPORT_JAVASCRIPT: return "IMPORT_JAVASCRIPT";
            case CHTLImportType::IMPORT_CHTL: return "IMPORT_CHTL";
            case CHTLImportType::IMPORT_CJMOD: return "IMPORT_CJMOD";
            case CHTLImportType::IMPORT_CONFIG: return "IMPORT_CONFIG";
            case CHTLImportType::IMPORT_TEMPLATE: return "IMPORT_TEMPLATE";
            case CHTLImportType::IMPORT_CUSTOM: return "IMPORT_CUSTOM";
            case CHTLImportType::IMPORT_ORIGIN: return "IMPORT_ORIGIN";
            default: return "UNKNOWN";
        }
    }

    std::string CHTLImportNode::TargetTypeToString(CHTLImportTargetType type) {
        switch (type) {
            case CHTLImportTargetType::TARGET_FILE: return "TARGET_FILE";
            case CHTLImportTargetType::TARGET_SPECIFIC_ITEM: return "TARGET_SPECIFIC_ITEM";
            case CHTLImportTargetType::TARGET_ALL_TYPE: return "TARGET_ALL_TYPE";
            case CHTLImportTargetType::TARGET_ALL_ITEMS: return "TARGET_ALL_ITEMS";
            default: return "UNKNOWN";
        }
    }

    std::string CHTLImportNode::PathTypeToString(CHTLPathType type) {
        switch (type) {
            case CHTLPathType::FILENAME_NO_EXT: return "FILENAME_NO_EXT";
            case CHTLPathType::FILENAME_WITH_EXT: return "FILENAME_WITH_EXT";
            case CHTLPathType::SPECIFIC_PATH_WITH_FILE: return "SPECIFIC_PATH_WITH_FILE";
            case CHTLPathType::SPECIFIC_PATH_NO_FILE: return "SPECIFIC_PATH_NO_FILE";
            case CHTLPathType::WILDCARD_PATH: return "WILDCARD_PATH";
            default: return "UNKNOWN";
        }
    }

    // CHTLPathResolver实现
    std::vector<std::string> CHTLPathResolver::ResolveHtmlPath(const std::string& path) {
        std::vector<std::string> results;
        
        // 基础文件类型搜索：编译文件所在目录（非递归）
        if (IsFilenameNoExt(path)) {
            // 按类型搜索相关文件
            results.push_back(path + ".html");
            results.push_back(path + ".htm");
        } else if (IsFilenameWithExt(path)) {
            // 直接搜索该文件
            results.push_back(path);
        }
        
        return results;
    }

    std::vector<std::string> CHTLPathResolver::ResolveStylePath(const std::string& path) {
        std::vector<std::string> results;
        
        if (IsFilenameNoExt(path)) {
            results.push_back(path + ".css");
        } else if (IsFilenameWithExt(path)) {
            results.push_back(path);
        }
        
        return results;
    }

    std::vector<std::string> CHTLPathResolver::ResolveJavaScriptPath(const std::string& path) {
        std::vector<std::string> results;
        
        if (IsFilenameNoExt(path)) {
            results.push_back(path + ".js");
            results.push_back(path + ".cjjs");
        } else if (IsFilenameWithExt(path)) {
            results.push_back(path);
        }
        
        return results;
    }

    std::vector<std::string> CHTLPathResolver::ResolveChtlPath(const std::string& path) {
        std::vector<std::string> results;
        
        // @Chtl搜索策略：官方模块目录→当前module文件夹→当前目录
        // 优先匹配cmod文件，其次chtl，不匹配cjmod文件
        
        if (IsFilenameNoExt(path)) {
            // 名称（不带后缀）搜索
            auto priorities = GetChtlFilePriority(); // cmod, chtl
            for (const auto& ext : priorities) {
                results.push_back(path + "." + ext);
            }
        } else if (IsFilenameWithExt(path)) {
            results.push_back(path);
        } else if (IsSpecificPathWithFile(path)) {
            results.push_back(path);
        }
        
        return results;
    }

    std::vector<std::string> CHTLPathResolver::ResolveCJmodPath(const std::string& path) {
        std::vector<std::string> results;
        
        // @CJmod搜索策略：同路径策略，仅匹配cjmod文件
        if (IsFilenameNoExt(path)) {
            results.push_back(path + ".cjmod");
        } else if (IsFilenameWithExt(path)) {
            results.push_back(path);
        } else if (IsSpecificPathWithFile(path)) {
            results.push_back(path);
        }
        
        return results;
    }

    // 路径类型判断实现
    CHTLPathType CHTLPathResolver::AnalyzePathType(const std::string& path) {
        if (IsWildcardPath(path)) {
            return CHTLPathType::WILDCARD_PATH;
        } else if (IsFilenameNoExt(path)) {
            return CHTLPathType::FILENAME_NO_EXT;
        } else if (IsFilenameWithExt(path)) {
            return CHTLPathType::FILENAME_WITH_EXT;
        } else if (IsSpecificPathWithFile(path)) {
            return CHTLPathType::SPECIFIC_PATH_WITH_FILE;
        } else {
            return CHTLPathType::SPECIFIC_PATH_NO_FILE;
        }
    }

    bool CHTLPathResolver::IsFilenameNoExt(const std::string& path) {
        // 简单判断：不包含路径分隔符且不包含扩展名
        return path.find('/') == std::string::npos && 
               path.find('\\') == std::string::npos && 
               path.find('.') == std::string::npos;
    }

    bool CHTLPathResolver::IsFilenameWithExt(const std::string& path) {
        // 包含扩展名但不包含路径分隔符
        return path.find('/') == std::string::npos && 
               path.find('\\') == std::string::npos && 
               path.find('.') != std::string::npos;
    }

    bool CHTLPathResolver::IsSpecificPathWithFile(const std::string& path) {
        // 包含路径分隔符且包含文件信息
        return (path.find('/') != std::string::npos || path.find('\\') != std::string::npos) &&
               path.find('.') != std::string::npos;
    }

    bool CHTLPathResolver::IsWildcardPath(const std::string& path) {
        // 检查是否包含通配符 .* 或 /*
        return path.find(".*") != std::string::npos || path.find("/*") != std::string::npos;
    }

    // 通配符处理实现
    std::vector<std::string> CHTLPathResolver::ExpandWildcardPath(const std::string& wildcardPath) {
        std::vector<std::string> results;
        
        // 处理 .* 和 /* 通配符
        if (wildcardPath.find(".*") != std::string::npos) {
            // 转换为 /* 格式
            std::string converted = wildcardPath;
            size_t pos = converted.find(".*");
            while (pos != std::string::npos) {
                converted.replace(pos, 2, "/*");
                pos = converted.find(".*", pos + 2);
            }
            results.push_back(converted);
        } else {
            results.push_back(wildcardPath);
        }
        
        return results;
    }

    std::string CHTLPathResolver::ConvertDotSlashPath(const std::string& path) {
        // 支持使用'/'替代'.'作为路径分隔符
        std::string result = path;
        
        // 将路径中的.替换为/（但保留文件扩展名中的.）
        size_t lastDot = result.find_last_of('.');
        size_t lastSlash = result.find_last_of('/');
        
        if (lastSlash == std::string::npos || (lastDot != std::string::npos && lastDot > lastSlash)) {
            // 有扩展名，只替换扩展名前的.
            if (lastDot != std::string::npos) {
                std::string pathPart = result.substr(0, lastDot);
                std::string extPart = result.substr(lastDot);
                std::replace(pathPart.begin(), pathPart.end(), '.', '/');
                result = pathPart + extPart;
            }
        } else {
            // 没有扩展名，替换所有.
            std::replace(result.begin(), result.end(), '.', '/');
        }
        
        return result;
    }

    std::vector<std::string> CHTLPathResolver::GetChtlFilePriority() {
        // @Chtl文件优先级：cmod → chtl，不匹配cjmod
        return {"cmod", "chtl"};
    }

    std::vector<std::string> CHTLPathResolver::GetCJmodFilePriority() {
        // @CJmod文件优先级：仅匹配cjmod
        return {"cjmod"};
    }

    bool CHTLPathResolver::ValidatePath(const std::string& path) {
        return !path.empty();
    }

    // CHTLCircularDependencyDetector实现
    bool CHTLCircularDependencyDetector::HasCircularDependency(const std::string& path) {
        // 检查导入栈中是否已存在该路径
        return std::find(importStack.begin(), importStack.end(), path) != importStack.end();
    }

    void CHTLCircularDependencyDetector::PushImport(const std::string& path) {
        importStack.push_back(path);
    }

    void CHTLCircularDependencyDetector::PopImport() {
        if (!importStack.empty()) {
            importStack.pop_back();
        }
    }

    bool CHTLCircularDependencyDetector::IsAlreadyImported(const std::string& path) {
        return importedPaths.find(path) != importedPaths.end();
    }

    void CHTLCircularDependencyDetector::MarkAsImported(const std::string& path) {
        importedPaths.insert(path);
    }

    void CHTLCircularDependencyDetector::RegisterPathAlias(const std::string& alias, const std::string& realPath) {
        pathAliases[alias] = realPath;
    }

    std::string CHTLCircularDependencyDetector::ResolvePath(const std::string& path) {
        auto it = pathAliases.find(path);
        return (it != pathAliases.end()) ? it->second : path;
    }

    void CHTLCircularDependencyDetector::ClearImportHistory() {
        importedPaths.clear();
        pathAliases.clear();
    }

    void CHTLCircularDependencyDetector::Reset() {
        ClearImportHistory();
        importStack.clear();
    }

    // CHTLImportNodeFactory实现
    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateHtmlImportNode(const std::string& path, const std::string& alias) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_HTML, path);
        if (!alias.empty()) {
            node->SetAliasName(alias);
        }
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateStyleImportNode(const std::string& path, const std::string& alias) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_STYLE, path);
        if (!alias.empty()) {
            node->SetAliasName(alias);
        }
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateJavaScriptImportNode(const std::string& path, const std::string& alias) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_JAVASCRIPT, path);
        if (!alias.empty()) {
            node->SetAliasName(alias);
        }
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateChtlImportNode(const std::string& path) {
        return std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_CHTL, path);
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateCJmodImportNode(const std::string& path) {
        return std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_CJMOD, path);
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateCustomImportNode(const std::string& type, const std::string& name, const std::string& path) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_CUSTOM, path);
        node->SetPrefix("[Custom]");
        node->SetItemName(name);
        node->SetTargetType(CHTLImportTargetType::TARGET_SPECIFIC_ITEM);
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateTemplateImportNode(const std::string& type, const std::string& name, const std::string& path) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_TEMPLATE, path);
        node->SetPrefix("[Template]");
        node->SetItemName(name);
        node->SetTargetType(CHTLImportTargetType::TARGET_SPECIFIC_ITEM);
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateOriginImportNode(const std::string& type, const std::string& name, const std::string& path) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_ORIGIN, path);
        node->SetPrefix("[Origin]");
        node->SetItemName(name);
        node->SetTargetType(CHTLImportTargetType::TARGET_SPECIFIC_ITEM);
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateBatchTemplateImportNode(const std::string& path) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_TEMPLATE, path);
        node->SetPrefix("[Template]");
        node->SetTargetType(CHTLImportTargetType::TARGET_ALL_ITEMS);
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateBatchCustomImportNode(const std::string& path) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_CUSTOM, path);
        node->SetPrefix("[Custom]");
        node->SetTargetType(CHTLImportTargetType::TARGET_ALL_ITEMS);
        return node;
    }

    std::shared_ptr<CHTLImportNode> CHTLImportNodeFactory::CreateConfigImportNode(const std::string& configName, const std::string& path) {
        auto node = std::make_shared<CHTLImportNode>(CHTLImportType::IMPORT_CONFIG, path);
        node->SetItemName(configName);
        return node;
    }

    // 解析方法实现
    CHTLImportType CHTLImportNodeFactory::ParseImportType(const std::string& importText) {
        if (importText.find("@Html") != std::string::npos) return CHTLImportType::IMPORT_HTML;
        if (importText.find("@Style") != std::string::npos) return CHTLImportType::IMPORT_STYLE;
        if (importText.find("@JavaScript") != std::string::npos) return CHTLImportType::IMPORT_JAVASCRIPT;
        if (importText.find("@Chtl") != std::string::npos) return CHTLImportType::IMPORT_CHTL;
        if (importText.find("@CJmod") != std::string::npos) return CHTLImportType::IMPORT_CJMOD;
        if (importText.find("[Custom]") != std::string::npos) return CHTLImportType::IMPORT_CUSTOM;
        if (importText.find("[Template]") != std::string::npos) return CHTLImportType::IMPORT_TEMPLATE;
        if (importText.find("[Origin]") != std::string::npos) return CHTLImportType::IMPORT_ORIGIN;
        if (importText.find("@Config") != std::string::npos) return CHTLImportType::IMPORT_CONFIG;
        
        return CHTLImportType::IMPORT_CHTL; // 默认
    }

    bool CHTLImportNodeFactory::IsValidImportPath(const std::string& path) {
        return !path.empty() && CHTLPathResolver::ValidatePath(path);
    }

    bool CHTLImportNodeFactory::IsValidAliasName(const std::string& alias) {
        if (alias.empty()) return true; // 空别名是有效的
        
        // 别名验证：字母开头，字母数字下划线组成
        if (!std::isalpha(alias[0])) return false;
        
        for (char c : alias) {
            if (!std::isalnum(c) && c != '_') {
                return false;
            }
        }
        
        return true;
    }

    bool CHTLImportNodeFactory::RequiresAs(CHTLImportType type) {
        // @Html/@Style/@JavaScript必须有as
        return type == CHTLImportType::IMPORT_HTML ||
               type == CHTLImportType::IMPORT_STYLE ||
               type == CHTLImportType::IMPORT_JAVASCRIPT;
    }

} // namespace CHTL