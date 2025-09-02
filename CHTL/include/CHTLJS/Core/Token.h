#ifndef CHTL_CHTLJS_CORE_TOKEN_H
#define CHTL_CHTLJS_CORE_TOKEN_H

#include <string>
#include <ostream>

namespace CHTL {
namespace JS {

/**
 * CHTL JS词法单元类型
 */
enum class TokenType {
    // 字面量
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量
    NUMBER,              // 数字
    
    // CHTL JS特有关键字
    MODULE,              // module
    LOAD,                // load
    LISTEN,              // listen
    DELEGATE,            // delegate
    ANIMATE,             // animate
    VIR,                 // vir
    INEVERAWAY,          // iNeverAway
    UTIL,                // util
    CHANGE,              // change
    THEN,                // then
    
    // 操作符
    DOUBLE_LEFT_BRACE,   // {{
    DOUBLE_RIGHT_BRACE,  // }}
    ARROW,               // ->
    AMPERSAND_ARROW,     // &->
    COLON,               // :
    SEMICOLON,           // ;
    COMMA,               // ,
    DOT,                 // .
    EQUALS,              // =
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    AMPERSAND,           // &
    HASH,                // #
    
    // 注释
    SINGLE_COMMENT,      // //注释
    MULTI_COMMENT,       // /*注释*/
    
    // JavaScript关键字（部分）
    FUNCTION,            // function
    VAR,                 // var
    LET,                 // let
    CONST,               // const
    RETURN,              // return
    IF,                  // if
    ELSE,                // else
    FOR,                 // for
    WHILE,               // while
    
    // 特殊
    EOF_TOKEN,           // 文件结束
    UNKNOWN              // 未知词法单元
};

/**
 * CHTL JS词法单元
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
     * 判断是否为CHTL JS关键字
     */
    bool IsCHTLJSKeyword() const {
        return type >= TokenType::MODULE && type <= TokenType::THEN;
    }
    
    /**
     * 判断是否为字面量
     */
    bool IsLiteral() const {
        return type == TokenType::IDENTIFIER || 
               type == TokenType::STRING_LITERAL || 
               type == TokenType::NUMBER;
    }
    
    /**
     * 判断是否为注释
     */
    bool IsComment() const {
        return type == TokenType::SINGLE_COMMENT || 
               type == TokenType::MULTI_COMMENT;
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

} // namespace JS
} // namespace CHTL

#endif // CHTL_CHTLJS_CORE_TOKEN_H