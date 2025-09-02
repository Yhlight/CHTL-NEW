#include "Scanner/CHTLUnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(bool debug) 
    : debugMode(debug), currentLine(1), currentColumn(1) {
    Logger::SetDebugMode(debug);
}

std::string CHTLUnifiedScanner::ReadFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw RuntimeError("无法打开文件: " + filePath);
    }
    
    SetCurrentFile(filePath);
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLUnifiedScanner::IsCHTLKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "text", "style", "script", "use", "html5", "inherit", "delete",
        "insert", "after", "before", "replace", "at", "top", "bottom",
        "from", "as", "except", "[Template]", "[Custom]", "[Origin]",
        "[Import]", "[Namespace]", "[Configuration]", "[Info]", "[Export]"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool CHTLUnifiedScanner::IsCHTLJSSyntax(const std::string& content, size_t pos) const {
    // 检查CHTL JS特有语法
    if (pos + 1 < content.length()) {
        // 检查 {{选择器}}
        if (content[pos] == '{' && content[pos + 1] == '{') {
            return true;
        }
        // 检查 -> 操作符
        if (content[pos] == '-' && content[pos + 1] == '>') {
            return true;
        }
        // 检查 &-> 操作符
        if (pos > 0 && content[pos - 1] == '&' && content[pos] == '-' && content[pos + 1] == '>') {
            return true;
        }
    }
    
    // 检查CHTL JS函数
    static const std::vector<std::string> chtljsFunctions = {
        "module", "listen", "delegate", "animate", "vir", "iNeverAway", "util"
    };
    
    for (const auto& func : chtljsFunctions) {
        if (pos + func.length() <= content.length()) {
            if (content.substr(pos, func.length()) == func) {
                // 检查后面是否跟着空白字符或 {
                if (pos + func.length() < content.length()) {
                    char nextChar = content[pos + func.length()];
                    if (std::isspace(nextChar) || nextChar == '{') {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::HandleComment(const std::string& content, size_t& pos, ScanContext& ctx) {
    if (pos + 1 < content.length()) {
        // 单行注释 //
        if (content[pos] == '/' && content[pos + 1] == '/') {
            ctx.buffer += content[pos];
            pos++;
            while (pos < content.length() && content[pos] != '\n') {
                ctx.buffer += content[pos];
                UpdatePosition(content[pos]);
                pos++;
            }
            return true;
        }
        
        // 多行注释 /* */
        if (content[pos] == '/' && content[pos + 1] == '*') {
            ctx.buffer += content[pos];
            ctx.buffer += content[pos + 1];
            pos += 2;
            UpdatePosition('/');
            UpdatePosition('*');
            
            while (pos + 1 < content.length()) {
                if (content[pos] == '*' && content[pos + 1] == '/') {
                    ctx.buffer += content[pos];
                    ctx.buffer += content[pos + 1];
                    UpdatePosition('*');
                    UpdatePosition('/');
                    pos += 2;
                    return true;
                }
                ctx.buffer += content[pos];
                UpdatePosition(content[pos]);
                pos++;
            }
        }
        
        // 生成器注释 --
        if (content[pos] == '-' && content[pos + 1] == '-') {
            ctx.buffer += content[pos];
            pos++;
            while (pos < content.length() && content[pos] != '\n') {
                ctx.buffer += content[pos];
                UpdatePosition(content[pos]);
                pos++;
            }
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::HandleString(const std::string& content, size_t& pos, ScanContext& ctx) {
    if (content[pos] == '"' || content[pos] == '\'') {
        if (ctx.state != ScanState::InString) {
            ctx.state = ScanState::InString;
            ctx.stringDelimiter = content[pos];
        } else if (content[pos] == ctx.stringDelimiter) {
            // 检查转义字符
            int backslashCount = 0;
            int checkPos = pos - 1;
            while (checkPos >= 0 && content[checkPos] == '\\') {
                backslashCount++;
                checkPos--;
            }
            if (backslashCount % 2 == 0) {
                ctx.state = ScanState::Normal;
                ctx.stringDelimiter = 0;
            }
        }
        return true;
    }
    return false;
}

CodeFragmentPtr CHTLUnifiedScanner::CreateFragment(FragmentType type, const std::string& content,
                                                   size_t startLine, size_t startCol,
                                                   size_t endLine, size_t endCol) {
    auto fragment = std::make_shared<CodeFragment>(type);
    fragment->content = content;
    fragment->startLine = startLine;
    fragment->startColumn = startCol;
    fragment->endLine = endLine;
    fragment->endColumn = endCol;
    fragment->sourcePath = currentFile;
    return fragment;
}

void CHTLUnifiedScanner::UpdatePosition(char ch) {
    if (ch == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
}

FragmentType CHTLUnifiedScanner::DetermineFragmentType(const std::string& content) const {
    // 去除首尾空白
    std::string trimmed = content;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.empty()) {
        return FragmentType::Unknown;
    }
    
    // 检查是否包含CHTL JS语法
    for (size_t i = 0; i < trimmed.length(); ++i) {
        if (IsCHTLJSSyntax(trimmed, i)) {
            return FragmentType::CHTLJS;
        }
    }
    
    // 检查CHTL特有关键字
    if (trimmed.find("use html5") != std::string::npos ||
        trimmed.find("use @Config") != std::string::npos ||
        trimmed.find("text {") != std::string::npos ||
        trimmed.find("@Style") != std::string::npos ||
        trimmed.find("@Element") != std::string::npos ||
        trimmed.find("@Var") != std::string::npos ||
        trimmed.find("[Template]") != std::string::npos ||
        trimmed.find("[Custom]") != std::string::npos ||
        trimmed.find("[Origin]") != std::string::npos ||
        trimmed.find("[Import]") != std::string::npos ||
        trimmed.find("[Namespace]") != std::string::npos ||
        trimmed.find("[Configuration]") != std::string::npos) {
        return FragmentType::CHTL;
    }
    
    // 检查是否有HTML元素后跟花括号（CHTL特征）
    std::regex htmlElementPattern(R"(\b(html|head|body|div|span|p|h[1-6]|a|button|input|form|table|tr|td|th|ul|ol|li|img|video|audio|canvas|svg|header|footer|nav|main|section|article|aside)\s*\{)");
    if (std::regex_search(trimmed, htmlElementPattern)) {
        return FragmentType::CHTL;
    }
    
    // 检查是否为纯CSS代码
    // CSS通常以选择器开始，例如 .class、#id、element
    std::regex cssStartPattern(R"(^\s*[\.#][\w-]+\s*\{|^\s*[\w-]+\s*\{)");
    if (std::regex_search(trimmed, cssStartPattern) &&
        trimmed.find(':') != std::string::npos &&
        trimmed.find(';') != std::string::npos) {
        return FragmentType::CSS;
    }
    
    // 检查是否为JavaScript代码
    if (trimmed.find("function") != std::string::npos ||
        trimmed.find("var ") != std::string::npos ||
        trimmed.find("let ") != std::string::npos ||
        trimmed.find("const ") != std::string::npos ||
        trimmed.find("=>") != std::string::npos) {
        return FragmentType::JavaScript;
    }
    
    // 默认为CHTL
    return FragmentType::CHTL;
}

CodeFragmentList CHTLUnifiedScanner::Scan(const std::string& content) {
    CodeFragmentList fragments;
    ScanContext ctx;
    
    currentLine = 1;
    currentColumn = 1;
    
    size_t pos = 0;
    ctx.startLine = currentLine;
    ctx.startColumn = currentColumn;
    
    while (pos < content.length()) {
        char ch = content[pos];
        
        // 处理字符串
        if (ctx.state == ScanState::InString || ch == '"' || ch == '\'') {
            HandleString(content, pos, ctx);
            ctx.buffer += ch;
            UpdatePosition(ch);
            pos++;
            continue;
        }
        
        // 处理注释
        if (HandleComment(content, pos, ctx)) {
            continue;
        }
        
        // 处理花括号深度
        if (ch == '{') {
            ctx.braceDepth++;
            
            // 检查是否进入局部样式块或脚本块
            if (ctx.state == ScanState::Normal || ctx.state == ScanState::InCHTL) {
                std::string prevWord;
                int checkPos = ctx.buffer.length() - 1;
                while (checkPos >= 0 && std::isspace(ctx.buffer[checkPos])) {
                    checkPos--;
                }
                
                // 提取前一个单词
                int wordEnd = checkPos;
                while (checkPos >= 0 && !std::isspace(ctx.buffer[checkPos])) {
                    checkPos--;
                }
                if (wordEnd >= 0 && checkPos < wordEnd) {
                    prevWord = ctx.buffer.substr(checkPos + 1, wordEnd - checkPos);
                }
                
                // 局部样式和脚本块不需要分离，保持在CHTL片段中
                ctx.buffer += ch;
            } else {
                ctx.buffer += ch;
            }
        } else if (ch == '}') {
            ctx.buffer += ch;
            ctx.braceDepth--;
            
            // 花括号深度为0时不需要特殊处理，因为我们不再分离局部样式/脚本块
        } else {
            ctx.buffer += ch;
        }
        
        UpdatePosition(ch);
        pos++;
    }
    
    // 处理剩余的缓冲区内容
    if (!ctx.buffer.empty()) {
        FragmentType type = DetermineFragmentType(ctx.buffer);
        auto fragment = CreateFragment(type, ctx.buffer,
                                     ctx.startLine, ctx.startColumn,
                                     currentLine, currentColumn);
        fragments.push_back(fragment);
    }
    
    // 日志输出
    if (debugMode) {
        LOG_INFO("扫描完成，共生成 " + std::to_string(fragments.size()) + " 个代码片段");
        for (size_t i = 0; i < fragments.size(); ++i) {
            LOG_INFO("片段 " + std::to_string(i + 1) + ": 类型=" + 
                     fragments[i]->GetTypeString() + 
                     ", 行=" + std::to_string(fragments[i]->startLine) + "-" + 
                     std::to_string(fragments[i]->endLine) +
                     ", 长度=" + std::to_string(fragments[i]->content.length()));
            if (debugMode) {
                LOG_DEBUG("片段内容预览: " + fragments[i]->content.substr(0, 100) + "...");
            }
        }
    }
    
    return fragments;
}

} // namespace CHTL