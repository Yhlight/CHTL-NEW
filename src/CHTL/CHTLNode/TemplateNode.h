#pragma once

#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

    // 模板类型枚举
    enum class CHTLTemplateType {
        TEMPLATE_STYLE,         // 样式组模板
        TEMPLATE_ELEMENT,       // 元素模板
        TEMPLATE_VAR            // 变量组模板
    };

    // 继承类型枚举
    enum class CHTLInheritanceType {
        COMPOSITION,            // 组合式继承 @Style 模板名;
        EXPLICIT                // 显性继承 inherit @Style 模板名;
    };

    // 模板节点基类
    // 专门处理CHTL模板系统的AST节点
    // 严格按照CHTL语法文档第159-287行实现
    class CHTLTemplateNode : public CHTLBaseNode {
    private:
        CHTLTemplateType templateType;                          // 模板类型
        std::string templateName;                               // 模板名称
        std::vector<std::string> inheritedTemplates;            // 继承的模板列表
        std::vector<CHTLInheritanceType> inheritanceTypes;      // 继承类型列表
        bool isUsage;                                          // 是否为模板使用（而非定义）

    public:
        // 构造函数
        CHTLTemplateNode(CHTLTemplateType type, const std::string& name);
        ~CHTLTemplateNode() override = default;

        // 模板类型管理
        CHTLTemplateType GetTemplateType() const;               // 获取模板类型
        void SetTemplateType(CHTLTemplateType type);            // 设置模板类型
        
        // 模板名称管理
        const std::string& GetTemplateName() const;             // 获取模板名称
        void SetTemplateName(const std::string& name);          // 设置模板名称
        
        // 使用/定义标记
        bool IsUsage() const;                                   // 是否为模板使用
        void SetIsUsage(bool usage);                           // 设置是否为使用
        
        // 继承管理
        void AddInheritance(const std::string& templateName, CHTLInheritanceType type); // 添加继承
        void RemoveInheritance(const std::string& templateName); // 移除继承
        const std::vector<std::string>& GetInheritedTemplates() const; // 获取继承列表
        const std::vector<CHTLInheritanceType>& GetInheritanceTypes() const; // 获取继承类型列表
        bool HasInheritance(const std::string& templateName) const; // 是否继承指定模板
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 静态工具方法
        static std::string TemplateTypeToString(CHTLTemplateType type);     // 模板类型转字符串
        static std::string InheritanceTypeToString(CHTLInheritanceType type); // 继承类型转字符串
    };

    // 样式组模板节点类
    // 处理 [Template] @Style 组名 { } 语法
    class CHTLTemplateStyleNode : public CHTLTemplateNode {
    private:
        std::unordered_map<std::string, std::string> styleProperties; // 样式属性
        std::vector<std::string> propertyOrder;                // 属性顺序（用于值替换规则）

    public:
        // 构造函数
        explicit CHTLTemplateStyleNode(const std::string& styleName);
        ~CHTLTemplateStyleNode() override = default;

        // 样式属性管理
        void AddStyleProperty(const std::string& property, const std::string& value); // 添加样式属性
        void RemoveStyleProperty(const std::string& property);  // 移除样式属性
        std::string GetStyleProperty(const std::string& property) const; // 获取样式属性
        bool HasStyleProperty(const std::string& property) const; // 是否有样式属性
        const std::unordered_map<std::string, std::string>& GetAllStyleProperties() const; // 获取所有属性
        
        // 属性顺序管理（重要：用于值替换规则）
        const std::vector<std::string>& GetPropertyOrder() const; // 获取属性顺序
        void SetPropertyOrder(const std::vector<std::string>& order); // 设置属性顺序
        
        // 继承处理
        void ApplyInheritance(const CHTLTemplateStyleNode& parentTemplate); // 应用继承
        void ProcessCompositionInheritance(const CHTLTemplateStyleNode& parent); // 处理组合式继承
        void ProcessExplicitInheritance(const CHTLTemplateStyleNode& parent); // 处理显性继承
        
        // CSS生成
        std::string GenerateCSS() const;                        // 生成CSS代码
        std::string GenerateInlineStyle() const;               // 生成内联样式
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 元素模板节点类
    // 处理 [Template] @Element 元素名 { } 语法
    class CHTLTemplateElementNode : public CHTLTemplateNode {
    private:
        std::vector<std::shared_ptr<CHTLBaseNode>> templateElements; // 模板元素列表

    public:
        // 构造函数
        explicit CHTLTemplateElementNode(const std::string& elementName);
        ~CHTLTemplateElementNode() override = default;

        // 模板元素管理
        void AddTemplateElement(std::shared_ptr<CHTLBaseNode> element); // 添加模板元素
        void RemoveTemplateElement(std::shared_ptr<CHTLBaseNode> element); // 移除模板元素
        const std::vector<std::shared_ptr<CHTLBaseNode>>& GetTemplateElements() const; // 获取模板元素
        size_t GetTemplateElementCount() const;                // 获取模板元素数量
        
        // 继承处理
        void ApplyInheritance(const CHTLTemplateElementNode& parentTemplate); // 应用继承
        
        // HTML生成
        std::string GenerateHTML() const;                      // 生成HTML代码
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 变量组模板节点类
    // 处理 [Template] @Var 变量组名 { } 语法
    class CHTLTemplateVarNode : public CHTLTemplateNode {
    private:
        std::unordered_map<std::string, std::string> variables; // 变量映射
        std::vector<std::string> variableOrder;                // 变量顺序

    public:
        // 构造函数
        explicit CHTLTemplateVarNode(const std::string& varGroupName);
        ~CHTLTemplateVarNode() override = default;

        // 变量管理
        void AddVariable(const std::string& varName, const std::string& value); // 添加变量
        void RemoveVariable(const std::string& varName);        // 移除变量
        std::string GetVariable(const std::string& varName) const; // 获取变量值
        bool HasVariable(const std::string& varName) const;     // 是否有变量
        const std::unordered_map<std::string, std::string>& GetAllVariables() const; // 获取所有变量
        
        // 变量顺序管理
        const std::vector<std::string>& GetVariableOrder() const; // 获取变量顺序
        void SetVariableOrder(const std::vector<std::string>& order); // 设置变量顺序
        
        // 变量引用处理
        std::string ResolveVariableReference(const std::string& varName) const; // 解析变量引用
        std::string ProcessVariableReferences(const std::string& text) const; // 处理文本中的变量引用
        
        // 继承处理
        void ApplyInheritance(const CHTLTemplateVarNode& parentTemplate); // 应用继承
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 模板使用节点类
    // 处理模板的使用语法 @Style DefaultText; 等
    class CHTLTemplateUsageNode : public CHTLBaseNode {
    private:
        CHTLTemplateType templateType;                          // 使用的模板类型
        std::string templateName;                               // 使用的模板名称
        bool isFullQualified;                                   // 是否使用全缀名
        std::string fullQualifiedPrefix;                       // 全缀名前缀

    public:
        // 构造函数
        CHTLTemplateUsageNode(CHTLTemplateType type, const std::string& name);
        ~CHTLTemplateUsageNode() override = default;

        // 模板使用管理
        CHTLTemplateType GetTemplateType() const;               // 获取模板类型
        const std::string& GetTemplateName() const;             // 获取模板名称
        void SetTemplateName(const std::string& name);          // 设置模板名称
        
        // 全缀名管理
        bool IsFullQualified() const;                          // 是否使用全缀名
        void SetFullQualified(bool qualified, const std::string& prefix = ""); // 设置全缀名
        const std::string& GetFullQualifiedPrefix() const;     // 获取全缀名前缀
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 模板节点工厂类
    class CHTLTemplateNodeFactory {
    public:
        // 创建模板定义节点
        static std::shared_ptr<CHTLTemplateStyleNode> CreateTemplateStyleNode(const std::string& styleName);
        static std::shared_ptr<CHTLTemplateElementNode> CreateTemplateElementNode(const std::string& elementName);
        static std::shared_ptr<CHTLTemplateVarNode> CreateTemplateVarNode(const std::string& varGroupName);
        
        // 创建模板使用节点
        static std::shared_ptr<CHTLTemplateUsageNode> CreateStyleUsageNode(const std::string& styleName);
        static std::shared_ptr<CHTLTemplateUsageNode> CreateElementUsageNode(const std::string& elementName);
        static std::shared_ptr<CHTLTemplateUsageNode> CreateVarUsageNode(const std::string& varGroupName);
        
        // 解析方法
        static CHTLTemplateType ParseTemplateType(const std::string& typeText);
        static std::shared_ptr<CHTLTemplateNode> ParseTemplate(const std::string& templateText);
        
        // 验证方法
        static bool IsValidTemplateName(const std::string& name);
        static bool IsValidTemplateType(const std::string& type);
    };

} // namespace CHTL