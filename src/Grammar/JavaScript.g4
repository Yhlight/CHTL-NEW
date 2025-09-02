grammar JavaScript;

// JavaScript语法定义 - 用于CHTL编译器的JavaScript片段解析
// 基于ECMAScript 2023标准，支持现代JavaScript语法

// 主规则
program
    : sourceElements? EOF
    ;

sourceElements
    : sourceElement+
    ;

sourceElement
    : statement
    | functionDeclaration
    | classDeclaration
    ;

// 语句
statement
    : block
    | variableStatement
    | emptyStatement
    | expressionStatement
    | ifStatement
    | iterationStatement
    | continueStatement
    | breakStatement
    | returnStatement
    | withStatement
    | labelledStatement
    | switchStatement
    | throwStatement
    | tryStatement
    | debuggerStatement
    ;

// 块语句
block
    : '{' statementList? '}'
    ;

statementList
    : statement+
    ;

// 变量声明
variableStatement
    : variableDeclaration ';'?
    ;

variableDeclaration
    : ('var' | 'let' | 'const') variableDeclarationList
    ;

variableDeclarationList
    : variableDeclarator (',' variableDeclarator)*
    ;

variableDeclarator
    : assignable ('=' singleExpression)?
    ;

// 空语句
emptyStatement
    : ';'
    ;

// 表达式语句
expressionStatement
    : expressionSequence ';'?
    ;

// if语句
ifStatement
    : 'if' '(' expressionSequence ')' statement ('else' statement)?
    ;

// 迭代语句
iterationStatement
    : 'do' statement 'while' '(' expressionSequence ')' ';'?
    | 'while' '(' expressionSequence ')' statement
    | 'for' '(' (expressionSequence | variableDeclaration)? ';' expressionSequence? ';' expressionSequence? ')' statement
    | 'for' '(' (singleExpression | variableDeclaration) 'in' expressionSequence ')' statement
    | 'for' '(' (singleExpression | variableDeclaration) 'of' expressionSequence ')' statement
    ;

// continue语句
continueStatement
    : 'continue' identifier? ';'?
    ;

// break语句
breakStatement
    : 'break' identifier? ';'?
    ;

// return语句
returnStatement
    : 'return' expressionSequence? ';'?
    ;

// with语句
withStatement
    : 'with' '(' expressionSequence ')' statement
    ;

// 标签语句
labelledStatement
    : identifier ':' statement
    ;

// switch语句
switchStatement
    : 'switch' '(' expressionSequence ')' caseBlock
    ;

caseBlock
    : '{' caseClauses? (defaultClause caseClauses?)? '}'
    ;

caseClauses
    : caseClause+
    ;

caseClause
    : 'case' expressionSequence ':' statementList?
    ;

defaultClause
    : 'default' ':' statementList?
    ;

// throw语句
throwStatement
    : 'throw' expressionSequence ';'?
    ;

// try语句
tryStatement
    : 'try' block (catchProduction finallyProduction? | finallyProduction)
    ;

catchProduction
    : 'catch' ('(' assignable ')')? block
    ;

finallyProduction
    : 'finally' block
    ;

// debugger语句
debuggerStatement
    : 'debugger' ';'?
    ;

// 函数声明
functionDeclaration
    : 'function' identifier '(' formalParameterList? ')' functionBody
    | 'async' 'function' identifier '(' formalParameterList? ')' functionBody
    | 'function' '*' identifier '(' formalParameterList? ')' functionBody
    | 'async' 'function' '*' identifier '(' formalParameterList? ')' functionBody
    ;

// 类声明
classDeclaration
    : 'class' identifier classTail
    | 'abstract' 'class' identifier classTail
    ;

classTail
    : ('extends' singleExpression)? '{' classElement* '}'
    ;

classElement
    : methodDefinition
    | fieldDefinition
    | classElementName ';'
    ;

methodDefinition
    : propertyName '(' formalParameterList? ')' functionBody
    | '*' propertyName '(' formalParameterList? ')' functionBody
    | 'async' propertyName '(' formalParameterList? ')' functionBody
    | 'async' '*' propertyName '(' formalParameterList? ')' functionBody
    | 'get' propertyName '(' ')' functionBody
    | 'set' propertyName '(' formalParameterList ')' functionBody
    ;

fieldDefinition
    : classElementName ('=' singleExpression)? ';'?
    ;

