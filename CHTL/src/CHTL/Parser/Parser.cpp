#include "CHTL/Parser/Parser.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"
#include "CHTL/Core/SpecializationProcessor.h"
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
        LOG_DEBUG("Expect失败: 期望类型=" + std::to_string(static_cast<int>(type)) + 
                  ", 实际类型=" + std::to_string(static_cast<int>(CurrentToken().type)) + 
                  ", 实际值='" + CurrentToken().value + "'");
        ReportError(message);
    }
    ConsumeToken();
}

void Parser::ExpectColonOrEquals(const std::string& message) {
    if (!CheckColonOrEquals()) {
        ReportError(message);
    }
    ConsumeToken();
}

bool Parser::CheckColonOrEquals() {
    return Check(TokenType::COLON) || Check(TokenType::EQUALS);
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
    
    // except约束（用于全局约束）
    if (Check(TokenType::EXCEPT)) {
        return ParseExcept();
    }
    
    // 元素
    if (IsElementStart()) {
        return ParseElement();
    }
    
    // 未知的顶层声明
    ReportError("期望顶层声明，但得到: " + CurrentToken().value);
    return nullptr;
}

void Parser::RecordViolation(const std::string& message) {
    if (boundaryChecker) {
        int line = currentIndex < tokens.size() ? tokens[currentIndex].line : -1;
        int column = currentIndex < tokens.size() ? tokens[currentIndex].column : -1;
        boundaryChecker->RecordViolation(message, line, column);
    }
}

