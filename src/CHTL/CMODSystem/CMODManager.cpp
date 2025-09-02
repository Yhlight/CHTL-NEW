#include "CMODManager.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// CMODModule实现
CMODModule::CMODModule(const std::string& modulePath) 
    : m_ModulePath(modulePath), m_IsLoaded(false) {
    m_SrcPath = Util::FileSystem::JoinPath(modulePath, "src");
    m_InfoPath = Util::FileSystem::JoinPath(modulePath, "info");
}

bool CMODModule::Load() {
    if (m_IsLoaded) {
        return true;
    }
    
    // 验证模块结构
    if (!ValidateModuleStructure()) {
        return false;
    }
    
    // 解析模块信息
    if (!ParseModuleInfo()) {
        return false;
    }
    
    // 解析导出表
    if (!ParseExportTable()) {
        return false;
    }
    
    // 加载子模块
    if (!LoadSubmodules()) {
        return false;
    }
    
    m_IsLoaded = true;
    return true;
}

void CMODModule::Unload() {
    // 卸载子模块
    for (auto& submodule : m_Submodules) {
        if (submodule) {
            submodule->Unload();
        }
    }
    m_Submodules.clear();
    
    // 清理导出项
    m_Exports.clear();
    
    m_IsLoaded = false;
}

const CMODExportItem* CMODModule::FindExport(const std::string& name) const {
    auto it = std::find_if(m_Exports.begin(), m_Exports.end(),
                          [&name](const CMODExportItem& item) {
                              return item.Name == name;
                          });
    
    return (it != m_Exports.end()) ? &(*it) : nullptr;
}

CMODModule* CMODModule::FindSubmodule(const std::string& name) const {
    auto it = std::find_if(m_Submodules.begin(), m_Submodules.end(),
                          [&name](const std::unique_ptr<CMODModule>& module) {
                              return module && module->GetInfo().Name == name;
                          });
    
    return (it != m_Submodules.end()) ? it->get() : nullptr;
}

bool CMODModule::ParseModuleInfo() {
    std::string infoFilePath = Util::FileSystem::JoinPath(m_InfoPath, "module.info");
    
    if (!Util::FileSystem::FileExists(infoFilePath)) {
        return false;
    }
    
    std::string content = Util::FileSystem::ReadFile(infoFilePath);
    if (content.empty()) {
        return false;
    }
    
    // 简化的INI解析
    auto lines = Util::StringUtil::Split(content, "\n");
    std::string currentSection;
    
    for (const auto& line : lines) {
        std::string trimmedLine = Util::StringUtil::Trim(line);
        
        if (trimmedLine.empty() || trimmedLine[0] == ';' || trimmedLine[0] == '#') {
            continue; // 跳过空行和注释
        }
        
        if (trimmedLine[0] == '[' && trimmedLine.back() == ']') {
            currentSection = trimmedLine.substr(1, trimmedLine.length() - 2);
            continue;
        }
        
        size_t equalPos = trimmedLine.find('=');
        if (equalPos != std::string::npos) {
            std::string key = Util::StringUtil::Trim(trimmedLine.substr(0, equalPos));
            std::string value = Util::StringUtil::Trim(trimmedLine.substr(equalPos + 1));
            
            if (currentSection == "Info") {
                if (key == "Name") m_Info.Name = value;
                else if (key == "Version") m_Info.Version = value;
                else if (key == "Description") m_Info.Description = value;
                else if (key == "Author") m_Info.Author = value;
                else if (key == "License") m_Info.License = value;
            }
        }
    }
    
    return !m_Info.Name.empty();
}

