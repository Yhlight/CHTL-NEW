#include "Scanner/CHTLUnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

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
    
    // 检查是否为CSS代码（简单判断）
    if (trimmed.find('{') != std::string::npos && 
        trimmed.find('}') != std::string::npos &&
        trimmed.find(':') != std::string::npos &&
        trimmed.find(';') != std::string::npos) {
        // 检查是否有CHTL特有语法
        if (trimmed.find("@Style") != std::string::npos ||
            trimmed.find("@Element") != std::string::npos ||
            trimmed.find("@Var") != std::string::npos ||
            trimmed.find("[Template]") != std::string::npos ||
            trimmed.find("[Custom]") != std::string::npos) {
            return FragmentType::CHTL;
        }
        return FragmentType::CSS;
    }
    
    // 检查是否为JavaScript代码（简单判断）
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
                
                if (prevWord == "style") {
                    // 保存当前缓冲区内容（不包括style）
                    std::string beforeStyle = ctx.buffer.substr(0, checkPos + 1);
                    if (!beforeStyle.empty()) {
                        auto fragment = CreateFragment(FragmentType::CHTL, beforeStyle,
                                                     ctx.startLine, ctx.startColumn,
                                                     currentLine, currentColumn);
                        fragments.push_back(fragment);
                    }
                    
                    // 开始新的局部样式块
                    ctx.state = ScanState::InLocalStyle;
                    ctx.buffer = "style{";
                    ctx.startLine = currentLine;
                    ctx.startColumn = currentColumn - 5; // "style"的长度
                    ctx.braceDepth = 1;
                } else if (prevWord == "script") {
                    // 保存当前缓冲区内容（不包括script）
                    std::string beforeScript = ctx.buffer.substr(0, checkPos + 1);
                    if (!beforeScript.empty()) {
                        auto fragment = CreateFragment(FragmentType::CHTL, beforeScript,
                                                     ctx.startLine, ctx.startColumn,
                                                     currentLine, currentColumn);
                        fragments.push_back(fragment);
                    }
                    
                    // 开始新的局部脚本块
                    ctx.state = ScanState::InLocalScript;
                    ctx.buffer = "script{";
                    ctx.startLine = currentLine;
                    ctx.startColumn = currentColumn - 6; // "script"的长度
                    ctx.braceDepth = 1;
                } else {
                    ctx.buffer += ch;
                }
            } else {
                ctx.buffer += ch;
            }
        } else if (ch == '}') {
            ctx.buffer += ch;
            ctx.braceDepth--;
            
            // 检查是否结束局部样式块或脚本块
            if (ctx.braceDepth == 0) {
                if (ctx.state == ScanState::InLocalStyle) {
                    auto fragment = CreateFragment(FragmentType::CHTL, ctx.buffer,
                                                 ctx.startLine, ctx.startColumn,
                                                 currentLine, currentColumn);
                    fragments.push_back(fragment);
                    ctx.buffer.clear();
                    ctx.state = ScanState::Normal;
                    ctx.startLine = currentLine;
                    ctx.startColumn = currentColumn + 1;
                } else if (ctx.state == ScanState::InLocalScript) {
                    // 局部脚本块需要进一步分析是否包含CHTL JS语法
                    FragmentType scriptType = DetermineFragmentType(ctx.buffer);
                    auto fragment = CreateFragment(scriptType, ctx.buffer,
                                                 ctx.startLine, ctx.startColumn,
                                                 currentLine, currentColumn);
                    fragments.push_back(fragment);
                    ctx.buffer.clear();
                    ctx.state = ScanState::Normal;
                    ctx.startLine = currentLine;
                    ctx.startColumn = currentColumn + 1;
                }
            }
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
        LOG_DEBUG("扫描完成，共生成 " + std::to_string(fragments.size()) + " 个代码片段");
        for (size_t i = 0; i < fragments.size(); ++i) {
            LOG_DEBUG("片段 " + std::to_string(i + 1) + ": 类型=" + 
                     fragments[i]->GetTypeString() + 
                     ", 行=" + std::to_string(fragments[i]->startLine) + "-" + 
                     std::to_string(fragments[i]->endLine));
        }
    }
    
    return fragments;
}

} // namespace CHTL