std::shared_ptr<ASTNode> Parser::ParseUseStatement() {
    // 检查语法边界
    if (boundaryChecker) {
        std::string error;
        if (!boundaryChecker->GetConstraint()->ValidateSyntax(
                SyntaxConstraint::Feature::UseStatement, error)) {
            RecordViolation(error);
        }
    }
    
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
    
    // use @Config 配置名; 或 use [Configuration] @Config 配置名;
    if (Check(TokenType::AT_CONFIG) || Check(TokenType::CONFIGURATION)) {
        bool isFullName = Check(TokenType::CONFIGURATION);
        if (isFullName) {
            ConsumeToken(); // 消费 [Configuration]
            Expect(TokenType::AT_CONFIG, "期望 '@Config'");
        }
        ConsumeToken();
        Expect(TokenType::IDENTIFIER, "期望标识符");
        std::string configName = CurrentToken().value;
        ConsumeToken();
        Expect(TokenType::SEMICOLON, "期望 ';'");
        
        // 从全局映射表获取配置组
        auto config = context->GetGlobalMap()->GetConfiguration(configName);
        if (config) {
            ApplyConfiguration(config);
        } else {
            ReportError("未找到配置组: " + configName);
        }
        
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
    
    // 添加到全局映射表（使用完整的命名空间路径）
    std::string fullName = name;
    std::string namespacePath = context->GetFullNamespacePath();
    if (!namespacePath.empty()) {
        fullName = namespacePath + "." + name;
    }
    context->GetGlobalMap()->AddTemplateStyle(fullName, node);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析样式属性和继承语句
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::INHERIT)) {
            // 处理继承语句: inherit @Style 样式组名;
            ConsumeToken(); // 消费 inherit
            Expect(TokenType::AT_STYLE, "期望 '@Style'");
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string inheritedStyleName = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 从全局映射表获取被继承的样式模板
            auto inheritedStyle = context->GetGlobalMap()->GetTemplateStyle(inheritedStyleName);
            if (inheritedStyle) {
                // 复制被继承样式的所有属性
                for (const auto& child : inheritedStyle->GetChildren()) {
                    if (child->GetType() == ASTNodeType::StyleProperty) {
                        node->AddChild(child);
                    }
                }
            } else {
                ReportError("未找到样式模板: " + inheritedStyleName);
            }
        } else {
            auto property = ParseStyleProperty();
            if (property) {
                node->AddChild(property);
            }
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
    
    // 添加到全局映射表（使用完整的命名空间路径）
    std::string fullName = name;
    std::string namespacePath = context->GetFullNamespacePath();
    if (!namespacePath.empty()) {
        fullName = namespacePath + "." + name;
    }
    context->GetGlobalMap()->AddTemplateElement(fullName, node);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析元素内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::INHERIT)) {
            // 处理继承语句: inherit @Element 元素模板名;
            ConsumeToken(); // 消费 inherit
            Expect(TokenType::AT_ELEMENT, "期望 '@Element'");
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string inheritedElementName = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 从全局映射表获取被继承的元素模板
            auto inheritedElement = context->GetGlobalMap()->GetTemplateElement(inheritedElementName);
            if (inheritedElement) {
                // 复制被继承元素的所有子节点
                for (const auto& child : inheritedElement->GetChildren()) {
                    node->AddChild(child);
                }
            } else {
                ReportError("未找到元素模板: " + inheritedElementName);
            }
        } else if (IsElementStart()) {
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
    
    // 添加到全局映射表（使用完整的命名空间路径）
    std::string fullName = name;
    std::string namespacePath = context->GetFullNamespacePath();
    if (!namespacePath.empty()) {
        fullName = namespacePath + "." + name;
    }
    context->GetGlobalMap()->AddTemplateVar(fullName, node);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析变量定义
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // 解析变量属性: 名称: 值;
        if (Check(TokenType::IDENTIFIER)) {
            std::string varName = CurrentToken().value;
            ConsumeToken();
            
            ExpectColonOrEquals("期望 ':' 或 '='");
            
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
    Expect(TokenType::ORIGIN, "期望 '[Origin]'");
    
    auto originNode = std::make_shared<OriginNode>();
    
    // 解析原始嵌入类型
    if (Check(TokenType::AT_HTML)) {
        ConsumeToken();
        originNode->SetType(OriginNode::OriginType::Html);
    } else if (Check(TokenType::AT_STYLE)) {
        ConsumeToken();
        originNode->SetType(OriginNode::OriginType::Style);
    } else if (Check(TokenType::AT_JAVASCRIPT)) {
        ConsumeToken();
        originNode->SetType(OriginNode::OriginType::JavaScript);
    } else if (Check(TokenType::AT) && PeekToken().type == TokenType::IDENTIFIER) {
        // 自定义原始嵌入类型，如 @Vue
        ConsumeToken(); // 消费 @
        
        if (!context->IsCustomOriginTypeDisabled()) {
            std::string customType = CurrentToken().value;
            ConsumeToken();
            originNode->SetType(OriginNode::OriginType::Custom);
            originNode->SetCustomTypeName(customType);
        } else {
            ReportError("自定义原始嵌入类型已被禁用");
            RecoverFromError();
            return nullptr;
        }
    } else {
        ReportError("期望原始嵌入类型 (@Html, @Style, @JavaScript 或自定义类型)");
        RecoverFromError();
        return nullptr;
    }
    
    // 解析可选的名称
    if (Check(TokenType::IDENTIFIER)) {
        originNode->SetName(CurrentToken().value);
        ConsumeToken();
    }
    
    // 解析内容块或引用
    if (Check(TokenType::LEFT_BRACE)) {
        ConsumeToken(); // 消费 {
        
        // 收集原始内容直到遇到 }
        std::string content;
        int braceDepth = 1;
        
        while (braceDepth > 0 && !Check(TokenType::EOF_TOKEN)) {
            if (Check(TokenType::LEFT_BRACE)) {
                braceDepth++;
            } else if (Check(TokenType::RIGHT_BRACE)) {
                braceDepth--;
                if (braceDepth == 0) break;
            }
            
            content += CurrentToken().value;
            if (CurrentToken().type == TokenType::NEWLINE) {
                content += "\n";
            } else {
                content += " ";
            }
            ConsumeToken();
        }
        
        Expect(TokenType::RIGHT_BRACE, "期望 '}'");
        originNode->SetContent(content);
    } else if (Check(TokenType::SEMICOLON)) {
        // 引用已定义的原始嵌入
        ConsumeToken();
    } else {
        ReportError("期望 '{' 或 ';'");
    }
    
    return originNode;
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
            if (Match(TokenType::AT_ELEMENT)) {
                // [Import] [Custom] @Element from path
                importType = ImportNode::ImportType::AllCustomElement;
            } else if (Match(TokenType::AT_STYLE)) {
                // [Import] [Custom] @Style from path
                importType = ImportNode::ImportType::AllCustomStyle;
            } else if (Match(TokenType::AT_VAR)) {
                // [Import] [Custom] @Var from path
                importType = ImportNode::ImportType::AllCustomVar;
            } else if (Check(TokenType::FROM)) {
                // [Import] [Custom] from path
                importType = ImportNode::ImportType::AllCustom;
            } else {
                ReportError("期望@Type或from关键字");
                return nullptr;
            }
        } else if (isTemplate) {
            if (Match(TokenType::AT_ELEMENT)) {
                // [Import] [Template] @Element from path
                importType = ImportNode::ImportType::AllTemplateElement;
            } else if (Match(TokenType::AT_STYLE)) {
                // [Import] [Template] @Style from path
                importType = ImportNode::ImportType::AllTemplateStyle;
            } else if (Match(TokenType::AT_VAR)) {
                // [Import] [Template] @Var from path
                importType = ImportNode::ImportType::AllTemplateVar;
            } else if (Check(TokenType::FROM)) {
                // [Import] [Template] from path
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
    Expect(TokenType::NAMESPACE, "期望 '[Namespace]'");
    
    // 期望命名空间名称
    if (!Check(TokenType::IDENTIFIER)) {
        ReportError("期望命名空间名称");
        return nullptr;
    }
    
    std::string namespaceName = CurrentToken().value;
    ConsumeToken();
    
    auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName);
    
    // 进入命名空间
    context->EnterNamespace(namespaceName);
    
    // 命名空间可以有花括号包围的内容，也可以没有
    if (Check(TokenType::LEFT_BRACE)) {
        ConsumeToken(); // 消费 {
        
        // 解析命名空间内的内容
        while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
            auto child = ParseTopLevelDeclaration();
            if (child) {
                namespaceNode->AddChild(child);
            } else {
                // 如果解析失败，尝试恢复
                RecoverFromError();
            }
        }
        
        Expect(TokenType::RIGHT_BRACE, "期望 '}'");
        
        // 退出命名空间
        context->ExitNamespace();
    }
    // 如果没有花括号，命名空间会影响后续的声明，直到文件结束或遇到另一个命名空间
    // 在这种情况下，命名空间的退出会在文件结束或遇到新命名空间时处理
    
    return namespaceNode;
}