bool CMODModule::ParseExportTable() {
    std::string exportFilePath = Util::FileSystem::JoinPath(m_InfoPath, "export.table");
    
    if (!Util::FileSystem::FileExists(exportFilePath)) {
        return true; // 导出表是可选的
    }
    
    std::string content = Util::FileSystem::ReadFile(exportFilePath);
    if (content.empty()) {
        return true;
    }
    
    auto lines = Util::StringUtil::Split(content, "\n");
    
    for (const auto& line : lines) {
        std::string trimmedLine = Util::StringUtil::Trim(line);
        
        if (trimmedLine.empty() || trimmedLine[0] == ';' || trimmedLine[0] == '#') {
            continue;
        }
        
        // 解析导出项格式: Name=Type:Path:FullQualifiedName:Public
        auto parts = Util::StringUtil::Split(trimmedLine, "=");
        if (parts.size() == 2) {
            std::string name = Util::StringUtil::Trim(parts[0]);
            auto valueParts = Util::StringUtil::Split(parts[1], ":");
            
            if (valueParts.size() >= 3) {
                CMODExportItem item;
                item.Name = name;
                item.Type = Util::StringUtil::Trim(valueParts[0]);
                item.Path = Util::StringUtil::Trim(valueParts[1]);
                item.FullQualifiedName = Util::StringUtil::Trim(valueParts[2]);
                item.IsPublic = (valueParts.size() > 3) ? (Util::StringUtil::Trim(valueParts[3]) == "true") : true;
                
                m_Exports.push_back(item);
            }
        }
    }
    
    return true;
}

bool CMODModule::LoadSubmodules() {
    // 扫描子模块目录
    auto submoduleDirs = Util::FileSystem::ListDirectory(m_SrcPath, false);
    
    for (const auto& dirInfo : submoduleDirs) {
        if (dirInfo.IsDirectory) {
            std::string submodulePath = dirInfo.Path;
            
            // 检查是否为有效的CMOD子模块
            std::string subInfoPath = Util::FileSystem::JoinPath(submodulePath, "info");
            if (Util::FileSystem::DirectoryExists(subInfoPath)) {
                auto submodule = std::make_unique<CMODModule>(submodulePath);
                if (submodule->Load()) {
                    m_Submodules.push_back(std::move(submodule));
                }
            }
        }
    }
    
    return true;
}

bool CMODModule::ValidateModuleStructure() {
    // 检查必需的目录结构
    if (!Util::FileSystem::DirectoryExists(m_SrcPath)) {
        return false;
    }
    
    if (!Util::FileSystem::DirectoryExists(m_InfoPath)) {
        return false;
    }
    
    // 检查module.info文件
    std::string infoFile = Util::FileSystem::JoinPath(m_InfoPath, "module.info");
    if (!Util::FileSystem::FileExists(infoFile)) {
        return false;
    }
    
    return true;
}

// CMODManager实现
CMODManager::CMODManager() : m_IsInitialized(false) {
}

bool CMODManager::Initialize() {
    if (m_IsInitialized) {
        return true;
    }
    
    // 添加默认搜索路径
    AddSearchPath("./modules");
    AddSearchPath("../modules");
    AddSearchPath("/usr/local/share/chtl/modules");
    
    m_IsInitialized = true;
    return true;
}

void CMODManager::AddSearchPath(const std::string& path) {
    if (std::find(m_ModuleSearchPaths.begin(), m_ModuleSearchPaths.end(), path) == m_ModuleSearchPaths.end()) {
        m_ModuleSearchPaths.push_back(path);
    }
}

bool CMODManager::LoadModule(const std::string& moduleName, const std::string& modulePath) {
    // 检查是否已加载
    if (IsModuleLoaded(moduleName)) {
        return true;
    }
    
    std::string actualPath = modulePath;
    
    // 如果没有提供路径，尝试解析
    if (actualPath.empty()) {
        actualPath = ResolveModulePath(moduleName);
        if (actualPath.empty()) {
            return false;
        }
    }
    
    // 创建并加载模块
    auto module = std::make_unique<CMODModule>(actualPath);
    if (!module->Load()) {
        return false;
    }
    
    // 添加到已加载模块列表
    m_LoadedModules[moduleName] = std::move(module);
    
    return true;
}

bool CMODManager::UnloadModule(const std::string& moduleName) {
    auto it = m_LoadedModules.find(moduleName);
    if (it != m_LoadedModules.end()) {
        it->second->Unload();
        m_LoadedModules.erase(it);
        return true;
    }
    return false;
}

CMODModule* CMODManager::FindModule(const std::string& moduleName) const {
    auto it = m_LoadedModules.find(moduleName);
    return (it != m_LoadedModules.end()) ? it->second.get() : nullptr;
}

