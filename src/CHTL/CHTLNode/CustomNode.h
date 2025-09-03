#pragma once

#include "BaseNode.h"
#include "TemplateNode.h"
#include <unordered_set>
#include <vector>

namespace CHTL {

    // 自定义类型枚举
    enum class CHTLCustomType {
        CUSTOM_STYLE,           // 自定义样式组
        CUSTOM_ELEMENT,         // 自定义元素
        CUSTOM_VAR              // 自定义变量组
    };

    // 特例化操作类型枚举
    enum class CHTLSpecializationType {
        VALUE_FILL,             // 值填充（无值样式组使用时填充值）
        PROPERTY_DELETE,        // 删除属性
        INHERITANCE_DELETE,     // 删除继承
        ELEMENT_DELETE,         // 删除元素
        ELEMENT_INSERT,         // 插入元素
        STYLE_ADD               // 增加样式（自定义元素特例化）
    };

    // 插入位置枚举
    enum class CHTLInsertPosition {
        AFTER,                  // after
        BEFORE,                 // before
        REPLACE,                // replace
        AT_TOP,                 // at top
        AT_BOTTOM               // at bottom
    };

    // 自定义节点基类
    // 专门处理CHTL自定义系统的AST节点
    // 严格按照CHTL语法文档第289-584行实现
    class CHTLCustomNode : public CHTLBaseNode {
    protected:
        CHTLCustomType customType;                              // 自定义类型
        std::string customName;                                 // 自定义名称
        bool allowsSpecialization;                             // 是否允许特例化（自定义与模板的最大区别）
        bool isUsage;                                          // 是否为使用（而非定义）
        bool isSpecialized;                                    // 是否已特例化
        
        // 继承信息（可以继承模板或其他自定义）
        std::vector<std::string> inheritedItems;               // 继承的项目列表
        std::vector<CHTLInheritanceType> inheritanceTypes;     // 继承类型列表

    public:
        // 构造函数
        CHTLCustomNode(CHTLCustomType type, const std::string& name);
        ~CHTLCustomNode() override = default;

        // 自定义类型管理
        CHTLCustomType GetCustomType() const;                   // 获取自定义类型
        void SetCustomType(CHTLCustomType type);                // 设置自定义类型
        
        // 自定义名称管理
        const std::string& GetCustomName() const;               // 获取自定义名称
        void SetCustomName(const std::string& name);            // 设置自定义名称
        
        // 特例化管理
        bool AllowsSpecialization() const;                     // 是否允许特例化
        void SetAllowsSpecialization(bool allows);             // 设置是否允许特例化
        bool IsSpecialized() const;                            // 是否已特例化
        void SetSpecialized(bool specialized);                 // 设置特例化状态
        
        // 使用/定义标记
        bool IsUsage() const;                                   // 是否为使用
        void SetIsUsage(bool usage);                           // 设置是否为使用
        
        // 继承管理（可以继承模板或其他自定义）
        void AddInheritance(const std::string& itemName, CHTLInheritanceType type); // 添加继承
        void RemoveInheritance(const std::string& itemName);    // 移除继承
        const std::vector<std::string>& GetInheritedItems() const; // 获取继承列表
        bool HasInheritance(const std::string& itemName) const; // 是否继承指定项
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 静态工具方法
        static std::string CustomTypeToString(CHTLCustomType type);     // 自定义类型转字符串
    };

    // 自定义样式组节点类
    // 处理 [Custom] @Style 名称 { } 语法
    // 支持无值样式组和特例化操作
    class CHTLCustomStyleNode : public CHTLCustomNode {
    private:
        std::unordered_map<std::string, std::string> styleProperties; // 样式属性
        std::unordered_set<std::string> noValueProperties;     // 无值属性集合
        std::vector<std::string> propertyOrder;                // 属性顺序
        std::unordered_set<std::string> deletedProperties;     // 删除的属性
        std::unordered_set<std::string> deletedInheritances;   // 删除的继承

