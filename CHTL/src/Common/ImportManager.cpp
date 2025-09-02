#include "Common/ImportManager.h"
#include "Common/Logger.h"
#include <algorithm>
#include <sstream>
#include <stack>
#include <queue>
#include <regex>

namespace CHTL {

namespace fs = std::filesystem;

ImportManager::ImportManager() 
    : allowDuplicateImports(false),
      useCanonicalPaths(true),
      cacheResolvedPaths(true) {
    // 添加默认搜索路径
    AddSearchPath(".");
    AddSearchPath("./lib");
    AddSearchPath("./modules");
}

std::string ImportManager::CanonicalizePath(const std::string& path) {
    try {
        if (useCanonicalPaths && fs::exists(path)) {
            return fs::canonical(path).string();
        }
        return fs::absolute(path).string();
    } catch (const std::exception& e) {
        LOG_DEBUG("无法规范化路径 " + path + ": " + e.what());
        return path;
    }
}

bool ImportManager::FileExists(const std::string& path) {
    try {
        return fs::exists(path) && fs::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

std::string ImportManager::GetFileExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        return path.substr(pos);
    }
    return "";
}

std::string ImportManager::SearchInModuleDirectory(const std::string& modulePath, 
                                                   const std::string& moduleName,
                                                   bool preferCMOD) {
    // 首先尝试乱序结构（直接在module目录下查找）
    std::vector<std::string> extensions = preferCMOD ? 
        std::vector<std::string>{".cmod", ".chtl", ".cjmod"} :
        std::vector<std::string>{".cjmod", ".cmod", ".chtl"};
    
    for (const auto& ext : extensions) {
        std::string candidate = modulePath + "/" + moduleName + ext;
        if (FileExists(candidate)) {
            LOG_DEBUG("在模块目录找到文件: " + candidate);
            return candidate;
        }
    }
    
    // 然后尝试有序结构（CMOD/CJMOD分类）
    std::vector<std::string> cmodFolders = {"CMOD", "cmod", "Cmod"};
    std::vector<std::string> cjmodFolders = {"CJMOD", "cjmod", "CJmod"};
    
    // 根据偏好决定搜索顺序
    if (preferCMOD) {
        // 先搜索CMOD文件夹
        for (const auto& folder : cmodFolders) {
            for (const auto& ext : {".cmod", ".chtl"}) {
                std::string candidate = modulePath + "/" + folder + "/" + moduleName + ext;
                if (FileExists(candidate)) {
                    LOG_DEBUG("在分类模块目录找到文件: " + candidate);
                    return candidate;
                }
            }
        }
        
        // 再搜索CJMOD文件夹
        for (const auto& folder : cjmodFolders) {
            std::string candidate = modulePath + "/" + folder + "/" + moduleName + ".cjmod";
            if (FileExists(candidate)) {
                LOG_DEBUG("在分类模块目录找到文件: " + candidate);
                return candidate;
            }
        }
    } else {
        // 只搜索CJMOD文件夹
        for (const auto& folder : cjmodFolders) {
            std::string candidate = modulePath + "/" + folder + "/" + moduleName + ".cjmod";
            if (FileExists(candidate)) {
                LOG_DEBUG("在分类模块目录找到文件: " + candidate);
                return candidate;
            }
        }
    }
    
    return "";
}

std::string ImportManager::ResolvePath(const std::string& importPath, const std::string& currentFile) {
    // 检查缓存
    if (cacheResolvedPaths) {
        auto it = pathCache.find(importPath + "|" + currentFile);
        if (it != pathCache.end()) {
            return it->second;
        }
    }
    
    std::string resolvedPath;
    
    // 1. 处理模块别名
    auto aliasIt = moduleAliases.find(importPath);
    if (aliasIt != moduleAliases.end()) {
        resolvedPath = aliasIt->second;
    }
    // 2. 绝对路径
    else if (!importPath.empty() && importPath[0] == '/') {
        resolvedPath = importPath;
    }
    // 3. 相对路径
    else if (importPath.substr(0, 2) == "./" || importPath.substr(0, 3) == "../") {
        fs::path currentDir = fs::path(currentFile).parent_path();
        resolvedPath = (currentDir / importPath).string();
    }
    // 4. 官方模块前缀 chtl::
    else if (importPath.substr(0, 6) == "chtl::") {
        std::string moduleName = importPath.substr(6);
        // 只在官方模块目录搜索
        for (const auto& searchPath : searchPaths) {
            std::string result = SearchInModuleDirectory(searchPath, moduleName, true);
            if (!result.empty()) {
                resolvedPath = result;
                break;
            }
        }
    }
    // 5. 在搜索路径中查找（支持module目录结构）
    else {
        // 首先检查是否是模块导入（无路径分隔符的名称）
        bool isModuleImport = (importPath.find('/') == std::string::npos && 
                              importPath.find('\\') == std::string::npos);
        
        if (isModuleImport) {
            // 检查是否是子模块导入（包含点号）
            size_t dotPos = importPath.find('.');
            if (dotPos != std::string::npos) {
                // 子模块导入，如 Chtholly.Space
                std::string mainModule = importPath.substr(0, dotPos);
                std::string subModule = importPath.substr(dotPos + 1);
                
                // 先找到主模块
                std::string mainModulePath;
                for (const auto& searchPath : searchPaths) {
                    mainModulePath = SearchInModuleDirectory(searchPath, mainModule, true);
                    if (!mainModulePath.empty()) {
                        break;
                    }
                }
                
                if (mainModulePath.empty()) {
                    // 在当前目录的module文件夹查找
                    fs::path currentDir = fs::path(currentFile).parent_path();
                    std::string moduleDir = (currentDir / "module").string();
                    if (fs::exists(moduleDir)) {
                        mainModulePath = SearchInModuleDirectory(moduleDir, mainModule, true);
                    }
                }
                
                if (!mainModulePath.empty()) {
                    // 检查是否是通配符子模块导入
                    if (subModule == "*") {
                        // 返回主模块路径，让调用者处理通配符展开
                        resolvedPath = mainModulePath;
                    } else {
                        // 在主模块目录下查找子模块
                        fs::path mainModuleDir = fs::path(mainModulePath).parent_path();
                        std::string subModulePath = SearchInModuleDirectory(mainModuleDir.string(), subModule, true);
                        if (!subModulePath.empty()) {
                            resolvedPath = subModulePath;
                        }
                    }
                }
            } else {
                // 普通模块导入
                // 按优先级搜索：官方模块目录 -> 当前目录module -> 当前目录
                
                // 1. 官方模块目录
                for (const auto& searchPath : searchPaths) {
                    std::string result = SearchInModuleDirectory(searchPath, importPath, true);
                    if (!result.empty()) {
                        resolvedPath = result;
                        break;
                    }
                }
            }
            
            // 2. 当前目录的module文件夹
            if (resolvedPath.empty()) {
                fs::path currentDir = fs::path(currentFile).parent_path();
                std::string moduleDir = (currentDir / "module").string();
                if (fs::exists(moduleDir)) {
                    std::string result = SearchInModuleDirectory(moduleDir, importPath, true);
                    if (!result.empty()) {
                        resolvedPath = result;
                    }
                }
            }
            
            // 3. 当前目录
            if (resolvedPath.empty()) {
                fs::path currentDir = fs::path(currentFile).parent_path();
                std::vector<std::string> extensions = {".cmod", ".chtl", ".cjmod"};
                for (const auto& ext : extensions) {
                    std::string candidate = (currentDir / (importPath + ext)).string();
                    if (FileExists(candidate)) {
                        resolvedPath = candidate;
                        break;
                    }
                }
            }
        } else {
            // 带路径的导入，按原有逻辑处理
            for (const auto& searchPath : searchPaths) {
                fs::path candidate = fs::path(searchPath) / importPath;
                
                // 尝试不同的扩展名
                std::vector<std::string> extensions = {"", ".chtl", ".cjjs", ".css", ".js"};
                for (const auto& ext : extensions) {
                    std::string fullPath = candidate.string() + ext;
                    if (FileExists(fullPath)) {
                        resolvedPath = fullPath;
                        break;
                    }
                }
                
                if (!resolvedPath.empty()) break;
            }
            
            // 如果还没找到，尝试相对于当前文件
            if (resolvedPath.empty()) {
                fs::path currentDir = fs::path(currentFile).parent_path();
                resolvedPath = (currentDir / importPath).string();
            }
        }
    }
    
    // 添加默认扩展名
    if (GetFileExtension(resolvedPath).empty()) {
        // 根据导入类型推断扩展名
        if (FileExists(resolvedPath + ".chtl")) {
            resolvedPath += ".chtl";
        } else if (FileExists(resolvedPath + ".cjjs")) {
            resolvedPath += ".cjjs";
        }
    }
    
    // 规范化路径
    resolvedPath = CanonicalizePath(resolvedPath);
    
    // 缓存结果
    if (cacheResolvedPaths) {
        pathCache[importPath + "|" + currentFile] = resolvedPath;
    }
    
    return resolvedPath;
}

void ImportManager::AddSearchPath(const std::string& path) {
    if (std::find(searchPaths.begin(), searchPaths.end(), path) == searchPaths.end()) {
        searchPaths.push_back(path);
        LOG_DEBUG("添加搜索路径: " + path);
    }
}

void ImportManager::SetSearchPaths(const std::vector<std::string>& paths) {
    searchPaths = paths;
}

void ImportManager::RegisterModuleAlias(const std::string& alias, const std::string& path) {
    moduleAliases[alias] = path;
    LOG_DEBUG("注册模块别名: " + alias + " -> " + path);
}

bool ImportManager::IsImported(const std::string& path) const {
    std::string canonical = const_cast<ImportManager*>(this)->CanonicalizePath(path);
    return importedFiles.find(canonical) != importedFiles.end();
}

void ImportManager::MarkImported(const ImportInfo& info) {
    std::string canonical = CanonicalizePath(info.resolvedPath);
    importedFiles[canonical] = info;
    importedFiles[canonical].canonicalPath = canonical;
    
    LOG_DEBUG("标记已导入: " + canonical);
}

const ImportInfo* ImportManager::GetImportInfo(const std::string& path) const {
    std::string canonical = const_cast<ImportManager*>(this)->CanonicalizePath(path);
    auto it = importedFiles.find(canonical);
    if (it != importedFiles.end()) {
        return &it->second;
    }
    return nullptr;
}

void ImportManager::EnterImport(const std::string& path) {
    importStack.push_back(CanonicalizePath(path));
    LOG_DEBUG("进入导入: " + path + " (栈深度: " + std::to_string(importStack.size()) + ")");
}

void ImportManager::ExitImport() {
    if (!importStack.empty()) {
        std::string path = importStack.back();
        importStack.pop_back();
        LOG_DEBUG("退出导入: " + path + " (栈深度: " + std::to_string(importStack.size()) + ")");
    }
}

bool ImportManager::IsInImportCycle(const std::string& path) const {
    std::string canonical = const_cast<ImportManager*>(this)->CanonicalizePath(path);
    return std::find(importStack.begin(), importStack.end(), canonical) != importStack.end();
}

void ImportManager::AddDependency(const std::string& from, const std::string& to) {
    std::string fromCanonical = CanonicalizePath(from);
    std::string toCanonical = CanonicalizePath(to);
    
    dependencyGraph[fromCanonical].path = fromCanonical;
    dependencyGraph[fromCanonical].dependencies.push_back(toCanonical);
    
    dependencyGraph[toCanonical].path = toCanonical;
    dependencyGraph[toCanonical].dependents.push_back(fromCanonical);
    
    LOG_DEBUG("添加依赖: " + fromCanonical + " -> " + toCanonical);
}

bool ImportManager::DetectCycleDFS(const std::string& node, std::vector<std::string>& cycle) {
    auto& nodeInfo = dependencyGraph[node];
    nodeInfo.visited = true;
    nodeInfo.inStack = true;
    
    for (const auto& dep : nodeInfo.dependencies) {
        if (!dependencyGraph[dep].visited) {
            if (DetectCycleDFS(dep, cycle)) {
                cycle.push_back(node);
                return true;
            }
        } else if (dependencyGraph[dep].inStack) {
            // 找到循环
            cycle.push_back(dep);
            cycle.push_back(node);
            return true;
        }
    }
    
    nodeInfo.inStack = false;
    return false;
}

std::vector<std::vector<std::string>> ImportManager::DetectCycles() {
    std::vector<std::vector<std::string>> cycles;
    
    // 重置访问标记
    for (auto& pair : dependencyGraph) {
        pair.second.visited = false;
        pair.second.inStack = false;
    }
    
    // DFS检测循环
    for (const auto& pair : dependencyGraph) {
        if (!pair.second.visited) {
            std::vector<std::string> cycle;
            if (DetectCycleDFS(pair.first, cycle)) {
                // 整理循环路径
                std::reverse(cycle.begin(), cycle.end());
                
                // 找到循环的起点
                size_t startIdx = 0;
                for (size_t i = 1; i < cycle.size(); ++i) {
                    if (cycle[i] == cycle[0]) {
                        startIdx = i;
                        break;
                    }
                }
                
                // 提取实际的循环
                std::vector<std::string> actualCycle(
                    cycle.begin(), 
                    cycle.begin() + startIdx + 1
                );
                
                cycles.push_back(actualCycle);
            }
        }
    }
    
    return cycles;
}

std::vector<std::string> ImportManager::GetDependencies(const std::string& path) const {
    std::string canonical = const_cast<ImportManager*>(this)->CanonicalizePath(path);
    auto it = dependencyGraph.find(canonical);
    if (it != dependencyGraph.end()) {
        return it->second.dependencies;
    }
    return {};
}

std::vector<std::string> ImportManager::GetDependents(const std::string& path) const {
    std::string canonical = const_cast<ImportManager*>(this)->CanonicalizePath(path);
    auto it = dependencyGraph.find(canonical);
    if (it != dependencyGraph.end()) {
        return it->second.dependents;
    }
    return {};
}

std::vector<std::string> ImportManager::TopologicalSort() {
    std::vector<std::string> result;
    std::unordered_map<std::string, int> inDegree;
    std::queue<std::string> zeroInDegree;
    
    // 计算入度
    for (const auto& pair : dependencyGraph) {
        const auto& node = pair.first;
        inDegree[node] = pair.second.dependents.size();
        
        if (inDegree[node] == 0) {
            zeroInDegree.push(node);
        }
    }
    
    // Kahn算法
    while (!zeroInDegree.empty()) {
        std::string current = zeroInDegree.front();
        zeroInDegree.pop();
        result.push_back(current);
        
        // 减少依赖节点的入度
        for (const auto& dep : dependencyGraph[current].dependencies) {
            inDegree[dep]--;
            if (inDegree[dep] == 0) {
                zeroInDegree.push(dep);
            }
        }
    }
    
    // 如果结果数量不等于节点数，说明有循环
    if (result.size() != dependencyGraph.size()) {
        LOG_INFO("依赖图中存在循环，无法完成拓扑排序");
        return {};
    }
    
    return result;
}

std::string ImportManager::GenerateImportReport() const {
    std::stringstream report;
    
    report << "=== 导入报告 ===\n";
    report << "已导入文件数: " << importedFiles.size() << "\n";
    report << "依赖关系数: " << dependencyGraph.size() << "\n\n";
    
    report << "已导入文件:\n";
    for (const auto& pair : importedFiles) {
        const auto& info = pair.second;
        report << "  - " << info.originalPath;
        if (!info.asName.empty()) {
            report << " as " << info.asName;
        }
        report << "\n";
        report << "    解析路径: " << info.resolvedPath << "\n";
        report << "    规范路径: " << info.canonicalPath << "\n";
    }
    
    report << "\n循环依赖检测:\n";
    auto cycles = const_cast<ImportManager*>(this)->DetectCycles();
    if (cycles.empty()) {
        report << "  未发现循环依赖\n";
    } else {
        report << "  发现 " << cycles.size() << " 个循环:\n";
        for (size_t i = 0; i < cycles.size(); ++i) {
            report << "  " << (i + 1) << ". ";
            for (size_t j = 0; j < cycles[i].size(); ++j) {
                if (j > 0) report << " -> ";
                report << fs::path(cycles[i][j]).filename().string();
            }
            report << "\n";
        }
    }
    
    return report.str();
}

std::string ImportManager::GenerateDependencyGraph() const {
    std::stringstream dot;
    
    dot << "digraph Dependencies {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=box];\n\n";
    
    // 节点
    for (const auto& pair : dependencyGraph) {
        std::string filename = fs::path(pair.first).filename().string();
        dot << "  \"" << filename << "\";\n";
    }
    
    dot << "\n";
    
    // 边
    for (const auto& pair : dependencyGraph) {
        std::string fromFile = fs::path(pair.first).filename().string();
        for (const auto& dep : pair.second.dependencies) {
            std::string toFile = fs::path(dep).filename().string();
            dot << "  \"" << fromFile << "\" -> \"" << toFile << "\";\n";
        }
    }
    
    dot << "}\n";
    
    return dot.str();
}

void ImportManager::Clear() {
    importedFiles.clear();
    importStack.clear();
    dependencyGraph.clear();
    pathCache.clear();
    
    LOG_DEBUG("清空导入管理器状态");
}

std::vector<std::string> ImportManager::ResolveWildcardImport(const std::string& pattern, 
                                                               const std::string& currentFile) {
    std::vector<std::string> results;
    
    // 处理 ".*" 格式转换为 "/*"
    std::string normalizedPattern = pattern;
    size_t dotStarPos = normalizedPattern.find(".*");
    if (dotStarPos != std::string::npos) {
        normalizedPattern.replace(dotStarPos, 2, "/*");
    }
    
    // 提取基础路径和文件模式
    size_t lastSlash = normalizedPattern.find_last_of("/\\");
    std::string basePath;
    std::string filePattern;
    
    if (lastSlash != std::string::npos) {
        basePath = normalizedPattern.substr(0, lastSlash);
        filePattern = normalizedPattern.substr(lastSlash + 1);
    } else {
        basePath = ".";
        filePattern = normalizedPattern;
    }
    
    // 解析基础路径
    std::string resolvedBase = ResolvePath(basePath, currentFile);
    if (resolvedBase.empty()) {
        LOG_ERROR("无法解析通配符导入的基础路径: " + basePath);
        return results;
    }
    
    // 构建正则表达式
    std::string regexPattern = filePattern;
    // 转义特殊字符
    std::string specialChars = ".+^${}[]|()";
    for (char c : specialChars) {
        size_t pos = 0;
        while ((pos = regexPattern.find(c, pos)) != std::string::npos) {
            regexPattern.insert(pos, "\\");
            pos += 2;
        }
    }
    // 将 * 转换为 .*
    size_t pos = 0;
    while ((pos = regexPattern.find("*", pos)) != std::string::npos) {
        regexPattern.replace(pos, 1, ".*");
        pos += 2;
    }
    
    std::regex fileRegex(regexPattern);
    
    // 遍历目录
    try {
        for (const auto& entry : std::filesystem::directory_iterator(resolvedBase)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // 检查是否匹配模式
                if (std::regex_match(filename, fileRegex)) {
                    results.push_back(entry.path().string());
                    LOG_DEBUG("通配符匹配文件: " + entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        LOG_ERROR("遍历目录失败: " + std::string(e.what()));
    }
    
    LOG_INFO("通配符导入 '" + pattern + "' 匹配到 " + std::to_string(results.size()) + " 个文件");
    
    return results;
}

} // namespace CHTL