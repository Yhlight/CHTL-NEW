#include "ModuleLoader.h"
#include "../Util/StringUtil/StringUtil.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace Module {

// OfficialModuleLoader实现
OfficialModuleLoader::OfficialModuleLoader() : m_IsInitialized(false) {
    m_CMODManager = std::make_unique<CMODManager>();
}

bool OfficialModuleLoader::Initialize(const std::string& moduleSourcePath) {
    if (m_IsInitialized) {
        return true;
    }
    
    m_ModuleSourcePath = moduleSourcePath;
    
    // 检查模块源码目录是否存在
    if (!Util::FileSystem::DirectoryExists(m_ModuleSourcePath)) {
        std::cerr << "模块源码目录不存在: " << m_ModuleSourcePath << std::endl;
        return false;
    }
    
    // 初始化CMOD管理器
    if (!m_CMODManager->Initialize()) {
        std::cerr << "CMOD管理器初始化失败" << std::endl;
        return false;
    }
    
    // 添加官方模块搜索路径
    m_CMODManager->AddSearchPath(m_ModuleSourcePath);
    
    m_IsInitialized = true;
    return true;
}

size_t OfficialModuleLoader::LoadAllOfficialModules() {
    if (!m_IsInitialized) {
        return 0;
    }
    
    auto moduleDirectories = ScanModuleSourceDirectory();
    size_t loadedCount = 0;
    
    for (const auto& modulePath : moduleDirectories) {
        std::string moduleName = Util::FileSystem::GetFileName(modulePath);
        
        if (LoadOfficialModule(moduleName)) {
            m_LoadedOfficialModules.push_back(moduleName);
            loadedCount++;
            std::cout << "✅ 官方模块已加载: " << moduleName << std::endl;
        } else {
            std::cerr << "❌ 官方模块加载失败: " << moduleName << std::endl;
        }
    }
    
    return loadedCount;
}

bool OfficialModuleLoader::LoadOfficialModule(const std::string& moduleName) {
    if (!m_IsInitialized) {
        return false;
    }
    
    std::string modulePath = Util::FileSystem::JoinPath(m_ModuleSourcePath, moduleName);
    
    // 验证模块结构
    if (!ValidateOfficialModuleStructure(modulePath)) {
        return false;
    }
    
    // 使用CMOD管理器加载模块
    return m_CMODManager->LoadModule(moduleName, modulePath);
}

bool OfficialModuleLoader::IsOfficialModule(const std::string& moduleName) const {
    return std::find(m_LoadedOfficialModules.begin(), m_LoadedOfficialModules.end(), moduleName) 
           != m_LoadedOfficialModules.end();
}

std::vector<std::string> OfficialModuleLoader::GetOfficialModuleList() const {
    return m_LoadedOfficialModules;
}

const CMODModuleInfo* OfficialModuleLoader::GetModuleInfo(const std::string& moduleName) const {
    if (!m_CMODManager) {
        return nullptr;
    }
    
    auto module = m_CMODManager->FindModule(moduleName);
    return module ? &module->GetInfo() : nullptr;
}

bool OfficialModuleLoader::ValidateOfficialModuleStructure(const std::string& modulePath) const {
    // 检查基本CMOD结构
    std::string srcPath = Util::FileSystem::JoinPath(modulePath, "src");
    std::string infoPath = Util::FileSystem::JoinPath(modulePath, "info");
    std::string moduleInfoFile = Util::FileSystem::JoinPath(infoPath, "module.info");
    
    if (!Util::FileSystem::DirectoryExists(srcPath)) {
        std::cerr << "模块缺少src目录: " << modulePath << std::endl;
        return false;
    }
    
    if (!Util::FileSystem::DirectoryExists(infoPath)) {
        std::cerr << "模块缺少info目录: " << modulePath << std::endl;
        return false;
    }
    
    if (!Util::FileSystem::FileExists(moduleInfoFile)) {
        std::cerr << "模块缺少module.info文件: " << modulePath << std::endl;
        return false;
    }
    
    // 检查是否为混合模块（CMOD+CJMOD）
    std::string cjmodPath = Util::FileSystem::JoinPath(modulePath, "cjmod");
    if (Util::FileSystem::DirectoryExists(cjmodPath)) {
        std::cout << "🔧 检测到CMOD+CJMOD混合模块: " << Util::FileSystem::GetFileName(modulePath) << std::endl;
    }
    
    return true;
}

