#include "CHTL/Generator/Generator.h"

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
    // 生成HTML5声明
    if (node->UseHTML5()) {
        context->SetUseHTML5(true);
    }
    
    // 访问所有子节点
    VisitChildren(node);
}

void Generator::VisitElement(ElementNode* node) {
    // 生成开始标签
    Emit("<" + node->GetTagName());
    GenerateAttributes(node->GetAttributes());
    Emit(">");
    
    // 访问子节点
    indentLevel++;
    VisitChildren(node);
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

// 其他Visit方法暂时为空实现
void Generator::VisitTemplateStyle(TemplateStyleNode* node) {}
void Generator::VisitTemplateElement(TemplateElementNode* node) {}
void Generator::VisitTemplateVar(TemplateVarNode* node) {}
void Generator::VisitCustomStyle(CustomStyleNode* node) {}
void Generator::VisitCustomElement(CustomElementNode* node) {}
void Generator::VisitCustomVar(CustomVarNode* node) {}
void Generator::VisitOrigin(OriginNode* node) {}
void Generator::VisitLocalStyle(LocalStyleNode* node) {}
void Generator::VisitLocalScript(LocalScriptNode* node) {}
void Generator::VisitStyleRule(StyleRuleNode* node) {}
void Generator::VisitStyleProperty(StylePropertyNode* node) {}
void Generator::VisitAttribute(AttributeNode* node) {}
void Generator::VisitComment(CommentNode* node) {}

} // namespace CHTL