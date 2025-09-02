#include "CJMOD/CJMODHandler.h"
#include "Common/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <functional>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace CHTL {

namespace fs = std::filesystem;

// CJMOD文件头魔数
const char CJMOD_MAGIC[] = "CHTLJMOD";
const uint32_t CJMOD_VERSION = 1;

// 文件头结构
struct CJMODHeader {
    char magic[8];
    uint32_t version;
    uint32_t fileCount;
    uint64_t timestamp;
};

bool CJMODStructure::IsValid() const {
    // 检查模块名称
    if (moduleName.empty()) return false;
    
    // 检查必要的目录是否存在
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) return false;
    if (!fs::exists(infoPath) || !fs::is_directory(infoPath)) return false;
    
    // 检查info文件
    fs::path infoFile = infoPath / (moduleName + ".chtl");
    if (!fs::exists(infoFile)) return false;
    
    // 检查是否有C++源文件
    bool hasCppFiles = false;
    for (const auto& entry : fs::directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension();
            if (ext == ".cpp" || ext == ".cc" || ext == ".cxx") {
                hasCppFiles = true;
                break;
            }
        }
    }
    
    return hasCppFiles;
}

CJMODHandler::CJMODHandler() {
    workingDirectory = fs::current_path();
}

CJMODHandler::~CJMODHandler() {
    // 卸载所有已加载的模块
    for (auto& [name, handle] : loadedModules) {
        if (handle) {
            UnloadDynamicLibrary(handle);
        }
    }
}

