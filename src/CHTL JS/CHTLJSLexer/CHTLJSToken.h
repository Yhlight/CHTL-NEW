#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTLJS {

    // CHTL JS Token类型枚举
    // 严格按照CHTL语法文档中的CHTL JS部分定义
    // 完全独立于CHTL Token系统
    enum class CHTLJSTokenType {
        // 基础Token
        IDENTIFIER,             // 标识符
        STRING_LITERAL,         // 字符串字面量
        NUMBER,                 // 数字
        
        // 符号Token
        LEFT_BRACE,             // {
        RIGHT_BRACE,            // }
        LEFT_BRACKET,           // [
        RIGHT_BRACKET,          // ]
        LEFT_PAREN,             // (
        RIGHT_PAREN,            // )
        SEMICOLON,              // ;
        COLON,                  // :
        EQUALS,                 // =
        COMMA,                  // ,
        DOT,                    // .
        ARROW,                  // -> (CHTL JS特有)
        AMPERSAND,              // &
        AMPERSAND_ARROW,        // &-> (事件绑定操作符)
        
        // CHTL JS专用符号
        DOUBLE_LEFT_BRACE,      // {{
        DOUBLE_RIGHT_BRACE,     // }}
        
        // 注释Token
        SINGLE_LINE_COMMENT,    // //
        MULTI_LINE_COMMENT,     // /**/
        GENERATOR_COMMENT,      // --
        
        // CHTL JS关键字Token
        MODULE,                 // module
        LOAD,                   // load
        LISTEN,                 // listen
        DELEGATE,               // delegate
        ANIMATE,                // animate
        VIR,                    // vir (虚对象)
        UTIL,                   // util
        CHANGE,                 // change
        THEN,                   // then
        TARGET,                 // target
        DURATION,               // duration
        EASING,                 // easing
        BEGIN,                  // begin
        WHEN,                   // when
        END,                    // end
        AT_KEYWORD,             // at (在动画中使用)
        LOOP,                   // loop
        DIRECTION,              // direction
        DELAY,                  // delay
        CALLBACK,               // callback
        
        // 事件相关Token
        CLICK,                  // click
        MOUSEENTER,             // mouseenter
        MOUSELEAVE,             // mouseleave
        MOUSEMOVE,              // mousemove
        
        // CSS选择器Token（在{{}}内使用）
        CSS_SELECTOR,           // 任意CSS选择器
        CLASS_SELECTOR,         // .className
        ID_SELECTOR,            // #idName
        TAG_SELECTOR,           // tagName
        DESCENDANT_SELECTOR,    // 后代选择器
        INDEX_SELECTOR,         // [index]
        
        // 特殊Token
        WHITESPACE,             // 空白字符
        NEWLINE,                // 换行
        TAB,                    // 制表符
        EOF_TOKEN,              // 文件结束
        UNKNOWN                 // 未知Token
    };

    // CHTL JS Token结构
    struct CHTLJSToken {
        CHTLJSTokenType type;       // Token类型
        std::string value;          // Token值
        size_t startPos;            // 起始位置
        size_t endPos;              // 结束位置
        int lineNumber;             // 行号
        int columnNumber;           // 列号
        
        CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t start, size_t end, int line, int col)
            : type(t), value(v), startPos(start), endPos(end), lineNumber(line), columnNumber(col) {}
    };

    // CHTL JS Token工具类
    // 完全独立于CHTL TokenUtil
    class CHTLJSTokenUtil {
    private:
        static std::unordered_map<std::string, CHTLJSTokenType> keywordMap;
        static std::unordered_map<std::string, CHTLJSTokenType> eventMap;
        static bool initialized;
        
        static void InitializeMaps();

    public:
        // Token类型判断
        static bool IsKeyword(const std::string& text);
        static bool IsEvent(const std::string& text);
        static bool IsSelector(const std::string& text);
        static bool IsCHTLJSOperator(const std::string& text);
        
        // Token类型获取
        static CHTLJSTokenType GetKeywordType(const std::string& text);
        static CHTLJSTokenType GetEventType(const std::string& text);
        
        // Token转换
        static std::string TokenTypeToString(CHTLJSTokenType type);
        static bool IsWhitespace(CHTLJSTokenType type);
        static bool IsComment(CHTLJSTokenType type);
        
        // CHTL JS特有验证方法
        static bool IsValidSelectorSyntax(const std::string& text);      // 验证{{}}选择器语法
        static bool IsValidArrowSyntax(const std::string& text);         // 验证->语法
        static bool IsValidEventBinding(const std::string& text);        // 验证&->事件绑定语法
        static bool IsValidCHTLJSFunction(const std::string& text);      // 验证CHTL JS函数语法
        
        // 选择器解析方法
        static CHTLJSTokenType ParseSelectorType(const std::string& selector);  // 解析选择器类型
    };

} // namespace CHTLJS