#include "TextNode.h"
#include "Visitor.h"
#include <regex>
#include <sstream>
#include <codecvt>
#include <locale>

namespace CHTL {

TextNode::TextNode(const std::string& textContent, size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::TEXT_NODE, "text", line, column),
      m_TextContent(textContent), m_IsQuoted(false), m_QuoteChar('\0'), 
      m_IsUnquotedLiteral(false), m_ContainsUTF8(false) {
    
    SetContent(textContent);
    AnalyzeTextType();
    DetectUTF8();
}

void TextNode::SetTextContent(const std::string& content) {
    m_TextContent = content;
    SetContent(content);
    AnalyzeTextType();
    DetectUTF8();
}

void TextNode::AnalyzeTextType() {
    if (m_TextContent.empty()) {
        return;
    }
    
    // 检查是否为引号字符串
    if ((m_TextContent.front() == '"' && m_TextContent.back() == '"') ||
        (m_TextContent.front() == '\'' && m_TextContent.back() == '\'')) {
        m_IsQuoted = true;
        m_QuoteChar = m_TextContent.front();
        m_IsUnquotedLiteral = false;
    }
    else {
        // 检查是否为无修饰字面量
        std::regex unquotedPattern(R"([a-zA-Z0-9\u4e00-\u9fff\-_\s.,!?]+)");
        if (std::regex_match(m_TextContent, unquotedPattern)) {
            m_IsUnquotedLiteral = true;
            m_IsQuoted = false;
            m_QuoteChar = '\0';
        }
    }
}

void TextNode::DetectUTF8() {
    // 检测是否包含UTF-8字符（特别是中文字符）
    for (unsigned char c : m_TextContent) {
        if (c > 127) {
            m_ContainsUTF8 = true;
            break;
        }
    }
}

std::string TextNode::GetProcessedText() const {
    if (m_IsQuoted && m_TextContent.length() >= 2) {
        // 去除引号
        std::string content = m_TextContent.substr(1, m_TextContent.length() - 2);
        
        // 处理转义字符
        return ProcessEscapeSequences(content);
    }
    
    return m_TextContent;
}

std::string TextNode::GetHTMLSafeText() const {
    std::string processedText = GetProcessedText();
    return EscapeHTML(processedText);
}

bool TextNode::ValidateUTF8() const {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(m_TextContent);
        return true;
    }
    catch (const std::range_error&) {
        return false;
    }
}

std::string TextNode::ProcessEscapeSequences(const std::string& text) const {
    std::string result = text;
    
    // 处理常见的转义字符
    std::regex escapePatterns[] = {
        std::regex(R"(\\n)"),      // 换行符
        std::regex(R"(\\t)"),      // 制表符
        std::regex(R"(\\r)"),      // 回车符
        std::regex(R"(\\\\)"),     // 反斜杠
        std::regex(R"(\\")"),      // 双引号
        std::regex(R"(\\')"),      // 单引号
    };
    
    std::string replacements[] = {
        "\n", "\t", "\r", "\\", "\"", "'"
    };
    
    for (size_t i = 0; i < 6; ++i) {
        result = std::regex_replace(result, escapePatterns[i], replacements[i]);
    }
    
    return result;
}

std::string TextNode::EscapeHTML(const std::string& text) const {
    std::string result = text;
    
    // HTML字符转义
    std::regex htmlEscapePatterns[] = {
        std::regex("&"),       // &符号
        std::regex("<"),       // 小于号
        std::regex(">"),       // 大于号
        std::regex("\""),      // 双引号
        std::regex("'"),       // 单引号
    };
    
    std::string htmlEscapeReplacements[] = {
        "&amp;", "&lt;", "&gt;", "&quot;", "&#39;"
    };
    
    for (size_t i = 0; i < 5; ++i) {
        result = std::regex_replace(result, htmlEscapePatterns[i], htmlEscapeReplacements[i]);
    }
    
    return result;
}

void TextNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitTextNode(this);
    }
}

std::unique_ptr<CHTLBaseNode> TextNode::Clone() const {
    auto cloned = std::make_unique<TextNode>(m_TextContent, GetLine(), GetColumn());
    
    // 复制基础属性
    cloned->SetNamespace(GetNamespace());
    cloned->m_IsQuoted = m_IsQuoted;
    cloned->m_QuoteChar = m_QuoteChar;
    cloned->m_IsUnquotedLiteral = m_IsUnquotedLiteral;
    cloned->m_ContainsUTF8 = m_ContainsUTF8;
    
    return cloned;
}

std::string TextNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "TextNode";
    
    if (m_IsQuoted) {
        oss << " [Quoted: " << m_QuoteChar << "]";
    }
    
    if (m_IsUnquotedLiteral) {
        oss << " [Unquoted Literal]";
    }
    
    if (m_ContainsUTF8) {
        oss << " [UTF-8]";
    }
    
    oss << " '" << m_TextContent.substr(0, 30);
    if (m_TextContent.length() > 30) {
        oss << "...";
    }
    oss << "'";
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    return oss.str();
}

} // namespace CHTL