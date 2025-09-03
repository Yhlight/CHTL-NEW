#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace CHTL {

    CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
        : sourceCode(source), currentPos(0), currentLine(1), currentColumn(1) {
    }

    std::vector<CodeChunk> CHTLUnifiedScanner::Scan() {
        chunks.clear();
        currentPos = 0;
        currentLine = 1;
        currentColumn = 1;

        while (!IsAtEnd()) {
            size_t startPos = currentPos;
            int startLine = currentLine;
            int startColumn = currentColumn;
            
            // 跳过空白字符
            while (!IsAtEnd() && std::isspace(PeekChar())) {
                AdvancePosition(PeekChar());
            }
            
            if (IsAtEnd()) break;
            
            size_t endPos;
            CodeType detectedType = CodeType::CHTL; // 默认类型
            
            // 尝试识别代码块类型
            if (ScanCHTLBlock(endPos)) {
                detectedType = CodeType::CHTL;
            } else if (ScanCHTLJSBlock(endPos)) {
                detectedType = CodeType::CHTL_JS;
            } else if (ScanCSSBlock(endPos)) {
                detectedType = CodeType::CSS;
            } else if (ScanJSBlock(endPos)) {
                detectedType = CodeType::JS;
            } else {
                // 默认作为CHTL处理
                endPos = FindNextBlockBoundary();
                detectedType = CodeType::CHTL;
            }
            
            // 提取代码片段
            std::string content = sourceCode.substr(currentPos, endPos - currentPos);
            
            // 执行最小语法单元切割（如果需要）
            if (detectedType == CodeType::CHTL || detectedType == CodeType::CHTL_JS) {
                PerformMinimalUnitCutting(content);
            }
            
            // 创建代码片段
            if (!content.empty()) {
                chunks.emplace_back(detectedType, content, currentPos, endPos, startLine, startColumn);
            }
            
            // 更新位置
            currentPos = endPos;
        }

        return chunks;
    }

    bool CHTLUnifiedScanner::ScanCHTLBlock(size_t& endPos) {
        std::string preview = PeekString(50); // 预览前50个字符
        
        // 检查CHTL特有语法
        if (IsCHTLKeyword(preview)) {
            endPos = FindCHTLBlockEnd();
            return true;
        }
        
        return false;
    }

    bool CHTLUnifiedScanner::ScanCHTLJSBlock(size_t& endPos) {
        std::string preview = PeekString(50);
        
        // 检查CHTL JS特有语法
        if (IsCHTLJSKeyword(preview)) {
            endPos = FindCHTLJSBlockEnd();
            return true;
        }
        
        return false;
    }

    bool CHTLUnifiedScanner::ScanCSSBlock(size_t& endPos) {
        // 简单的CSS块检测
        std::string preview = PeekString(20);
        
        // 检查是否包含CSS特征
        if (preview.find("style") != std::string::npos && preview.find("{") != std::string::npos) {
            endPos = FindCSSBlockEnd();
            return true;
        }
        
        return false;
    }

    bool CHTLUnifiedScanner::ScanJSBlock(size_t& endPos) {
        // 简单的JS块检测
        std::string preview = PeekString(20);
        
        // 检查是否包含JS特征
        if (preview.find("script") != std::string::npos && preview.find("{") != std::string::npos) {
            endPos = FindJSBlockEnd();
            return true;
        }
        
        return false;
    }

    bool CHTLUnifiedScanner::IsCHTLKeyword(const std::string& text) const {
        // 检查CHTL关键字
        return text.find("[Template]") != std::string::npos ||
               text.find("[Custom]") != std::string::npos ||
               text.find("[Origin]") != std::string::npos ||
               text.find("[Import]") != std::string::npos ||
               text.find("[Configuration]") != std::string::npos ||
               text.find("[Namespace]") != std::string::npos ||
               text.find("@Style") != std::string::npos ||
               text.find("@Element") != std::string::npos ||
               text.find("@Var") != std::string::npos ||
               text.find("inherit") != std::string::npos ||
               text.find("delete") != std::string::npos ||
               text.find("insert") != std::string::npos ||
               text.find("except") != std::string::npos ||
               text.find("use") != std::string::npos;
    }

    bool CHTLUnifiedScanner::IsCHTLJSKeyword(const std::string& text) const {
        // 检查CHTL JS关键字
        return text.find("module") != std::string::npos ||
               text.find("load") != std::string::npos ||
               text.find("listen") != std::string::npos ||
               text.find("delegate") != std::string::npos ||
               text.find("animate") != std::string::npos ||
               text.find("vir") != std::string::npos ||
               text.find("util") != std::string::npos ||
               text.find("{{") != std::string::npos ||
               text.find("->") != std::string::npos ||
               text.find("&->") != std::string::npos;
    }

    // 辅助方法实现
    void CHTLUnifiedScanner::AdvancePosition(char c) {
        if (c == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        currentPos++;
    }

    bool CHTLUnifiedScanner::IsAtEnd() const {
        return currentPos >= sourceCode.length();
    }

    char CHTLUnifiedScanner::PeekChar(size_t offset) const {
        size_t pos = currentPos + offset;
        return (pos < sourceCode.length()) ? sourceCode[pos] : '\0';
    }

    std::string CHTLUnifiedScanner::PeekString(size_t length) const {
        size_t end = std::min(currentPos + length, sourceCode.length());
        return sourceCode.substr(currentPos, end - currentPos);
    }

    size_t CHTLUnifiedScanner::FindNextBlockBoundary() {
        size_t pos = currentPos;
        int braceLevel = 0;
        
        while (pos < sourceCode.length()) {
            char c = sourceCode[pos];
            
            if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel <= 0) {
                    return pos + 1;
                }
            }
            
            pos++;
        }
        
        return sourceCode.length();
    }

    size_t CHTLUnifiedScanner::FindCHTLBlockEnd() {
        return FindNextBlockBoundary();
    }

    size_t CHTLUnifiedScanner::FindCHTLJSBlockEnd() {
        return FindNextBlockBoundary();
    }

    size_t CHTLUnifiedScanner::FindCSSBlockEnd() {
        return FindNextBlockBoundary();
    }

    size_t CHTLUnifiedScanner::FindJSBlockEnd() {
        return FindNextBlockBoundary();
    }

    void CHTLUnifiedScanner::PerformMinimalUnitCutting(std::string& fragment) {
        // 执行最小语法单元切割
        // 例如：{{box}}->click 需被切割为 {{box}}-> 与 click 两个单元
        // 当前为简化实现，后续可以根据需要完善
    }

    void CHTLUnifiedScanner::Reset() {
        chunks.clear();
        currentPos = 0;
        currentLine = 1;
        currentColumn = 1;
    }

    const std::vector<CodeChunk>& CHTLUnifiedScanner::GetChunks() const {
        return chunks;
    }

    void CHTLUnifiedScanner::PrintChunks() const {
        std::cout << "=== 统一扫描器结果 ===" << std::endl;
        for (size_t i = 0; i < chunks.size(); i++) {
            const auto& chunk = chunks[i];
            std::cout << "片段 " << i << ": " << GetChunkTypeString(chunk.type) << std::endl;
            std::cout << "  位置: " << chunk.startPos << "-" << chunk.endPos << std::endl;
            std::cout << "  行号: " << chunk.lineNumber << ", 列号: " << chunk.columnNumber << std::endl;
            std::cout << "  内容: " << chunk.content.substr(0, 100) << "..." << std::endl;
        }
    }

    std::string CHTLUnifiedScanner::GetChunkTypeString(CodeType type) const {
        switch (type) {
            case CodeType::CHTL: return "CHTL";
            case CodeType::CHTL_JS: return "CHTL_JS";
            case CodeType::CSS: return "CSS";
            case CodeType::JS: return "JS";
            default: return "UNKNOWN";
        }
    }

} // namespace CHTL