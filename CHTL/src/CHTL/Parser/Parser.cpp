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
        Expect(TokenType::IDENTIFIER, "期望标识符");
        std::string configName = CurrentToken().value;
        ConsumeToken();
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
    if (!Check(TokenType::IDENTIFIER)) {
        ReportError("期望标识符");
        return nullptr;
    }
    std::string name = CurrentToken().value;
    ConsumeToken();
    auto node = std::make_shared<TemplateStyleNode>(name);
    
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
    if (!Check(TokenType::IDENTIFIER)) {
        ReportError("期望标识符");
        return nullptr;
    }
    std::string name = CurrentToken().value;
    ConsumeToken();
    auto node = std::make_shared<TemplateElementNode>(name);
    
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
    if (!Check(TokenType::IDENTIFIER)) {
        ReportError("期望标识符");
        return nullptr;
    }
    std::string name = CurrentToken().value;
    ConsumeToken();
    auto node = std::make_shared<TemplateVarNode>(name);
    
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
            
            // 添加变量到节点
            LOG_DEBUG("添加变量: " + varName + " = " + value);
            node->AddVariable(varName, value);
        } else {
            // 跳过未知token
            ConsumeToken();
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
    Expect(TokenType::IMPORT, "期望 '[Import]'");
    
    auto importNode = std::make_shared<ImportNode>();
    ImportNode::ImportType importType;
    std::string itemName;
    std::string customTypeName;
    
    // 检查是否是 [Custom], [Template], [Origin], [Configuration]
    bool isCustom = false;
    bool isTemplate = false;
    bool isOrigin = false;
    bool isConfiguration = false;
    
    if (Check(TokenType::CUSTOM)) {
        isCustom = true;
        ConsumeToken();
    } else if (Check(TokenType::TEMPLATE)) {
        isTemplate = true;
        ConsumeToken();
    } else if (Check(TokenType::ORIGIN)) {
        isOrigin = true;
        ConsumeToken();
    } else if (Check(TokenType::CONFIGURATION)) {
        isConfiguration = true;
        ConsumeToken();
    }
    
    // 解析@Type
    if (Check(TokenType::AT_HTML)) {
        ConsumeToken();
        if (isOrigin) {
            importType = ImportNode::ImportType::OriginHtml;
        } else {
            importType = ImportNode::ImportType::Html;
        }
    } else if (Check(TokenType::AT_STYLE)) {
        ConsumeToken();
        if (isCustom) {
            importType = ImportNode::ImportType::CustomStyle;
        } else if (isTemplate) {
            importType = ImportNode::ImportType::TemplateStyle;
        } else if (isOrigin) {
            importType = ImportNode::ImportType::OriginStyle;
        } else {
            importType = ImportNode::ImportType::Style;
        }
    } else if (Check(TokenType::AT_JAVASCRIPT)) {
        ConsumeToken();
        if (isOrigin) {
            importType = ImportNode::ImportType::OriginJavaScript;
        } else {
            importType = ImportNode::ImportType::JavaScript;
        }
    } else if (Check(TokenType::AT_ELEMENT)) {
        ConsumeToken();
        if (isCustom) {
            importType = ImportNode::ImportType::CustomElement;
        } else if (isTemplate) {
            importType = ImportNode::ImportType::TemplateElement;
        } else {
            ReportError("@Element只能用于[Custom]或[Template]");
            return nullptr;
        }
    } else if (Check(TokenType::AT_VAR)) {
        ConsumeToken();
        if (isCustom) {
            importType = ImportNode::ImportType::CustomVar;
        } else if (isTemplate) {
            importType = ImportNode::ImportType::TemplateVar;
        } else {
            ReportError("@Var只能用于[Custom]或[Template]");
            return nullptr;
        }
    } else if (Check(TokenType::AT_CHTL)) {
        ConsumeToken();
        importType = ImportNode::ImportType::Chtl;
    } else if (Check(TokenType::AT_CJMOD)) {
        ConsumeToken();
        importType = ImportNode::ImportType::CJmod;
    } else if (Check(TokenType::AT_CONFIG)) {
        ConsumeToken();
        importType = ImportNode::ImportType::Config;
    } else if (Check(TokenType::AMPERSAND) && PeekToken().type == TokenType::IDENTIFIER) {
        // 自定义Origin类型，如@Vue
        ConsumeToken(); // 消费 @
        if (!Check(TokenType::IDENTIFIER)) {
            ReportError("期望自定义类型名称");
            return nullptr;
        }
        customTypeName = CurrentToken().value;
        ConsumeToken();
        importType = ImportNode::ImportType::OriginCustom;
        importNode->SetCustomTypeName(customTypeName);
    } else {
        // 批量导入
        if (isCustom) {
            // 检查是否指定了具体类型
            if (Check(TokenType::FROM)) {
                importType = ImportNode::ImportType::AllCustom;
            } else {
                ReportError("期望@Type或from关键字");
                return nullptr;
            }
        } else if (isTemplate) {
            if (Check(TokenType::FROM)) {
                importType = ImportNode::ImportType::AllTemplate;
            } else {
                ReportError("期望@Type或from关键字");
                return nullptr;
            }
        } else if (isOrigin) {
            if (Check(TokenType::FROM)) {
                importType = ImportNode::ImportType::AllOrigin;
            } else {
                ReportError("期望@Type或from关键字");
                return nullptr;
            }
        } else if (isConfiguration) {
            if (Check(TokenType::FROM)) {
                importType = ImportNode::ImportType::AllConfiguration;
            } else {
                ReportError("期望@Type或from关键字");
                return nullptr;
            }
        } else {
            ReportError("期望导入类型");
            return nullptr;
        }
    }
    
    importNode->SetImportType(importType);
    
    // 解析可选的项目名称
    if (!Check(TokenType::FROM) && Check(TokenType::IDENTIFIER)) {
        itemName = CurrentToken().value;
        ConsumeToken();
        importNode->SetItemName(itemName);
    }
    
    // 解析from子句
    Expect(TokenType::FROM, "期望 'from'");
    
    // 解析路径（可以是字符串或标识符）
    std::string fromPath;
    if (Check(TokenType::STRING_LITERAL)) {
        fromPath = CurrentToken().value;
        ConsumeToken();
    } else if (Check(TokenType::IDENTIFIER)) {
        // 支持点号分隔的路径
        fromPath = CurrentToken().value;
        ConsumeToken();
        
        while (Check(TokenType::DOT)) {
            fromPath += ".";
            ConsumeToken();
            if (!Check(TokenType::IDENTIFIER)) {
                ReportError("期望路径段");
                return nullptr;
            }
            fromPath += CurrentToken().value;
            ConsumeToken();
        }
    } else {
        ReportError("期望导入路径");
        return nullptr;
    }
    
    importNode->SetFromPath(fromPath);
    
    // 解析可选的as子句
    if (Check(TokenType::AS)) {
        ConsumeToken();
        if (!Check(TokenType::IDENTIFIER)) {
            ReportError("期望标识符");
            return nullptr;
        }
        std::string asName = CurrentToken().value;
        ConsumeToken();
        importNode->SetAsName(asName);
    }
    
    Expect(TokenType::SEMICOLON, "期望 ';'");
    
    return importNode;
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
            
            // 处理变量引用
            attrValue = ProcessVariableReferences(attrValue);
            
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 创建属性节点并添加到元素
            auto attrNode = std::make_shared<AttributeNode>(attrName, attrValue);
            element->AddAttributeNode(attrNode);
        }
        // 元素模板引用
        else if (Check(TokenType::AT_ELEMENT)) {
            ConsumeToken(); // 消费 @Element
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string templateName = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 从全局映射表获取模板
            auto templateNode = context->GetGlobalMap()->GetTemplateElement(templateName);
            if (templateNode) {
                // 将模板中的所有元素添加到当前元素
                for (const auto& child : templateNode->GetChildren()) {
                    // 深拷贝子元素
                    element->AddChild(child);
                }
            } else {
                ReportError("未找到元素模板: " + templateName);
            }
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
    
    // 处理变量引用
    value = ProcessVariableReferences(value);
    
    Expect(TokenType::SEMICOLON, "期望 ';'");
    
    // 创建属性节点
    return std::make_shared<AttributeNode>(name, value);
}

