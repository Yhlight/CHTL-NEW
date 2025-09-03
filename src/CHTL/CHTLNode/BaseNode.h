#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

    // AST节点类型枚举
    enum class CHTLNodeType {
        // 基础节点类型
        BASE,                   // 基础节点
        ELEMENT,                // HTML元素节点
        TEXT,                   // 文本节点
        COMMENT,                // 注释节点
        
        // 样式相关节点
        STYLE,                  // 样式节点
        STYLE_PROPERTY,         // 样式属性节点
        CSS_SELECTOR,           // CSS选择器节点
        
        // 脚本相关节点
        SCRIPT,                 // 脚本节点
        
        // 模板相关节点
        TEMPLATE,               // 模板节点
        TEMPLATE_STYLE,         // 样式组模板节点
        TEMPLATE_ELEMENT,       // 元素模板节点
        TEMPLATE_VAR,           // 变量组模板节点
        
        // 自定义相关节点
        CUSTOM,                 // 自定义节点
        CUSTOM_STYLE,           // 自定义样式组节点
        CUSTOM_ELEMENT,         // 自定义元素节点
        CUSTOM_VAR,             // 自定义变量组节点
        
        // 原始嵌入相关节点
        ORIGIN,                 // 原始嵌入节点
        ORIGIN_HTML,            // HTML原始嵌入节点
        ORIGIN_STYLE,           // Style原始嵌入节点
        ORIGIN_JAVASCRIPT,      // JavaScript原始嵌入节点
        ORIGIN_CUSTOM,          // 自定义类型原始嵌入节点
        
        // 导入相关节点
        IMPORT,                 // 导入节点
        IMPORT_PATH,            // 导入路径节点
        IMPORT_ALIAS,           // 导入别名节点
        
        // 配置相关节点
        CONFIGURATION,          // 配置节点
        CONFIG_ITEM,            // 配置项节点
        NAME_CONFIG,            // Name配置节点
        ORIGIN_TYPE_CONFIG,     // OriginType配置节点
        
        // 命名空间相关节点
        NAMESPACE,              // 命名空间节点
        NAMESPACE_CONTENT,      // 命名空间内容节点
        
        // 操作相关节点
        INHERIT,                // 继承操作节点
        DELETE,                 // 删除操作节点
        INSERT,                 // 插入操作节点
        EXCEPT,                 // 约束操作节点
        USE,                    // use语句节点
        
        // 属性相关节点
        ATTRIBUTE,              // 属性节点
        ATTRIBUTE_VALUE         // 属性值节点
    };

    // AST基础节点类
    // 所有CHTL AST节点的基类
    class CHTLBaseNode : public std::enable_shared_from_this<CHTLBaseNode> {
    protected:
        CHTLNodeType nodeType;                              // 节点类型
        std::string nodeName;                               // 节点名称
        std::string nodeValue;                              // 节点值
        std::vector<std::shared_ptr<CHTLBaseNode>> children; // 子节点列表
        std::weak_ptr<CHTLBaseNode> parent;                 // 父节点（使用weak_ptr避免循环引用）
        std::unordered_map<std::string, std::string> attributes; // 节点属性
        
        // 位置信息
        size_t startPos;                                    // 起始位置
        size_t endPos;                                      // 结束位置
        int lineNumber;                                     // 行号
        int columnNumber;                                   // 列号

    public:
        // 构造函数
        CHTLBaseNode(CHTLNodeType type, const std::string& name = "", const std::string& value = "");
        virtual ~CHTLBaseNode() = default;

        // 基本访问方法
        CHTLNodeType GetNodeType() const;                   // 获取节点类型
        const std::string& GetNodeName() const;             // 获取节点名称
        const std::string& GetNodeValue() const;            // 获取节点值
        
        void SetNodeName(const std::string& name);          // 设置节点名称
        void SetNodeValue(const std::string& value);        // 设置节点值
        
        // 位置信息方法
        void SetPosition(size_t start, size_t end, int line, int column);  // 设置位置信息
        size_t GetStartPos() const;                         // 获取起始位置
        size_t GetEndPos() const;                           // 获取结束位置
        int GetLineNumber() const;                          // 获取行号
        int GetColumnNumber() const;                        // 获取列号
        
        // 子节点管理
        void AddChild(std::shared_ptr<CHTLBaseNode> child);  // 添加子节点
        void RemoveChild(std::shared_ptr<CHTLBaseNode> child); // 移除子节点
        void InsertChild(size_t index, std::shared_ptr<CHTLBaseNode> child); // 插入子节点
        std::shared_ptr<CHTLBaseNode> GetChild(size_t index) const; // 获取子节点
        size_t GetChildCount() const;                       // 获取子节点数量
        const std::vector<std::shared_ptr<CHTLBaseNode>>& GetChildren() const; // 获取所有子节点
        
        // 父节点管理
        void SetParent(std::weak_ptr<CHTLBaseNode> parent);   // 设置父节点
        std::shared_ptr<CHTLBaseNode> GetParent() const;     // 获取父节点
        bool HasParent() const;                             // 是否有父节点
        
        // 属性管理
        void SetAttribute(const std::string& key, const std::string& value); // 设置属性
        std::string GetAttribute(const std::string& key) const; // 获取属性
        bool HasAttribute(const std::string& key) const;    // 是否有属性
        void RemoveAttribute(const std::string& key);       // 移除属性
        const std::unordered_map<std::string, std::string>& GetAllAttributes() const; // 获取所有属性
        
        // 查找方法
        std::vector<std::shared_ptr<CHTLBaseNode>> FindChildrenByType(CHTLNodeType type) const; // 按类型查找子节点
        std::shared_ptr<CHTLBaseNode> FindChildByName(const std::string& name) const; // 按名称查找子节点
        std::shared_ptr<CHTLBaseNode> FindFirstChildByType(CHTLNodeType type) const; // 查找第一个指定类型的子节点
        
        // 工具方法
        bool IsLeafNode() const;                            // 是否为叶子节点
        int GetDepth() const;                               // 获取节点深度
        std::string GetPath() const;                        // 获取节点路径
        
        // 虚方法（子类可重写）
        virtual std::string ToString() const;               // 转换为字符串表示
        virtual std::shared_ptr<CHTLBaseNode> Clone() const; // 克隆节点
        virtual bool Validate() const;                      // 验证节点有效性
        
        // 调试方法
        virtual void PrintTree(int indent = 0) const;       // 打印节点树（调试用）
        virtual void PrintInfo() const;                     // 打印节点信息（调试用）
        
        // 静态工具方法
        static std::string NodeTypeToString(CHTLNodeType type); // 节点类型转字符串
        static std::shared_ptr<CHTLBaseNode> CreateNode(CHTLNodeType type, const std::string& name = "", const std::string& value = ""); // 创建节点
    };

    // 节点创建工厂类
    class CHTLNodeFactory {
    public:
        // 创建基础节点
        static std::shared_ptr<CHTLBaseNode> CreateElementNode(const std::string& tagName);
        static std::shared_ptr<CHTLBaseNode> CreateTextNode(const std::string& text);
        static std::shared_ptr<CHTLBaseNode> CreateCommentNode(const std::string& comment);
        
        // 创建样式节点
        static std::shared_ptr<CHTLBaseNode> CreateStyleNode();
        static std::shared_ptr<CHTLBaseNode> CreateStylePropertyNode(const std::string& property, const std::string& value);
        static std::shared_ptr<CHTLBaseNode> CreateSelectorNode(const std::string& selector);
        
        // 创建属性节点
        static std::shared_ptr<CHTLBaseNode> CreateAttributeNode(const std::string& name, const std::string& value);
        
        // 验证方法
        static bool IsValidNodeType(CHTLNodeType type);
        static bool IsValidNodeName(const std::string& name);
    };

} // namespace CHTL