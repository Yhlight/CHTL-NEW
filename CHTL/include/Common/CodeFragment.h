#ifndef CHTL_COMMON_CODEFRAGMENT_H
#define CHTL_COMMON_CODEFRAGMENT_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 代码片段类型枚举
 */
enum class FragmentType {
    CHTL,        // CHTL代码片段
    CHTLJS,      // CHTL JS代码片段
    CSS,         // CSS代码片段
    JavaScript,  // JavaScript代码片段
    HTML,        // HTML代码片段
    Unknown      // 未知类型
};

/**
 * 代码片段结构
 */
struct CodeFragment {
    FragmentType type;           // 片段类型
    std::string content;         // 片段内容
    size_t startLine;           // 起始行号
    size_t startColumn;         // 起始列号
    size_t endLine;             // 结束行号
    size_t endColumn;           // 结束列号
    std::string sourcePath;     // 源文件路径
    
    CodeFragment(FragmentType t = FragmentType::Unknown) 
        : type(t), startLine(0), startColumn(0), endLine(0), endColumn(0) {}
    
    /**
     * 获取片段类型的字符串表示
     */
    std::string GetTypeString() const {
        switch (type) {
            case FragmentType::CHTL: return "CHTL";
            case FragmentType::CHTLJS: return "CHTLJS";
            case FragmentType::CSS: return "CSS";
            case FragmentType::JavaScript: return "JavaScript";
            case FragmentType::HTML: return "HTML";
            default: return "Unknown";
        }
    }
    
    /**
     * 判断是否为空片段
     */
    bool IsEmpty() const {
        return content.empty();
    }
};

using CodeFragmentPtr = std::shared_ptr<CodeFragment>;
using CodeFragmentList = std::vector<CodeFragmentPtr>;

/**
 * 创建代码片段的辅助函数
 */
inline CodeFragmentPtr MakeCodeFragment(FragmentType type, const std::string& content) {
    auto fragment = std::make_shared<CodeFragment>(type);
    fragment->content = content;
    return fragment;
}

} // namespace CHTL

#endif // CHTL_COMMON_CODEFRAGMENT_H