std::string OfficialModuleLoader::GenerateOfficialModuleReport() const {
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    oss << "║                              🏛️ 官方模块报告 🏛️                                           ║" << std::endl;
    oss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    oss << "\n📊 官方模块统计:" << std::endl;
    oss << "  模块源码路径: " << m_ModuleSourcePath << std::endl;
    oss << "  已加载官方模块: " << m_LoadedOfficialModules.size() << " 个" << std::endl;
    
    if (!m_LoadedOfficialModules.empty()) {
        oss << "\n📋 官方模块详情:" << std::endl;
        oss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
        
        for (const auto& moduleName : m_LoadedOfficialModules) {
            const auto* info = GetModuleInfo(moduleName);
            if (info) {
                oss << "\n📦 " << moduleName << std::endl;
                oss << "  版本: " << info->Version << std::endl;
                oss << "  描述: " << info->Description << std::endl;
                oss << "  作者: " << info->Author << std::endl;
                std::string moduleType = (info->Metadata.count("Type") > 0) ? info->Metadata.at("Type") : "CMOD";
                oss << "  类型: " << moduleType << std::endl;
                
                auto module = m_CMODManager->FindModule(moduleName);
                if (module) {
                    oss << "  导出项: " << module->GetExports().size() << " 个" << std::endl;
                    oss << "  子模块: " << module->GetSubmodules().size() << " 个" << std::endl;
                }
            }
        }
    }
    
    if (m_CMODManager) {
        oss << "\n" << m_CMODManager->GenerateModuleReport() << std::endl;
    }
    
    return oss.str();
}

std::vector<std::string> OfficialModuleLoader::ScanModuleSourceDirectory() const {
    std::vector<std::string> moduleDirectories;
    
    if (!Util::FileSystem::DirectoryExists(m_ModuleSourcePath)) {
        return moduleDirectories;
    }
    
    auto dirContents = Util::FileSystem::ListDirectory(m_ModuleSourcePath, false);
    
    for (const auto& dirInfo : dirContents) {
        if (dirInfo.IsDirectory) {
            // 检查是否为有效的模块目录
            if (ValidateOfficialModuleStructure(dirInfo.Path)) {
                moduleDirectories.push_back(dirInfo.Path);
            }
        }
    }
    
    return moduleDirectories;
}

std::string OfficialModuleLoader::ParseModuleType(const std::string& modulePath) const {
    std::string infoFile = Util::FileSystem::JoinPath(modulePath, "info/module.info");
    
    if (!Util::FileSystem::FileExists(infoFile)) {
        return "UNKNOWN";
    }
    
    std::string content = Util::FileSystem::ReadFile(infoFile);
    auto lines = Util::StringUtil::Split(content, "\n");
    
    for (const auto& line : lines) {
        std::string trimmedLine = Util::StringUtil::Trim(line);
        if (Util::StringUtil::StartsWith(trimmedLine, "Type")) {
            auto parts = Util::StringUtil::Split(trimmedLine, "=");
            if (parts.size() >= 2) {
                return Util::StringUtil::Trim(parts[1]);
            }
        }
    }
    
    // 检查是否有CJMOD目录来判断类型
    std::string cjmodPath = Util::FileSystem::JoinPath(modulePath, "cjmod");
    if (Util::FileSystem::DirectoryExists(cjmodPath)) {
        return "CMOD+CJMOD";
    }
    
    return "CMOD";
}

// ModuleBuilder实现
bool ModuleBuilder::BuildModule(const std::string& sourceModulePath, const std::string& outputPath) {
    if (!Util::FileSystem::DirectoryExists(sourceModulePath)) {
        return false;
    }
    
    // 创建输出目录
    if (!Util::FileSystem::CreateDirectory(outputPath, true)) {
        return false;
    }
    
    // 复制模块文件
    return CopyModuleFiles(sourceModulePath, outputPath);
}

