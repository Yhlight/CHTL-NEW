#include "Common/CMODHandler.h"
#include "Common/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <functional>

namespace CHTL {

namespace fs = std::filesystem;

// CMOD文件头魔数
const char CMOD_MAGIC[] = "CHTLCMOD";
const uint32_t CMOD_VERSION = 1;

// 文件头结构
struct CMODHeader {
    char magic[8];
    uint32_t version;
    uint32_t fileCount;
    uint64_t timestamp;
};

bool CMODStructure::IsValid() const {
    // 检查模块名称
    if (moduleName.empty()) return false;
    
    // 检查必要的目录是否存在
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) return false;
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) return false;
    
    // 如果没有子模块，检查主模块文件是否存在
    if (!HasSubModules()) {
        fs::path mainFile = srcPath / (moduleName + ".chtl");
        if (!fs::exists(mainFile)) return false;
    }
    
    // 检查info文件
    fs::path infoFile = infoPath / (moduleName + ".chtl");
    if (!fs::exists(infoFile)) return false;
    
    return true;
}

CMODHandler::CMODHandler() {
    workingDirectory = fs::current_path();
}

bool CMODHandler::ValidateCMODStructure(const fs::path& modulePath) {
    if (!fs::exists(modulePath) || !fs::is_directory(modulePath)) {
        LOG_ERROR("模块路径不存在或不是目录: " + modulePath.string());
        return false;
    }
    
    // 检查src和info目录
    fs::path srcPath = modulePath / "src";
    fs::path infoPath = modulePath / "info";
    
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        LOG_ERROR("缺少src目录: " + srcPath.string());
        return false;
    }
    
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) {
        LOG_ERROR("缺少info目录: " + infoPath.string());
        return false;
    }
    
    return true;
}

std::unique_ptr<CMODStructure> CMODHandler::ScanModuleStructure(const fs::path& modulePath) {
    auto structure = std::make_unique<CMODStructure>();
    
    structure->moduleName = modulePath.filename().string();
    structure->srcPath = modulePath / "src";
    structure->infoPath = modulePath / "info";
    
    // 扫描子模块
    if (fs::exists(structure->srcPath)) {
        for (const auto& entry : fs::directory_iterator(structure->srcPath)) {
            if (entry.is_directory()) {
                // 检查是否是有效的子模块
                fs::path subSrcPath = entry.path() / "src";
                fs::path subInfoPath = entry.path() / "info";
                
                if (fs::exists(subSrcPath) && fs::exists(subInfoPath)) {
                    auto subModule = ScanModuleStructure(entry.path());
                    if (subModule && subModule->IsValid()) {
                        structure->subModules.push_back(std::move(*subModule));
                    }
                }
            }
        }
    }
    
    return structure;
}

void CMODHandler::WriteFile(std::ostream& out, const fs::path& filePath, 
                           const fs::path& relativePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filePath.string());
    }
    
    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // 写入相对路径长度和路径
    std::string relPathStr = relativePath.string();
    uint32_t pathLen = relPathStr.length();
    out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
    out.write(relPathStr.c_str(), pathLen);
    
    // 写入文件大小
    uint64_t size64 = fileSize;
    out.write(reinterpret_cast<const char*>(&size64), sizeof(size64));
    
    // 写入文件内容
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    out.write(buffer.data(), fileSize);
}

void CMODHandler::ReadFile(std::istream& in, const fs::path& outputPath) {
    // 读取相对路径
    uint32_t pathLen;
    in.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    
    std::string relPathStr(pathLen, '\0');
    in.read(&relPathStr[0], pathLen);
    
    // 读取文件大小
    uint64_t fileSize;
    in.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    
    // 构建完整路径
    fs::path fullPath = outputPath / relPathStr;
    
    // 创建目录
    fs::create_directories(fullPath.parent_path());
    
    // 读取并写入文件内容
    std::vector<char> buffer(fileSize);
    in.read(buffer.data(), fileSize);
    
    std::ofstream outFile(fullPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("无法创建文件: " + fullPath.string());
    }
    
    outFile.write(buffer.data(), fileSize);
}

bool CMODHandler::PackModule(const fs::path& modulePath, 
                             const fs::path& outputPath) {
    try {
        LOG_INFO("开始打包模块: " + modulePath.string());
        
        // 验证模块结构
        if (!ValidateCMODStructure(modulePath)) {
            return false;
        }
        
        // 扫描模块结构
        auto structure = ScanModuleStructure(modulePath);
        if (!structure || !structure->IsValid()) {
            LOG_ERROR("无效的模块结构");
            return false;
        }
        
        // 创建输出文件
        std::ofstream out(outputPath, std::ios::binary);
        if (!out.is_open()) {
            LOG_ERROR("无法创建输出文件: " + outputPath.string());
            return false;
        }
        
        // 写入文件头
        CMODHeader header;
        std::memcpy(header.magic, CMOD_MAGIC, sizeof(header.magic));
        header.version = CMOD_VERSION;
        header.fileCount = 0; // 稍后更新
        header.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        
        out.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        // 收集所有文件
        std::vector<std::pair<fs::path, fs::path>> files; // (实际路径, 相对路径)
        
        // 递归收集文件的lambda函数
        std::function<void(const fs::path&, const fs::path&)> collectFiles = 
            [&](const fs::path& dirPath, const fs::path& relBase) {
                for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
                    if (entry.is_regular_file()) {
                        fs::path relPath = relBase / fs::relative(entry.path(), dirPath);
                        files.push_back({entry.path(), relPath});
                    }
                }
            };
        
        // 收集src和info目录中的文件
        collectFiles(structure->srcPath, "src");
        collectFiles(structure->infoPath, "info");
        
        // 更新文件数量
        header.fileCount = files.size();
        out.seekp(0);
        out.write(reinterpret_cast<const char*>(&header), sizeof(header));
        out.seekp(0, std::ios::end);
        
        // 写入所有文件
        for (const auto& [filePath, relPath] : files) {
            WriteFile(out, filePath, relPath);
            LOG_DEBUG("打包文件: " + relPath.string());
        }
        
        out.close();
        LOG_INFO("模块打包成功: " + outputPath.string());
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("打包模块时出错: " + std::string(e.what()));
        return false;
    }
}

