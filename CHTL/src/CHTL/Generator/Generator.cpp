#include "CHTL/Generator/Generator.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"

namespace CHTL {

Generator::Generator(std::shared_ptr<CompileContext> ctx) 
    : context(ctx), result(nullptr), currentMode(GenerateMode::HTML),
      currentContext(GeneratorContext::InHTML), indentLevel(0), prettyPrint(true) {
}

void Generator::Generate(std::shared_ptr<ProgramNode> ast, CompileResult& compileResult) {
    result = &compileResult;
    result->Clear();
    
    // 重置状态
    htmlStream.str("");
    styleStream.str("");
    scriptStream.str("");
    currentStream.str("");
    indentLevel = 0;
    
    // 开始生成
    if (ast) {
        ast->Accept(this);
    }
    
    // 收集结果
    result->htmlContent = htmlStream.str();
    result->globalStyles = styleStream.str();
    result->globalScripts = scriptStream.str();
}

void Generator::Indent() {
    if (prettyPrint) {
        for (int i = 0; i < indentLevel; ++i) {
            currentStream << "    ";
        }
    }
}

void Generator::Emit(const std::string& str) {
    switch (currentMode) {
        case GenerateMode::HTML:
            htmlStream << str;
            break;
        case GenerateMode::Style:
            styleStream << str;
            break;
        case GenerateMode::Script:
            scriptStream << str;
            break;
    }
}

void Generator::EmitLine(const std::string& str) {
    Indent();
    Emit(str);
    if (prettyPrint) {
        Emit("\n");
    }
}

void Generator::SwitchMode(GenerateMode mode) {
    currentMode = mode;
    
    // 更新上下文
    switch (mode) {
        case GenerateMode::HTML:
            currentContext = GeneratorContext::InHTML;
            break;
        case GenerateMode::Style:
            currentContext = GeneratorContext::InCSS;
            break;
        case GenerateMode::Script:
            currentContext = GeneratorContext::InJavaScript;
            break;
    }
}

void Generator::VisitProgram(ProgramNode* node) {
    LOG_DEBUG("开始生成代码");
    
    // 生成HTML5声明
    if (node->UseHTML5()) {
        context->SetUseHTML5(true);
    }
    
    // 重置模式为HTML
    SwitchMode(GenerateMode::HTML);
    
    // 访问所有子节点
    VisitChildren(node);
    
    // 输出收集的全局样式
    if (!globalStyleRules.empty()) {
        output << "\n<style>\n";
        for (const auto& rule : globalStyleRules) {
            output << rule;
        }
        output << "</style>\n";
    }
    
    LOG_DEBUG("代码生成完成");
}

void Generator::VisitElement(ElementNode* node) {
    // 检查是否有局部样式和脚本
    std::string inlineStyle;
    bool hasLocalStyle = false;
    bool hasLocalScript = false;
    std::string firstClassSelector;
    std::string firstIdSelector;
    
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() == ASTNodeType::LocalStyle) {
            hasLocalStyle = true;
            auto styleNode = static_cast<LocalStyleNode*>(child.get());
            inlineStyle = styleNode->GenerateInlineStyle();
            
            // 查找第一个类选择器和ID选择器（用于自动化）
            for (const auto& rule : styleNode->GetRules()) {
                if (rule->GetType() == StyleRuleNode::SelectorType::Class && 
                    firstClassSelector.empty()) {
                    // 去掉点号前缀
                    firstClassSelector = rule->GetSelector().substr(1);
                } else if (rule->GetType() == StyleRuleNode::SelectorType::Id && 
                           firstIdSelector.empty()) {
                    // 去掉井号前缀
                    firstIdSelector = rule->GetSelector().substr(1);
                }
            }
        } else if (child->GetType() == ASTNodeType::LocalScript) {
            hasLocalScript = true;
            // TODO: 解析script中的选择器
        }
    }
    
    // 准备属性
    auto attrs = node->GetAttributes();
    
    // 选择器自动化：自动添加class/id
    if (!context->IsStyleAutoAddClassDisabled() && hasLocalStyle && 
        attrs.find("class") == attrs.end() && !firstClassSelector.empty()) {
        attrs["class"] = firstClassSelector;
    }
    
    if (!context->IsStyleAutoAddIdDisabled() && hasLocalStyle && 
        attrs.find("id") == attrs.end() && !firstIdSelector.empty()) {
        attrs["id"] = firstIdSelector;
    }
    
    if (!context->IsScriptAutoAddClassDisabled() && hasLocalScript && !hasLocalStyle &&
        attrs.find("class") == attrs.end() && !firstClassSelector.empty()) {
        attrs["class"] = firstClassSelector;
    }
    
    if (!context->IsScriptAutoAddIdDisabled() && hasLocalScript && !hasLocalStyle &&
        attrs.find("id") == attrs.end() && !firstIdSelector.empty()) {
        attrs["id"] = firstIdSelector;
    }
    
    // 记录当前元素的类名和ID（用于&引用替换）
    currentElementClass = (attrs.find("class") != attrs.end()) ? attrs["class"] : "";
    currentElementId = (attrs.find("id") != attrs.end()) ? attrs["id"] : "";
    
    if (!inlineStyle.empty()) {
        // 如果已经有style属性，合并它们
        if (attrs.find("style") != attrs.end()) {
            attrs["style"] = attrs["style"] + " " + inlineStyle;
        } else {
            attrs["style"] = inlineStyle;
        }
    }
    
    // 生成开始标签
    EmitLine("<" + node->GetTagName() + GenerateAttributesString(attrs) + ">");
    
    // 访问子节点（跳过LocalStyle节点）
    indentLevel++;
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() != ASTNodeType::LocalStyle) {
            child->Accept(this);
        }
    }
    indentLevel--;
    
    // 清理当前元素信息
    currentElementClass = "";
    currentElementId = "";
    
    // 生成结束标签
    EmitLine("</" + node->GetTagName() + ">");
}