classElementName
    : propertyName
    | 'static' propertyName
    | 'private' propertyName
    | 'static' 'private' propertyName
    ;

// 形式参数列表
formalParameterList
    : formalParameterArg (',' formalParameterArg)* (',' lastFormalParameterArg)?
    | lastFormalParameterArg
    ;

formalParameterArg
    : assignable ('=' singleExpression)?
    ;

lastFormalParameterArg
    : '...' singleExpression
    ;

// 函数体
functionBody
    : '{' sourceElements? '}'
    ;

// 表达式序列
expressionSequence
    : singleExpression (',' singleExpression)*
    ;

// 单个表达式
singleExpression
    : 'function' identifier? '(' formalParameterList? ')' functionBody                                          # FunctionExpression
    | 'class' identifier? classTail                                                                             # ClassExpression
    | 'async' 'function' identifier? '(' formalParameterList? ')' functionBody                                 # AsyncFunctionExpression
    | 'async' 'function' '*' identifier? '(' formalParameterList? ')' functionBody                             # AsyncGeneratorExpression
    | 'function' '*' identifier? '(' formalParameterList? ')' functionBody                                     # GeneratorExpression
    | 'async' arrowFunctionParameters '=>' arrowFunctionBody                                                   # AsyncArrowFunction
    | arrowFunctionParameters '=>' arrowFunctionBody                                                           # ArrowFunction
    | singleExpression '[' expressionSequence ']'                                                              # MemberIndexExpression
    | singleExpression '?.' '[' expressionSequence ']'                                                         # OptionalMemberIndexExpression
    | singleExpression '.' identifierName                                                                      # MemberDotExpression
    | singleExpression '?.' identifierName                                                                     # OptionalMemberDotExpression
    | 'new' singleExpression arguments?                                                                        # NewExpression
    | singleExpression arguments                                                                               # CallExpression
    | singleExpression '?.' arguments                                                                          # OptionalCallExpression
    | singleExpression '++'                                                                                    # PostIncrementExpression
    | singleExpression '--'                                                                                    # PostDecreaseExpression
    | '++' singleExpression                                                                                    # PreIncrementExpression
    | '--' singleExpression                                                                                    # PreDecreaseExpression
    | '+' singleExpression                                                                                     # UnaryPlusExpression
    | '-' singleExpression                                                                                     # UnaryMinusExpression
    | '~' singleExpression                                                                                     # BitNotExpression
    | '!' singleExpression                                                                                     # NotExpression
    | 'await' singleExpression                                                                                 # AwaitExpression
    | 'typeof' singleExpression                                                                                # TypeofExpression
    | 'void' singleExpression                                                                                  # VoidExpression
    | 'delete' singleExpression                                                                                # DeleteExpression
    | singleExpression '**' singleExpression                                                                   # PowerExpression
    | singleExpression ('*' | '/' | '%') singleExpression                                                     # MultiplicativeExpression
    | singleExpression ('+' | '-') singleExpression                                                           # AdditiveExpression
    | singleExpression ('<<' | '>>' | '>>>') singleExpression                                                 # BitShiftExpression
    | singleExpression ('<' | '>' | '<=' | '>=') singleExpression                                             # RelationalExpression
    | singleExpression 'instanceof' singleExpression                                                          # InstanceofExpression
    | singleExpression 'in' singleExpression                                                                  # InExpression
    | singleExpression ('==' | '!=' | '===' | '!==') singleExpression                                        # EqualityExpression
    | singleExpression '&' singleExpression                                                                   # BitAndExpression
    | singleExpression '^' singleExpression                                                                   # BitXOrExpression
    | singleExpression '|' singleExpression                                                                   # BitOrExpression
    | singleExpression '&&' singleExpression                                                                  # LogicalAndExpression
    | singleExpression '||' singleExpression                                                                  # LogicalOrExpression
    | singleExpression '??' singleExpression                                                                  # CoalesceExpression
    | singleExpression '?' singleExpression ':' singleExpression                                              # TernaryExpression
    | singleExpression '=' singleExpression                                                                   # AssignmentExpression
    | singleExpression assignmentOperator singleExpression                                                   # AssignmentOperatorExpression
    | 'import' '(' singleExpression ')'                                                                       # ImportExpression
    | 'yield' singleExpression?                                                                               # YieldExpression
    | 'yield' '*' singleExpression                                                                            # YieldStarExpression
    | 'this'                                                                                                  # ThisExpression
    | identifier                                                                                              # IdentifierExpression
    | literal                                                                                                 # LiteralExpression
    | arrayLiteral                                                                                            # ArrayLiteralExpression
    | objectLiteral                                                                                           # ObjectLiteralExpression
    | '(' expressionSequence ')'                                                                              # ParenthesizedExpression
    | templateStringLiteral                                                                                   # TemplateStringExpression
    ;

