#include "CHTLImportManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

CHTLImportManager::CHTLImportManager() {
    InitializeDefaultSearchPaths();
    InitializeOfficialModulePrefixes();
}

void CHTLImportManager::InitializeDefaultSearchPaths() {
    // 根据完善选择器自动化与引用规则文档初始化默认搜索路径
    
    // 当前目录
    AddSearchPath(".", false);
    
    // 标准CHTL模块路径
    AddSearchPath("./modules", true);
    AddSearchPath("./chtl_modules", true);
    
    // 相对路径搜索
    AddSearchPath("../", false);
    AddSearchPath("../../", false);
    
    // 系统路径（如果存在）
    AddSearchPath("/usr/local/chtl/modules", true);
    AddSearchPath("/opt/chtl/modules", true);
}

void CHTLImportManager::InitializeOfficialModulePrefixes() {
    // 官方模块前缀映射
    m_OfficialModulePrefixes["chtl::ui"] = "/system/modules/ui/";
    m_OfficialModulePrefixes["chtl::animation"] = "/system/modules/animation/";
    m_OfficialModulePrefixes["chtl::layout"] = "/system/modules/layout/";
    m_OfficialModulePrefixes["chtl::theme"] = "/system/modules/theme/";
    m_OfficialModulePrefixes["chtl::component"] = "/system/modules/component/";
    m_OfficialModulePrefixes["chtl::util"] = "/system/modules/util/";
}

ImportResolutionResult CHTLImportManager::ResolveImport(const std::string& importStatement) {
    ImportResolutionResult result;
    
    try {
        ImportItem importItem = ParseImportPath(importStatement);
        
        // 处理as语法
        ProcessAsSyntax(importStatement, importItem);
        
        // 解析文件路径
        if (importItem.Type == ImportType::OFFICIAL_MODULE) {
            importItem.ResolvedPath = ProcessOfficialModuleImport(importItem.OriginalPath);
        }
        else {
            importItem.ResolvedPath = ResolveFilePath(importItem.OriginalPath);
        }
        
        // 处理通配符导入
        if (importItem.IsWildcard) {
            importItem.WildcardMatches = ProcessWildcardImport(importItem.ResolvedPath);
            
            if (importItem.WildcardMatches.empty()) {
                AddWarning("通配符导入未匹配到任何文件: " + importItem.OriginalPath);
            }
        }
        
        // 验证导入路径
        if (!ValidateImportPath(importItem.ResolvedPath)) {
            AddError("无效的导入路径: " + importItem.OriginalPath);
            result.IsSuccess = false;
            result.Errors = m_Errors;
            return result;
        }
        
        // 检查循环依赖
        std::unordered_set<std::string> visitedPaths;
        if (CheckCircularDependency(importItem, visitedPaths)) {
            AddError("检测到循环依赖: " + importItem.OriginalPath);
            result.IsSuccess = false;
            result.Errors = m_Errors;
            return result;
        }
        
        importItem.IsResolved = true;
        
        // 缓存导入项
        m_ImportCache[importItem.OriginalPath] = importItem;
        
        result.IsSuccess = true;
        result.ResolvedImports.push_back(importItem);
        result.Warnings = m_Warnings;
        
    }
    catch (const std::exception& e) {
        AddError("导入解析异常: " + std::string(e.what()));
        result.IsSuccess = false;
        result.Errors = m_Errors;
    }
    
    return result;
}

ImportResolutionResult CHTLImportManager::ResolveImports(const std::vector<std::string>& importStatements) {
    ImportResolutionResult result;
    result.IsSuccess = true;
    
    for (const auto& statement : importStatements) {
        auto singleResult = ResolveImport(statement);
        
        if (!singleResult.IsSuccess) {
            result.IsSuccess = false;
        }
        
        result.ResolvedImports.insert(result.ResolvedImports.end(), 
                                     singleResult.ResolvedImports.begin(), 
                                     singleResult.ResolvedImports.end());
        
        result.Errors.insert(result.Errors.end(), 
                            singleResult.Errors.begin(), 
                            singleResult.Errors.end());
        
        result.Warnings.insert(result.Warnings.end(), 
                              singleResult.Warnings.begin(), 
                              singleResult.Warnings.end());
    }
    
    return result;
}