void Generator::VisitText(TextNode* node) {
    EmitLine(node->GetContent());
}

void Generator::GenerateAttributes(const std::unordered_map<std::string, std::string>& attrs) {
    for (const auto& [name, value] : attrs) {
        Emit(" " + name + "=\"" + value + "\"");
    }
}

std::string Generator::GenerateAttributesString(const std::unordered_map<std::string, std::string>& attrs) {
    std::string result;
    for (const auto& [name, value] : attrs) {
        result += " " + name + "=\"" + value + "\"";
    }
    return result;
}

// 其他Visit方法暂时为空实现
void Generator::VisitTemplateStyle(TemplateStyleNode* node) {
    // 模板定义不生成输出，只在使用时生效
}
void Generator::VisitTemplateElement(TemplateElementNode* node) {
    // 模板定义不生成输出，只在使用时生效
}
void Generator::VisitTemplateVar(TemplateVarNode* node) {
    // 模板定义不生成输出，只在使用时生效
}
void Generator::VisitCustomStyle(CustomStyleNode* node) {}
void Generator::VisitCustomElement(CustomElementNode* node) {}
void Generator::VisitCustomVar(CustomVarNode* node) {}
void Generator::VisitOrigin(OriginNode* node) {}
void Generator::VisitLocalStyle(LocalStyleNode* node) {
    // 收集样式规则到全局样式块
    if (!node->GetRules().empty()) {
        for (const auto& rule : node->GetRules()) {
            std::string selector = rule->GetSelector();
            
            // 处理 & 引用（替换为当前元素的类名或ID）
            if (selector.find("&") != std::string::npos) {
                // 优先使用类名
                if (!currentElementClass.empty()) {
                    size_t pos = 0;
                    while ((pos = selector.find("&", pos)) != std::string::npos) {
                        selector.replace(pos, 1, "." + currentElementClass);
                        pos += currentElementClass.length() + 1;
                    }
                } else if (!currentElementId.empty()) {
                    size_t pos = 0;
                    while ((pos = selector.find("&", pos)) != std::string::npos) {
                        selector.replace(pos, 1, "#" + currentElementId);
                        pos += currentElementId.length() + 1;
                    }
                }
            }
            
            // 构建样式规则字符串
            std::ostringstream ruleStr;
            ruleStr << selector << " {\n";
            for (const auto& prop : rule->GetProperties()) {
                ruleStr << "    " << prop->GetName() << ": " << prop->GetValue() << ";\n";
            }
            ruleStr << "}\n";
            
            // 添加到全局样式规则集合
            globalStyleRules.push_back(ruleStr.str());
        }
    }
    
    // 内联样式已经在元素生成时处理
}
void Generator::VisitLocalScript(LocalScriptNode* node) {}
void Generator::VisitStyleRule(StyleRuleNode* node) {}
void Generator::VisitStyleProperty(StylePropertyNode* node) {}
void Generator::VisitAttribute(AttributeNode* node) {}
void Generator::VisitComment(CommentNode* node) {
    // 只处理生成器注释
    if (node->GetCommentType() == CommentNode::CommentType::Generator) {
        std::string content = node->GetContent();
        
        // 根据当前上下文决定注释格式
        if (currentContext == GeneratorContext::InCSS || 
            currentContext == GeneratorContext::InStyle) {
            // CSS注释格式
            output << "/* " << content << " */";
        } else if (currentContext == GeneratorContext::InJavaScript || 
                   currentContext == GeneratorContext::InScript) {
            // JavaScript注释格式
            output << "// " << content;
        } else {
            // HTML注释格式
            output << "<!-- " << content << " -->";
        }
        output << "\n";
    }
    // 其他类型的注释（// 和 /**/）不会被生成到输出中
}

void Generator::VisitDelete(DeleteNode* node) {
    // delete操作在解析阶段应该已经应用到AST上
    // 这里不需要生成任何内容
    LOG_DEBUG("处理Delete节点");
}

void Generator::VisitInsert(InsertNode* node) {
    // insert操作也应该在解析阶段应用
    // 这里生成插入的内容
    LOG_DEBUG("处理Insert节点");
    
    // 生成插入的内容
    for (const auto& child : node->GetContent()) {
        child->Accept(this);
    }
}

void Generator::VisitElementMatch(ElementMatchNode* node) {
    // 元素匹配特例化
    LOG_DEBUG("处理ElementMatch节点: " + node->GetElementName());
    
    // 生成子节点（通常是样式或属性）
    VisitChildren(node);
}

} // namespace CHTL