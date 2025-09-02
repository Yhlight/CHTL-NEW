#pragma once

namespace CHTL {

// 前向声明
class CHTLBaseNode;
class ElementNode;
class TextNode;
class CommentNode;
class StyleNode;
class ScriptNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ConfigNode;
class ImportNode;
class NamespaceNode;

/**
 * CHTL节点访问者接口
 * 实现访问者模式，用于遍历和操作AST节点
 */
class CHTLNodeVisitor {
public:
    /**
     * 虚析构函数
     */
    virtual ~CHTLNodeVisitor() = default;
    
    // 基础节点访问方法
    /**
     * 访问基础节点
     * @param node 节点指针
     */
    virtual void VisitBaseNode(CHTLBaseNode* node) = 0;
    
    /**
     * 访问元素节点
     * @param node 元素节点指针
     */
    virtual void VisitElementNode(ElementNode* node) = 0;
    
    /**
     * 访问文本节点
     * @param node 文本节点指针
     */
    virtual void VisitTextNode(TextNode* node) = 0;
    
    /**
     * 访问注释节点
     * @param node 注释节点指针
     */
    virtual void VisitCommentNode(CommentNode* node) = 0;
    
    /**
     * 访问样式节点
     * @param node 样式节点指针
     */
    virtual void VisitStyleNode(StyleNode* node) = 0;
    
    /**
     * 访问脚本节点（局部script属于CHTL）
     * @param node 脚本节点指针
     */
    virtual void VisitScriptNode(ScriptNode* node) = 0;
    
    /**
     * 访问模板节点
     * @param node 模板节点指针
     */
    virtual void VisitTemplateNode(TemplateNode* node) = 0;
    
    /**
     * 访问自定义节点
     * @param node 自定义节点指针
     */
    virtual void VisitCustomNode(CustomNode* node) = 0;
    
    /**
     * 访问原始嵌入节点
     * @param node 原始嵌入节点指针
     */
    virtual void VisitOriginNode(OriginNode* node) = 0;
    
    /**
     * 访问配置节点
     * @param node 配置节点指针
     */
    virtual void VisitConfigNode(ConfigNode* node) = 0;
    
    /**
     * 访问导入节点
     * @param node 导入节点指针
     */
    virtual void VisitImportNode(ImportNode* node) = 0;
    
    /**
     * 访问命名空间节点
     * @param node 命名空间节点指针
     */
    virtual void VisitNamespaceNode(NamespaceNode* node) = 0;
};

/**
 * CHTL默认访问者
 * 提供默认的访问行为
 */
class CHTLDefaultVisitor : public CHTLNodeVisitor {
public:
    /**
     * 构造函数
     */
    CHTLDefaultVisitor() = default;
    
    /**
     * 析构函数
     */
    ~CHTLDefaultVisitor() override = default;
    
    // 实现所有访问方法的默认行为
    void VisitBaseNode(CHTLBaseNode* node) override;
    void VisitElementNode(ElementNode* node) override;
    void VisitTextNode(TextNode* node) override;
    void VisitCommentNode(CommentNode* node) override;
    void VisitStyleNode(StyleNode* node) override;
    void VisitScriptNode(ScriptNode* node) override;
    void VisitTemplateNode(TemplateNode* node) override;
    void VisitCustomNode(CustomNode* node) override;
    void VisitOriginNode(OriginNode* node) override;
    void VisitConfigNode(ConfigNode* node) override;
    void VisitImportNode(ImportNode* node) override;
    void VisitNamespaceNode(NamespaceNode* node) override;

protected:
    /**
     * 访问子节点
     * @param node 父节点
     */
    void VisitChildren(CHTLBaseNode* node);
};

/**
 * CHTL HTML生成访问者
 * 专门用于生成HTML代码
 */
class CHTLHTMLGeneratorVisitor : public CHTLDefaultVisitor {
private:
    std::string m_GeneratedHTML;        // 生成的HTML代码
    std::string m_GeneratedCSS;         // 生成的CSS代码
    std::string m_GeneratedJS;          // 生成的JavaScript代码
    int m_IndentLevel;                  // 缩进级别

public:
    /**
     * 构造函数
     */
    CHTLHTMLGeneratorVisitor();
    
    /**
     * 析构函数
     */
    ~CHTLHTMLGeneratorVisitor() override = default;
    
    /**
     * 获取生成的HTML代码
     * @return HTML代码
     */
    const std::string& GetGeneratedHTML() const { return m_GeneratedHTML; }
    
    /**
     * 获取生成的CSS代码
     * @return CSS代码
     */
    const std::string& GetGeneratedCSS() const { return m_GeneratedCSS; }
    
    /**
     * 获取生成的JavaScript代码
     * @return JavaScript代码
     */
    const std::string& GetGeneratedJS() const { return m_GeneratedJS; }
    
    /**
     * 重置生成器
     */
    void Reset();
    
    // 重写访问方法以生成代码
    void VisitElementNode(ElementNode* node) override;
    void VisitTextNode(TextNode* node) override;
    void VisitStyleNode(StyleNode* node) override;
    void VisitScriptNode(ScriptNode* node) override;

private:
    /**
     * 生成HTML标签
     * @param tagName 标签名
     * @param attributes 属性映射
     * @param isClosing 是否为闭合标签
     * @return HTML标签字符串
     */
    std::string GenerateHTMLTag(const std::string& tagName, 
                               const std::unordered_map<std::string, std::string>& attributes,
                               bool isClosing = false);
    
    /**
     * 生成CSS选择器
     * @param className 类名
     * @param idName ID名
     * @param properties CSS属性
     * @return CSS代码
     */
    std::string GenerateCSSSelector(const std::string& className,
                                   const std::string& idName,
                                   const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 添加缩进
     * @param content 内容
     * @return 带缩进的内容
     */
    std::string AddIndent(const std::string& content);
};

} // namespace CHTL