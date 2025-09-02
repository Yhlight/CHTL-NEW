#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * CHTL令牌类型枚举
 * 涵盖所有CHTL语法特征的令牌类型
 */
enum class CHTLTokenType {
    // 基础令牌
    IDENTIFIER,                 // 标识符
    STRING_LITERAL,            // 字符串字面量 ""
    SINGLE_QUOTE_STRING,       // 单引号字符串 ''
    UNQUOTED_LITERAL,          // 无修饰字面量
    NUMBER,                    // 数字
    
    // 分隔符和操作符
    LEFT_BRACE,                // {
    RIGHT_BRACE,               // }
    LEFT_BRACKET,              // [
    RIGHT_BRACKET,             // ]
    LEFT_PAREN,                // (
    RIGHT_PAREN,               // )
    SEMICOLON,                 // ;
    COLON,                     // :
    COMMA,                     // ,
    DOT,                       // .
    EQUAL,                     // =
    AMPERSAND,                 // &
    
    // 注释令牌
    SINGLE_LINE_COMMENT,       // //
    MULTI_LINE_COMMENT,        // /**/
    GENERATOR_COMMENT,         // --
    
    // CHTL关键字令牌
    TEXT,                      // text
    STYLE,                     // style (局部样式块)
    SCRIPT,                    // script (局部脚本块，属于CHTL)
    
    // 块结构关键字
    TEMPLATE_BLOCK,            // [Template]
    CUSTOM_BLOCK,              // [Custom]
    ORIGIN_BLOCK,              // [Origin]
    CONFIGURATION_BLOCK,       // [Configuration]
    IMPORT_BLOCK,              // [Import]
    NAMESPACE_BLOCK,           // [Namespace]
    INFO_BLOCK,                // [Info]
    EXPORT_BLOCK,              // [Export]
    NAME_BLOCK,                // [Name]
    ORIGIN_TYPE_BLOCK,         // [OriginType]
    
    // @类型标识符
    AT_STYLE,                  // @Style
    AT_ELEMENT,                // @Element
    AT_VAR,                    // @Var
    AT_HTML,                   // @Html
    AT_JAVASCRIPT,             // @JavaScript
    AT_CHTL,                   // @Chtl
    AT_CJMOD,                  // @CJmod
    AT_CONFIG,                 // @Config
    
    // 操作关键字
    INHERIT,                   // inherit
    DELETE,                    // delete
    INSERT,                    // insert
    AFTER,                     // after
    BEFORE,                    // before
    REPLACE,                   // replace
    AT_TOP,                    // at top
    AT_BOTTOM,                 // at bottom
    EXCEPT,                    // except
    FROM,                      // from
    AS,                        // as
    USE,                       // use
    HTML5,                     // html5
    
    // 选择器和引用
    CLASS_SELECTOR,            // .className
    ID_SELECTOR,               // #idName
    CONTEXT_REFERENCE,         // & (上下文推导)
    PSEUDO_CLASS,              // :hover, :focus等
    PSEUDO_ELEMENT,            // ::before, ::after等
    
    // 索引访问
    INDEX_ACCESS,              // [0], [1]等
    
    // 特殊令牌
    EOF_TOKEN,                 // 文件结束
    NEWLINE,                   // 换行符
    WHITESPACE,                // 空白字符
    UNKNOWN,                   // 未知令牌
    
    // 自定义类型标识符（动态）
    CUSTOM_AT_TYPE             // @Vue、@Markdown等自定义类型
};

/**
 * CHTL令牌结构
 * 存储令牌的完整信息
 */
struct CHTLToken {
    CHTLTokenType Type;         // 令牌类型
    std::string Value;          // 令牌值
    size_t Line;               // 行号
    size_t Column;             // 列号
    size_t Position;           // 在源代码中的位置
    size_t Length;             // 令牌长度
    
    CHTLToken() = default;
    CHTLToken(CHTLTokenType type, const std::string& value, 
             size_t line, size_t column, size_t position, size_t length)
        : Type(type), Value(value), Line(line), Column(column), 
          Position(position), Length(length) {}
    
    /**
     * 获取令牌类型的字符串表示
     * @return 类型名称
     */
    std::string GetTypeName() const;
    
    /**
     * 检查是否为关键字令牌
     * @return 是否为关键字
     */
    bool IsKeyword() const;
    
    /**
     * 检查是否为块结构令牌
     * @return 是否为块结构
     */
    bool IsBlockStructure() const;
    
    /**
     * 检查是否为@类型标识符
     * @return 是否为@类型
     */
    bool IsAtType() const;
    
    /**
     * 检查是否为选择器令牌
     * @return 是否为选择器
     */
    bool IsSelector() const;
};

/**
 * CHTL令牌管理器
 * 负责令牌的创建、管理和查询
 */
class CHTLTokenManager {
private:
    std::unordered_map<std::string, CHTLTokenType> m_KeywordMap;     // 关键字映射
    std::unordered_map<std::string, CHTLTokenType> m_CustomAtTypes;  // 自定义@类型映射
    
public:
    /**
     * 构造函数
     */
    CHTLTokenManager();
    
    /**
     * 析构函数
     */
    ~CHTLTokenManager() = default;
    
    /**
     * 根据字符串获取令牌类型
     * @param value 令牌值
     * @return 令牌类型
     */
    CHTLTokenType GetTokenType(const std::string& value);
    
    /**
     * 检查是否为关键字
     * @param value 字符串值
     * @return 是否为关键字
     */
    bool IsKeyword(const std::string& value);
    
    /**
     * 注册自定义@类型
     * @param typeName 类型名称（如@Vue）
     */
    void RegisterCustomAtType(const std::string& typeName);
    
    /**
     * 获取所有已注册的自定义@类型
     * @return 自定义类型列表
     */
    std::vector<std::string> GetCustomAtTypes() const;
    
    /**
     * 检查是否为有效的标识符
     * @param value 字符串值
     * @return 是否为有效标识符
     */
    bool IsValidIdentifier(const std::string& value);
    
    /**
     * 检查是否为有效的类选择器
     * @param value 字符串值
     * @return 是否为有效类选择器
     */
    bool IsValidClassSelector(const std::string& value);
    
    /**
     * 检查是否为有效的ID选择器
     * @param value 字符串值
     * @return 是否为有效ID选择器
     */
    bool IsValidIdSelector(const std::string& value);

private:
    /**
     * 初始化关键字映射表
     */
    void InitializeKeywordMap();
};

} // namespace CHTL