size_t ModuleBuilder::BuildAllOfficialModules(const std::string& sourceDir, const std::string& outputDir) {
    if (!Util::FileSystem::DirectoryExists(sourceDir)) {
        return 0;
    }
    
    // 创建输出目录
    Util::FileSystem::CreateDirectory(outputDir, true);
    
    auto dirContents = Util::FileSystem::ListDirectory(sourceDir, false);
    size_t builtCount = 0;
    
    for (const auto& dirInfo : dirContents) {
        if (dirInfo.IsDirectory) {
            std::string moduleName = Util::FileSystem::GetFileName(dirInfo.Path);
            std::string outputModulePath = Util::FileSystem::JoinPath(outputDir, moduleName);
            
            if (BuildModule(dirInfo.Path, outputModulePath)) {
                builtCount++;
                std::cout << "✅ 模块构建成功: " << moduleName << std::endl;
            } else {
                std::cerr << "❌ 模块构建失败: " << moduleName << std::endl;
            }
        }
    }
    
    return builtCount;
}

bool ModuleBuilder::ValidateBuiltModule(const std::string& builtModulePath) {
    // 检查必需的目录和文件
    std::string srcPath = Util::FileSystem::JoinPath(builtModulePath, "src");
    std::string infoPath = Util::FileSystem::JoinPath(builtModulePath, "info");
    std::string moduleInfoFile = Util::FileSystem::JoinPath(infoPath, "module.info");
    
    return Util::FileSystem::DirectoryExists(srcPath) &&
           Util::FileSystem::DirectoryExists(infoPath) &&
           Util::FileSystem::FileExists(moduleInfoFile);
}

std::string ModuleBuilder::GenerateBuildReport(const std::string& sourceDir, const std::string& outputDir) {
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    oss << "║                              🔨 模块构建报告 🔨                                            ║" << std::endl;
    oss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    oss << "\n📊 构建统计:" << std::endl;
    oss << "  源目录: " << sourceDir << std::endl;
    oss << "  输出目录: " << outputDir << std::endl;
    
    if (Util::FileSystem::DirectoryExists(sourceDir)) {
        auto sourceModules = Util::FileSystem::ListDirectory(sourceDir, false);
        oss << "  源模块数量: " << sourceModules.size() << std::endl;
    }
    
    if (Util::FileSystem::DirectoryExists(outputDir)) {
        auto builtModules = Util::FileSystem::ListDirectory(outputDir, false);
        oss << "  构建模块数量: " << builtModules.size() << std::endl;
        
        oss << "\n📋 构建模块详情:" << std::endl;
        for (const auto& moduleInfo : builtModules) {
            if (moduleInfo.IsDirectory) {
                bool isValid = ValidateBuiltModule(moduleInfo.Path);
                oss << "  📦 " << moduleInfo.Name << ": " << (isValid ? "✅ 有效" : "❌ 无效") << std::endl;
            }
        }
    }
    
    return oss.str();
}

bool ModuleBuilder::CopyModuleFiles(const std::string& sourcePath, const std::string& destPath) {
    try {
        // 递归复制整个模块目录
        auto sourceContents = Util::FileSystem::ListDirectory(sourcePath, true);
        
        for (const auto& item : sourceContents) {
            std::string relativePath = Util::FileSystem::GetRelativePath(item.Path, sourcePath);
            std::string destItemPath = Util::FileSystem::JoinPath(destPath, relativePath);
            
            if (item.IsDirectory) {
                Util::FileSystem::CreateDirectory(destItemPath, true);
            } else {
                // 确保目标目录存在
                std::string destDir = Util::FileSystem::GetDirectoryPath(destItemPath);
                Util::FileSystem::CreateDirectory(destDir, true);
                
                // 复制文件
                Util::FileSystem::CopyFile(item.Path, destItemPath, true);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool ModuleBuilder::ProcessCJMODFiles(const std::string& cjmodPath, const std::string& outputPath) {
    // TODO: 处理CJMOD文件的编译和转换
    // 这里应该调用CJMOD编译器来处理.cjmod文件
    return true;
}

} // namespace Module
} // namespace CHTL