ImportItem CHTLImportManager::ParseImportPath(const std::string& importPath) {
    ImportItem item;
    
    std::string cleanPath = importPath;
    
    // 移除前后空白
    cleanPath.erase(0, cleanPath.find_first_not_of(" \t\n\r"));
    cleanPath.erase(cleanPath.find_last_not_of(" \t\n\r") + 1);
    
    // 检查官方模块前缀
    if (cleanPath.find("chtl::") == 0) {
        item.Type = ImportType::OFFICIAL_MODULE;
        item.OriginalPath = cleanPath;
        return item;
    }
    
    // 检查通配符
    if (cleanPath.find('*') != std::string::npos) {
        item.IsWildcard = true;
    }
    
    // 确定导入类型
    item.Type = DetermineImportType(cleanPath);
    item.OriginalPath = cleanPath;
    
    return item;
}

ImportType CHTLImportManager::DetermineImportType(const std::string& filePath) {
    std::string extension;
    
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filePath.substr(dotPos);
    }
    
    if (extension == ".chtl") {
        return ImportType::CHTL_FILE;
    }
    else if (extension == ".html") {
        return ImportType::HTML_FILE;
    }
    else if (extension == ".css") {
        return ImportType::CSS_FILE;
    }
    else if (extension == ".js") {
        return ImportType::JS_FILE;
    }
    else if (extension == ".cjjs") {
        return ImportType::CJJS_FILE;
    }
    else if (filePath.find("/CMOD/") != std::string::npos || filePath.find("\\CMOD\\") != std::string::npos) {
        return ImportType::CMOD_MODULE;
    }
    else if (filePath.find("/CJMOD/") != std::string::npos || filePath.find("\\CJMOD\\") != std::string::npos) {
        return ImportType::CJMOD_MODULE;
    }
    
    // 默认为CHTL文件
    return ImportType::CHTL_FILE;
}

std::string CHTLImportManager::ResolveFilePath(const std::string& originalPath) {
    // 如果是绝对路径，直接返回
    if (originalPath.front() == '/' || originalPath.find(":\\") != std::string::npos) {
        return originalPath;
    }
    
    // 在搜索路径中查找
    for (const auto& searchPath : m_SearchPaths) {
        std::string candidatePath = searchPath.BasePath + "/" + originalPath;
        
        if (FileExists(candidatePath)) {
            return candidatePath;
        }
        
        // 如果是递归搜索
        if (searchPath.IsRecursive) {
            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(searchPath.BasePath)) {
                    if (entry.is_regular_file()) {
                        std::string relativePath = std::filesystem::relative(entry.path(), searchPath.BasePath);
                        
                        if (relativePath == originalPath) {
                            return entry.path().string();
                        }
                    }
                }
            }
            catch (const std::filesystem::filesystem_error&) {
                // 目录不存在或无法访问，继续下一个搜索路径
                continue;
            }
        }
    }
    
    // 未找到文件，返回原始路径（可能会在后续验证中失败）
    return originalPath;
}

