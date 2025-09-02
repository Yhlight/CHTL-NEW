#ifndef CHTL_SCANNER_CHTLUNIFIEDSCANNER_H
#define CHTL_SCANNER_CHTLUNIFIEDSCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "Common/CodeFragment.h"
#include "Common/Error.h"
#include "Common/Logger.h"

namespace CHTL {

/**
 * CHTL统一扫描器
 * 负责精准代码切割，将源代码切分为不同类型的代码片段
 */
class CHTLUnifiedScanner {
private:
    bool debugMode;
    std::string currentFile;
    size_t currentLine;
    size_t currentColumn;
    
    // 扫描状态
    enum class ScanState {
        Normal,           // 普通状态
        InCHTL,          // 在CHTL代码中
        InCHTLJS,        // 在CHTL JS代码中
        InCSS,           // 在CSS代码中
        InJavaScript,    // 在JavaScript代码中
        InComment,       // 在注释中
        InString,        // 在字符串中
        InLocalStyle,    // 在局部样式块中
        InLocalScript    // 在局部脚本块中
    };
    
    struct ScanContext {
        ScanState state;
        size_t startLine;
        size_t startColumn;
        std::string buffer;
        char stringDelimiter;  // 字符串定界符 ' 或 "
        int braceDepth;        // 花括号深度
        
        ScanContext() : state(ScanState::Normal), startLine(0), startColumn(0),
                       stringDelimiter(0), braceDepth(0) {}
    };
    
    /**
     * 检查是否为CHTL关键字
     */
    bool IsCHTLKeyword(const std::string& word) const;
    
    /**
     * 检查是否为CHTL JS语法
     */
    bool IsCHTLJSSyntax(const std::string& content, size_t pos) const;
    
    /**
     * 处理注释
     */
    bool HandleComment(const std::string& content, size_t& pos, ScanContext& ctx);
    
    /**
     * 处理字符串
     */
    bool HandleString(const std::string& content, size_t& pos, ScanContext& ctx);
    
    /**
     * 处理CHTL代码块
     */
    bool HandleCHTLBlock(const std::string& content, size_t& pos, ScanContext& ctx,
                         CodeFragmentList& fragments);
    
    /**
     * 处理局部样式块
     */
    bool HandleLocalStyle(const std::string& content, size_t& pos, ScanContext& ctx,
                          CodeFragmentList& fragments);
    
    /**
     * 处理局部脚本块
     */
    bool HandleLocalScript(const std::string& content, size_t& pos, ScanContext& ctx,
                           CodeFragmentList& fragments);
    
    /**
     * 切割代码片段
     */
    void SplitFragment(const std::string& content, ScanContext& ctx,
                       CodeFragmentList& fragments);
    
    /**
     * 判断片段类型
     */
    FragmentType DetermineFragmentType(const std::string& content) const;
    
    /**
     * 创建代码片段
     */
    CodeFragmentPtr CreateFragment(FragmentType type, const std::string& content,
                                   size_t startLine, size_t startCol,
                                   size_t endLine, size_t endCol);
    
    /**
     * 更新位置信息
     */
    void UpdatePosition(char ch);
    
public:
    explicit CHTLUnifiedScanner(bool debug = false);
    ~CHTLUnifiedScanner() = default;
    
    /**
     * 读取文件内容
     */
    std::string ReadFile(const std::string& filePath);
    
    /**
     * 扫描并切割代码
     * @param content 源代码内容
     * @return 切割后的代码片段列表
     */
    CodeFragmentList Scan(const std::string& content);
    
    /**
     * 设置当前处理的文件路径
     */
    void SetCurrentFile(const std::string& filePath) {
        currentFile = filePath;
    }
    
    /**
     * 获取当前处理的文件路径
     */
    const std::string& GetCurrentFile() const {
        return currentFile;
    }
};

} // namespace CHTL

#endif // CHTL_SCANNER_CHTLUNIFIEDSCANNER_H