std::shared_ptr<ASTNode> Parser::ParseLocalStyle() {
    Expect(TokenType::STYLE, "期望 'style'");
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto styleNode = std::make_shared<LocalStyleNode>(CurrentToken().line, CurrentToken().column);
    
    // 解析样式属性或样式模板引用
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::AT_STYLE)) {
            // 样式模板引用: @Style 模板名;
            ConsumeToken(); // 消费 @Style
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string templateName = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 从全局映射表获取模板
            auto templateNode = context->GetGlobalMap()->GetTemplateStyle(templateName);
            if (templateNode) {
                // 将模板中的所有属性添加到当前样式节点
                for (const auto& child : templateNode->GetChildren()) {
                    if (child->GetType() == ASTNodeType::StyleProperty) {
                        auto prop = std::static_pointer_cast<StylePropertyNode>(child);
                        styleNode->AddProperty(prop->GetName(), prop->GetValue());
                    }
                }
            } else {
                ReportError("未找到样式模板: " + templateName);
            }
        }
        else if (Check(TokenType::IDENTIFIER)) {
            // 样式属性名
            std::string propName = CurrentToken().value;
            ConsumeToken();
            
            Expect(TokenType::COLON, "期望 ':'");
            
            // 样式属性值
            std::string propValue = ParseStringOrUnquotedLiteral();
            
            // 处理变量引用
            propValue = ProcessVariableReferences(propValue);
            
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
        
        // 处理变量引用
        value = ProcessVariableReferences(value);
        
        Expect(TokenType::SEMICOLON, "期望 ';'");
        
        // 创建样式属性节点
        return std::make_shared<StylePropertyNode>(propName, value);
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
        
        // 检查是否有紧跟的单位（如px, %, em等）
        if (Check(TokenType::IDENTIFIER)) {
            // 可能是CSS单位
            std::string unit = CurrentToken().value;
            // 常见的CSS单位
            if (unit == "px" || unit == "%" || unit == "em" || unit == "rem" || 
                unit == "vh" || unit == "vw" || unit == "pt" || unit == "pc" ||
                unit == "cm" || unit == "mm" || unit == "in" || unit == "ex" ||
                unit == "ch" || unit == "deg" || unit == "rad" || unit == "turn" ||
                unit == "s" || unit == "ms" || unit == "fr") {
                value += unit;
                ConsumeToken();
            }
        }
        
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

std::string Parser::ProcessVariableReferences(const std::string& value) {
    std::string result = value;
    size_t pos = 0;
    
    LOG_DEBUG("处理变量引用: " + value);
    
    // 查找所有的变量引用模式: VarGroupName(varName)
    while ((pos = result.find('(', pos)) != std::string::npos) {
        // 找到左括号，检查是否是变量引用
        size_t endPos = result.find(')', pos);
        if (endPos == std::string::npos) {
            pos++;
            continue;
        }
        
        // 提取变量组名（左括号前的标识符）
        size_t startPos = pos;
        while (startPos > 0 && (std::isalnum(result[startPos-1]) || result[startPos-1] == '_')) {
            startPos--;
        }
        
        if (startPos < pos) {
            std::string varGroupName = result.substr(startPos, pos - startPos);
            std::string varName = result.substr(pos + 1, endPos - pos - 1);
            
            // 去除varName两端的空格
            size_t first = varName.find_first_not_of(" \t");
            size_t last = varName.find_last_not_of(" \t");
            if (first != std::string::npos && last != std::string::npos) {
                varName = varName.substr(first, last - first + 1);
            }
            
            // 从全局映射表获取变量组
            LOG_DEBUG("查找变量组: " + varGroupName + ", 变量名: " + varName);
            auto varGroup = context->GetGlobalMap()->GetTemplateVar(varGroupName);
            if (varGroup) {
                std::string varValue = varGroup->GetVariable(varName);
                LOG_DEBUG("找到变量值: " + varValue);
                if (!varValue.empty()) {
                    // 替换整个变量引用
                    result.replace(startPos, endPos - startPos + 1, varValue);
                    pos = startPos + varValue.length();
                    continue;
                }
            } else {
                LOG_DEBUG("未找到变量组: " + varGroupName);
            }
        }
        
        pos++;
    }
    
    LOG_DEBUG("处理后的值: " + result);
    return result;
}

} // namespace CHTL