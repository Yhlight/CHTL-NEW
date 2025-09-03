#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

    // CHTL Token类型枚举
    // 严格按照CHTL语法文档定义
    enum class CHTLTokenType {
        // 基础Token
        IDENTIFIER,             // 标识符
        STRING_LITERAL,         // 字符串字面量（双引号）
        SINGLE_QUOTE_STRING,    // 单引号字符串
        UNQUOTED_STRING,        // 无修饰字面量
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
        AMPERSAND,              // &
        AT,                     // @
        
        // 注释Token
        SINGLE_LINE_COMMENT,    // //
        MULTI_LINE_COMMENT,     // /**/
        GENERATOR_COMMENT,      // --
        
        // CHTL关键字Token
        TEXT,                   // text
        STYLE,                  // style
        SCRIPT,                 // script
        
        // 模板相关Token
        TEMPLATE,               // [Template]
        CUSTOM,                 // [Custom]
        ORIGIN,                 // [Origin]
        CONFIGURATION,          // [Configuration]
        NAMESPACE,              // [Namespace]
        IMPORT,                 // [Import]
        INFO,                   // [Info]
        EXPORT,                 // [Export]
        NAME,                   // [Name]
        ORIGIN_TYPE,            // [OriginType]
        
        // 类型Token
        AT_STYLE,               // @Style
        AT_ELEMENT,             // @Element
        AT_VAR,                 // @Var
        AT_HTML,                // @Html
        AT_JAVASCRIPT,          // @JavaScript
        AT_CHTL,                // @Chtl
        AT_CJMOD,               // @CJmod
        AT_CONFIG,              // @Config
        
        // 操作关键字Token
        INHERIT,                // inherit
        DELETE,                 // delete
        INSERT,                 // insert
        AFTER,                  // after
        BEFORE,                 // before
        REPLACE,                // replace
        AT_TOP,                 // at top
        AT_BOTTOM,              // at bottom
        FROM,                   // from
        AS,                     // as
        EXCEPT,                 // except
        USE,                    // use
        HTML5,                  // html5
        
        // CSS选择器相关Token
        CLASS_SELECTOR,         // .className
        ID_SELECTOR,            // #idName
        PSEUDO_CLASS,           // :hover, :focus等
        PSEUDO_ELEMENT,         // ::before, ::after等
        
        // 特殊Token
        WHITESPACE,             // 空白字符
        NEWLINE,                // 换行
        TAB,                    // 制表符
        EOF_TOKEN,              // 文件结束
        UNKNOWN,                // 未知Token
        
        // HTML元素Token（动态生成）
        HTML_ELEMENT            // html, head, body, div, span等HTML元素
    };

    // CHTL Token结构
    struct CHTLToken {
        CHTLTokenType type;         // Token类型
        std::string value;          // Token值
        size_t startPos;            // 起始位置
        size_t endPos;              // 结束位置
        int lineNumber;             // 行号
        int columnNumber;           // 列号
        
        CHTLToken(CHTLTokenType t, const std::string& v, size_t start, size_t end, int line, int col)
            : type(t), value(v), startPos(start), endPos(end), lineNumber(line), columnNumber(col) {}
    };

    // CHTL Token工具类
    class CHTLTokenUtil {
    private:
        static std::unordered_map<std::string, CHTLTokenType> keywordMap;
        static std::unordered_map<std::string, CHTLTokenType> htmlElementMap;
        static bool initialized;
        
        static void InitializeMaps();

    public:
        // Token类型判断
        static bool IsKeyword(const std::string& text);
        static bool IsHTMLElement(const std::string& text);
        static bool IsOperator(const std::string& text);
        static bool IsSelector(const std::string& text);
        
        // Token类型获取
        static CHTLTokenType GetKeywordType(const std::string& text);
        static CHTLTokenType GetHTMLElementType(const std::string& text);
        
        // Token转换
        static std::string TokenTypeToString(CHTLTokenType type);
        static bool IsWhitespace(CHTLTokenType type);
        static bool IsComment(CHTLTokenType type);
        
        // 验证方法
        static bool IsValidIdentifier(const std::string& text);
        static bool IsValidClassName(const std::string& text);
        static bool IsValidIdName(const std::string& text);
    };

} // namespace CHTL