std::shared_ptr<ASTNode> Parser::ParseConfiguration() {
    Expect(TokenType::CONFIGURATION, "期望 '[Configuration]'");
    
    std::string configName;
    
    // 检查是否有 @Config 和名称
    if (Check(TokenType::AT_CONFIG)) {
        ConsumeToken(); // 消费 @Config
        if (Check(TokenType::IDENTIFIER)) {
            configName = CurrentToken().value;
            ConsumeToken();
        }
    }
    
    auto configNode = std::make_shared<ConfigurationNode>(configName);
    
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析配置内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::LEFT_BRACKET)) {
            // 子配置组，如 [Name] 或 [OriginType]
            ConsumeToken(); // 消费 [
            if (!Check(TokenType::IDENTIFIER)) {
                ReportError("期望子配置组名称");
                RecoverFromError();
                continue;
            }
            std::string subGroupName = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::RIGHT_BRACKET, "期望 ']'");
            
            // 解析子配置组
            auto subGroup = ParseConfigSubGroup(subGroupName);
            if (subGroup) {
                configNode->AddSubGroup(subGroupName, subGroup);
            }
        } else if (Check(TokenType::IDENTIFIER)) {
            // 配置选项，如 DEBUG_MODE = true;
            std::string optionName = CurrentToken().value;
            ConsumeToken();
            
            ExpectColonOrEquals("期望 ':' 或 '='");
            
            std::string optionValue;
            if (Check(TokenType::STRING_LITERAL)) {
                optionValue = CurrentToken().value;
                ConsumeToken();
            } else if (Check(TokenType::IDENTIFIER)) {
                // 支持 true/false 等标识符值
                optionValue = CurrentToken().value;
                ConsumeToken();
            } else if (Check(TokenType::NUMBER)) {
                optionValue = CurrentToken().value;
                ConsumeToken();
            } else {
                ReportError("期望配置值");
            }
            
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            configNode->AddOption(optionName, optionValue);
        } else {
            // 跳过未知token
            ConsumeToken();
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 如果是无名配置组，立即应用到上下文
    if (configName.empty()) {
        ApplyConfiguration(configNode);
    }
    
    // 存储配置组到全局映射表
    context->GetGlobalMap()->AddConfiguration(configName.empty() ? "" : configName, configNode);
    
    return configNode;
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
        if (Check(TokenType::IDENTIFIER) && (PeekToken().type == TokenType::COLON || PeekToken().type == TokenType::EQUALS)) {
            std::string attrName = CurrentToken().value;
            ConsumeToken();
            ExpectColonOrEquals("期望 ':' 或 '='");
            std::string attrValue = ParseStringOrUnquotedLiteral();
            
            // 处理变量引用
            attrValue = ProcessVariableReferences(attrValue);
            
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 创建属性节点并添加到元素
            auto attrNode = std::make_shared<AttributeNode>(attrName, attrValue);
            element->AddAttributeNode(attrNode);
        }
        // 元素模板引用（支持全缀名）
        else if (Check(TokenType::AT_ELEMENT) || 
                 (Check(TokenType::CUSTOM) && PeekToken().type == TokenType::AT_ELEMENT) ||
                 (Check(TokenType::TEMPLATE) && PeekToken().type == TokenType::AT_ELEMENT)) {
            
            bool isCustom = false;
            bool isTemplate = false;
            
            // 检查是否有[Custom]或[Template]前缀
            if (Check(TokenType::CUSTOM)) {
                isCustom = true;
                ConsumeToken(); // 消费 [Custom]
            } else if (Check(TokenType::TEMPLATE)) {
                isTemplate = true;
                ConsumeToken(); // 消费 [Template]
            }
            
            ConsumeToken(); // 消费 @Element
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string templateName = CurrentToken().value;
            ConsumeToken();
            
            // 检查是否有 from 子句
            std::string namespacePath;
            if (Check(TokenType::FROM)) {
                ConsumeToken(); // 消费 from
                
                // 解析命名空间路径
                if (!Check(TokenType::IDENTIFIER)) {
                    ReportError("期望命名空间名称");
                } else {
                    namespacePath = CurrentToken().value;
                    ConsumeToken();
                    
                    // 支持点号分隔的命名空间路径
                    while (Check(TokenType::DOT)) {
                        namespacePath += ".";
                        ConsumeToken();
                        if (!Check(TokenType::IDENTIFIER)) {
                            ReportError("期望命名空间段");
                            break;
                        }
                        namespacePath += CurrentToken().value;
                        ConsumeToken();
                    }
                }
            }
            
            // 检查是否有特例化块
            bool hasSpecialization = Check(TokenType::LEFT_BRACE);
            
            if (!hasSpecialization) {
                Expect(TokenType::SEMICOLON, "期望 ';' 或 '{'");
            }
            
            // 构建完整的模板名称
            std::string fullTemplateName = templateName;
            if (!namespacePath.empty()) {
                fullTemplateName = namespacePath + "." + templateName;
            }
            
            // 从全局映射表获取模板（根据全缀名选择）
            std::shared_ptr<ASTNode> templateNode;
            if (isCustom) {
                // 查找自定义元素
                templateNode = context->GetGlobalMap()->GetCustomElement(fullTemplateName);
                if (!templateNode) {
                    ReportError("未找到自定义元素: " + fullTemplateName);
                }
            } else {
                // 查找模板元素（默认或显式指定[Template]）
                templateNode = context->GetGlobalMap()->GetTemplateElement(fullTemplateName);
                if (!templateNode) {
                    ReportError("未找到元素模板: " + fullTemplateName);
                }
            }
            
            if (templateNode) {
                // 将模板中的所有元素添加到当前元素
                for (const auto& child : templateNode->GetChildren()) {
                    // 深拷贝子元素
                    element->AddChild(child);
                }
            }
            
            // 解析特例化块（如果有）
            if (hasSpecialization) {
                ConsumeToken(); // 消费 {
                
                // 特例化操作列表
                std::vector<std::shared_ptr<ASTNode>> specializations;
                
                while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
                    if (Check(TokenType::DELETE)) {
                        auto deleteNode = ParseDelete();
                        if (deleteNode) {
                            specializations.push_back(deleteNode);
                        }
                    } else if (Check(TokenType::INSERT)) {
                        auto insertNode = ParseInsert();
                        if (insertNode) {
                            specializations.push_back(insertNode);
                        }
                    } else if (Check(TokenType::IDENTIFIER)) {
                        // 元素名称匹配特例化
                        std::string elementName = CurrentToken().value;
                        ConsumeToken();
                        
                        // 检查是否有索引 [n]
                        int index = -1;  // -1 表示匹配所有同名元素
                        if (Check(TokenType::LEFT_BRACKET)) {
                            ConsumeToken(); // 消费 [
                            if (Check(TokenType::NUMBER)) {
                                index = std::stoi(CurrentToken().value);
                                ConsumeToken();
                                Expect(TokenType::RIGHT_BRACKET, "期望 ']'");
                            } else {
                                ReportError("期望数字索引");
                            }
                        }
                        
                        // 解析特例化内容块
                        Expect(TokenType::LEFT_BRACE, "期望 '{'");
                        
                        // 创建一个特殊的特例化节点来记录元素匹配
                        auto matchNode = std::make_shared<ElementMatchNode>(elementName, index);
                        
                        while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
                            // 可以添加属性或样式
                            if (Check(TokenType::STYLE)) {
                                auto style = ParseLocalStyle();
                                if (style) {
                                    matchNode->AddChild(style);
                                }
                            } else if (Check(TokenType::IDENTIFIER)) {
                                // 属性
                                auto attr = ParseAttribute();
                                if (attr) {
                                    matchNode->AddChild(attr);
                                }
                            } else {
                                ConsumeToken(); // 跳过未知token
                            }
                        }
                        
                        Expect(TokenType::RIGHT_BRACE, "期望 '}'");
                        specializations.push_back(matchNode);
                    } else {
                        ReportError("期望 'delete'、'insert' 或元素名");
                        ConsumeToken(); // 跳过无效token
                    }
                }
                
                Expect(TokenType::RIGHT_BRACE, "期望 '}'");
                
                // 应用特例化操作到元素
                if (templateNode && !specializations.empty()) {
                    // 创建特例化处理器
                    SpecializationProcessor processor;
                    
                    // 深拷贝模板元素
                    auto clonedTemplate = std::static_pointer_cast<ElementNode>(templateNode);
                    
                    // 应用特例化
                    processor.ProcessSpecializations(clonedTemplate, specializations);
                    
                    // 使用特例化后的元素
                    element = clonedTemplate;
                }
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
        // except约束
        else if (Check(TokenType::EXCEPT)) {
            auto except = ParseExcept();
            if (except) {
                element->AddChild(except);
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
    
    ExpectColonOrEquals("期望 ':' 或 '='");
    
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
        if (Check(TokenType::AT_STYLE) ||
            (Check(TokenType::CUSTOM) && PeekToken().type == TokenType::AT_STYLE) ||
            (Check(TokenType::TEMPLATE) && PeekToken().type == TokenType::AT_STYLE)) {
            
            bool isCustom = false;
            bool isTemplate = false;
            
            // 检查是否有[Custom]或[Template]前缀
            if (Check(TokenType::CUSTOM)) {
                isCustom = true;
                ConsumeToken(); // 消费 [Custom]
            } else if (Check(TokenType::TEMPLATE)) {
                isTemplate = true;
                ConsumeToken(); // 消费 [Template]
            }
            
            // 样式模板引用: @Style 模板名 [from 命名空间];
            ConsumeToken(); // 消费 @Style
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string templateName = CurrentToken().value;
            ConsumeToken();
            
            // 检查是否有 from 子句
            std::string namespacePath;
            if (Check(TokenType::FROM)) {
                ConsumeToken(); // 消费 from
                
                // 解析命名空间路径
                if (!Check(TokenType::IDENTIFIER)) {
                    ReportError("期望命名空间名称");
                } else {
                    namespacePath = CurrentToken().value;
                    ConsumeToken();
                    
                    // 支持点号分隔的命名空间路径
                    while (Check(TokenType::DOT)) {
                        namespacePath += ".";
                        ConsumeToken();
                        if (!Check(TokenType::IDENTIFIER)) {
                            ReportError("期望命名空间段");
                            break;
                        }
                        namespacePath += CurrentToken().value;
                        ConsumeToken();
                    }
                }
            }
            
            // 检查是否有特例化块
            bool hasSpecialization = Check(TokenType::LEFT_BRACE);
            
            if (!hasSpecialization) {
                Expect(TokenType::SEMICOLON, "期望 ';' 或 '{'");
            }
            
            // 构建完整的模板名称
            std::string fullTemplateName = templateName;
            if (!namespacePath.empty()) {
                fullTemplateName = namespacePath + "." + templateName;
            }
            
            // 从全局映射表获取模板（根据全缀名选择）
            std::shared_ptr<TemplateStyleNode> templateNode;
            if (isCustom) {
                // 查找自定义样式组
                auto customStyle = context->GetGlobalMap()->GetCustomStyle(fullTemplateName);
                if (!customStyle) {
                    ReportError("未找到自定义样式组: " + fullTemplateName);
                } else {
                    // 将自定义样式组的属性添加到当前样式节点
                    for (const auto& child : customStyle->GetChildren()) {
                        if (child->GetType() == ASTNodeType::StyleProperty) {
                            auto prop = std::static_pointer_cast<StylePropertyNode>(child);
                            styleNode->AddProperty(prop->GetName(), prop->GetValue());
                        }
                    }
                }
            } else {
                // 查找模板样式（默认或显式指定[Template]）
                templateNode = context->GetGlobalMap()->GetTemplateStyle(fullTemplateName);
                if (templateNode) {
                    // 将模板中的所有属性添加到当前样式节点
                    for (const auto& child : templateNode->GetChildren()) {
                        if (child->GetType() == ASTNodeType::StyleProperty) {
                            auto prop = std::static_pointer_cast<StylePropertyNode>(child);
                            styleNode->AddProperty(prop->GetName(), prop->GetValue());
                        }
                    }
                } else {
                    ReportError("未找到样式模板: " + fullTemplateName);
                }
            }
            
            // 解析特例化块（如果有）- 仅适用于自定义样式
            if (hasSpecialization && isCustom) {
                ConsumeToken(); // 消费 {
                
                while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
                    if (Check(TokenType::DELETE)) {
                        // 解析删除样式属性
                        ConsumeToken(); // 消费 delete
                        
                        // 收集要删除的属性名
                        std::vector<std::string> propsToDelete;
                        while (!Check(TokenType::SEMICOLON) && !Check(TokenType::EOF_TOKEN)) {
                            if (Check(TokenType::IDENTIFIER)) {
                                propsToDelete.push_back(CurrentToken().value);
                                ConsumeToken();
                                
                                if (Check(TokenType::COMMA)) {
                                    ConsumeToken();
                                }
                            } else {
                                ReportError("期望属性名");
                                ConsumeToken();
                            }
                        }
                        
                        Expect(TokenType::SEMICOLON, "期望 ';'");
                        
                        // TODO: 应用删除操作
                        // 这需要在LocalStyleNode中实现RemoveProperty方法
                    } else {
                        ReportError("样式特例化中只支持 'delete'");
                        ConsumeToken();
                    }
                }
                
                Expect(TokenType::RIGHT_BRACE, "期望 '}'");
            } else if (hasSpecialization && !isCustom) {
                ReportError("只有自定义样式支持特例化");
                // 跳过特例化块
                int braceLevel = 1;
                ConsumeToken(); // 消费 {
                while (braceLevel > 0 && !Check(TokenType::EOF_TOKEN)) {
                    if (Check(TokenType::LEFT_BRACE)) {
                        braceLevel++;
                    } else if (Check(TokenType::RIGHT_BRACE)) {
                        braceLevel--;
                    }
                    ConsumeToken();
                }
            }
        }
        else if (Check(TokenType::DOT) || Check(TokenType::HASH) || Check(TokenType::AMPERSAND)) {
            // 样式规则（类选择器、ID选择器、& 引用）
            auto rule = std::dynamic_pointer_cast<StyleRuleNode>(ParseStyleRule());
            if (rule) {
                styleNode->AddRule(rule);
            }
        }
        else if (Check(TokenType::IDENTIFIER)) {
            // 样式属性名
            std::string propName = CurrentToken().value;
            ConsumeToken();
            
            ExpectColonOrEquals("期望 ':' 或 '='");
            
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
    // 检查语法边界
    if (boundaryChecker) {
        std::string error;
        if (!boundaryChecker->GetConstraint()->ValidateSyntax(
                SyntaxConstraint::Feature::LocalScriptBlock, error)) {
            RecordViolation(error);
        }
        // 进入Script上下文
        boundaryChecker->GetConstraint()->EnterContext(SyntaxConstraint::Context::Script);
    }
    
    Expect(TokenType::SCRIPT, "期望 'script'");
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto scriptNode = std::make_shared<LocalScriptNode>();
    
    // 解析脚本内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // TODO: 解析脚本内容
        ConsumeToken();
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 退出Script上下文
    if (boundaryChecker) {
        boundaryChecker->GetConstraint()->ExitContext();
    }
    
    return scriptNode;
}

std::shared_ptr<ASTNode> Parser::ParseExcept() {
    Expect(TokenType::EXCEPT, "期望 'except'");
    
    auto exceptNode = std::make_shared<ExceptNode>();
    
    // 解析约束列表
    bool first = true;
    while (!Check(TokenType::SEMICOLON) && !Check(TokenType::EOF_TOKEN)) {
        if (!first) {
            Expect(TokenType::COMMA, "期望 ','");
        }
        first = false;
        
        // 检查是否是类型约束
        if (Check(TokenType::AMPERSAND)) {
            // @Html, @Style等
            ConsumeToken(); // 消费 @
            if (Check(TokenType::IDENTIFIER)) {
                std::string typeName = CurrentToken().value;
                ConsumeToken();
                exceptNode->AddConstraint(ExceptNode::ConstraintType::Type, typeName);
            } else {
                ReportError("期望类型名");
            }
        } else if (Check(TokenType::LEFT_BRACKET)) {
            // [Custom], [Template]等
            ConsumeToken(); // 消费 [
            if (!Check(TokenType::IDENTIFIER)) {
                ReportError("期望修饰符");
                continue;
            }
            std::string modifier = CurrentToken().value;
            ConsumeToken();
            Expect(TokenType::RIGHT_BRACKET, "期望 ']'");
            
            // 检查是否有具体的类型
            if (Check(TokenType::AMPERSAND)) {
                ConsumeToken(); // 消费 @
                if (Check(TokenType::IDENTIFIER)) {
                    std::string typeName = CurrentToken().value;
                    ConsumeToken();
                    exceptNode->AddConstraint(ExceptNode::ConstraintType::Type, 
                                            typeName, "[" + modifier + "]");
                } else {
                    // 只有修饰符，如 [Custom]
                    exceptNode->AddConstraint(ExceptNode::ConstraintType::Type, 
                                            "", "[" + modifier + "]");
                }
            } else {
                // 只有修饰符，如 [Template]
                exceptNode->AddConstraint(ExceptNode::ConstraintType::Type, 
                                        "", "[" + modifier + "]");
            }
        } else if (Check(TokenType::IDENTIFIER)) {
            // 精确约束，如 span, div
            std::string elementName = CurrentToken().value;
            ConsumeToken();
            exceptNode->AddConstraint(ExceptNode::ConstraintType::Exact, elementName);
        } else {
            ReportError("无效的约束");
            ConsumeToken(); // 跳过无效token
        }
    }
    
    Expect(TokenType::SEMICOLON, "期望 ';'");
    
    return exceptNode;
}

std::shared_ptr<ASTNode> Parser::ParseDelete() {
    Expect(TokenType::DELETE, "期望 'delete'");
    
    auto deleteNode = std::make_shared<DeleteNode>(DeleteNode::DeleteTarget::Element);
    
    // 解析删除目标
    while (!Check(TokenType::SEMICOLON) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::AT_ELEMENT) || Check(TokenType::AT_STYLE) || Check(TokenType::AT_VAR)) {
            // 删除模板引用，如 delete @Element Line;
            deleteNode = std::make_shared<DeleteNode>(DeleteNode::DeleteTarget::Template);
            
            TokenType atType = CurrentToken().type;
            ConsumeToken(); // 消费 @Element/@Style/@Var
            
            Expect(TokenType::IDENTIFIER, "期望标识符");
            std::string templateName = CurrentToken().value;
            ConsumeToken();
            
            // 构建完整的模板引用名称
            if (atType == TokenType::AT_ELEMENT) {
                deleteNode->AddTarget("@Element " + templateName);
            } else if (atType == TokenType::AT_STYLE) {
                deleteNode->AddTarget("@Style " + templateName);
            } else if (atType == TokenType::AT_VAR) {
                deleteNode->AddTarget("@Var " + templateName);
            }
        } else if (Check(TokenType::IDENTIFIER)) {
            // 删除元素或属性
            std::string target = CurrentToken().value;
            ConsumeToken();
            
            // 检查是否有索引，如 div[1]
            if (Check(TokenType::LEFT_BRACKET)) {
                ConsumeToken(); // 消费 [
                if (Check(TokenType::NUMBER)) {
                    target += "[" + CurrentToken().value + "]";
                    ConsumeToken();
                    Expect(TokenType::RIGHT_BRACKET, "期望 ']'");
                } else {
                    ReportError("期望数字索引");
                }
            }
            
            deleteNode->AddTarget(target);
        } else if (Check(TokenType::COMMA)) {
            ConsumeToken(); // 消费逗号
        } else {
            ReportError("无效的删除目标");
            ConsumeToken(); // 跳过无效token
        }
    }
    
    Expect(TokenType::SEMICOLON, "期望 ';'");
    
    return deleteNode;
}