std::vector<std::string> CHTLImportManager::ProcessWildcardImport(const std::string& wildcardPath) {
    std::vector<std::string> matches;
    
    // 将通配符路径转换为正则表达式
    std::string pattern = wildcardPath;
    std::replace(pattern.begin(), pattern.end(), '*', '.');
    pattern = ".*" + pattern + ".*";
    
    std::regex wildcardRegex(pattern);
    
    // 在所有搜索路径中查找匹配文件
    for (const auto& searchPath : m_SearchPaths) {
        try {
            if (searchPath.IsRecursive) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(searchPath.BasePath)) {
                    if (entry.is_regular_file()) {
                        std::string filePath = entry.path().string();
                        
                        if (std::regex_match(filePath, wildcardRegex)) {
                            matches.push_back(filePath);
                        }
                    }
                }
            }
            else {
                for (const auto& entry : std::filesystem::directory_iterator(searchPath.BasePath)) {
                    if (entry.is_regular_file()) {
                        std::string filePath = entry.path().string();
                        
                        if (std::regex_match(filePath, wildcardRegex)) {
                            matches.push_back(filePath);
                        }
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error&) {
            // 目录不存在或无法访问，继续下一个搜索路径
            continue;
        }
    }
    
    return matches;
}

std::string CHTLImportManager::ProcessOfficialModuleImport(const std::string& moduleName) {
    auto it = m_OfficialModulePrefixes.find(moduleName);
    if (it != m_OfficialModulePrefixes.end()) {
        return it->second;
    }
    
    // 未知官方模块
    AddWarning("未知的官方模块: " + moduleName);
    return moduleName;
}

void CHTLImportManager::ProcessAsSyntax(const std::string& importStatement, ImportItem& importItem) {
    // 解析as语法：import "path/to/file.chtl" as MyModule;
    std::regex asPattern(R"(import\s+[""']([^""']+)[""']\s+as\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(importStatement, match, asPattern)) {
        importItem.AsName = match[2].str();
    }
}

bool CHTLImportManager::ValidateImportPath(const std::string& path) {
    // 基本路径验证
    if (path.empty()) {
        return false;
    }
    
    // 检查非法字符
    std::regex illegalChars(R"([<>:|?*])");
    if (std::regex_search(path, illegalChars)) {
        return false;
    }
    
    // 检查路径遍历攻击
    if (path.find("../") != std::string::npos || path.find("..\\") != std::string::npos) {
        AddWarning("检测到路径遍历，请确保路径安全: " + path);
    }
    
    return true;
}

bool CHTLImportManager::FileExists(const std::string& filePath) {
    try {
        return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    }
    catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool CHTLImportManager::DirectoryExists(const std::string& dirPath) {
    try {
        return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
    }
    catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

std::string CHTLImportManager::ReadFileContent(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        AddError("无法打开文件: " + filePath);
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    
    return content.str();
}

CMODInfo CHTLImportManager::ParseCMODInfo(const std::string& modPath) {
    CMODInfo info;
    
    // CMOD结构：模块文件夹/info/模块信息
    std::string infoPath = modPath + "/info";
    
    if (!DirectoryExists(infoPath)) {
        AddError("CMOD info目录不存在: " + infoPath);
        return info;
    }
    
    // 查找info文件
    std::string infoFile = infoPath + "/module.info";
    if (!FileExists(infoFile)) {
        infoFile = infoPath + "/info.txt";
    }
    
    if (!FileExists(infoFile)) {
        AddError("CMOD info文件不存在: " + infoPath);
        return info;
    }
    
    return ParseCMODInfoFile(infoFile);
}

CMODInfo CHTLImportManager::ParseCMODInfoFile(const std::string& infoPath) {
    CMODInfo info;
    
    std::string content = ReadFileContent(infoPath);
    if (content.empty()) {
        return info;
    }
    
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        // 移除前后空白
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line.front() == '#') {
            continue; // 跳过空行和注释
        }
        
        // 检查段标记
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // 解析键值对
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // 移除空白
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (currentSection == "Info") {
                if (key == "Name") info.ModuleName = value;
                else if (key == "Version") info.Version = value;
                else if (key == "Author") info.Author = value;
                else if (key == "Description") info.Description = value;
            }
            else if (currentSection == "Export") {
                info.ExportTable[key] = value;
            }
            else if (currentSection == "Dependencies") {
                info.Dependencies.push_back(value);
            }
        }
    }
    
    return info;
}

CJMODInfo CHTLImportManager::ParseCJMODInfo(const std::string& modPath) {
    CJMODInfo info;
    
    // CJMOD结构类似CMOD
    std::string infoPath = modPath + "/info";
    
    if (!DirectoryExists(infoPath)) {
        AddError("CJMOD info目录不存在: " + infoPath);
        return info;
    }
    
    std::string infoFile = infoPath + "/cjmod.info";
    if (!FileExists(infoFile)) {
        infoFile = infoPath + "/info.txt";
    }
    
    if (!FileExists(infoFile)) {
        AddError("CJMOD info文件不存在: " + infoPath);
        return info;
    }
    
    return ParseCJMODInfoFile(infoFile);
}

