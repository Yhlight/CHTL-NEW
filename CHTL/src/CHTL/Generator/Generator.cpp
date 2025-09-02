#include "CHTL/Generator/Generator.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"

namespace CHTL {

Generator::Generator(std::shared_ptr<CompileContext> ctx) 
    : context(ctx), result(nullptr), currentMode(GenerateMode::HTML),
      indentLevel(0), prettyPrint(true) {
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
    
    LOG_DEBUG("代码生成完成");
}

void Generator::VisitElement(ElementNode* node) {
    // 检查是否有局部样式
    std::string inlineStyle;
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() == ASTNodeType::LocalStyle) {
            auto styleNode = static_cast<LocalStyleNode*>(child.get());
            inlineStyle = styleNode->GenerateInlineStyle();
            break; // 只取第一个局部样式块
        }
    }
    
    // 准备属性
    auto attrs = node->GetAttributes();
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
void Generator::VisitTemplateVar(TemplateVarNode* node) {}
void Generator::VisitCustomStyle(CustomStyleNode* node) {}
void Generator::VisitCustomElement(CustomElementNode* node) {}
void Generator::VisitCustomVar(CustomVarNode* node) {}
void Generator::VisitOrigin(OriginNode* node) {}
void Generator::VisitLocalStyle(LocalStyleNode* node) {
    // 局部样式不生成单独的元素，而是作为元素的style属性处理
    // 样式属性已经在元素生成时处理
}
void Generator::VisitLocalScript(LocalScriptNode* node) {}
void Generator::VisitStyleRule(StyleRuleNode* node) {}
void Generator::VisitStyleProperty(StylePropertyNode* node) {}
void Generator::VisitAttribute(AttributeNode* node) {}
void Generator::VisitComment(CommentNode* node) {}

} // namespace CHTL