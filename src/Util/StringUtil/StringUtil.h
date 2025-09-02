#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>
#include <locale>

namespace CHTL {
namespace Util {

/**
 * CHTL字符串工具类
 * 提供UTF-8字符串处理、编码转换、文本操作等功能
 */
class StringUtil {
public:
    /**
     * UTF-8字符串长度计算
     * @param str UTF-8字符串
     * @return 字符数量（非字节数量）
     */
    static size_t UTF8Length(const std::string& str);
    
    /**
     * UTF-8字符串截取
     * @param str 原字符串
     * @param start 起始字符位置
     * @param length 截取字符数量
     * @return 截取的字符串
     */
    static std::string UTF8Substring(const std::string& str, size_t start, size_t length = std::string::npos);
    
    /**
     * 字符串分割
     * @param str 原字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串数组
     */
    static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
    
    /**
     * 字符串连接
     * @param strings 字符串数组
     * @param separator 连接符
     * @return 连接后的字符串
     */
    static std::string Join(const std::vector<std::string>& strings, const std::string& separator);
    
    /**
     * 去除首尾空白字符
     * @param str 原字符串
     * @return 处理后的字符串
     */
    static std::string Trim(const std::string& str);
    
    /**
     * 去除左侧空白字符
     * @param str 原字符串
     * @return 处理后的字符串
     */
    static std::string TrimLeft(const std::string& str);
    
    /**
     * 去除右侧空白字符
     * @param str 原字符串
     * @return 处理后的字符串
     */
    static std::string TrimRight(const std::string& str);
    
    /**
     * 字符串替换
     * @param str 原字符串
     * @param from 被替换的字符串
     * @param to 替换为的字符串
     * @param replaceAll 是否替换所有匹配
     * @return 替换后的字符串
     */
    static std::string Replace(const std::string& str, const std::string& from, 
                              const std::string& to, bool replaceAll = true);
    
    /**
     * 字符串转大写
     * @param str 原字符串
     * @return 大写字符串
     */
    static std::string ToUpper(const std::string& str);
    
    /**
     * 字符串转小写
     * @param str 原字符串
     * @return 小写字符串
     */
    static std::string ToLower(const std::string& str);
    
    /**
     * 检查字符串是否以指定前缀开始
     * @param str 字符串
     * @param prefix 前缀
     * @return 是否以前缀开始
     */
    static bool StartsWith(const std::string& str, const std::string& prefix);
    
    /**
     * 检查字符串是否以指定后缀结束
     * @param str 字符串
     * @param suffix 后缀
     * @return 是否以后缀结束
     */
    static bool EndsWith(const std::string& str, const std::string& suffix);
    
    /**
     * 检查字符串是否包含指定子串
     * @param str 字符串
     * @param substring 子串
     * @return 是否包含
     */
    static bool Contains(const std::string& str, const std::string& substring);
    
    /**
     * 字符串格式化
     * @param format 格式字符串
     * @param args 参数
     * @return 格式化后的字符串
     */
    template<typename... Args>
    static std::string Format(const std::string& format, Args... args);
    
    /**
     * 转义HTML特殊字符
     * @param str 原字符串
     * @return 转义后的字符串
     */
    static std::string EscapeHTML(const std::string& str);
    
    /**
     * 转义CSS特殊字符
     * @param str 原字符串
     * @return 转义后的字符串
     */
    static std::string EscapeCSS(const std::string& str);
    
    /**
     * 转义JavaScript特殊字符
     * @param str 原字符串
     * @return 转义后的字符串
     */
    static std::string EscapeJavaScript(const std::string& str);
    
    /**
     * 生成随机字符串
     * @param length 长度
     * @param charset 字符集
     * @return 随机字符串
     */
    static std::string GenerateRandomString(size_t length, const std::string& charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    
    /**
     * 计算字符串哈希值
     * @param str 字符串
     * @return 哈希值
     */
    static size_t Hash(const std::string& str);
    
    /**
     * 检查是否为有效的UTF-8字符串
     * @param str 字符串
     * @return 是否有效
     */
    static bool IsValidUTF8(const std::string& str);
    
    /**
     * 字符串编码转换
     * @param str 原字符串
     * @param fromEncoding 源编码
     * @param toEncoding 目标编码
     * @return 转换后的字符串
     */
    static std::string ConvertEncoding(const std::string& str, const std::string& fromEncoding, const std::string& toEncoding);

private:
    /**
     * 检查是否为空白字符
     * @param c 字符
     * @return 是否为空白字符
     */
    static bool IsWhitespace(char c);
    
    /**
     * UTF-8字符边界检查
     * @param str 字符串
     * @param pos 位置
     * @return 是否为字符边界
     */
    static bool IsUTF8CharBoundary(const std::string& str, size_t pos);
};

} // namespace Util
} // namespace CHTL