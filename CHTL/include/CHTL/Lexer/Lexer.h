#ifndef CHTL_CHTL_LEXER_LEXER_H
#define CHTL_CHTL_LEXER_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "CHTL/Core/Token.h"
#include "Common/Error.h"

namespace CHTL {

/**
 * CHTL词法分析器
 * 负责将源代码转换为Token序列
 */
class Lexer {
private:
    std::string source;              // 源代码
    std::string filename;            // 文件名
    size_t position;                 // 当前位置
    size_t line;                     // 当前行号
    size_t column;                   // 当前列号
    std::vector<Token> tokens;       // Token列表
    
    // 关键字映射表
    static const std::unordered_map<std::string, TokenType> keywordMap;
    static const std::unordered_map<std::string, TokenType> atKeywordMap;
    static const std::unordered_map<std::string, TokenType> bracketKeywordMap;
    
    /**
     * 获取当前字符
     */
    char CurrentChar() const {
        return position < source.length() ? source[position] : '\0';
    }
    
    /**
     * 获取下一个字符（不移动位置）
     */
    char PeekChar(size_t offset = 1) const {
        size_t pos = position + offset;
        return pos < source.length() ? source[pos] : '\0';
    }
    
    /**
     * 前进一个字符
     */
    void Advance() {
        if (position < source.length()) {
            if (source[position] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            position++;
        }
    }
    
    /**
     * 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * 扫描单行注释
     */
    Token ScanSingleLineComment();
    
    /**
     * 扫描多行注释
     */
    Token ScanMultiLineComment();
    
    /**
     * 扫描生成器注释
     */
    Token ScanGeneratorComment();
    
    /**
     * 扫描字符串字面量
     */
    Token ScanStringLiteral(char delimiter);
    
    /**
     * 扫描标识符或关键字
     */
    Token ScanIdentifierOrKeyword();
    
    /**
     * 扫描数字
     */
    Token ScanNumber();
    
    /**
     * 扫描无修饰字面量
     */
    Token ScanUnquotedLiteral();
    
    /**
     * 扫描十六进制颜色值
     */
    Token ScanHexColor();
    
    /**
     * 扫描@前缀关键字
     */
    Token ScanAtKeyword();
    
    /**
     * 扫描[]包裹的关键字
     */
    Token ScanBracketKeyword();
    
    /**
     * 扫描操作符
     */
    Token ScanOperator();
    
    /**
     * 判断是否为标识符起始字符
     */
    bool IsIdentifierStart(char ch) const {
        return std::isalpha(ch) || ch == '_' || ch == '$';
    }
    
    /**
     * 判断是否为标识符字符
     */
    bool IsIdentifierChar(char ch) const {
        return std::isalnum(ch) || ch == '_' || ch == '$' || ch == '-';
    }
    
    /**
     * 判断是否为无修饰字面量字符
     */
    bool IsUnquotedLiteralChar(char ch) const {
        // 无修饰字面量可以包含更多字符，但不包括某些特殊字符
        return ch != ';' && ch != ':' && ch != '=' && ch != '{' && ch != '}' && 
               ch != '[' && ch != ']' && ch != '(' && ch != ')' && ch != ',' &&
               ch != '\n' && ch != '\r' && ch != '\0';
    }
    
    /**
     * 创建Token
     */
    Token MakeToken(TokenType type, const std::string& value, 
                    size_t startLine, size_t startColumn, size_t length);
    
    /**
     * 报告词法错误
     */
    void ReportError(const std::string& message);
    
public:
    /**
     * 构造函数
     */
    explicit Lexer(const std::string& src, const std::string& fname = "");
    
    /**
     * 执行词法分析
     */
    void Tokenize();
    
    /**
     * 获取下一个Token
     */
    Token NextToken();
    
    /**
     * 获取所有Token
     */
    const std::vector<Token>& GetTokens() const { return tokens; }
    
    /**
     * 重置词法分析器
     */
    void Reset();
    
    /**
     * 设置源代码
     */
    void SetSource(const std::string& src, const std::string& fname = "");
};

} // namespace CHTL

#endif // CHTL_CHTL_LEXER_LEXER_H