// 赋值操作符
assignmentOperator
    : '*='
    | '/='
    | '%='
    | '+='
    | '-='
    | '<<='
    | '>>='
    | '>>>='
    | '&='
    | '^='
    | '|='
    | '**='
    | '&&='
    | '||='
    | '??='
    ;

// 字面量
literal
    : NullLiteral
    | BooleanLiteral
    | StringLiteral
    | NumericLiteral
    | RegularExpressionLiteral
    ;

// 数组字面量
arrayLiteral
    : '[' elementList? ']'
    ;

elementList
    : arrayElement (',' arrayElement)* ','?
    ;

arrayElement
    : '...' singleExpression
    | singleExpression
    | /* empty */
    ;

// 对象字面量
objectLiteral
    : '{' propertyAssignment (',' propertyAssignment)* ','? '}'
    | '{' '}'
    ;

propertyAssignment
    : propertyName ':' singleExpression                                                                       # PropertyExpressionAssignment
    | '[' singleExpression ']' ':' singleExpression                                                           # ComputedPropertyExpressionAssignment
    | 'async'? propertyName '(' formalParameterList? ')' functionBody                                         # MethodAssignment
    | 'async'? '*' propertyName '(' formalParameterList? ')' functionBody                                     # GeneratorMethodAssignment
    | 'get' propertyName '(' ')' functionBody                                                                 # GetterAssignment
    | 'set' propertyName '(' formalParameterList ')' functionBody                                             # SetterAssignment
    | '...' singleExpression                                                                                  # SpreadAssignment
    ;

// 属性名
propertyName
    : identifierName
    | StringLiteral
    | NumericLiteral
    | '[' singleExpression ']'
    ;

// 参数
arguments
    : '(' argumentList? ')'
    ;

argumentList
    : argument (',' argument)* ','?
    ;

argument
    : '...' singleExpression
    | singleExpression
    ;

// 标识符名称
identifierName
    : identifier
    | reservedWord
    ;

// 保留字
reservedWord
    : keyword
    | futureReservedWord
    | NullLiteral
    | BooleanLiteral
    ;

// 关键字
keyword
    : 'break'
    | 'case'
    | 'catch'
    | 'class'
    | 'const'
    | 'continue'
    | 'debugger'
    | 'default'
    | 'delete'
    | 'do'
    | 'else'
    | 'export'
    | 'extends'
    | 'finally'
    | 'for'
    | 'function'
    | 'if'
    | 'import'
    | 'in'
    | 'instanceof'
    | 'let'
    | 'new'
    | 'return'
    | 'super'
    | 'switch'
    | 'this'
    | 'throw'
    | 'try'
    | 'typeof'
    | 'var'
    | 'void'
    | 'while'
    | 'with'
    | 'yield'
    | 'async'
    | 'await'
    | 'of'
    ;

// 未来保留字
futureReservedWord
    : 'enum'
    | 'implements'
    | 'interface'
    | 'package'
    | 'private'
    | 'protected'
    | 'public'
    | 'static'
    ;

// 箭头函数参数
arrowFunctionParameters
    : identifier
    | '(' formalParameterList? ')'
    ;

// 箭头函数体
arrowFunctionBody
    : singleExpression
    | functionBody
    ;

// 可赋值对象
assignable
    : identifier
    | arrayLiteral
    | objectLiteral
    ;

// 模板字符串字面量
templateStringLiteral
    : BackTick templateStringAtom* BackTick
    ;

templateStringAtom
    : TemplateStringAtom
    | TemplateStringStartExpression singleExpression TemplateStringEndExpression
    ;

// 标识符
identifier
    : Identifier
    | 'async'
    | 'await'
    | 'yield'
    | 'of'
    ;

// 词法规则

// 字面量
NullLiteral
    : 'null'
    ;

BooleanLiteral
    : 'true'
    | 'false'
    ;

// 字符串字面量
StringLiteral
    : '"' DoubleStringCharacter* '"'
    | '\'' SingleStringCharacter* '\''
    ;

