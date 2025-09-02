#include "Visitor.h"
#include "CHTLBaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"
#include "TemplateNode.h"
#include "CustomNode.h"
#include <sstream>

namespace CHTL {

// CHTLDefaultVisitor实现
void CHTLDefaultVisitor::VisitBaseNode(CHTLBaseNode* node) {
    if (node) {
        VisitChildren(node);
    }
}

void CHTLDefaultVisitor::VisitElementNode(ElementNode* node) {
    if (node) {
        VisitChildren(node);
    }
}

void CHTLDefaultVisitor::VisitTextNode(TextNode* node) {
    // 文本节点通常没有子节点
    (void)node; // 避免未使用警告
}

void CHTLDefaultVisitor::VisitCommentNode(CommentNode* node) {
    // 注释节点通常没有子节点
    (void)node; // 避免未使用警告
}

void CHTLDefaultVisitor::VisitStyleNode(StyleNode* node) {
    if (node) {
        VisitChildren(node);
    }
}

void CHTLDefaultVisitor::VisitScriptNode(ScriptNode* node) {
    if (node) {
        VisitChildren(node);
    }
}

void CHTLDefaultVisitor::VisitTemplateNode(TemplateNode* node) {
    if (node) {
        VisitChildren(node);
    }
}

void CHTLDefaultVisitor::VisitCustomNode(CustomNode* node) {
    if (node) {
        VisitChildren(node);
    }
}

void CHTLDefaultVisitor::VisitOriginNode(OriginNode* node) {
    // 暂时简化实现
    (void)node;
}

void CHTLDefaultVisitor::VisitConfigNode(ConfigNode* node) {
    // 暂时简化实现
    (void)node;
}

void CHTLDefaultVisitor::VisitImportNode(ImportNode* node) {
    // 暂时简化实现
    (void)node;
}

void CHTLDefaultVisitor::VisitNamespaceNode(NamespaceNode* node) {
    // 暂时简化实现
    (void)node;
}

void CHTLDefaultVisitor::VisitChildren(CHTLBaseNode* node) {
    if (node) {
        for (const auto& child : node->GetChildren()) {
            child->Accept(this);
        }
    }
}

// CHTLHTMLGeneratorVisitor实现
CHTLHTMLGeneratorVisitor::CHTLHTMLGeneratorVisitor() : CHTLDefaultVisitor(), m_IndentLevel(0) {
}

CHTLHTMLGeneratorVisitor::~CHTLHTMLGeneratorVisitor() {
}

void CHTLHTMLGeneratorVisitor::Reset() {
    m_GeneratedHTML.clear();
    m_GeneratedCSS.clear();
    m_GeneratedJS.clear();
    m_IndentLevel = 0;
}

std::string CHTLHTMLGeneratorVisitor::GenerateHTMLTag(const std::string& tagName, 
                                                     const std::unordered_map<std::string, std::string>& attributes,
                                                     bool isClosing) {
    std::ostringstream tag;
    
    if (isClosing) {
        tag << "</" << tagName << ">";
    }
    else {
        tag << "<" << tagName;
        
        for (const auto& attr : attributes) {
            tag << " " << attr.first << "=\"" << attr.second << "\"";
        }
        
        tag << ">";
    }
    
    return tag.str();
}

std::string CHTLHTMLGeneratorVisitor::GenerateCSSSelector(const std::string& className,
                                                         const std::string& idName,
                                                         const std::unordered_map<std::string, std::string>& properties) {
    std::ostringstream css;
    
    if (!className.empty()) {
        css << "." << className;
    }
    else if (!idName.empty()) {
        css << "#" << idName;
    }
    else {
        return "";
    }
    
    css << " {\n";
    
    for (const auto& prop : properties) {
        css << "  " << prop.first << ": " << prop.second << ";\n";
    }
    
    css << "}\n";
    
    return css.str();
}

std::string CHTLHTMLGeneratorVisitor::AddIndent(const std::string& content) {
    std::string indent(m_IndentLevel * 2, ' ');
    return indent + content;
}

// 实现CHTLHTMLGeneratorVisitor的虚函数
void CHTLHTMLGeneratorVisitor::VisitElementNode(ElementNode* node) {
    if (node) {
        // 默认实现：调用基类方法
        CHTLDefaultVisitor::VisitElementNode(node);
    }
}

void CHTLHTMLGeneratorVisitor::VisitTextNode(TextNode* node) {
    if (node) {
        // 默认实现：调用基类方法
        CHTLDefaultVisitor::VisitTextNode(node);
    }
}

void CHTLHTMLGeneratorVisitor::VisitStyleNode(StyleNode* node) {
    if (node) {
        // 默认实现：调用基类方法
        CHTLDefaultVisitor::VisitStyleNode(node);
    }
}

void CHTLHTMLGeneratorVisitor::VisitScriptNode(ScriptNode* node) {
    if (node) {
        // 默认实现：调用基类方法
        CHTLDefaultVisitor::VisitScriptNode(node);
    }
}

} // namespace CHTL