#ifndef CHTL_CHTL_GENERATOR_GENERATOR_H
#define CHTL_CHTL_GENERATOR_GENERATOR_H

#include <string>
#include <memory>
#include <sstream>
#include <unordered_map>
#include "CHTL/AST/ASTVisitor.h"
#include "CHTL/Core/Context.h"
#include "CHTL/Core/CompileResult.h"

namespace CHTL {

/**
 * CHTL代码生成器
 * 负责将AST转换为HTML/CSS/JS代码
 */
class Generator : public DefaultASTVisitor {
private:
    std::shared_ptr<CompileContext> context;      // 编译上下文
    CompileResult* result;                        // 编译结果
    
    // 输出流
    std::ostringstream htmlStream;                // HTML输出流
    std::ostringstream styleStream;               // 样式输出流
    std::ostringstream scriptStream;              // 脚本输出流
    std::ostringstream currentStream;             // 当前输出流
    
    // 生成状态
    enum class GenerateMode {
        HTML,
        Style,
        Script
    } currentMode;
    
    int indentLevel;                              // 缩进级别
    bool prettyPrint;                             // 是否格式化输出
    
    /**
     * 输出缩进
     */
    void Indent();
    
    /**
     * 输出字符串
     */
    void Emit(const std::string& str);
    
    /**
     * 输出带缩进的行
     */
    void EmitLine(const std::string& str = "");
    
    /**
     * 切换输出模式
     */
    void SwitchMode(GenerateMode mode);
    
    /**
     * 生成HTML属性
     */
    void GenerateAttributes(const std::unordered_map<std::string, std::string>& attrs);
    
    /**
     * 生成样式规则
     */
    void GenerateStyleRule(StyleRuleNode* rule);
    
    /**
     * 处理局部样式中的选择器自动化
     */
    void ProcessLocalStyleSelectors(LocalStyleNode* node);
    
    /**
     * 处理局部脚本中的CHTL语法
     */
    void ProcessLocalScriptSyntax(LocalScriptNode* node);
    
public:
    explicit Generator(std::shared_ptr<CompileContext> ctx);
    ~Generator() override = default;
    
    /**
     * 生成代码
     * @param ast AST根节点
     * @param result 编译结果
     */
    void Generate(std::shared_ptr<ProgramNode> ast, CompileResult& result);
    
    /**
     * 设置格式化输出
     */
    void SetPrettyPrint(bool enable) { prettyPrint = enable; }
    
    // 重写访问者方法
    void VisitProgram(ProgramNode* node) override;
    void VisitElement(ElementNode* node) override;
    void VisitText(TextNode* node) override;
    void VisitTemplateStyle(TemplateStyleNode* node) override;
    void VisitTemplateElement(TemplateElementNode* node) override;
    void VisitTemplateVar(TemplateVarNode* node) override;
    void VisitCustomStyle(CustomStyleNode* node) override;
    void VisitCustomElement(CustomElementNode* node) override;
    void VisitCustomVar(CustomVarNode* node) override;
    void VisitOrigin(OriginNode* node) override;
    void VisitLocalStyle(LocalStyleNode* node) override;
    void VisitLocalScript(LocalScriptNode* node) override;
    void VisitStyleRule(StyleRuleNode* node) override;
    void VisitStyleProperty(StylePropertyNode* node) override;
    void VisitAttribute(AttributeNode* node) override;
    void VisitComment(CommentNode* node) override;
};

} // namespace CHTL

#endif // CHTL_CHTL_GENERATOR_GENERATOR_H