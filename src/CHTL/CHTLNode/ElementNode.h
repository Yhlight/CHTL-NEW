#pragma once

#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

    // HTML元素节点类
    // 专门处理HTML元素的AST节点
    class CHTLElementNode : public CHTLBaseNode {
    private:
        std::string tagName;                                    // HTML标签名
        std::unordered_map<std::string, std::string> htmlAttributes; // HTML属性
        bool isSelfClosing;                                     // 是否为自闭合标签
        bool hasLocalStyle;                                     // 是否包含局部样式
        bool hasLocalScript;                                    // 是否包含局部脚本
        
        // 自动化相关
        std::string autoClassName;                              // 自动添加的类名
        std::string autoIdName;                                 // 自动添加的ID名
        
        static std::unordered_set<std::string> selfClosingTags; // 自闭合标签集合
        static void InitializeSelfClosingTags();

    public:
        // 构造函数
        explicit CHTLElementNode(const std::string& tag);
        ~CHTLElementNode() override = default;

        // 标签管理
        const std::string& GetTagName() const;                 // 获取标签名
        void SetTagName(const std::string& tag);               // 设置标签名
        bool IsSelfClosing() const;                            // 是否为自闭合标签
        
        // HTML属性管理（区别于基类的通用属性）
        void SetHTMLAttribute(const std::string& name, const std::string& value); // 设置HTML属性
        std::string GetHTMLAttribute(const std::string& name) const;    // 获取HTML属性
        bool HasHTMLAttribute(const std::string& name) const;           // 是否有HTML属性
        void RemoveHTMLAttribute(const std::string& name);              // 移除HTML属性
        const std::unordered_map<std::string, std::string>& GetAllHTMLAttributes() const; // 获取所有HTML属性
        
        // 样式和脚本管理
        void SetHasLocalStyle(bool hasStyle);                  // 设置是否有局部样式
        void SetHasLocalScript(bool hasScript);                // 设置是否有局部脚本
        bool HasLocalStyle() const;                            // 是否有局部样式
        bool HasLocalScript() const;                           // 是否有局部脚本
        
        // 自动化类名和ID管理
        void SetAutoClassName(const std::string& className);   // 设置自动类名
        void SetAutoIdName(const std::string& idName);         // 设置自动ID名
        const std::string& GetAutoClassName() const;           // 获取自动类名
        const std::string& GetAutoIdName() const;              // 获取自动ID名
        bool HasAutoClassName() const;                         // 是否有自动类名
        bool HasAutoIdName() const;                            // 是否有自动ID名
        
        // 子节点特化管理
        std::shared_ptr<CHTLBaseNode> GetStyleNode() const;    // 获取样式节点
        std::shared_ptr<CHTLBaseNode> GetScriptNode() const;   // 获取脚本节点
        std::vector<std::shared_ptr<CHTLBaseNode>> GetTextNodes() const; // 获取文本节点
        std::vector<std::shared_ptr<CHTLBaseNode>> GetElementNodes() const; // 获取子元素节点
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // HTML生成方法
        std::string GenerateOpeningTag() const;                // 生成开始标签
        std::string GenerateClosingTag() const;                // 生成结束标签
        std::string GenerateHTML() const;                      // 生成完整HTML
        
        // 工具方法
        bool IsBlockElement() const;                           // 是否为块级元素
        bool IsInlineElement() const;                          // 是否为行内元素
        bool IsVoidElement() const;                            // 是否为空元素
        
        // 静态方法
        static bool IsValidHTMLTag(const std::string& tag);    // 验证HTML标签是否有效
    };

} // namespace CHTL