bool CJMODHandler::ValidateCJMODStructure(const fs::path& modulePath) {
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

std::unique_ptr<CJMODStructure> CJMODHandler::ScanModuleStructure(const fs::path& modulePath) {
    auto structure = std::make_unique<CJMODStructure>();
    
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

bool CJMODHandler::CompileModule(const fs::path& srcPath, const fs::path& outputPath) {
    // TODO: 实现C++模块编译
    // 这里需要调用C++编译器（g++/clang++）来编译模块
    // 示例命令：g++ -shared -fPIC -std=c++17 -o output.so src/*.cpp
    
    LOG_INFO("编译CJMOD模块: " + srcPath.string() + " -> " + outputPath.string());
    
    // 简化实现：假设已经有预编译的.so/.dll文件
    return true;
}

void* CJMODHandler::LoadDynamicLibrary(const fs::path& libPath) {
    void* handle = nullptr;
    
#ifdef _WIN32
    handle = LoadLibrary(libPath.string().c_str());
    if (!handle) {
        LOG_ERROR("加载动态库失败: " + libPath.string() + 
                 " 错误: " + std::to_string(GetLastError()));
    }
#else
    handle = dlopen(libPath.string().c_str(), RTLD_LAZY);
    if (!handle) {
        LOG_ERROR("加载动态库失败: " + libPath.string() + 
                 " 错误: " + std::string(dlerror()));
    }
#endif
    
    return handle;
}

void CJMODHandler::UnloadDynamicLibrary(void* handle) {
    if (!handle) return;
    
#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

bool CJMODHandler::PackModule(const fs::path& modulePath, const fs::path& outputPath) {
    try {
        LOG_INFO("开始打包CJMOD模块: " + modulePath.string());
        
        // 验证模块结构
        if (!ValidateCJMODStructure(modulePath)) {
            return false;
        }
        
        // 扫描模块结构
        auto structure = ScanModuleStructure(modulePath);
        if (!structure || !structure->IsValid()) {
            LOG_ERROR("无效的CJMOD模块结构");
            return false;
        }
        
        // 创建输出文件
        std::ofstream out(outputPath, std::ios::binary);
        if (!out.is_open()) {
            LOG_ERROR("无法创建输出文件: " + outputPath.string());
            return false;
        }
        
        // 写入文件头
        CJMODHeader header;
        std::memcpy(header.magic, CJMOD_MAGIC, sizeof(header.magic));
        header.version = CJMOD_VERSION;
        header.fileCount = 0; // 稍后更新
        header.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        
        out.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        // 收集所有文件
        std::vector<std::pair<fs::path, fs::path>> files;
        
        // 递归收集文件
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
        
        // 写入所有文件（复用CMOD的文件格式）
        for (const auto& [filePath, relPath] : files) {
            // 写入相对路径长度和路径
            std::string relPathStr = relPath.string();
            uint32_t pathLen = relPathStr.length();
            out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
            out.write(relPathStr.c_str(), pathLen);
            
            // 读取文件内容
            std::ifstream file(filePath, std::ios::binary);
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            // 写入文件大小
            uint64_t size64 = fileSize;
            out.write(reinterpret_cast<const char*>(&size64), sizeof(size64));
            
            // 写入文件内容
            std::vector<char> buffer(fileSize);
            file.read(buffer.data(), fileSize);
            out.write(buffer.data(), fileSize);
            
            LOG_DEBUG("打包文件: " + relPath.string());
        }
        
        out.close();
        LOG_INFO("CJMOD模块打包成功: " + outputPath.string());
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("打包CJMOD模块时出错: " + std::string(e.what()));
        return false;
    }
}

bool CJMODHandler::UnpackModule(const fs::path& cjmodPath, const fs::path& outputPath) {
    try {
        LOG_INFO("开始解包CJMOD模块: " + cjmodPath.string());
        
        // 打开CJMOD文件
        std::ifstream in(cjmodPath, std::ios::binary);
        if (!in.is_open()) {
            LOG_ERROR("无法打开CJMOD文件: " + cjmodPath.string());
            return false;
        }
        
        // 读取并验证文件头
        CJMODHeader header;
        in.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        if (std::memcmp(header.magic, CJMOD_MAGIC, sizeof(header.magic)) != 0) {
            LOG_ERROR("无效的CJMOD文件格式");
            return false;
        }
        
        if (header.version != CJMOD_VERSION) {
            LOG_ERROR("不支持的CJMOD版本: " + std::to_string(header.version));
            return false;
        }
        
        LOG_INFO("CJMOD文件包含 " + std::to_string(header.fileCount) + " 个文件");
        
        // 创建输出目录
        fs::create_directories(outputPath);
        
        // 读取所有文件
        for (uint32_t i = 0; i < header.fileCount; ++i) {
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
            LOG_DEBUG("解包文件: " + relPathStr);
        }
        
        in.close();
        LOG_INFO("CJMOD模块解包成功: " + outputPath.string());
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("解包CJMOD模块时出错: " + std::string(e.what()));
        return false;
    }
}

bool CJMODHandler::LoadModule(const fs::path& modulePath) {
    try {
        LOG_INFO("加载CJMOD模块: " + modulePath.string());
        
        fs::path actualPath = modulePath;
        
        // 如果是.cjmod文件，先解包
        if (modulePath.extension() == ".cjmod") {
            // 创建临时目录
            fs::path tempDir = fs::temp_directory_path() / 
                              ("chtl_cjmod_" + std::to_string(std::hash<std::string>{}(modulePath.string())));
            fs::create_directories(tempDir);
            
            // 解包模块
            if (!UnpackModule(modulePath, tempDir)) {
                LOG_ERROR("解包CJMOD模块失败");
                return false;
            }
            
            actualPath = tempDir;
        }
        
        // TODO: 编译并加载C++模块
        // 这里需要：
        // 1. 编译src目录下的C++源码为动态库
        // 2. 加载动态库
        // 3. 获取模块接口并初始化
        
        LOG_INFO("CJMOD模块加载成功（TODO: 实现编译和加载）");
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("加载CJMOD模块时出错: " + std::string(e.what()));
        return false;
    }
}

void CJMODHandler::UnloadModule(const std::string& moduleName) {
    auto it = loadedModules.find(moduleName);
    if (it != loadedModules.end()) {
        if (it->second) {
            UnloadDynamicLibrary(it->second);
        }
        loadedModules.erase(it);
        LOG_INFO("卸载CJMOD模块: " + moduleName);
    }
}

std::vector<fs::path> CJMODHandler::FindModules(const std::string& moduleName) {
    std::vector<fs::path> found;
    
    // 搜索顺序：官方模块目录 -> 本地module目录 -> 当前目录
    std::vector<fs::path> searchPaths = {
        GetOfficialModuleDirectory(),
        GetLocalModuleDirectory(),
        workingDirectory
    };
    
    for (const auto& searchPath : searchPaths) {
        if (!fs::exists(searchPath)) continue;
        
        // 查找.cjmod文件
        fs::path cjmodFile = searchPath / (moduleName + ".cjmod");
        if (fs::exists(cjmodFile)) {
            found.push_back(cjmodFile);
        }
        
        // 查找模块目录
        fs::path moduleDir = searchPath / moduleName;
        if (fs::exists(moduleDir) && fs::is_directory(moduleDir)) {
            if (ValidateCJMODStructure(moduleDir)) {
                found.push_back(moduleDir);
            }
        }
        
        // 在分类结构中查找
        for (const auto& subdir : {"CJMOD", "cjmod", "CJmod"}) {
            fs::path classifiedPath = searchPath / subdir;
            if (!fs::exists(classifiedPath)) continue;
            
            // 查找.cjmod文件
            fs::path classifiedCjmod = classifiedPath / (moduleName + ".cjmod");
            if (fs::exists(classifiedCjmod)) {
                found.push_back(classifiedCjmod);
            }
            
            // 查找模块目录
            fs::path classifiedDir = classifiedPath / moduleName;
            if (fs::exists(classifiedDir) && fs::is_directory(classifiedDir)) {
                if (ValidateCJMODStructure(classifiedDir)) {
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

fs::path CJMODHandler::GetOfficialModuleDirectory() const {
    // 获取编译器所在目录的module文件夹
    fs::path exePath = fs::current_path(); // 简化实现
    return exePath / "module";
}

fs::path CJMODHandler::GetLocalModuleDirectory() const {
    return workingDirectory / "module";
}

void* CJMODHandler::GetModuleHandle(const std::string& moduleName) const {
    auto it = loadedModules.find(moduleName);
    return (it != loadedModules.end()) ? it->second : nullptr;
}

} // namespace CHTL