#ifndef CHTL_COMMON_CMODHANDLER_H
#define CHTL_COMMON_CMODHANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace CHTL {

/**
 * CMOD文件结构
 */
struct CMODStructure {
    std::string moduleName;                // 模块名称
    std::filesystem::path srcPath;         // src目录路径
    std::filesystem::path infoPath;        // info目录路径
    std::vector<CMODStructure> subModules; // 子模块
    
    bool IsValid() const;
    bool HasSubModules() const { return !subModules.empty(); }
};

/**
 * CMOD处理器
 * 负责CMOD模块的打包和解包
 */
class CMODHandler {
private:
    std::filesystem::path workingDirectory;
    
    /**
     * 验证CMOD目录结构
     */
    bool ValidateCMODStructure(const std::filesystem::path& modulePath);
    
    /**
     * 递归扫描模块结构
     */
    std::unique_ptr<CMODStructure> ScanModuleStructure(const std::filesystem::path& modulePath);
    
    /**
     * 写入文件到打包数据
     */
    void WriteFile(std::ostream& out, const std::filesystem::path& filePath, 
                   const std::filesystem::path& relativePath);
    
    /**
     * 读取文件从打包数据
     */
    void ReadFile(std::istream& in, const std::filesystem::path& outputPath);
    
    /**
     * 写入目录结构信息
     */
    void WriteDirectoryInfo(std::ostream& out, const CMODStructure& structure);
    
    /**
     * 读取目录结构信息
     */
    std::unique_ptr<CMODStructure> ReadDirectoryInfo(std::istream& in);
    
public:
    CMODHandler();
    ~CMODHandler() = default;
    
    /**
     * 设置工作目录
     */
    void SetWorkingDirectory(const std::filesystem::path& dir) { workingDirectory = dir; }
    
    /**
     * 打包CMOD模块
     * @param modulePath 模块目录路径
     * @param outputPath 输出.cmod文件路径
     * @return 是否成功
     */
    bool PackModule(const std::filesystem::path& modulePath, 
                   const std::filesystem::path& outputPath);
    
    /**
     * 解包CMOD模块
     * @param cmodPath .cmod文件路径
     * @param outputPath 输出目录路径
     * @return 是否成功
     */
    bool UnpackModule(const std::filesystem::path& cmodPath, 
                     const std::filesystem::path& outputPath);
    
    /**
     * 验证CMOD文件
     * @param cmodPath .cmod文件路径
     * @return 是否有效
     */
    bool ValidateCMODFile(const std::filesystem::path& cmodPath);
    
    /**
     * 获取CMOD信息
     * @param cmodPath .cmod文件路径
     * @return 模块结构信息
     */
    std::unique_ptr<CMODStructure> GetCMODInfo(const std::filesystem::path& cmodPath);
    
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
};

} // namespace CHTL

#endif // CHTL_COMMON_CMODHANDLER_H