grammar CSS;

// CSS语法定义 - 用于CHTL编译器的CSS片段解析
// 支持CSS3标准和CHTL特定的CSS扩展

// 主规则
stylesheet
    : (charset | imports | namespace | cssRule)* EOF
    ;

// 字符集声明
charset
    : CHARSET_SYM STRING ';'
    ;

// 导入规则
imports
    : IMPORT_SYM (STRING | URI) mediaList? ';'
    ;

// 命名空间
namespace
    : NAMESPACE_SYM (IDENT | STRING | URI)? ';'
    ;

// 媒体查询列表
mediaList
    : medium (',' medium)*
    ;

medium
    : IDENT
    ;

// CSS规则
cssRule
    : selector (',' selector)* '{' declaration* '}'
    | atRule
    ;

// at规则
atRule
    : AT_KEYWORD any* (block | ';')
    ;

// 块
block
    : '{' (any | block | atRule)* '}'
    ;

// 选择器
selector
    : simpleSelector (combinator simpleSelector)*
    ;

// 简单选择器
simpleSelector
    : elementName (HASH | class_ | attrib | pseudo)*
    | (HASH | class_ | attrib | pseudo)+
    ;

// 元素名
elementName
    : IDENT
    | '*'
    ;

// 类选择器
class_
    : '.' IDENT
    ;

// 属性选择器
attrib
    : '[' IDENT ((INCLUDES | DASHMATCH | PREFIXMATCH | SUFFIXMATCH | SUBSTRINGMATCH | '=') (IDENT | STRING))? ']'
    ;

// 伪类/伪元素
pseudo
    : ':' ':'? (IDENT | FUNCTION any* ')')
    ;

// 组合器
combinator
    : PLUS
    | GREATER
    | TILDE
    | S
    ;

// 声明
declaration
    : property ':' expr prio?
    ;

// 属性
property
    : IDENT
    ;

// 优先级
prio
    : IMPORTANT_SYM
    ;

// 表达式
expr
    : term (operator term)*
    ;

// 项
term
    : unaryOperator? (NUMBER | PERCENTAGE | LENGTH | EMS | EXS | ANGLE | TIME | FREQ | DIMENSION)
    | STRING
    | IDENT
    | URI
    | hexcolor
    | FUNCTION expr ')'
    ;

// 一元操作符
unaryOperator
    : '-'
    | '+'
    ;

// 操作符
operator
    : '/'
    | ','
    | /* empty */
    ;

// 十六进制颜色
hexcolor
    : HASH
    ;

// any规则（用于未识别内容）
any
    : IDENT
    | NUMBER
    | PERCENTAGE
    | LENGTH
    | EMS
    | EXS
    | ANGLE
    | TIME
    | FREQ
    | DIMENSION
    | STRING
    | DELIM
    | URI
    | HASH
    | UNICODE_RANGE
    | INCLUDES
    | DASHMATCH
    | PREFIXMATCH
    | SUFFIXMATCH
    | SUBSTRINGMATCH
    | FUNCTION any* ')'
    | '(' any* ')'
    | '[' any* ']'
    ;

// 词法规则
CHARSET_SYM     : '@charset';
IMPORT_SYM      : '@import';
NAMESPACE_SYM   : '@namespace';
AT_KEYWORD      : '@' IDENT;

IMPORTANT_SYM   : '!important';

// 字符串
STRING          : '"' (~["\\\r\n] | '\\' .)* '"'
                | '\'' (~['\\\r\n] | '\\' .)* '\''
                ;

// 标识符
IDENT           : '-'? NMSTART NMCHAR*;

// 数字
NUMBER          : [0-9]+ ('.' [0-9]+)?;

// 百分比
PERCENTAGE      : NUMBER '%';

// 长度单位
LENGTH          : NUMBER ('px' | 'cm' | 'mm' | 'in' | 'pt' | 'pc' | 'em' | 'ex' | 'ch' | 'rem' | 'vw' | 'vh' | 'vmin' | 'vmax');

// Em单位
EMS             : NUMBER 'em';

// Ex单位
EXS             : NUMBER 'ex';

// 角度
ANGLE           : NUMBER ('deg' | 'rad' | 'grad' | 'turn');

// 时间
TIME            : NUMBER ('s' | 'ms');

// 频率
FREQ            : NUMBER ('Hz' | 'kHz');

// 维度
DIMENSION       : NUMBER IDENT;

// URI
URI             : 'url(' S* (STRING | (~[)'"\\])*) S* ')';

// 哈希
HASH            : '#' NMCHAR+;

// Unicode范围
UNICODE_RANGE   : 'U+' [0-9A-Fa-f?]+('-' [0-9A-Fa-f]+)?;

// 包含匹配
INCLUDES        : '~=';

// 破折号匹配
DASHMATCH       : '|=';

// 前缀匹配
PREFIXMATCH     : '^=';

// 后缀匹配
SUFFIXMATCH     : '$=';

// 子串匹配
SUBSTRINGMATCH  : '*=';

// 函数
FUNCTION        : IDENT '(';

// 分隔符
DELIM           : ~[a-zA-Z0-9\r\n\f\t ];

// 操作符
PLUS            : '+';
GREATER         : '>';
TILDE           : '~';

// 辅助规则
fragment NMSTART : [a-zA-Z_]
                 | NONASCII
                 | ESCAPE
                 ;

fragment NMCHAR  : [a-zA-Z0-9_-]
                 | NONASCII
                 | ESCAPE
                 ;

fragment NONASCII : [\u0080-\uFFFF];

fragment ESCAPE   : '\\' (~[\r\n\f] | '\r\n' | '\r' | '\n' | '\f');

// 空白字符
S               : [ \t\r\n\f]+;

// 注释
COMMENT         : '/*' .*? '*/' -> skip;

// CDO/CDC (HTML注释标记)
CDO             : '<!--';
CDC             : '-->';

// 跳过空白
WS              : [ \t\r\n\f]+ -> skip;