    public:
        // 构造函数
        explicit CHTLCustomStyleNode(const std::string& styleName);
        ~CHTLCustomStyleNode() override = default;

        // 样式属性管理
        void AddStyleProperty(const std::string& property, const std::string& value); // 添加有值属性
        void AddNoValueProperty(const std::string& property);   // 添加无值属性
        void RemoveStyleProperty(const std::string& property);  // 移除属性
        std::string GetStyleProperty(const std::string& property) const; // 获取属性值
        bool HasStyleProperty(const std::string& property) const; // 是否有属性
        bool IsNoValueProperty(const std::string& property) const; // 是否为无值属性
        const std::unordered_set<std::string>& GetNoValueProperties() const; // 获取无值属性集合
        
        // 特例化操作
        void FillNoValueProperty(const std::string& property, const std::string& value); // 填充无值属性
        bool CanFillProperty(const std::string& property) const; // 是否可以填充属性
        
        // 删除操作
        void DeleteProperty(const std::string& property);       // 删除属性
        void DeleteInheritance(const std::string& inheritanceName); // 删除继承
        bool IsPropertyDeleted(const std::string& property) const; // 属性是否被删除
        bool IsInheritanceDeleted(const std::string& inheritanceName) const; // 继承是否被删除
        const std::unordered_set<std::string>& GetDeletedProperties() const; // 获取删除的属性
        const std::unordered_set<std::string>& GetDeletedInheritances() const; // 获取删除的继承
        
        // 继承处理（可以继承模板或其他自定义样式组）
        void ApplyTemplateInheritance(const CHTLTemplateStyleNode& templateStyle); // 应用模板继承
        void ApplyCustomInheritance(const CHTLCustomStyleNode& customStyle); // 应用自定义继承
        
        // CSS生成
        std::string GenerateCSS() const;                        // 生成CSS代码
        std::string GenerateInlineStyle() const;               // 生成内联样式
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 自定义元素节点类
    // 处理 [Custom] @Element 名称 { } 语法
    // 支持特例化操作：增加样式、索引访问、插入元素、删除元素
    class CHTLCustomElementNode : public CHTLCustomNode {
    private:
        std::vector<std::shared_ptr<CHTLBaseNode>> customElements; // 自定义元素列表
        std::unordered_set<size_t> deletedElementIndices;      // 删除的元素索引
        std::unordered_set<std::string> deletedElementTypes;   // 删除的元素类型
        std::unordered_set<std::string> deletedInheritances;   // 删除的继承

    public:
        // 构造函数
        explicit CHTLCustomElementNode(const std::string& elementName);
        ~CHTLCustomElementNode() override = default;

        // 自定义元素管理
        void AddCustomElement(std::shared_ptr<CHTLBaseNode> element); // 添加自定义元素
        void RemoveCustomElement(std::shared_ptr<CHTLBaseNode> element); // 移除自定义元素
        const std::vector<std::shared_ptr<CHTLBaseNode>>& GetCustomElements() const; // 获取自定义元素
        size_t GetCustomElementCount() const;                  // 获取自定义元素数量
        
        // 索引访问
        std::shared_ptr<CHTLBaseNode> GetElementByIndex(size_t index) const; // 按索引获取元素
        std::vector<std::shared_ptr<CHTLBaseNode>> GetElementsByType(const std::string& elementType) const; // 按类型获取元素
        
        // 删除操作
        void DeleteElement(const std::string& elementType);     // 删除指定类型的所有元素
        void DeleteElementByIndex(size_t index);                // 按索引删除元素
        void DeleteInheritance(const std::string& inheritanceName); // 删除继承
        bool IsElementDeleted(size_t index) const;              // 元素是否被删除
        bool IsElementTypeDeleted(const std::string& elementType) const; // 元素类型是否被删除
        bool IsInheritanceDeleted(const std::string& inheritanceName) const; // 继承是否被删除
        
