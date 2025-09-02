#pragma once

#include "CHTLJSBaseNode.h"
#include <unordered_map>

namespace CHTLJS {

/**
 * 虚对象键类型枚举
 */
enum class VirKeyType {
    FUNCTION_KEY,       // 函数键
    OBJECT_KEY,         // 对象键
    ARRAY_KEY,          // 数组键
    VALUE_KEY           // 普通值键
};

/**
 * 虚对象键结构
 */
struct VirKey {
    std::string Name;                   // 键名
    VirKeyType Type;                    // 键类型
    std::string Value;                  // 键值
    std::string FunctionSignature;     // 函数签名（如果是函数）
    bool IsResolved;                    // 是否已解析
    
    VirKey() : Type(VirKeyType::VALUE_KEY), IsResolved(false) {}
    VirKey(const std::string& name, VirKeyType type, const std::string& value)
        : Name(name), Type(type), Value(value), IsResolved(true) {}
};

/**
 * CHTL JS虚对象节点
 * 表示vir虚对象声明和使用，提供访问CHTL JS函数元信息的能力
 */
class VirObjectNode : public CHTLJSBaseNode {
private:
    std::string m_VirName;                              // 虚对象名称
    std::string m_CHTLJSFunctionContent;                // CHTL JS函数内容
    std::string m_CHTLJSFunctionType;                   // CHTL JS函数类型（listen/delegate/animate）
    std::unordered_map<std::string, VirKey> m_VirKeys;  // 虚对象键映射
    std::unordered_map<std::string, std::string> m_FunctionReferences; // 函数引用映射
    std::unordered_map<std::string, std::string> m_ObjectReferences;   // 对象引用映射
    
    bool m_IsActive;                                    // 是否激活
    bool m_HasFunctionKeys;                             // 是否有函数键
    bool m_HasObjectKeys;                               // 是否有对象键
    bool m_IsResolved;                                  // 是否已解析

public:
    /**
     * 构造函数
     * @param virName 虚对象名称
     * @param line 行号
     * @param column 列号
     */
    VirObjectNode(const std::string& virName, size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~VirObjectNode() override = default;
    
    /**
     * 获取虚对象名称
     * @return 虚对象名称
     */
    const std::string& GetVirName() const { return m_VirName; }
    
    /**
     * 设置虚对象名称
     * @param name 新名称
     */
    void SetVirName(const std::string& name);
    
    /**
     * 获取CHTL JS函数内容
     * @return 函数内容
     */
    const std::string& GetCHTLJSFunctionContent() const { return m_CHTLJSFunctionContent; }
    
    /**
     * 设置CHTL JS函数内容
     * @param content 函数内容
     */
    void SetCHTLJSFunctionContent(const std::string& content);
    
    /**
     * 获取CHTL JS函数类型
     * @return 函数类型
     */
    const std::string& GetCHTLJSFunctionType() const { return m_CHTLJSFunctionType; }
    
    /**
     * 设置CHTL JS函数类型
     * @param type 函数类型
     */
    void SetCHTLJSFunctionType(const std::string& type) { m_CHTLJSFunctionType = type; }
    
    // 虚对象键管理
    /**
     * 添加虚对象键
     * @param key 键结构
     */
    void AddVirKey(const VirKey& key);
    
    /**
     * 添加函数键
     * @param keyName 键名
     * @param functionContent 函数内容
     * @param signature 函数签名
     */
    void AddFunctionKey(const std::string& keyName, 
                       const std::string& functionContent,
                       const std::string& signature = "");
    
    /**
     * 添加对象键
     * @param keyName 键名
     * @param objectContent 对象内容
     */
    void AddObjectKey(const std::string& keyName, const std::string& objectContent);
    
    /**
     * 添加数组键
     * @param keyName 键名
     * @param arrayContent 数组内容
     */
    void AddArrayKey(const std::string& keyName, const std::string& arrayContent);
    
    /**
     * 添加值键
     * @param keyName 键名
     * @param value 值
     */
    void AddValueKey(const std::string& keyName, const std::string& value);
    
    /**
     * 获取虚对象键
     * @param keyName 键名
     * @return 键结构指针
     */
    VirKey* GetVirKey(const std::string& keyName);
    
    /**
     * 获取所有虚对象键
     * @return 键映射
     */
    const std::unordered_map<std::string, VirKey>& GetVirKeys() const { return m_VirKeys; }
    
    /**
     * 检查是否有指定键
     * @param keyName 键名
     * @return 是否存在
     */
    bool HasVirKey(const std::string& keyName) const;
    
    // 虚对象引用解析
    /**
     * 解析虚对象键引用（Test->click）
     * @param keyName 键名
     * @return 解析后的JavaScript代码
     */
    std::string ResolveKeyReference(const std::string& keyName) const;
    
    /**
     * 解析函数引用
     * @param keyName 键名
     * @return 函数引用代码
     */
    std::string ResolveFunctionReference(const std::string& keyName) const;
    
    /**
     * 解析对象引用
     * @param keyName 键名
     * @return 对象引用代码
     */
    std::string ResolveObjectReference(const std::string& keyName) const;
    
    // 状态管理
    /**
     * 激活虚对象
     */
    void Activate() { m_IsActive = true; }
    
    /**
     * 停用虚对象
     */
    void Deactivate() { m_IsActive = false; }
    
    /**
     * 检查是否激活
     * @return 是否激活
     */
    bool IsActive() const { return m_IsActive; }
    
    /**
     * 检查是否已解析
     * @return 是否已解析
     */
    bool IsResolved() const { return m_IsResolved; }
    
    // CHTL JS函数解析
    /**
     * 解析CHTL JS函数内容
     * @param functionContent 函数内容
     */
    void ParseCHTLJSFunction(const std::string& functionContent);
    
    /**
     * 检测函数类型
     * @param content 函数内容
     * @return 函数类型
     */
    std::string DetectCHTLJSFunctionType(const std::string& content);
    
    /**
     * 提取函数键值对
     * @param content 函数内容
     */
    void ExtractFunctionKeyValuePairs(const std::string& content);
    
    // 代码生成
    /**
     * 生成虚对象JavaScript代码
     * @return JavaScript代码
     */
    std::string GenerateVirObjectCode() const;
    
    /**
     * 生成键引用映射代码
     * @return 键引用映射代码
     */
    std::string GenerateKeyReferenceMap() const;
    
    // 访问者模式实现
    void Accept(CHTLJSNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLJSBaseNode> Clone() const override;
    
    // JavaScript代码生成
    std::string GenerateJavaScript() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 分析键类型
     * @param value 键值
     * @return 键类型
     */
    VirKeyType AnalyzeKeyType(const std::string& value) const;
    
    /**
     * 验证虚对象名称
     * @param name 名称
     * @return 是否有效
     */
    bool IsValidVirName(const std::string& name) const;
    
    /**
     * 处理函数签名
     * @param functionContent 函数内容
     * @return 函数签名
     */
    std::string ExtractFunctionSignature(const std::string& functionContent) const;
    
    /**
     * 生成唯一函数名
     * @param keyName 键名
     * @return 唯一函数名
     */
    std::string GenerateUniqueFunctionName(const std::string& keyName) const;
};

} // namespace CHTLJS