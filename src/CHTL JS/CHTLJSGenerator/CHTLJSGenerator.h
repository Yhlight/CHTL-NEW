#pragma once

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSNode/EnhanceSelectorNode.h"
#include "../CHTLJSNode/VirObjectNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTLJS {

/**
 * CHTL JS生成结果结构（完全独立于CHTL）
 */
struct CHTLJSGenerationResult {
    std::string JavaScriptContent;              // 生成的JavaScript内容
    std::string ModuleLoadCode;                 // 模块加载代码（AMD风格）
    std::string VirtualObjectCode;              // 虚对象代码
    std::string EventDelegateCode;              // 事件委托代码
    std::string AnimationCode;                  // 动画代码
    std::string EnhancedSelectorCode;           // 增强选择器代码
    bool IsSuccess;                             // 是否生成成功
    std::string ErrorMessage;                   // 错误信息
    std::vector<std::string> Warnings;         // 警告信息
    
    CHTLJSGenerationResult() : IsSuccess(false) {}
};

/**
 * CHTL JS生成器（完全独立于CHTL生成器）
 * 负责从CHTL JS AST生成JavaScript代码
 * 专门处理CHTL JS特有的语法特征
 */
class CHTLJSGenerator {
private:
    std::unique_ptr<CHTLJSBaseNode> m_RootNode;         // CHTL JS AST根节点
    
    // CHTL JS生成内容
    std::string m_GeneratedJavaScript;                  // 生成的JavaScript
    std::string m_ModuleLoadCode;                       // 模块加载代码
    std::string m_VirtualObjectCode;                    // 虚对象代码
    std::string m_EventDelegateCode;                    // 事件委托代码
    std::string m_AnimationCode;                        // 动画代码
    std::string m_EnhancedSelectorCode;                 // 增强选择器代码
    
    // CHTL JS特有管理
    std::unordered_map<std::string, VirObjectNode*> m_VirtualObjects;       // 虚对象映射
    std::unordered_map<std::string, EnhancedSelectorNode*> m_EnhancedSelectors; // 增强选择器映射
    std::unordered_map<std::string, AnimateNode*> m_Animations;             // 动画映射
    std::vector<std::string> m_ModulePaths;             // 模块路径列表
    
    // 事件委托全局注册表
    std::unordered_map<std::string, std::vector<std::string>> m_EventDelegateRegistry;
    
    // 增强选择器缓存
    std::unordered_map<std::string, std::string> m_SelectorQueryCache;     // 选择器查询缓存
    
    // 错误和警告
    bool m_HasError;                                    // 是否有生成错误
    std::string m_ErrorMessage;                         // 错误信息
    std::vector<std::string> m_Warnings;               // 警告列表
    
    // CHTL JS生成选项
    bool m_GenerateSourceMaps;                          // 是否生成源映射
    bool m_MinifyJavaScript;                           // 是否压缩JavaScript
    bool m_AddCHTLJSComments;                          // 是否添加CHTL JS注释
    bool m_OptimizeSelectors;                          // 是否优化选择器
    bool m_MergeEventDelegates;                        // 是否合并事件委托
    
    // AST根节点
    std::shared_ptr<CHTLJSBaseNode> m_ASTRoot;          // 当前AST根节点

public:
    /**
     * 构造函数
     */
    CHTLJSGenerator();
    
    /**
     * 析构函数
     */
    ~CHTLJSGenerator() = default;
    
    /**
     * 从CHTL JS AST生成JavaScript代码
     * @param rootNode CHTL JS AST根节点
     * @return CHTL JS生成结果
     */
    CHTLJSGenerationResult Generate(std::unique_ptr<CHTLJSBaseNode> rootNode);
    
    /**
     * 设置AST根节点
     * @param rootNode AST根节点
     */
    void SetAST(std::shared_ptr<CHTLJSBaseNode> rootNode);
    
    /**
     * 生成JavaScript内容
     * @return JavaScript字符串
     */
    std::string GenerateJavaScript();
    
    /**
     * 获取生成的JavaScript内容
     * @return JavaScript内容
     */
    const std::string& GetJavaScriptContent() const { return m_GeneratedJavaScript; }
    
    /**
     * 获取模块加载代码
     * @return 模块加载代码
     */
    const std::string& GetModuleLoadCode() const { return m_ModuleLoadCode; }
    
    /**
     * 获取虚对象代码
     * @return 虚对象代码
     */
    const std::string& GetVirtualObjectCode() const { return m_VirtualObjectCode; }
    
    /**
     * 获取事件委托代码
     * @return 事件委托代码
     */
    const std::string& GetEventDelegateCode() const { return m_EventDelegateCode; }
    
    /**
     * 获取动画代码
     * @return 动画代码
     */
    const std::string& GetAnimationCode() const { return m_AnimationCode; }
    
    /**
     * 设置CHTL JS生成选项
     * @param sourceMaps 是否生成源映射
     * @param minifyJS 是否压缩JavaScript
     * @param chtljsComments 是否添加CHTL JS注释
     * @param optimizeSelectors 是否优化选择器
     * @param mergeEventDelegates 是否合并事件委托
     */
    void SetCHTLJSGenerationOptions(bool sourceMaps = false, bool minifyJS = false, 
                                   bool chtljsComments = true, bool optimizeSelectors = true, 
                                   bool mergeEventDelegates = true);
    
    /**
     * 重置CHTL JS生成器
     */
    void Reset();

private:
    // CHTL JS预处理阶段
    /**
     * 收集CHTL JS定义
     * @param node CHTL JS节点
     */
    void CollectCHTLJSDefinitions(CHTLJSBaseNode* node);
    