CJMODInfo CHTLImportManager::ParseCJMODInfoFile(const std::string& infoPath) {
    CJMODInfo info;
    
    std::string content = ReadFileContent(infoPath);
    if (content.empty()) {
        return info;
    }
    
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line.front() == '#') {
            continue;
        }
        
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (currentSection == "Info") {
                if (key == "Name") info.ModuleName = value;
                else if (key == "Version") info.Version = value;
                else if (key == "APIVersion") info.APIVersion = value;
            }
            else if (currentSection == "Syntax") {
                info.SyntaxDefinitions[key] = value;
            }
            else if (currentSection == "Extensions") {
                info.CHTLJSExtensions.push_back(value);
            }
        }
    }
    
    return info;
}

void CHTLImportManager::AddSearchPath(const std::string& path, bool recursive) {
    m_SearchPaths.emplace_back(path, recursive);
}

bool CHTLImportManager::RegisterCMOD(const std::string& modPath) {
    CMODInfo info = ParseCMODInfo(modPath);
    
    if (info.ModuleName.empty()) {
        AddError("无效的CMOD模块: " + modPath);
        return false;
    }
    
    m_CMODRegistry[info.ModuleName] = info;
    AddWarning("注册CMOD模块: " + info.ModuleName);
    
    return true;
}

bool CHTLImportManager::RegisterCJMOD(const std::string& modPath) {
    CJMODInfo info = ParseCJMODInfo(modPath);
    
    if (info.ModuleName.empty()) {
        AddError("无效的CJMOD模块: " + modPath);
        return false;
    }
    
    m_CJMODRegistry[info.ModuleName] = info;
    AddWarning("注册CJMOD模块: " + info.ModuleName);
    
    return true;
}

std::string CHTLImportManager::GetImportContent(const ImportItem& importItem) {
    if (!importItem.IsResolved) {
        AddError("导入项未解析: " + importItem.OriginalPath);
        return "";
    }
    
    return ReadFileContent(importItem.ResolvedPath);
}

std::vector<ImportItem> CHTLImportManager::GetImportDependencies(const ImportItem& importItem) {
    std::vector<ImportItem> dependencies;
    
    if (importItem.Type == ImportType::CMOD_MODULE) {
        auto it = m_CMODRegistry.find(importItem.OriginalPath);
        if (it != m_CMODRegistry.end()) {
            for (const auto& dep : it->second.Dependencies) {
                ImportItem depItem = ParseImportPath(dep);
                dependencies.push_back(depItem);
            }
        }
    }
    
    return dependencies;
}

bool CHTLImportManager::CheckCircularDependency(const ImportItem& importItem, std::unordered_set<std::string>& visitedPaths) {
    if (visitedPaths.find(importItem.ResolvedPath) != visitedPaths.end()) {
        return true; // 发现循环依赖
    }
    
    visitedPaths.insert(importItem.ResolvedPath);
    
    auto dependencies = GetImportDependencies(importItem);
    for (const auto& dep : dependencies) {
        if (CheckCircularDependency(dep, visitedPaths)) {
            return true;
        }
    }
    
    visitedPaths.erase(importItem.ResolvedPath);
    return false;
}

void CHTLImportManager::AddError(const std::string& error) {
    m_Errors.push_back(error);
}

void CHTLImportManager::AddWarning(const std::string& warning) {
    m_Warnings.push_back(warning);
}

void CHTLImportManager::Reset() {
    m_ImportCache.clear();
    m_Errors.clear();
    m_Warnings.clear();
    m_RegisteredNamespaces.clear();
    m_NamespaceAliases.clear();
}

std::string CHTLImportManager::FormatImportPath(const std::string& path) {
    std::string formatted = path;
    
    // 标准化路径分隔符
    std::replace(formatted.begin(), formatted.end(), '\\', '/');
    
    // 移除重复的斜杠
    std::regex duplicateSlash(R"(/+)");
    formatted = std::regex_replace(formatted, duplicateSlash, "/");
    
    return formatted;
}

} // namespace CHTL