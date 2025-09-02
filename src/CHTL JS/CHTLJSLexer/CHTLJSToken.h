#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTLJS {

/**
 * CHTL JS令牌类型枚举
 * 涵盖所有CHTL JS语法特征的令牌类型（完全独立于CHTL）
 */
enum class CHTLJSTokenType {
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
    
    // CHTL JS专用操作符
    ARROW_OPERATOR,            // ->（等价于.，CHTL JS语法标识）
    EVENT_BIND_OPERATOR,       // &->（事件绑定操作符）
    
    // CHTL JS核心关键字
    MODULE,                    // module（模块导入）
    LOAD,                      // load（模块加载）
    LISTEN,                    // listen（增强监听器）
    DELEGATE,                  // delegate（事件委托）
    ANIMATE,                   // animate（动画）
    VIR,                       // vir（虚对象）
    
    // 动画相关关键字
    TARGET,                    // target
    DURATION,                  // duration
    EASING,                    // easing
    BEGIN,                     // begin
    WHEN,                      // when
    END,                       // end
    LOOP,                      // loop
    DIRECTION,                 // direction
    DELAY,                     // delay
    CALLBACK,                  // callback
    AT,                        // at（动画关键帧时刻）
    
    // 增强选择器令牌
    SELECTOR_START,            // {{
    SELECTOR_END,              // }}
    SELECTOR_CONTENT,          // {{...}}内部的选择器内容
    CLASS_SELECTOR,            // .className（在{{}}内）
    ID_SELECTOR,               // #idName（在{{}}内）
    TAG_SELECTOR,              // tagName（在{{}}内）
    DESCENDANT_SELECTOR,       // 后代选择器（空格分隔）
    INDEX_SELECTOR,            // [0], [1]等精确访问
    
    // 特殊令牌
    EOF_TOKEN,                 // 文件结束
    NEWLINE,                   // 换行符
    WHITESPACE,                // 空白字符
    COMMENT,                   // 注释
    UNKNOWN                    // 未知令牌
};

/**
 * CHTL JS令牌结构
 * 存储令牌的完整信息（独立于CHTL Token）
 */
struct CHTLJSToken {
    CHTLJSTokenType Type;       // 令牌类型
    std::string Value;          // 令牌值
    size_t Line;               // 行号
    size_t Column;             // 列号
    size_t Position;           // 在源代码中的位置
    size_t Length;             // 令牌长度
    bool IsCHTLJSSyntax;       // 是否为CHTL JS特有语法
    
    CHTLJSToken() = default;
    CHTLJSToken(CHTLJSTokenType type, const std::string& value, 
               size_t line, size_t column, size_t position, size_t length, bool isCHTLJS = true)
        : Type(type), Value(value), Line(line), Column(column), 
          Position(position), Length(length), IsCHTLJSSyntax(isCHTLJS) {}
    
    /**
     * 获取令牌类型的字符串表示
     * @return 类型名称
     */
    std::string GetTypeName() const;
    
    /**
     * 检查是否为CHTL JS关键字令牌
     * @return 是否为关键字
     */
    bool IsKeyword() const;
    
    /**
     * 检查是否为增强选择器令牌
     * @return 是否为选择器
     */
    bool IsEnhancedSelector() const;
    
    /**
     * 检查是否为CHTL JS函数令牌
     * @return 是否为CHTL JS函数
     */
    bool IsCHTLJSFunction() const;
    
    /**
     * 检查是否为动画相关令牌
     * @return 是否为动画令牌
     */
    bool IsAnimationToken() const;
};

/**
 * CHTL JS令牌管理器
 * 负责CHTL JS令牌的创建、管理和查询（完全独立于CHTL）
 */
class CHTLJSTokenManager {
private:
    std::unordered_map<std::string, CHTLJSTokenType> m_KeywordMap;     // 关键字映射
    std::unordered_map<std::string, bool> m_CHTLJSFunctions;           // CHTL JS函数映射
    
public:
    /**
     * 构造函数
     */
    CHTLJSTokenManager();
    
    /**
     * 析构函数
     */
    ~CHTLJSTokenManager() = default;
    
    /**
     * 根据字符串获取令牌类型
     * @param value 令牌值
     * @return 令牌类型
     */
    CHTLJSTokenType GetTokenType(const std::string& value);
    
    /**
     * 检查是否为CHTL JS关键字
     * @param value 字符串值
     * @return 是否为关键字
     */
    bool IsKeyword(const std::string& value);
    
    /**
     * 检查是否为CHTL JS函数
     * @param value 字符串值
     * @return 是否为CHTL JS函数
     */
    bool IsCHTLJSFunction(const std::string& value);
    
    /**
     * 注册CHTL JS函数
     * @param functionName 函数名称
     */
    void RegisterCHTLJSFunction(const std::string& functionName);
    
    /**
     * 检查是否为有效的增强选择器
     * @param selector 选择器字符串
     * @return 是否有效
     */
    bool IsValidEnhancedSelector(const std::string& selector);
    
    /**
     * 解析增强选择器内容
     * @param content {{}}内的内容
     * @return 解析后的选择器类型
     */
    CHTLJSTokenType ParseSelectorContent(const std::string& content);

private:
    /**
     * 初始化关键字映射表
     */
    void InitializeKeywordMap();
    
    /**
     * 初始化CHTL JS函数映射
     */
    void InitializeCHTLJSFunctions();
};

} // namespace CHTLJS