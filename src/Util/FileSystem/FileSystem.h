#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace CHTL {
namespace Util {

/**
 * 文件信息结构
 */
struct FileInfo {
    std::string Path;           // 文件路径
    std::string Name;           // 文件名
    std::string Extension;      // 扩展名
    size_t Size;               // 文件大小
    bool IsDirectory;          // 是否为目录
    std::string LastModified;  // 最后修改时间
    
    FileInfo() : Size(0), IsDirectory(false) {}
};

/**
 * CHTL文件系统工具类
 * 提供文件读写、路径操作、目录管理等功能
 */
class FileSystem {
public:
    /**
     * 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容，失败返回空字符串
     */
    static std::string ReadFile(const std::string& filePath);
    
    /**
     * 写入文件内容
     * @param filePath 文件路径
     * @param content 文件内容
     * @param append 是否追加模式
     * @return 是否成功
     */
    static bool WriteFile(const std::string& filePath, const std::string& content, bool append = false);
    
    /**
     * 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    static bool FileExists(const std::string& filePath);
    
    /**
     * 检查目录是否存在
     * @param dirPath 目录路径
     * @return 是否存在
     */
    static bool DirectoryExists(const std::string& dirPath);
    
    /**
     * 创建目录
     * @param dirPath 目录路径
     * @param recursive 是否递归创建
     * @return 是否成功
     */
    static bool CreateDirectory(const std::string& dirPath, bool recursive = true);
    
    /**
     * 删除文件
     * @param filePath 文件路径
     * @return 是否成功
     */
    static bool DeleteFile(const std::string& filePath);
    
    /**
     * 删除目录
     * @param dirPath 目录路径
     * @param recursive 是否递归删除
     * @return 是否成功
     */
    static bool DeleteDirectory(const std::string& dirPath, bool recursive = false);
    
    /**
     * 复制文件
     * @param srcPath 源文件路径
     * @param destPath 目标文件路径
     * @param overwrite 是否覆盖
     * @return 是否成功
     */
    static bool CopyFile(const std::string& srcPath, const std::string& destPath, bool overwrite = false);
    
    /**
     * 移动文件
     * @param srcPath 源文件路径
     * @param destPath 目标文件路径
     * @return 是否成功
     */
    static bool MoveFile(const std::string& srcPath, const std::string& destPath);
    
    /**
     * 获取文件信息
     * @param filePath 文件路径
     * @return 文件信息
     */
    static FileInfo GetFileInfo(const std::string& filePath);
    
    /**
     * 列出目录内容
     * @param dirPath 目录路径
     * @param recursive 是否递归
     * @return 文件信息列表
     */
    static std::vector<FileInfo> ListDirectory(const std::string& dirPath, bool recursive = false);
    
    /**
     * 获取文件扩展名
     * @param filePath 文件路径
     * @return 扩展名（不含点）
     */
    static std::string GetExtension(const std::string& filePath);
    
    /**
     * 获取文件名（不含路径）
     * @param filePath 文件路径
     * @return 文件名
     */
    static std::string GetFileName(const std::string& filePath);
    
    /**
     * 获取文件名（不含扩展名）
     * @param filePath 文件路径
     * @return 文件名
     */
    static std::string GetFileNameWithoutExtension(const std::string& filePath);
    
    /**
     * 获取目录路径
     * @param filePath 文件路径
     * @return 目录路径
     */
    static std::string GetDirectoryPath(const std::string& filePath);
    
    /**
     * 规范化路径
     * @param path 路径
     * @return 规范化后的路径
     */
    static std::string NormalizePath(const std::string& path);
    
    /**
     * 连接路径
     * @param basePath 基础路径
     * @param relativePath 相对路径
     * @return 连接后的路径
     */
    static std::string JoinPath(const std::string& basePath, const std::string& relativePath);
    
    /**
     * 获取绝对路径
     * @param path 路径
     * @return 绝对路径
     */
    static std::string GetAbsolutePath(const std::string& path);
    
    /**
     * 获取相对路径
     * @param path 目标路径
     * @param basePath 基础路径
     * @return 相对路径
     */
    static std::string GetRelativePath(const std::string& path, const std::string& basePath);
    
    /**
     * 检查路径是否为绝对路径
     * @param path 路径
     * @return 是否为绝对路径
     */
    static bool IsAbsolutePath(const std::string& path);
    
    /**
     * 获取当前工作目录
     * @return 当前工作目录
     */
    static std::string GetCurrentDirectory();
    
    /**
     * 设置当前工作目录
     * @param dirPath 目录路径
     * @return 是否成功
     */
    static bool SetCurrentDirectory(const std::string& dirPath);
    
    /**
     * 获取临时目录
     * @return 临时目录路径
     */
    static std::string GetTempDirectory();
    
    /**
     * 创建临时文件
     * @param prefix 文件名前缀
     * @param suffix 文件名后缀
     * @return 临时文件路径
     */
    static std::string CreateTempFile(const std::string& prefix = "chtl_", const std::string& suffix = ".tmp");

private:
    /**
     * 获取系统路径分隔符
     * @return 路径分隔符
     */
    static char GetPathSeparator();
};

} // namespace Util
} // namespace CHTL