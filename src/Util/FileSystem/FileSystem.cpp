#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>

namespace CHTL {
namespace Util {

std::string FileSystem::ReadFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::WriteFile(const std::string& filePath, const std::string& content, bool append) {
    std::ios::openmode mode = std::ios::binary;
    if (append) {
        mode |= std::ios::app;
    } else {
        mode |= std::ios::trunc;
    }
    
    std::ofstream file(filePath, mode);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool FileSystem::FileExists(const std::string& filePath) {
    try {
        return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    } catch (...) {
        return false;
    }
}

bool FileSystem::DirectoryExists(const std::string& dirPath) {
    try {
        return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
    } catch (...) {
        return false;
    }
}

bool FileSystem::CreateDirectory(const std::string& dirPath, bool recursive) {
    try {
        if (recursive) {
            return std::filesystem::create_directories(dirPath);
        } else {
            return std::filesystem::create_directory(dirPath);
        }
    } catch (...) {
        return false;
    }
}

bool FileSystem::DeleteFile(const std::string& filePath) {
    try {
        return std::filesystem::remove(filePath);
    } catch (...) {
        return false;
    }
}

bool FileSystem::DeleteDirectory(const std::string& dirPath, bool recursive) {
    try {
        if (recursive) {
            return std::filesystem::remove_all(dirPath) > 0;
        } else {
            return std::filesystem::remove(dirPath);
        }
    } catch (...) {
        return false;
    }
}

bool FileSystem::CopyFile(const std::string& srcPath, const std::string& destPath, bool overwrite) {
    try {
        std::filesystem::copy_options options = std::filesystem::copy_options::none;
        if (overwrite) {
            options = std::filesystem::copy_options::overwrite_existing;
        }
        
        std::filesystem::copy_file(srcPath, destPath, options);
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::MoveFile(const std::string& srcPath, const std::string& destPath) {
    try {
        std::filesystem::rename(srcPath, destPath);
        return true;
    } catch (...) {
        return false;
    }
}

FileInfo FileSystem::GetFileInfo(const std::string& filePath) {
    FileInfo info;
    
    try {
        if (!std::filesystem::exists(filePath)) {
            return info;
        }
        
        info.Path = filePath;
        info.Name = GetFileName(filePath);
        info.Extension = GetExtension(filePath);
        info.IsDirectory = std::filesystem::is_directory(filePath);
        
        if (!info.IsDirectory) {
            info.Size = std::filesystem::file_size(filePath);
        }
        
        auto ftime = std::filesystem::last_write_time(filePath);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        auto time_t = std::chrono::system_clock::to_time_t(sctp);
        
        std::ostringstream timeStream;
        timeStream << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        info.LastModified = timeStream.str();
        
    } catch (...) {
        // 忽略异常，返回默认信息
    }
    
    return info;
}

std::vector<FileInfo> FileSystem::ListDirectory(const std::string& dirPath, bool recursive) {
    std::vector<FileInfo> result;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
                result.push_back(GetFileInfo(entry.path().string()));
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                result.push_back(GetFileInfo(entry.path().string()));
            }
        }
    } catch (...) {
        // 忽略异常，返回空列表
    }
    
    return result;
}

std::string FileSystem::GetExtension(const std::string& filePath) {
    try {
        std::filesystem::path path(filePath);
        std::string ext = path.extension().string();
        if (!ext.empty() && ext[0] == '.') {
            return ext.substr(1); // 去掉点
        }
        return ext;
    } catch (...) {
        return "";
    }
}

std::string FileSystem::GetFileName(const std::string& filePath) {
    try {
        return std::filesystem::path(filePath).filename().string();
    } catch (...) {
        return "";
    }
}

std::string FileSystem::GetFileNameWithoutExtension(const std::string& filePath) {
    try {
        return std::filesystem::path(filePath).stem().string();
    } catch (...) {
        return "";
    }
}

std::string FileSystem::GetDirectoryPath(const std::string& filePath) {
    try {
        return std::filesystem::path(filePath).parent_path().string();
    } catch (...) {
        return "";
    }
}

std::string FileSystem::NormalizePath(const std::string& path) {
    try {
        return std::filesystem::path(path).lexically_normal().string();
    } catch (...) {
        return path;
    }
}

std::string FileSystem::JoinPath(const std::string& basePath, const std::string& relativePath) {
    try {
        std::filesystem::path base(basePath);
        std::filesystem::path relative(relativePath);
        return (base / relative).string();
    } catch (...) {
        return basePath + GetPathSeparator() + relativePath;
    }
}

std::string FileSystem::GetAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (...) {
        return path;
    }
}

std::string FileSystem::GetRelativePath(const std::string& path, const std::string& basePath) {
    try {
        return std::filesystem::relative(path, basePath).string();
    } catch (...) {
        return path;
    }
}

bool FileSystem::IsAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::path(path).is_absolute();
    } catch (...) {
        return false;
    }
}

std::string FileSystem::GetCurrentDirectory() {
    try {
        return std::filesystem::current_path().string();
    } catch (...) {
        return "";
    }
}

bool FileSystem::SetCurrentDirectory(const std::string& dirPath) {
    try {
        std::filesystem::current_path(dirPath);
        return true;
    } catch (...) {
        return false;
    }
}

std::string FileSystem::GetTempDirectory() {
    try {
        return std::filesystem::temp_directory_path().string();
    } catch (...) {
        return "/tmp"; // Linux默认临时目录
    }
}

std::string FileSystem::CreateTempFile(const std::string& prefix, const std::string& suffix) {
    std::string tempDir = GetTempDirectory();
    std::string fileName;
    
    // 生成唯一文件名
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    
    do {
        fileName = prefix + std::to_string(dis(gen)) + suffix;
    } while (FileExists(JoinPath(tempDir, fileName)));
    
    std::string tempFilePath = JoinPath(tempDir, fileName);
    
    // 创建空文件
    WriteFile(tempFilePath, "");
    
    return tempFilePath;
}

char FileSystem::GetPathSeparator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

} // namespace Util
} // namespace CHTL