std::shared_ptr<ASTNode> Parser::ParseInsert() {
    Expect(TokenType::INSERT, "期望 'insert'");
    
    // 解析位置
    InsertNode::InsertPosition position = InsertNode::InsertPosition::After;
    
    if (Check(TokenType::AFTER)) {
        position = InsertNode::InsertPosition::After;
        ConsumeToken();
    } else if (Check(TokenType::BEFORE)) {
        position = InsertNode::InsertPosition::Before;
        ConsumeToken();
    } else if (Check(TokenType::REPLACE)) {
        position = InsertNode::InsertPosition::Replace;
        ConsumeToken();
    } else if (Check(TokenType::AT)) {
        ConsumeToken(); // 消费 at
        if (Check(TokenType::TOP)) {
            position = InsertNode::InsertPosition::AtTop;
            ConsumeToken();
        } else if (Check(TokenType::BOTTOM)) {
            position = InsertNode::InsertPosition::AtBottom;
            ConsumeToken();
        } else {
            ReportError("期望 'top' 或 'bottom'");
        }
    }
    
    auto insertNode = std::make_shared<InsertNode>(position);
    
    // 解析选择器（如果有）
    if (position != InsertNode::InsertPosition::AtTop && 
        position != InsertNode::InsertPosition::AtBottom &&
        Check(TokenType::IDENTIFIER)) {
        
        std::string selector = CurrentToken().value;
        ConsumeToken();
        
        // 检查是否有索引，如 div[0]
        if (Check(TokenType::LEFT_BRACKET)) {
            ConsumeToken(); // 消费 [
            if (Check(TokenType::NUMBER)) {
                selector += "[" + CurrentToken().value + "]";
                ConsumeToken();
                Expect(TokenType::RIGHT_BRACKET, "期望 ']'");
            } else {
                ReportError("期望数字索引");
            }
        }
        
        insertNode->SetSelector(selector);
    }
    
    // 解析要插入的内容
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (IsElementStart()) {
            auto element = ParseElement();
            if (element) {
                insertNode->AddContent(element);
            }
        } else if (Check(TokenType::AT_ELEMENT)) {
            // 元素模板引用
            // TODO: 解析元素模板引用
            ConsumeToken(); // 暂时跳过
        } else {
            ConsumeToken(); // 跳过未知token
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return insertNode;
}

std::shared_ptr<ASTNode> Parser::ParseStyleRule() {
    auto rule = std::make_shared<StyleRuleNode>();
    
    // 解析选择器
    if (Check(TokenType::DOT)) {
        // 类选择器 .classname
        ConsumeToken(); // 消费 .
        if (!Check(TokenType::IDENTIFIER)) {
            ReportError("期望类名");
            return nullptr;
        }
        rule->SetType(StyleRuleNode::SelectorType::Class);
        rule->SetSelector("." + CurrentToken().value);
        ConsumeToken();
    } else if (Check(TokenType::HASH)) {
        // ID选择器 #id
        ConsumeToken(); // 消费 #
        if (!Check(TokenType::IDENTIFIER)) {
            ReportError("期望ID名");
            return nullptr;
        }
        rule->SetType(StyleRuleNode::SelectorType::Id);
        rule->SetSelector("#" + CurrentToken().value);
        ConsumeToken();
    } else if (Check(TokenType::AMPERSAND)) {
        // & 引用选择器
        ConsumeToken(); // 消费 &
        rule->SetType(StyleRuleNode::SelectorType::Reference);
        
        // 检查是否是伪类或伪元素
        if (Check(TokenType::COLON)) {
            ConsumeToken(); // 消费 :
            
            // 检查是否是伪元素 ::
            if (Check(TokenType::COLON)) {
                ConsumeToken(); // 消费第二个 :
                rule->SetType(StyleRuleNode::SelectorType::PseudoElement);
                if (!Check(TokenType::IDENTIFIER)) {
                    ReportError("期望伪元素名");
                    return nullptr;
                }
                rule->SetSelector("&::" + CurrentToken().value);
                ConsumeToken();
            } else {
                // 伪类
                rule->SetType(StyleRuleNode::SelectorType::PseudoClass);
                if (!Check(TokenType::IDENTIFIER)) {
                    ReportError("期望伪类名");
                    return nullptr;
                }
                rule->SetSelector("&:" + CurrentToken().value);
                ConsumeToken();
            }
        } else {
            rule->SetSelector("&");
        }
    } else {
        ReportError("期望选择器 (., #, 或 &)");
        return nullptr;
    }
    
    // 解析样式块
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        auto prop = std::dynamic_pointer_cast<StylePropertyNode>(ParseStyleProperty());
        if (prop) {
            rule->AddProperty(prop);
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return rule;
}

std::shared_ptr<ASTNode> Parser::ParseStyleProperty() {
    // 解析样式属性: 名称: 值; 或 名称, (无值属性)
    if (Check(TokenType::IDENTIFIER)) {
        std::string propName = CurrentToken().value;
        ConsumeToken();
        
        // 检查是否是无值属性（后面跟逗号或分号）
        if (Check(TokenType::COMMA) || Check(TokenType::SEMICOLON)) {
            // 无值属性
            if (Check(TokenType::COMMA)) {
                ConsumeToken(); // 消费逗号
            } else {
                ConsumeToken(); // 消费分号
            }
            
            // 创建无值样式属性节点（值为空字符串表示无值）
            return std::make_shared<StylePropertyNode>(propName, "");
        } else {
            // 有值属性
            ExpectColonOrEquals("期望 ':' 或 '='");
            
            std::string value = ParseStringOrUnquotedLiteral();
            
            // 处理变量引用
            value = ProcessVariableReferences(value);
            
            LOG_DEBUG("ParseStyleProperty: 期望分号, 当前Token类型=" + std::to_string(static_cast<int>(CurrentToken().type)) + 
                      ", 值='" + CurrentToken().value + "'");
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            // 创建样式属性节点
            return std::make_shared<StylePropertyNode>(propName, value);
        }
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
    // 错误恢复策略：根据当前上下文智能恢复
    
    // 记录当前位置用于调试
    if (currentIndex < tokens.size()) {
        LOG_DEBUG("错误恢复开始，当前token: " + tokens[currentIndex].value + 
                  " 类型: " + std::to_string(static_cast<int>(tokens[currentIndex].type)));
    }
    
    // 策略1：如果在花括号内，尝试跳到匹配的右花括号
    int braceDepth = 0;
    bool inBraceContext = false;
    
    // 检查之前是否有未匹配的左花括号
    for (size_t i = 0; i < currentIndex && i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::LEFT_BRACE) {
            braceDepth++;
        } else if (tokens[i].type == TokenType::RIGHT_BRACE) {
            braceDepth--;
        }
    }
    
    if (braceDepth > 0) {
        inBraceContext = true;
        LOG_DEBUG("在花括号内部，深度: " + std::to_string(braceDepth));
    }
    
    // 错误恢复：跳到下一个合适的位置
    while (!Check(TokenType::EOF_TOKEN)) {
        TokenType currentType = CurrentToken().type;
        
        // 如果在花括号内，优先找到匹配的右花括号
        if (inBraceContext) {
            if (currentType == TokenType::LEFT_BRACE) {
                braceDepth++;
            } else if (currentType == TokenType::RIGHT_BRACE) {
                braceDepth--;
                if (braceDepth == 0) {
                    LOG_DEBUG("找到匹配的右花括号");
                    ConsumeToken(); // 消费右花括号
                    break;
                }
            }
        }
        
        // 检查是否到达了新的顶层声明
        if (braceDepth == 0 && (IsTopLevelKeyword() || IsElementStart())) {
            LOG_DEBUG("找到新的顶层声明");
            break;
        }
        
        // 检查是否到达了语句结束
        if (currentType == TokenType::SEMICOLON && braceDepth == 0) {
            ConsumeToken(); // 消费分号
            LOG_DEBUG("找到语句结束");
            break;
        }
        
        ConsumeToken();
    }
    
    LOG_DEBUG("错误恢复完成");
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
            
            // 检查是否包含特例化语法 varName = newValue
            size_t equalPos = varName.find('=');
            std::string actualVarName = varName;
            std::string overrideValue;
            bool hasOverride = false;
            
            if (equalPos != std::string::npos) {
                // 提取实际变量名和覆盖值
                actualVarName = varName.substr(0, equalPos);
                overrideValue = varName.substr(equalPos + 1);
                
                // 去除空格
                actualVarName.erase(0, actualVarName.find_first_not_of(" \t"));
                actualVarName.erase(actualVarName.find_last_not_of(" \t") + 1);
                overrideValue.erase(0, overrideValue.find_first_not_of(" \t"));
                overrideValue.erase(overrideValue.find_last_not_of(" \t") + 1);
                
                hasOverride = true;
            }
            
            // 从全局映射表获取变量组
            LOG_DEBUG("查找变量组: " + varGroupName + ", 变量名: " + actualVarName);
            
            if (hasOverride) {
                // 使用覆盖值
                LOG_DEBUG("使用特例化值: " + overrideValue);
                result.replace(startPos, endPos - startPos + 1, overrideValue);
                pos = startPos + overrideValue.length();
                continue;
            } else {
                // 使用原始值
                auto varGroup = context->GetGlobalMap()->GetTemplateVar(varGroupName);
                if (varGroup) {
                    std::string varValue = varGroup->GetVariable(actualVarName);
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
        }
        
        pos++;
    }
    
    LOG_DEBUG("处理后的值: " + result);
    return result;
}

std::shared_ptr<ASTNode> Parser::ParseConfigSubGroup(const std::string& groupName) {
    Expect(TokenType::LEFT_BRACE, "期望 '{'");
    
    auto subGroup = std::make_shared<ConfigurationNode>(groupName);
    
    // 解析子配置组内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::IDENTIFIER)) {
            std::string key = CurrentToken().value;
            ConsumeToken();
            
            ExpectColonOrEquals("期望 ':' 或 '='");
            
            std::string value;
            // 支持组选项语法，如 [@Style, @style, @CSS]
            if (Check(TokenType::LEFT_BRACKET)) {
                ConsumeToken(); // 消费 [
                value = "[";
                bool first = true;
                int optionCount = 0;
                while (!Check(TokenType::RIGHT_BRACKET) && !Check(TokenType::EOF_TOKEN)) {
                    // 检查组选项数量限制
                    if (optionCount >= context->GetOptionCount()) {
                        ReportError("组选项数量超过限制 OPTION_COUNT=" + 
                                   std::to_string(context->GetOptionCount()));
                        // 跳过剩余的选项
                        while (!Check(TokenType::RIGHT_BRACKET) && !Check(TokenType::EOF_TOKEN)) {
                            ConsumeToken();
                        }
                        break;
                    }
                    
                    if (!first) value += ", ";
                    first = false;
                    
                    if (Check(TokenType::AMPERSAND)) {
                        value += "@";
                        ConsumeToken();
                    }
                    if (Check(TokenType::IDENTIFIER)) {
                        value += CurrentToken().value;
                        ConsumeToken();
                        optionCount++;
                    }
                    if (Check(TokenType::COMMA)) {
                        ConsumeToken();
                    }
                }
                value += "]";
                Expect(TokenType::RIGHT_BRACKET, "期望 ']'");
            } else if (Check(TokenType::AMPERSAND)) {
                // 单个 @Type 值
                value = "@";
                ConsumeToken();
                if (Check(TokenType::IDENTIFIER)) {
                    value += CurrentToken().value;
                    ConsumeToken();
                }
            } else if (Check(TokenType::STRING_LITERAL)) {
                value = CurrentToken().value;
                ConsumeToken();
            } else if (Check(TokenType::IDENTIFIER)) {
                value = CurrentToken().value;
                ConsumeToken();
            } else if (Check(TokenType::NUMBER)) {
                value = CurrentToken().value;
                ConsumeToken();
            }
            
            Expect(TokenType::SEMICOLON, "期望 ';'");
            
            subGroup->AddOption(key, value);
        } else {
            ConsumeToken(); // 跳过未知token
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 '}'");
    
    return subGroup;
}

void Parser::ApplyConfiguration(std::shared_ptr<ConfigurationNode> config) {
    if (!config) return;
    
    // 应用配置选项到上下文
    auto options = config->GetOptions();
    
    // 处理各种配置选项
    auto it = options.find("DISABLE_STYLE_AUTO_ADD_CLASS");
    if (it != options.end()) {
        context->SetDisableStyleAutoAddClass(it->second == "true");
    }
    
    it = options.find("DISABLE_STYLE_AUTO_ADD_ID");
    if (it != options.end()) {
        context->SetDisableStyleAutoAddId(it->second == "true");
    }
    
    it = options.find("DISABLE_SCRIPT_AUTO_ADD_CLASS");
    if (it != options.end()) {
        context->SetDisableScriptAutoAddClass(it->second == "true");
    }
    
    it = options.find("DISABLE_SCRIPT_AUTO_ADD_ID");
    if (it != options.end()) {
        context->SetDisableScriptAutoAddId(it->second == "true");
    }
    
    it = options.find("DISABLE_DEFAULT_NAMESPACE");
    if (it != options.end()) {
        context->SetDisableDefaultNamespace(it->second == "true");
    }
    
    it = options.find("DEBUG_MODE");
    if (it != options.end()) {
        context->SetDebugMode(it->second == "true");
    }
    
    it = options.find("INDEX_INITIAL_COUNT");
    if (it != options.end()) {
        try {
            context->SetIndexInitialCount(std::stoi(it->second));
        } catch (...) {
            LOG_ERROR("无效的INDEX_INITIAL_COUNT值: " + it->second);
        }
    }
    
    it = options.find("DISABLE_NAME_GROUP");
    if (it != options.end()) {
        context->SetDisableNameGroup(it->second == "true");
    }
    
    it = options.find("DISABLE_CUSTOM_ORIGIN_TYPE");
    if (it != options.end()) {
        context->SetDisableCustomOriginType(it->second == "true");
    }
    
    it = options.find("OPTION_COUNT");
    if (it != options.end()) {
        try {
            context->SetOptionCount(std::stoi(it->second));
        } catch (...) {
            LOG_ERROR("无效的OPTION_COUNT值: " + it->second);
        }
    }
    
    // 配置已经通过各个选项应用到上下文了
}

} // namespace CHTL