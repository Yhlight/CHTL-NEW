#ifndef CHTL_CHTL_CORE_TOKEN_H
#define CHTL_CHTL_CORE_TOKEN_H

#include <string>
#include <ostream>

namespace CHTL {

/**
 * CHTL词法单元类型
 */
enum class TokenType {
    // 字面量
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量 "..." 或 '...'
    UNQUOTED_LITERAL,    // 无修饰字面量
    NUMBER,              // 数字
    
    // 关键字
    TEXT,                // text
    STYLE,               // style
    SCRIPT,              // script
    USE,                 // use
    HTML5,               // html5
    INHERIT,             // inherit
    DELETE,              // delete
    INSERT,              // insert
    AFTER,               // after
    BEFORE,              // before
    REPLACE,             // replace
    AT,                  // at
    TOP,                 // top
    BOTTOM,              // bottom
    FROM,                // from
    AS,                  // as
    EXCEPT,              // except
    
    // 模板和自定义
    TEMPLATE,            // [Template]
    CUSTOM,              // [Custom]
    ORIGIN,              // [Origin]
    IMPORT,              // [Import]
    NAMESPACE,           // [Namespace]
    CONFIGURATION,       // [Configuration]
    INFO,                // [Info]
    EXPORT,              // [Export]
    
    // @前缀关键字
    AT_STYLE,            // @Style
    AT_ELEMENT,          // @Element
    AT_VAR,              // @Var
    AT_HTML,             // @Html
    AT_JAVASCRIPT,       // @JavaScript
    AT_CHTL,             // @Chtl
    AT_CJMOD,            // @CJmod
    AT_CONFIG,           // @Config
    
    // 操作符
    COLON,               // :
    SEMICOLON,           // ;
    COMMA,               // ,
    DOT,                 // .
    EQUALS,              // =
    HASH,                // #
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    AMPERSAND,           // &
    ARROW,               // ->
    DOUBLE_DASH,         // --
    
    // 注释
    SINGLE_COMMENT,      // //注释
    MULTI_COMMENT,       // /*注释*/
    GENERATOR_COMMENT,   // --注释
    
    // 特殊
    NEWLINE,             // 换行符
    EOF_TOKEN,           // 文件结束
    UNKNOWN              // 未知词法单元
};

/**
 * CHTL词法单元
 */
struct Token {
    TokenType type;      // 词法单元类型
    std::string value;   // 词法单元值
    size_t line;         // 行号
    size_t column;       // 列号
    size_t length;       // 长度
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "",
          size_t l = 0, size_t c = 0, size_t len = 0)
        : type(t), value(v), line(l), column(c), length(len) {}
    
    /**
     * 判断是否为关键字
     */
    bool IsKeyword() const {
        return type >= TokenType::TEXT && type <= TokenType::EXCEPT;
    }
    
    /**
     * 判断是否为字面量
     */
    bool IsLiteral() const {
        return type == TokenType::IDENTIFIER || 
               type == TokenType::STRING_LITERAL || 
               type == TokenType::UNQUOTED_LITERAL ||
               type == TokenType::NUMBER;
    }
    
    /**
     * 判断是否为注释
     */
    bool IsComment() const {
        return type == TokenType::SINGLE_COMMENT || 
               type == TokenType::MULTI_COMMENT || 
               type == TokenType::GENERATOR_COMMENT;
    }
    
    /**
     * 获取类型字符串
     */
    std::string GetTypeString() const;
    
    /**
     * 输出Token信息
     */
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

} // namespace CHTL

#endif // CHTL_CHTL_CORE_TOKEN_H