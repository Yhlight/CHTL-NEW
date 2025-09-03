#pragma once

#include "BaseNode.h"

namespace CHTL {

    // 文本节点类
    // 专门处理text {}语法的AST节点
    class CHTLTextNode : public CHTLBaseNode {
    private:
        std::string textContent;                    // 文本内容
        bool isUnquoted;                           // 是否为无修饰字面量
        bool hasVariableReference;                 // 是否包含变量引用
        std::vector<std::string> variableReferences; // 变量引用列表

    public:
        // 构造函数
        explicit CHTLTextNode(const std::string& content = "");
        ~CHTLTextNode() override = default;

        // 文本内容管理
        const std::string& GetTextContent() const;             // 获取文本内容
        void SetTextContent(const std::string& content);       // 设置文本内容
        void AppendText(const std::string& text);              // 追加文本
        void ClearText();                                      // 清空文本
        
        // 字面量类型管理
        bool IsUnquoted() const;                               // 是否为无修饰字面量
        void SetUnquoted(bool unquoted);                       // 设置是否为无修饰字面量
        
        // 变量引用管理
        bool HasVariableReference() const;                     // 是否包含变量引用
        const std::vector<std::string>& GetVariableReferences() const; // 获取变量引用列表
        void AddVariableReference(const std::string& varName); // 添加变量引用
        void RemoveVariableReference(const std::string& varName); // 移除变量引用
        void ClearVariableReferences();                        // 清空变量引用
        
        // 文本处理方法
        std::string ProcessVariableReferences() const;         // 处理变量引用
        std::string EscapeHTML() const;                        // HTML转义
        std::string UnescapeHTML() const;                      // HTML反转义
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 文本节点特有方法
        bool IsEmpty() const;                                  // 是否为空文本
        size_t GetTextLength() const;                          // 获取文本长度
        bool ContainsOnlyWhitespace() const;                   // 是否只包含空白字符
        
        // 格式化方法
        void TrimWhitespace();                                 // 去除首尾空白
        void NormalizeWhitespace();                            // 标准化空白字符
        
        // 静态工具方法
        static bool IsValidTextContent(const std::string& content); // 验证文本内容是否有效
        static std::string SanitizeText(const std::string& content); // 清理文本内容
    };

} // namespace CHTL