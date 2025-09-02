#include "StringUtil.h"
#include <algorithm>
#include <sstream>
#include <random>
#include <functional>

namespace CHTL {
namespace Util {

size_t StringUtil::UTF8Length(const std::string& str) {
    size_t length = 0;
    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        if (c < 0x80) {
            i += 1; // ASCII字符
        } else if ((c >> 5) == 0x6) {
            i += 2; // 2字节UTF-8字符
        } else if ((c >> 4) == 0xe) {
            i += 3; // 3字节UTF-8字符
        } else if ((c >> 3) == 0x1e) {
            i += 4; // 4字节UTF-8字符
        } else {
            i += 1; // 无效字符，跳过
        }
        length++;
    }
    return length;
}

std::string StringUtil::UTF8Substring(const std::string& str, size_t start, size_t length) {
    if (start >= UTF8Length(str)) {
        return "";
    }
    
    size_t byteStart = 0;
    size_t charCount = 0;
    
    // 找到起始字节位置
    for (size_t i = 0; i < str.size() && charCount < start; ) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        if (c < 0x80) {
            i += 1;
        } else if ((c >> 5) == 0x6) {
            i += 2;
        } else if ((c >> 4) == 0xe) {
            i += 3;
        } else if ((c >> 3) == 0x1e) {
            i += 4;
        } else {
            i += 1;
        }
        
        if (charCount == start) {
            byteStart = i;
        }
        charCount++;
    }
    
    if (length == std::string::npos) {
        return str.substr(byteStart);
    }
    
    // 找到结束字节位置
    size_t byteEnd = byteStart;
    size_t endCharCount = 0;
    
    for (size_t i = byteStart; i < str.size() && endCharCount < length; ) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        if (c < 0x80) {
            i += 1;
        } else if ((c >> 5) == 0x6) {
            i += 2;
        } else if ((c >> 4) == 0xe) {
            i += 3;
        } else if ((c >> 3) == 0x1e) {
            i += 4;
        } else {
            i += 1;
        }
        
        endCharCount++;
        byteEnd = i;
    }
    
    return str.substr(byteStart, byteEnd - byteStart);
}

std::vector<std::string> StringUtil::Split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    
    if (delimiter.empty()) {
        result.push_back(str);
        return result;
    }
    
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

std::string StringUtil::Join(const std::vector<std::string>& strings, const std::string& separator) {
    if (strings.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) {
            oss << separator;
        }
        oss << strings[i];
    }
    
    return oss.str();
}

std::string StringUtil::Trim(const std::string& str) {
    return TrimLeft(TrimRight(str));
}

std::string StringUtil::TrimLeft(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), IsWhitespace);
    return std::string(start, str.end());
}

std::string StringUtil::TrimRight(const std::string& str) {
    auto end = std::find_if_not(str.rbegin(), str.rend(), IsWhitespace);
    return std::string(str.begin(), end.base());
}

std::string StringUtil::Replace(const std::string& str, const std::string& from, 
                               const std::string& to, bool replaceAll) {
    if (from.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    
    do {
        pos = result.find(from, pos);
        if (pos != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
    } while (replaceAll && pos != std::string::npos);
    
    return result;
}

std::string StringUtil::ToUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtil::ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool StringUtil::StartsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

bool StringUtil::EndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

bool StringUtil::Contains(const std::string& str, const std::string& substring) {
    return str.find(substring) != std::string::npos;
}

std::string StringUtil::EscapeHTML(const std::string& str) {
    std::string result = str;
    result = Replace(result, "&", "&amp;", true);
    result = Replace(result, "<", "&lt;", true);
    result = Replace(result, ">", "&gt;", true);
    result = Replace(result, "\"", "&quot;", true);
    result = Replace(result, "'", "&#x27;", true);
    return result;
}

std::string StringUtil::EscapeCSS(const std::string& str) {
    std::string result = str;
    result = Replace(result, "\\", "\\\\", true);
    result = Replace(result, "\"", "\\\"", true);
    result = Replace(result, "'", "\\'", true);
    result = Replace(result, "\n", "\\n", true);
    result = Replace(result, "\r", "\\r", true);
    result = Replace(result, "\t", "\\t", true);
    return result;
}

std::string StringUtil::EscapeJavaScript(const std::string& str) {
    std::string result = str;
    result = Replace(result, "\\", "\\\\", true);
    result = Replace(result, "\"", "\\\"", true);
    result = Replace(result, "'", "\\'", true);
    result = Replace(result, "\n", "\\n", true);
    result = Replace(result, "\r", "\\r", true);
    result = Replace(result, "\t", "\\t", true);
    result = Replace(result, "\b", "\\b", true);
    result = Replace(result, "\f", "\\f", true);
    return result;
}

std::string StringUtil::GenerateRandomString(size_t length, const std::string& charset) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    
    return result;
}

size_t StringUtil::Hash(const std::string& str) {
    return std::hash<std::string>{}(str);
}

bool StringUtil::IsValidUTF8(const std::string& str) {
    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        if (c < 0x80) {
            i += 1; // ASCII字符
        } else if ((c >> 5) == 0x6) {
            if (i + 1 >= str.size()) return false;
            i += 2; // 2字节UTF-8字符
        } else if ((c >> 4) == 0xe) {
            if (i + 2 >= str.size()) return false;
            i += 3; // 3字节UTF-8字符
        } else if ((c >> 3) == 0x1e) {
            if (i + 3 >= str.size()) return false;
            i += 4; // 4字节UTF-8字符
        } else {
            return false; // 无效UTF-8序列
        }
    }
    return true;
}

std::string StringUtil::ConvertEncoding(const std::string& str, const std::string& fromEncoding, const std::string& toEncoding) {
    // 简化实现，主要处理UTF-8
    if (fromEncoding == "UTF-8" && toEncoding == "UTF-8") {
        return str;
    }
    
    // TODO: 实现其他编码转换
    return str;
}

bool StringUtil::IsWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool StringUtil::IsUTF8CharBoundary(const std::string& str, size_t pos) {
    if (pos >= str.size()) {
        return true;
    }
    
    unsigned char c = static_cast<unsigned char>(str[pos]);
    return (c & 0x80) == 0 || (c & 0xC0) == 0xC0;
}

} // namespace Util
} // namespace CHTL