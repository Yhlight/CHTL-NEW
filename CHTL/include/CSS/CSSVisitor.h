#ifndef CHTL_CSS_CSSVISITOR_H
#define CHTL_CSS_CSSVISITOR_H

#include "css3ParserBaseListener.h"
#include <string>
#include <sstream>
#include <vector>

namespace CHTL {

/**
 * CSS访问器
 * 遍历CSS解析树并重构CSS代码
 */
class CSSVisitor : public css3ParserBaseListener {
private:
    std::stringstream output;
    int indentLevel;
    bool prettyPrint;
    bool inRule;
    
    /**
     * 获取缩进字符串
     */
    std::string GetIndent() const {
        if (!prettyPrint) return "";
        return std::string(indentLevel * 2, ' ');
    }
    
public:
    CSSVisitor(bool pretty = true) 
        : indentLevel(0), prettyPrint(pretty), inRule(false) {}
    
    std::string GetOutput() const { return output.str(); }
    void ClearOutput() { 
        output.str(""); 
        output.clear(); 
        indentLevel = 0;
        inRule = false;
    }
    
    // 样式表级别
    void enterStylesheet(css3Parser::StylesheetContext* ctx) override {
        // 开始处理样式表
    }
    
    void exitStylesheet(css3Parser::StylesheetContext* ctx) override {
        // 结束处理样式表
    }
    
    // 规则集处理
    void enterRuleset(css3Parser::RulesetContext* ctx) override {
        inRule = true;
        // 输出选择器
        if (ctx->selectorGroup()) {
            output << GetIndent() << ctx->selectorGroup()->getText();
            output << (prettyPrint ? " {\n" : "{");
            indentLevel++;
        }
    }
    
    void exitRuleset(css3Parser::RulesetContext* ctx) override {
        indentLevel--;
        output << GetIndent() << "}" << (prettyPrint ? "\n\n" : "");
        inRule = false;
    }
    
    // 属性声明处理
    void enterDeclaration(css3Parser::DeclarationContext* ctx) override {
        if (ctx->property() && ctx->expr()) {
            output << GetIndent();
            output << ctx->property()->getText() << ":";
            output << (prettyPrint ? " " : "");
            output << ctx->expr()->getText();
            
            // 检查是否有important
            if (ctx->prio()) {
                output << " " << ctx->prio()->getText();
            }
        }
    }
    
    void exitDeclaration(css3Parser::DeclarationContext* ctx) override {
        output << ";";
        if (prettyPrint) output << "\n";
    }
    
    // @规则处理
    void enterNestedStatement(css3Parser::NestedStatementContext* ctx) override {
        if (ctx->getText().substr(0, 1) == "@") {
            output << GetIndent() << ctx->getText();
            if (prettyPrint) output << "\n";
        }
    }
    
    // 媒体查询处理
    void enterMedia(css3Parser::MediaContext* ctx) override {
        output << GetIndent() << "@media";
        if (ctx->mediaQueryList()) {
            output << " " << ctx->mediaQueryList()->getText();
        }
        output << (prettyPrint ? " {\n" : "{");
        indentLevel++;
    }
    
    void exitMedia(css3Parser::MediaContext* ctx) override {
        indentLevel--;
        output << GetIndent() << "}" << (prettyPrint ? "\n\n" : "");
    }
    
    // @import处理
    void enterImportRule(css3Parser::ImportRuleContext* ctx) override {
        output << GetIndent() << "@import ";
        if (ctx->ws()) {
            output << ctx->ws()->getText();
        }
        output << ";";
        if (prettyPrint) output << "\n";
    }
    
    // @charset处理
    void enterCharset(css3Parser::CharsetContext* ctx) override {
        output << "@charset " << ctx->String_()->getText() << ";";
        if (prettyPrint) output << "\n\n";
    }
};

} // namespace CHTL

#endif // CHTL_CSS_CSSVISITOR_H