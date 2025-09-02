#include "CHTL/Parser/Parser.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"

namespace CHTL {

Parser::Parser(std::shared_ptr<CompileContext> ctx) 
    : currentIndex(0), context(ctx) {
}

void Parser::Reset() {
    currentIndex = 0;
    tokens.clear();
}

std::shared_ptr<ProgramNode> Parser::Parse(const std::vector<Token>& tokenList) {
    tokens = tokenList;
    currentIndex = 0;
    
    LOG_DEBUG("开始解析，Token数量: " + std::to_string(tokens.size()));
    
    auto program = std::make_shared<ProgramNode>(context->GetCurrentFile());
    
    // 解析顶层声明
    while (!Check(TokenType::EOF_TOKEN)) {
        try {
            auto node = ParseTopLevelDeclaration();
            if (node) {
                program->AddChild(node);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("解析错误: " + std::string(e.what()));
            // 尝试恢复：跳到下一个可能的声明开始位置
            RecoverFromError();
        }
    }
    
    return program;
}

bool Parser::Match(TokenType type) {
    if (Check(type)) {
        ConsumeToken();
        return true;
    }
    return false;
}

void Parser::Expect(TokenType type, const std::string& message) {
    if (!Check(type)) {
        ReportError(message);
    }
    ConsumeToken();
}

void Parser::ReportError(const std::string& message) {
    const Token& token = CurrentToken();
    throw SyntaxError(message, context->GetCurrentFile(), token.line, token.column);
}

std::shared_ptr<ASTNode> Parser::ParseTopLevelDeclaration() {
    // 跳过注释
    while (CurrentToken().IsComment()) {
        ConsumeToken();
    }
    
    // 检查EOF
    if (Check(TokenType::EOF_TOKEN)) {
        return nullptr;
    }
    
    // use语句
    if (Check(TokenType::USE)) {
        return ParseUseStatement();
    }
    
    // [Template]声明
    if (Check(TokenType::TEMPLATE)) {
        return ParseTemplate();
    }
    
    // [Custom]声明
    if (Check(TokenType::CUSTOM)) {
        return ParseCustom();
    }
    
    // [Origin]声明
    if (Check(TokenType::ORIGIN)) {
        return ParseOrigin();
    }
    
    // [Import]声明
    if (Check(TokenType::IMPORT)) {
        return ParseImport();
    }
    
    // [Namespace]声明
    if (Check(TokenType::NAMESPACE)) {
        return ParseNamespace();
    }
    
    // [Configuration]声明
    if (Check(TokenType::CONFIGURATION)) {
        return ParseConfiguration();
    }
    
    // 元素
    if (IsElementStart()) {
        return ParseElement();
    }
    
    // 未知的顶层声明
    ReportError("期望顶层声明，但得到: " + CurrentToken().value);
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseUseStatement() {
    Expect(TokenType::USE, "期望 'use'");
    
    // use html5;
    if (Check(TokenType::HTML5)) {
        ConsumeToken();
        Expect(TokenType::SEMICOLON, "期望 ';'");
        
        // 设置程序使用HTML5
        context->SetUseHTML5(true);
        
        // 创建一个特殊的节点表示use语句
        auto useNode = std::make_shared<UseNode>("html5");
        return useNode;
    }
    
    // use @Config 配置名;
    if (Check(TokenType::AT_CONFIG)) {
        ConsumeToken();
        std::string configName = ParseIdentifier()->ToString();
        Expect(TokenType::SEMICOLON, "期望 ';'");
        
        // TODO: 设置活动配置
        
        auto useNode = std::make_shared<UseNode>(configName);
        return useNode;
    }
    
    ReportError("无效的use语句");
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseTemplate() {
    Expect(TokenType::TEMPLATE, "期望 '[Template]'");
    
    // @Style, @Element 或 @Var
    if (Match(TokenType::AT_STYLE)) {
        return ParseTemplateStyle();
    } else if (Match(TokenType::AT_ELEMENT)) {
        return ParseTemplateElement();
    } else if (Match(TokenType::AT_VAR)) {
        return ParseTemplateVar();
    }
    
    ReportError("期望 @Style, @Element 或 @Var");
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseTemplateStyle() {
    std::string name = ParseIdentifier()->ToString();
    auto node = std::make_shared<TemplateStyleNode>();
    
    // 添加到全局映射表
    context->GetGlobalMap()->AddTemplateStyle(name, node);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析样式属性
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        auto property = ParseStyleProperty();
        if (property) {
            node->AddChild(property);
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return node;
}

std::shared_ptr<ASTNode> Parser::ParseTemplateElement() {
    std::string name = ParseIdentifier()->ToString();
    auto node = std::make_shared<TemplateElementNode>();
    
    // 添加到全局映射表
    context->GetGlobalMap()->AddTemplateElement(name, node);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析元素内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (IsElementStart()) {
            auto element = ParseElement();
            if (element) {
                node->AddChild(element);
            }
        } else {
            ConsumeToken(); // 跳过未知token
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return node;
}

std::shared_ptr<ASTNode> Parser::ParseTemplateVar() {
    std::string name = ParseIdentifier()->ToString();
    auto node = std::make_shared<TemplateVarNode>();
    
    // 添加到全局映射表
    context->GetGlobalMap()->AddTemplateVar(name, node);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析变量定义
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // 解析变量属性: 名称: 值;
        if (Check(TokenType::IDENTIFIER)) {
            std::string varName = CurrentToken().value;
            ConsumeToken();
            
            Expect(TokenType::COLON, "期望 ':'");
            
            std::string value = ParseStringOrUnquotedLiteral();
            
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // TODO: 添加变量到节点
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return node;
}

std::shared_ptr<ASTNode> Parser::ParseCustom() {
    // TODO: 实现Custom解析
    ConsumeToken(); // 暂时跳过
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseOrigin() {
    // TODO: 实现Origin解析
    ConsumeToken(); // 暂时跳过
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseImport() {
    // TODO: 实现Import解析
    ConsumeToken(); // 暂时跳过
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseNamespace() {
    // TODO: 实现Namespace解析
    ConsumeToken(); // 暂时跳过
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseConfiguration() {
    // TODO: 实现Configuration解析
    ConsumeToken(); // 暂时跳过
    return nullptr;
}

std::shared_ptr<ElementNode> Parser::ParseElement() {
    if (!Check(TokenType::IDENTIFIER)) {
        ReportError("期望元素名称");
    }
    
    std::string tagName = CurrentToken().value;
    ConsumeToken();
    
    auto element = std::make_shared<ElementNode>(tagName, CurrentToken().line, CurrentToken().column);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析元素内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // 属性
        if (Check(TokenType::IDENTIFIER) && PeekToken().type == TokenType::COLON) {
            std::string attrName = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::COLON, "期望 ':'");
            std::string attrValue = ParseStringOrUnquotedLiteral();
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 将属性添加到元素
            element->SetAttribute(attrName, attrValue);
        }
        // 子元素
        else if (IsElementStart()) {
            auto child = ParseElement();
            if (child) {
                element->AddChild(child);
            }
        }
        // text节点
        else if (Check(TokenType::TEXT)) {
            auto text = ParseText();
            if (text) {
                element->AddChild(text);
            }
        }
        // style块
        else if (Check(TokenType::STYLE)) {
            auto style = ParseLocalStyle();
            if (style) {
                element->AddChild(style);
            }
        }
        // script块
        else if (Check(TokenType::SCRIPT)) {
            auto script = ParseLocalScript();
            if (script) {
                element->AddChild(script);
            }
        }
        else {
            ConsumeToken(); // 跳过未知token
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return element;
}

std::shared_ptr<TextNode> Parser::ParseText() {
    Expect(TokenType::TEXT, "期望 'text'");
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto textNode = std::make_shared<TextNode>("", CurrentToken().line, CurrentToken().column);
    
    // 读取文本内容
    if (Check(TokenType::STRING_LITERAL) || Check(TokenType::UNQUOTED_LITERAL)) {
        textNode->SetContent(CurrentToken().value);
        ConsumeToken();
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return textNode;
}

std::shared_ptr<ASTNode> Parser::ParseAttribute() {
    std::string name = CurrentToken().value;
    ConsumeToken();
    
    Expect(TokenType::COLON, "期望 ':'");
    
    std::string value = ParseStringOrUnquotedLiteral();
    
    Expect(TokenType::SEMICOLON, "期望 ';'");
    
    // TODO: 创建属性节点
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseLocalStyle() {
    Expect(TokenType::STYLE, "期望 'style'");
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto styleNode = std::make_shared<LocalStyleNode>(CurrentToken().line, CurrentToken().column);
    
    // 解析样式属性
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::IDENTIFIER)) {
            // 样式属性名
            std::string propName = CurrentToken().value;
            ConsumeToken();
            
            Expect(TokenType::COLON, "期望 ':'");
            
            // 样式属性值
            std::string propValue = ParseStringOrUnquotedLiteral();
            
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            styleNode->AddProperty(propName, propValue);
        } else {
            // 跳过未知token
            ConsumeToken();
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return styleNode;
}

std::shared_ptr<ASTNode> Parser::ParseLocalScript() {
    Expect(TokenType::SCRIPT, "期望 'script'");
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto scriptNode = std::make_shared<LocalScriptNode>();
    
    // 解析脚本内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // TODO: 解析脚本内容
        ConsumeToken();
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return scriptNode;
}

std::shared_ptr<ASTNode> Parser::ParseStyleRule() {
    // TODO: 实现样式规则解析
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseStyleProperty() {
    // 解析样式属性: 名称: 值;
    if (Check(TokenType::IDENTIFIER)) {
        std::string propName = CurrentToken().value;
        ConsumeToken();
        
        Expect(TokenType::COLON, "期望 ':'");
        
        std::string value = ParseStringOrUnquotedLiteral();
        
        Expect(TokenType::SEMICOLON, "期望 ';'");
        
        // TODO: 创建样式属性节点
        return nullptr;
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseExpression() {
    // TODO: 实现表达式解析
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseLiteral() {
    if (Check(TokenType::STRING_LITERAL) || Check(TokenType::UNQUOTED_LITERAL) || Check(TokenType::NUMBER)) {
        auto literal = std::make_shared<StringLiteralNode>(CurrentToken().value, CurrentToken().line, CurrentToken().column);
        ConsumeToken();
        return literal;
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseIdentifier() {
    if (!Check(TokenType::IDENTIFIER)) {
        ReportError("期望标识符");
    }
    
    auto id = std::make_shared<IdentifierNode>(CurrentToken().value, CurrentToken().line, CurrentToken().column);
    ConsumeToken();
    return id;
}

bool Parser::IsTopLevelKeyword() const {
    TokenType type = CurrentToken().type;
    return type == TokenType::USE || 
           type == TokenType::TEMPLATE || 
           type == TokenType::CUSTOM || 
           type == TokenType::ORIGIN || 
           type == TokenType::IMPORT || 
           type == TokenType::NAMESPACE || 
           type == TokenType::CONFIGURATION;
}

bool Parser::IsElementStart() const {
    // 检查是否为HTML元素名称（标识符但不是关键字）
    if (CurrentToken().type == TokenType::IDENTIFIER) {
        // 检查是否为CHTL关键字
        const std::string& value = CurrentToken().value;
        return value != "text" && value != "style" && value != "script" && 
               value != "inherit" && value != "delete" && value != "insert";
    }
    return false;
}

bool Parser::IsStylePropertyStart() const {
    return Check(TokenType::IDENTIFIER) || Check(TokenType::AT_STYLE);
}

std::string Parser::ParseStringOrUnquotedLiteral() {
    if (Check(TokenType::STRING_LITERAL)) {
        std::string value = CurrentToken().value;
        ConsumeToken();
        return value;
    } else if (Check(TokenType::UNQUOTED_LITERAL)) {
        std::string value = CurrentToken().value;
        ConsumeToken();
        return value;
    } else if (Check(TokenType::IDENTIFIER)) {
        // 某些情况下，标识符也可以作为值
        std::string value = CurrentToken().value;
        ConsumeToken();
        return value;
    } else if (Check(TokenType::NUMBER)) {
        std::string value = CurrentToken().value;
        ConsumeToken();
        return value;
    }
    
    ReportError("期望字符串或字面量值");
    return "";
}

void Parser::RecoverFromError() {
    // 错误恢复：跳到下一个可能的声明开始位置
    while (!Check(TokenType::EOF_TOKEN)) {
        if (IsTopLevelKeyword() || IsElementStart()) {
            break;
        }
        ConsumeToken();
    }
}

} // namespace CHTL