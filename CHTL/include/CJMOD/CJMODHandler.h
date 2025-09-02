#ifndef CHTL_CJMOD_CJMODHANDLER_H
#define CHTL_CJMOD_CJMODHANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <functional>
#include <unordered_map>

namespace CHTL {

/**
 * CJMOD文件结构
 */
struct CJMODStructure {
    std::string moduleName;                   // 模块名称
    std::filesystem::path srcPath;            // src目录路径（C++源码）
    std::filesystem::path infoPath;           // info目录路径
    std::vector<CJMODStructure> subModules;   // 子模块
    
    bool IsValid() const;
    bool HasSubModules() const { return !subModules.empty(); }
};

/**
 * CJMOD处理器
 * 负责CJMOD模块的打包、解包和加载
 */
class CJMODHandler {
private:
    std::filesystem::path workingDirectory;
    std::unordered_map<std::string, void*> loadedModules; // 已加载的模块
    
    /**
     * 验证CJMOD目录结构
     */
    bool ValidateCJMODStructure(const std::filesystem::path& modulePath);
    
    /**
     * 递归扫描模块结构
     */
    std::unique_ptr<CJMODStructure> ScanModuleStructure(const std::filesystem::path& modulePath);
    
    /**
     * 编译C++源码为动态库
     */
    bool CompileModule(const std::filesystem::path& srcPath, 
                      const std::filesystem::path& outputPath);
    
    /**
     * 加载动态库
     */
    void* LoadDynamicLibrary(const std::filesystem::path& libPath);
    
    /**
     * 卸载动态库
     */
    void UnloadDynamicLibrary(void* handle);
    
public:
    CJMODHandler();
    ~CJMODHandler();
    
    /**
     * 设置工作目录
     */
    void SetWorkingDirectory(const std::filesystem::path& dir) { workingDirectory = dir; }
    
    /**
     * 打包CJMOD模块
     * @param modulePath 模块目录路径
     * @param outputPath 输出.cjmod文件路径
     * @return 是否成功
     */
    bool PackModule(const std::filesystem::path& modulePath, 
                   const std::filesystem::path& outputPath);
    
    /**
     * 解包CJMOD模块
     * @param cjmodPath .cjmod文件路径
     * @param outputPath 输出目录路径
     * @return 是否成功
     */
    bool UnpackModule(const std::filesystem::path& cjmodPath, 
                     const std::filesystem::path& outputPath);
    
    /**
     * 加载CJMOD模块
     * @param modulePath 模块路径（.cjmod文件或目录）
     * @return 是否成功
     */
    bool LoadModule(const std::filesystem::path& modulePath);
    
    /**
     * 卸载CJMOD模块
     * @param moduleName 模块名称
     */
    void UnloadModule(const std::string& moduleName);
    
    /**
     * 查找模块
     * @param moduleName 模块名称
     * @return 找到的模块路径列表
     */
    std::vector<std::filesystem::path> FindModules(const std::string& moduleName);
    
    /**
     * 获取官方模块目录
     */
    std::filesystem::path GetOfficialModuleDirectory() const;
    
    /**
     * 获取本地模块目录
     */
    std::filesystem::path GetLocalModuleDirectory() const;
    
    /**
     * 获取已加载的模块句柄
     */
    void* GetModuleHandle(const std::string& moduleName) const;
};

} // namespace CHTL

#endif // CHTL_CJMOD_CJMODHANDLER_H