// 模板字符串
BackTick
    : '`'
    ;

TemplateStringAtom
    : '`' ('\\' . | ~('\\' | '`' | '$'))* '`'
    ;

TemplateStringStartExpression
    : '${'
    ;

TemplateStringEndExpression
    : '}'
    ;

// 数字字面量
NumericLiteral
    : DecimalLiteral
    | HexIntegerLiteral
    | OctalIntegerLiteral
    | BinaryIntegerLiteral
    ;

// 正则表达式字面量
RegularExpressionLiteral
    : '/' RegularExpressionBody '/' RegularExpressionFlags?
    ;

// 标识符
Identifier
    : IdentifierStart IdentifierPart*
    ;

// 辅助规则
fragment DoubleStringCharacter
    : ~["\\\r\n]
    | '\\' EscapeSequence
    | LineContinuation
    ;

fragment SingleStringCharacter
    : ~['\\\r\n]
    | '\\' EscapeSequence
    | LineContinuation
    ;

fragment EscapeSequence
    : CharacterEscapeSequence
    | '0'
    | HexEscapeSequence
    | UnicodeEscapeSequence
    ;

fragment CharacterEscapeSequence
    : SingleEscapeCharacter
    | NonEscapeCharacter
    ;

fragment SingleEscapeCharacter
    : ['"\\bfnrtv]
    ;

fragment NonEscapeCharacter
    : ~['"\\bfnrtv0-9xu\r\n]
    ;

fragment HexEscapeSequence
    : 'x' HexDigit HexDigit
    ;

fragment UnicodeEscapeSequence
    : 'u' HexDigit HexDigit HexDigit HexDigit
    | 'u' '{' HexDigit+ '}'
    ;

fragment LineContinuation
    : '\\' [\r\n\u2028\u2029]
    ;

fragment DecimalLiteral
    : DecimalIntegerLiteral '.' DecimalDigit* ExponentPart?
    | '.' DecimalDigit+ ExponentPart?
    | DecimalIntegerLiteral ExponentPart?
    ;

fragment DecimalIntegerLiteral
    : '0'
    | [1-9] DecimalDigit*
    ;

fragment DecimalDigit
    : [0-9]
    ;

fragment ExponentPart
    : [eE] [+-]? DecimalDigit+
    ;

fragment HexIntegerLiteral
    : '0' [xX] HexDigit+
    ;

fragment HexDigit
    : [0-9a-fA-F]
    ;

fragment OctalIntegerLiteral
    : '0' [0-7]+
    ;

fragment BinaryIntegerLiteral
    : '0' [bB] [01]+
    ;

fragment RegularExpressionBody
    : RegularExpressionFirstChar RegularExpressionChar*
    ;

fragment RegularExpressionFirstChar
    : ~[*\r\n\u2028\u2029\\/[]
    | RegularExpressionBackslashSequence
    | RegularExpressionClass
    ;

fragment RegularExpressionChar
    : ~[\r\n\u2028\u2029\\/[]
    | RegularExpressionBackslashSequence
    | RegularExpressionClass
    ;

fragment RegularExpressionBackslashSequence
    : '\\' ~[\r\n\u2028\u2029]
    ;

fragment RegularExpressionClass
    : '[' RegularExpressionClassChar* ']'
    ;

fragment RegularExpressionClassChar
    : ~[\r\n\u2028\u2029\]\\]
    | RegularExpressionBackslashSequence
    ;

fragment RegularExpressionFlags
    : [gimsuvy]+
    ;

fragment IdentifierStart
    : [\p{L}]
    | '$'
    | '_'
    | '\\' UnicodeEscapeSequence
    ;

fragment IdentifierPart
    : IdentifierStart
    | [\p{Mn}]
    | [\p{Mc}]
    | [\p{Nd}]
    | [\p{Pc}]
    | '\u200C'
    | '\u200D'
    ;

// 空白和注释
MultiLineComment
    : '/*' .*? '*/' -> skip
    ;

SingleLineComment
    : '//' ~[\r\n\u2028\u2029]* -> skip
    ;

WhiteSpaces
    : [\t\u000B\u000C\u0020\u00A0\u1680\u180E\u2000-\u200A\u202F\u205F\u3000\uFEFF]+ -> skip
    ;

LineTerminator
    : [\r\n\u2028\u2029] -> skip
    ;