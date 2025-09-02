#pragma once

#include <string>
#include <vector>

namespace CHTL_JS
{
    // 定义 CHTL JS 的所有 Token 类型
    enum class TokenType
    {
        // =================================================
        // 特殊类型
        // =================================================
        UNKNOWN,                // 未知类型
        END_OF_FILE,            // 文件结束

        // =================================================
        // 关键字
        // =================================================
        KEYWORD_MODULE,         // module
        KEYWORD_LOAD,           // load
        KEYWORD_LISTEN,         // listen
        KEYWORD_DELEGATE,       // delegate
        KEYWORD_ANIMATE,        // animate
        KEYWORD_VIR,            // vir

        // =================================================
        // 标识符和字面量
        // =================================================
        IDENTIFIER,             // 变量名, 函数名等
        STRING_LITERAL,         // "..." or '...'
        NUMBER_LITERAL,         // 123, 0.5

        // =================================================
        // 符号
        // =================================================
        L_BRACE,                // {
        R_BRACE,                // }
        L_PAREN,                // (
        R_PAREN,                // )
        L_BRACKET,              // [
        R_BRACKET,              // ]
        COLON,                  // :
        EQUAL,                  // =
        SEMICOLON,              // ;
        COMMA,                  // ,
        DOT,                    // .
        ARROW,                  // ->
        BIND_ARROW,             // &->

        // =================================================
        // CHTL JS 特有符号
        // =================================================
        CSS_SELECTOR_START,     // {{
        CSS_SELECTOR_END,       // }}
        CSS_SELECTOR_CONTENT,   // {{...}} 内部的内容
    };

    // Token 结构体
    struct Token
    {
        TokenType type = TokenType::UNKNOWN;
        std::string value;
        size_t line = 0;
        size_t column = 0;
    };
}