        // 插入操作
        void InsertElement(CHTLInsertPosition position, const std::string& targetSelector, 
                          std::shared_ptr<CHTLBaseNode> element); // 插入元素
        void InsertElementAtIndex(CHTLInsertPosition position, size_t targetIndex,
                                 std::shared_ptr<CHTLBaseNode> element); // 按索引插入元素
        
        // 特例化操作：增加样式
        void AddStyleToElement(size_t index, std::shared_ptr<CHTLBaseNode> styleNode); // 为指定索引元素添加样式
        void AddStyleToElementType(const std::string& elementType, std::shared_ptr<CHTLBaseNode> styleNode); // 为指定类型元素添加样式
        
        // 继承处理
        void ApplyTemplateInheritance(const CHTLTemplateElementNode& templateElement); // 应用模板继承
        void ApplyCustomInheritance(const CHTLCustomElementNode& customElement); // 应用自定义继承
        
        // HTML生成
        std::string GenerateHTML() const;                      // 生成HTML代码
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 自定义变量组节点类
    // 处理 [Custom] @Var 名称 { } 语法
    // 支持变量组特例化
    class CHTLCustomVarNode : public CHTLCustomNode {
    private:
        std::unordered_map<std::string, std::string> variables; // 变量映射
        std::vector<std::string> variableOrder;                // 变量顺序

    public:
        // 构造函数
        explicit CHTLCustomVarNode(const std::string& varGroupName);
        ~CHTLCustomVarNode() override = default;

        // 变量管理
        void AddVariable(const std::string& varName, const std::string& value); // 添加变量
        void RemoveVariable(const std::string& varName);        // 移除变量
        std::string GetVariable(const std::string& varName) const; // 获取变量值
        bool HasVariable(const std::string& varName) const;     // 是否有变量
        const std::unordered_map<std::string, std::string>& GetAllVariables() const; // 获取所有变量
        
        // 变量组特例化
        std::string ProcessSpecializedReference(const std::string& varName, const std::string& specializedValue) const; // 处理特例化引用
        
        // 变量引用处理
        std::string ResolveVariableReference(const std::string& varName) const; // 解析变量引用
        std::string ProcessVariableReferences(const std::string& text) const; // 处理文本中的变量引用
        
        // 继承处理
        void ApplyTemplateInheritance(const CHTLTemplateVarNode& templateVar); // 应用模板继承
        void ApplyCustomInheritance(const CHTLCustomVarNode& customVar); // 应用自定义继承
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 自定义使用节点类
    // 处理自定义的使用语法，支持特例化操作
    class CHTLCustomUsageNode : public CHTLBaseNode {
    private:
        CHTLCustomType customType;                              // 使用的自定义类型
        std::string customName;                                 // 使用的自定义名称
        bool isFullQualified;                                   // 是否使用全缀名
        std::string fullQualifiedPrefix;                       // 全缀名前缀
        
        // 特例化信息
        bool hasSpecialization;                                // 是否有特例化
        std::unordered_map<std::string, std::string> specializationValues; // 特例化值
        std::unordered_set<std::string> deletedItems;          // 删除的项目

    public:
        // 构造函数
        CHTLCustomUsageNode(CHTLCustomType type, const std::string& name);
        ~CHTLCustomUsageNode() override = default;

        // 自定义使用管理
        CHTLCustomType GetCustomType() const;                   // 获取自定义类型
        const std::string& GetCustomName() const;               // 获取自定义名称
        void SetCustomName(const std::string& name);            // 设置自定义名称
        
        // 全缀名管理
        bool IsFullQualified() const;                          // 是否使用全缀名
        void SetFullQualified(bool qualified, const std::string& prefix = ""); // 设置全缀名
        const std::string& GetFullQualifiedPrefix() const;     // 获取全缀名前缀
        
