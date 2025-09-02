#pragma once

#include "CHTLBaseNode.h"

namespace CHTL {

/**
 * CHTL文本节点
 * 表示text{}文本块，支持各种字面量类型
 */
class TextNode : public CHTLBaseNode {
private:
    std::string m_TextContent;                  // 文本内容
    bool m_IsQuoted;                           // 是否为引号字符串
    char m_QuoteChar;                          // 引号字符（"或'）
    bool m_IsUnquotedLiteral;                  // 是否为无修饰字面量
    bool m_ContainsUTF8;                       // 是否包含UTF-8字符

public:
    /**
     * 构造函数
     * @param textContent 文本内容
     * @param line 行号
     * @param column 列号
     */
    TextNode(const std::string& textContent, size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~TextNode() override = default;
    
    /**
     * 获取文本内容
     * @return 文本内容
     */
    const std::string& GetTextContent() const { return m_TextContent; }
    
    /**
     * 设置文本内容
     * @param content 新文本内容
     */
    void SetTextContent(const std::string& content);
    
    /**
     * 检查是否为引号字符串
     * @return 是否为引号字符串
     */
    bool IsQuoted() const { return m_IsQuoted; }
    
    /**
     * 获取引号字符
     * @return 引号字符
     */
    char GetQuoteChar() const { return m_QuoteChar; }
    
    /**
     * 检查是否为无修饰字面量
     * @return 是否为无修饰字面量
     */
    bool IsUnquotedLiteral() const { return m_IsUnquotedLiteral; }
    
    /**
     * 检查是否包含UTF-8字符
     * @return 是否包含UTF-8字符
     */
    bool ContainsUTF8() const { return m_ContainsUTF8; }
    
    /**
     * 获取处理后的文本内容（去除引号、处理转义）
     * @return 处理后的文本
     */
    std::string GetProcessedText() const;
    
    /**
     * 获取HTML安全的文本内容（转义HTML特殊字符）
     * @return HTML安全的文本
     */
    std::string GetHTMLSafeText() const;
    
    /**
     * 验证UTF-8编码
     * @return 是否为有效UTF-8
     */
    bool ValidateUTF8() const;
    
    // 访问者模式实现
    void Accept(CHTLNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLBaseNode> Clone() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 分析文本类型
     */
    void AnalyzeTextType();
    
    /**
     * 检测UTF-8字符
     */
    void DetectUTF8();
    
    /**
     * 处理转义字符
     * @param text 原始文本
     * @return 处理后的文本
     */
    std::string ProcessEscapeSequences(const std::string& text) const;
    
    /**
     * HTML字符转义
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string EscapeHTML(const std::string& text) const;
};

} // namespace CHTL