std::string CMODManager::ResolveModulePath(const std::string& moduleName) const {
    // 检查别名
    auto aliasIt = m_ModuleAliases.find(moduleName);
    std::string actualName = (aliasIt != m_ModuleAliases.end()) ? aliasIt->second : moduleName;
    
    // 在搜索路径中查找模块
    for (const auto& searchPath : m_ModuleSearchPaths) {
        std::string modulePath = Util::FileSystem::JoinPath(searchPath, actualName);
        
        if (Util::FileSystem::DirectoryExists(modulePath)) {
            // 检查是否为有效的CMOD模块
            std::string infoPath = Util::FileSystem::JoinPath(modulePath, "info");
            std::string srcPath = Util::FileSystem::JoinPath(modulePath, "src");
            
            if (Util::FileSystem::DirectoryExists(infoPath) && Util::FileSystem::DirectoryExists(srcPath)) {
                return modulePath;
            }
        }
    }
    
    return "";
}

std::vector<std::string> CMODManager::GetLoadedModules() const {
    std::vector<std::string> result;
    for (const auto& modulePair : m_LoadedModules) {
        result.push_back(modulePair.first);
    }
    return result;
}

bool CMODManager::IsModuleLoaded(const std::string& moduleName) const {
    return m_LoadedModules.find(moduleName) != m_LoadedModules.end();
}

void CMODManager::SetModuleAlias(const std::string& alias, const std::string& moduleName) {
    m_ModuleAliases[alias] = moduleName;
}

std::string CMODManager::GetModuleByAlias(const std::string& alias) const {
    auto it = m_ModuleAliases.find(alias);
    return (it != m_ModuleAliases.end()) ? it->second : "";
}

void CMODManager::Clear() {
    // 卸载所有模块
    for (auto& modulePair : m_LoadedModules) {
        if (modulePair.second) {
            modulePair.second->Unload();
        }
    }
    
    m_LoadedModules.clear();
    m_ModuleAliases.clear();
}

std::string CMODManager::GenerateModuleReport() const {
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    oss << "║                                🔧 CMOD模块报告 🔧                                          ║" << std::endl;
    oss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    oss << "\n📊 模块统计:" << std::endl;
    oss << "  已加载模块数量: " << m_LoadedModules.size() << std::endl;
    oss << "  搜索路径数量: " << m_ModuleSearchPaths.size() << std::endl;
    oss << "  模块别名数量: " << m_ModuleAliases.size() << std::endl;
    
    if (!m_LoadedModules.empty()) {
        oss << "\n📋 已加载模块详情:" << std::endl;
        oss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
        
        for (const auto& modulePair : m_LoadedModules) {
            const auto& module = modulePair.second;
            const auto& info = module->GetInfo();
            
            oss << "\n📦 模块: " << modulePair.first << std::endl;
            oss << "  版本: " << info.Version << std::endl;
            oss << "  描述: " << info.Description << std::endl;
            oss << "  作者: " << info.Author << std::endl;
            oss << "  路径: " << module->GetModulePath() << std::endl;
            oss << "  导出项数量: " << module->GetExports().size() << std::endl;
            oss << "  子模块数量: " << module->GetSubmodules().size() << std::endl;
            
            if (!module->GetExports().empty()) {
                oss << "  导出项:" << std::endl;
                for (const auto& exportItem : module->GetExports()) {
                    oss << "    • " << exportItem.Name << " (" << exportItem.Type << ")" << std::endl;
                }
            }
        }
    }
    
    return oss.str();
}

std::vector<std::string> CMODManager::ScanModulesInSearchPaths() const {
    std::vector<std::string> foundModules;
    
    for (const auto& searchPath : m_ModuleSearchPaths) {
        if (!Util::FileSystem::DirectoryExists(searchPath)) {
            continue;
        }
        
        auto dirContents = Util::FileSystem::ListDirectory(searchPath, false);
        for (const auto& dirInfo : dirContents) {
            if (dirInfo.IsDirectory && ValidateModulePath(dirInfo.Path)) {
                foundModules.push_back(dirInfo.Path);
            }
        }
    }
    
    return foundModules;
}

bool CMODManager::ValidateModulePath(const std::string& path) const {
    std::string infoPath = Util::FileSystem::JoinPath(path, "info");
    std::string srcPath = Util::FileSystem::JoinPath(path, "src");
    std::string moduleInfoFile = Util::FileSystem::JoinPath(infoPath, "module.info");
    
    return Util::FileSystem::DirectoryExists(infoPath) &&
           Util::FileSystem::DirectoryExists(srcPath) &&
           Util::FileSystem::FileExists(moduleInfoFile);
}

} // namespace CHTL