        // 特例化管理
        bool HasSpecialization() const;                        // 是否有特例化
        void AddSpecialization(const std::string& key, const std::string& value); // 添加特例化值
        void RemoveSpecialization(const std::string& key);     // 移除特例化值
        std::string GetSpecialization(const std::string& key) const; // 获取特例化值
        const std::unordered_map<std::string, std::string>& GetAllSpecializations() const; // 获取所有特例化值
        
        // 删除操作
        void AddDeletion(const std::string& itemName);          // 添加删除项
        void RemoveDeletion(const std::string& itemName);       // 移除删除项
        bool IsDeleted(const std::string& itemName) const;      // 是否被删除
        const std::unordered_set<std::string>& GetDeletedItems() const; // 获取删除的项目
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 操作节点类（delete, insert等）
    // 处理自定义系统中的各种操作
    class CHTLOperatorNode : public CHTLBaseNode {
    private:
        CHTLSpecializationType operationType;                   // 操作类型
        std::string targetName;                                 // 目标名称
        std::string targetSelector;                             // 目标选择器
        size_t targetIndex;                                     // 目标索引
        CHTLInsertPosition insertPosition;                      // 插入位置（仅插入操作使用）

    public:
        // 构造函数
        CHTLOperatorNode(CHTLSpecializationType type, const std::string& target);
        ~CHTLOperatorNode() override = default;

        // 操作类型管理
        CHTLSpecializationType GetOperationType() const;        // 获取操作类型
        void SetOperationType(CHTLSpecializationType type);     // 设置操作类型
        
        // 目标管理
        const std::string& GetTargetName() const;               // 获取目标名称
        const std::string& GetTargetSelector() const;           // 获取目标选择器
        size_t GetTargetIndex() const;                         // 获取目标索引
        void SetTargetName(const std::string& target);          // 设置目标名称
        void SetTargetSelector(const std::string& selector);    // 设置目标选择器
        void SetTargetIndex(size_t index);                     // 设置目标索引
        
        // 插入位置管理（仅插入操作）
        CHTLInsertPosition GetInsertPosition() const;           // 获取插入位置
        void SetInsertPosition(CHTLInsertPosition position);    // 设置插入位置
        
        // 操作执行
        bool CanExecute() const;                                // 是否可以执行
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 静态工具方法
        static std::string OperationTypeToString(CHTLSpecializationType type); // 操作类型转字符串
        static std::string InsertPositionToString(CHTLInsertPosition position); // 插入位置转字符串
    };

    // 自定义节点工厂类
    class CHTLCustomNodeFactory {
    public:
        // 创建自定义定义节点
        static std::shared_ptr<CHTLCustomStyleNode> CreateCustomStyleNode(const std::string& styleName);
        static std::shared_ptr<CHTLCustomElementNode> CreateCustomElementNode(const std::string& elementName);
        static std::shared_ptr<CHTLCustomVarNode> CreateCustomVarNode(const std::string& varGroupName);
        
        // 创建自定义使用节点
        static std::shared_ptr<CHTLCustomUsageNode> CreateStyleUsageNode(const std::string& styleName);
        static std::shared_ptr<CHTLCustomUsageNode> CreateElementUsageNode(const std::string& elementName);
        static std::shared_ptr<CHTLCustomUsageNode> CreateVarUsageNode(const std::string& varGroupName);
        
        // 创建操作节点
        static std::shared_ptr<CHTLOperatorNode> CreateDeleteNode(const std::string& target);
        static std::shared_ptr<CHTLOperatorNode> CreateInsertNode(CHTLInsertPosition position, const std::string& target);
        
        // 解析方法
        static CHTLCustomType ParseCustomType(const std::string& typeText);
        static CHTLInsertPosition ParseInsertPosition(const std::string& positionText);
        
        // 验证方法
        static bool IsValidCustomName(const std::string& name);
        static bool IsValidCustomType(const std::string& type);
    };

} // namespace CHTL