    /**
     * 处理虚对象解析
     */
    void ProcessVirtualObjectResolution();
    
    /**
     * 构建事件委托全局注册表
     */
    void BuildEventDelegateRegistry();
    
    /**
     * 优化增强选择器查询
     */
    void OptimizeEnhancedSelectorQueries();
    
    // JavaScript代码生成
    /**
     * 生成CHTL JS JavaScript代码
     * @param node CHTL JS节点
     */
    void GenerateCHTLJSJavaScript(CHTLJSBaseNode* node);
    
    /**
     * 生成模块加载JavaScript
     * @param modulePaths 模块路径列表
     */
    void GenerateModuleLoadJavaScript(const std::vector<std::string>& modulePaths);
    
    /**
     * 生成AMD风格模块加载器
     * @param paths 模块路径
     * @return AMD加载器代码
     */
    std::string GenerateAMDModuleLoader(const std::vector<std::string>& paths);
    
    /**
     * 生成增强选择器JavaScript
     * @param selectorNode 选择器节点
     */
    void GenerateEnhancedSelectorJavaScript(EnhancedSelectorNode* selectorNode);
    
    /**
     * 生成DOM查询优化代码
     * @param selector 选择器
     * @return 优化的查询代码
     */
    std::string GenerateOptimizedDOMQuery(const std::string& selector);
    
    /**
     * 生成选择器优先级查找代码
     * @param content 选择器内容
     * @return 优先级查找代码
     */
    std::string GenerateSelectorPriorityCode(const std::string& content);
    
    // 虚对象代码生成
    /**
     * 生成虚对象JavaScript
     * @param virNode 虚对象节点
     */
    void GenerateVirtualObjectJavaScript(VirObjectNode* virNode);
    
    /**
     * 生成vir键引用映射
     * @param virNode 虚对象节点
     * @return 键引用映射代码
     */
    std::string GenerateVirKeyReferenceMap(VirObjectNode* virNode);
    
    /**
     * 生成vir函数定义
     * @param virNode 虚对象节点
     * @return 函数定义代码
     */
    std::string GenerateVirFunctionDefinitions(VirObjectNode* virNode);
    
    // 事件系统代码生成
    /**
     * 生成listen函数JavaScript
     * @param selector 选择器
     * @param events 事件映射
     * @return listen代码
     */
    std::string GenerateListenJavaScript(const std::string& selector, 
                                        const std::unordered_map<std::string, std::string>& events);
    
    /**
     * 生成delegate函数JavaScript
     * @param parentSelector 父选择器
     * @param targetSelectors 目标选择器列表
     * @param events 事件映射
     * @return delegate代码
     */
    std::string GenerateDelegateJavaScript(const std::string& parentSelector,
                                          const std::vector<std::string>& targetSelectors,
                                          const std::unordered_map<std::string, std::string>& events);
    
    /**
     * 生成事件绑定操作符JavaScript（&->）
     * @param selector 选择器
     * @param eventType 事件类型
     * @param handler 处理器
     * @return 事件绑定代码
     */
    std::string GenerateEventBindJavaScript(const std::string& selector,
                                           const std::string& eventType,
                                           const std::string& handler);
    
    /**
     * 生成事件委托全局注册表代码
     * @return 全局注册表代码
     */
    std::string GenerateEventDelegateRegistryCode();
    
    // 动画代码生成
    /**
     * 生成动画JavaScript
     * @param animateNode 动画节点
     */
    void GenerateAnimationJavaScript(AnimateNode* animateNode);
    
    /**
     * 生成requestAnimationFrame封装代码
     * @param animationName 动画名称
     * @param animationLogic 动画逻辑
     * @return 封装后的代码
     */
    std::string GenerateRequestAnimationFrameWrapper(const std::string& animationName,
                                                    const std::string& animationLogic);
    
    /**
     * 生成动画关键帧处理代码
     * @param keyframes 关键帧列表
     * @return 关键帧处理代码
     */
    std::string GenerateKeyframeHandlingCode(const std::vector<AnimationKeyframe>& keyframes);
    
    /**
     * 生成缓动函数代码
     * @param easing 缓动函数名称
     * @return 缓动函数代码
     */
    std::string GenerateEasingFunctionCode(const std::string& easing);
    
    // 工具和优化方法
    /**
     * 添加CHTL JS源码注释
     * @param comment 注释内容
     * @return 格式化的注释
     */
    std::string AddCHTLJSSourceComment(const std::string& comment);
    
    /**
     * 优化生成的JavaScript代码
     */
    void OptimizeCHTLJSGeneratedCode();
    
    /**
     * 压缩JavaScript代码
     * @param jsCode 原始代码
     * @return 压缩后的代码
     */
    std::string MinifyJavaScriptCode(const std::string& jsCode);
    
    /**
     * 验证生成的CHTL JS代码
     * @return 是否有效
     */
    bool ValidateCHTLJSGeneratedCode();
    
    /**
     * 处理选择器缓存优化
     */
    void ProcessSelectorCacheOptimization();
    
    /**
     * 合并重复的事件委托
     */
    void MergeRedundantEventDelegates();
    
    // 错误处理
    /**
     * 设置CHTL JS生成错误
     * @param message 错误信息
     */
    void SetCHTLJSGenerationError(const std::string& message);
    
    /**
     * 添加CHTL JS生成警告
     * @param message 警告信息
     */
    void AddCHTLJSGenerationWarning(const std::string& message);
    
    /**
     * 生成错误处理代码
     * @param context 错误上下文
     * @return 错误处理代码
     */
    std::string GenerateErrorHandlingCode(const std::string& context);
};

} // namespace CHTLJS