bool CMODHandler::UnpackModule(const fs::path& cmodPath, 
                               const fs::path& outputPath) {
    try {
        LOG_INFO("开始解包模块: " + cmodPath.string());
        
        // 打开CMOD文件
        std::ifstream in(cmodPath, std::ios::binary);
        if (!in.is_open()) {
            LOG_ERROR("无法打开CMOD文件: " + cmodPath.string());
            return false;
        }
        
        // 读取并验证文件头
        CMODHeader header;
        in.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        if (std::memcmp(header.magic, CMOD_MAGIC, sizeof(header.magic)) != 0) {
            LOG_ERROR("无效的CMOD文件格式");
            return false;
        }
        
        if (header.version != CMOD_VERSION) {
            LOG_ERROR("不支持的CMOD版本: " + std::to_string(header.version));
            return false;
        }
        
        LOG_INFO("CMOD文件包含 " + std::to_string(header.fileCount) + " 个文件");
        
        // 创建输出目录
        fs::create_directories(outputPath);
        
        // 读取所有文件
        for (uint32_t i = 0; i < header.fileCount; ++i) {
            ReadFile(in, outputPath);
        }
        
        in.close();
        LOG_INFO("模块解包成功: " + outputPath.string());
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("解包模块时出错: " + std::string(e.what()));
        return false;
    }
}

bool CMODHandler::ValidateCMODFile(const fs::path& cmodPath) {
    try {
        std::ifstream in(cmodPath, std::ios::binary);
        if (!in.is_open()) {
            return false;
        }
        
        CMODHeader header;
        in.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        if (std::memcmp(header.magic, CMOD_MAGIC, sizeof(header.magic)) != 0) {
            return false;
        }
        
        if (header.version != CMOD_VERSION) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

std::unique_ptr<CMODStructure> CMODHandler::GetCMODInfo(const fs::path& cmodPath) {
    // TODO: 实现从CMOD文件读取模块信息
    return nullptr;
}

std::vector<fs::path> CMODHandler::FindModules(const std::string& moduleName) {
    std::vector<fs::path> found;
    
    // 搜索顺序：官方模块目录 -> 本地module目录 -> 当前目录
    std::vector<fs::path> searchPaths = {
        GetOfficialModuleDirectory(),
        GetLocalModuleDirectory(),
        workingDirectory
    };
    
    for (const auto& searchPath : searchPaths) {
        if (!fs::exists(searchPath)) continue;
        
        // 查找.cmod文件
        fs::path cmodFile = searchPath / (moduleName + ".cmod");
        if (fs::exists(cmodFile)) {
            found.push_back(cmodFile);
        }
        
        // 查找模块目录
        fs::path moduleDir = searchPath / moduleName;
        if (fs::exists(moduleDir) && fs::is_directory(moduleDir)) {
            if (ValidateCMODStructure(moduleDir)) {
                found.push_back(moduleDir);
            }
        }
        
        // 在分类结构中查找
        for (const auto& subdir : {"CMOD", "cmod", "Cmod"}) {
            fs::path classifiedPath = searchPath / subdir;
            if (!fs::exists(classifiedPath)) continue;
            
            // 查找.cmod文件
            fs::path classifiedCmod = classifiedPath / (moduleName + ".cmod");
            if (fs::exists(classifiedCmod)) {
                found.push_back(classifiedCmod);
            }
            
            // 查找模块目录
            fs::path classifiedDir = classifiedPath / moduleName;
            if (fs::exists(classifiedDir) && fs::is_directory(classifiedDir)) {
                if (ValidateCMODStructure(classifiedDir)) {
                    found.push_back(classifiedDir);
                }
            }
        }
    }
    
    // 去重
    std::sort(found.begin(), found.end());
    found.erase(std::unique(found.begin(), found.end()), found.end());
    
    return found;
}

fs::path CMODHandler::GetOfficialModuleDirectory() const {
    // 获取编译器所在目录的module文件夹
    // TODO: 实际实现应该获取编译器的安装目录
    fs::path exePath = fs::current_path(); // 简化实现
    return exePath / "module";
}

fs::path CMODHandler::GetLocalModuleDirectory() const {
    return workingDirectory / "module";
}

} // namespace CHTL