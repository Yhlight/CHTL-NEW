#pragma once

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * CHTL解析状态枚举
 * 定义解析过程中的各种状态
 */
enum class CHTLParseState {
    // 基础状态
    INITIAL,                    // 初始状态
    PARSING_ELEMENT,           // 解析元素
    PARSING_TEXT,              // 解析文本节点
    PARSING_ATTRIBUTES,        // 解析属性
    PARSING_STYLE_BLOCK,       // 解析局部样式块
    PARSING_SCRIPT_BLOCK,      // 解析局部脚本块（属于CHTL）
    
    // 模板状态
    PARSING_TEMPLATE_BLOCK,    // 解析模板块
    PARSING_TEMPLATE_STYLE,    // 解析样式组模板
    PARSING_TEMPLATE_ELEMENT,  // 解析元素模板
    PARSING_TEMPLATE_VAR,      // 解析变量组模板
    
    // 自定义状态
    PARSING_CUSTOM_BLOCK,      // 解析自定义块
    PARSING_CUSTOM_STYLE,      // 解析自定义样式组
    PARSING_CUSTOM_ELEMENT,    // 解析自定义元素
    PARSING_CUSTOM_VAR,        // 解析自定义变量组
    
    // 原始嵌入状态
    PARSING_ORIGIN_BLOCK,      // 解析原始嵌入块
    PARSING_ORIGIN_HTML,       // 解析HTML原始嵌入
    PARSING_ORIGIN_STYLE,      // 解析样式原始嵌入
    PARSING_ORIGIN_JAVASCRIPT, // 解析JavaScript原始嵌入
    PARSING_ORIGIN_CUSTOM,     // 解析自定义类型原始嵌入
    
    // 配置状态
    PARSING_CONFIGURATION,     // 解析配置块
    PARSING_NAME_CONFIG,       // 解析Name配置块
    PARSING_ORIGIN_TYPE_CONFIG, // 解析OriginType配置块
    
    // 导入状态
    PARSING_IMPORT_BLOCK,      // 解析导入块
    PARSING_IMPORT_PATH,       // 解析导入路径
    PARSING_IMPORT_AS,         // 解析as重命名
    PARSING_IMPORT_FROM,       // 解析from命名空间
    
    // 命名空间状态
    PARSING_NAMESPACE_BLOCK,   // 解析命名空间块
    PARSING_NAMESPACE_CONTENT, // 解析命名空间内容
    
    // 特殊操作状态
    PARSING_INHERIT,           // 解析继承
    PARSING_DELETE,            // 解析删除操作
    PARSING_INSERT,            // 解析插入操作
    PARSING_EXCEPT,            // 解析约束
    
    // 选择器状态
    PARSING_CLASS_SELECTOR,    // 解析类选择器
    PARSING_ID_SELECTOR,       // 解析ID选择器
    PARSING_CONTEXT_REFERENCE, // 解析上下文引用
    PARSING_PSEUDO_CLASS,      // 解析伪类
    PARSING_PSEUDO_ELEMENT,    // 解析伪元素
    
    // 索引和特例化状态
    PARSING_INDEX_ACCESS,      // 解析索引访问
    PARSING_SPECIALIZATION,    // 解析特例化
    
    // 错误和结束状态
    ERROR_STATE,               // 错误状态
    END_STATE                  // 结束状态
};

/**
 * 状态上下文结构
 * 存储状态相关的上下文信息
 */
struct StateContext {
    CHTLParseState State;               // 当前状态
    std::string ElementName;            // 当前元素名称
    std::string BlockType;              // 当前块类型
    std::string TemplateName;           // 当前模板名称
    std::string CustomName;             // 当前自定义名称
    std::string NamespaceName;          // 当前命名空间名称
    int BraceLevel;                     // 大括号层级
    int BracketLevel;                   // 方括号层级
    bool InString;                      // 是否在字符串内
    char StringChar;                    // 字符串引号字符
    bool InComment;                     // 是否在注释内
    
    StateContext() : State(CHTLParseState::INITIAL), BraceLevel(0), 
                    BracketLevel(0), InString(false), StringChar('\0'), InComment(false) {}
};

/**
 * CHTL状态机
 * 管理CHTL解析过程中的状态转换和上下文
 * 使用RAII模式自动管理状态生命周期
 */
class CHTLStateMachine {
private:
    std::stack<StateContext> m_StateStack;     // 状态栈
    StateContext m_CurrentContext;             // 当前上下文
    std::unordered_map<CHTLParseState, std::vector<CHTLParseState>> m_ValidTransitions; // 有效状态转换表
    std::string m_ErrorMessage;                // 错误信息
    bool m_HasError;                          // 是否有错误

public:
    /**
     * 构造函数
     */
    CHTLStateMachine();
    
    /**
     * 析构函数
     */
    ~CHTLStateMachine() = default;
    
    /**
     * 状态转换
     * @param newState 新状态
     * @param context 上下文信息
     * @return 是否转换成功
     */
    bool TransitionTo(CHTLParseState newState, const std::string& context = "");
    
    /**
     * 推入新状态（保存当前状态）
     * @param newState 新状态
     * @param context 上下文信息
     * @return 是否推入成功
     */
    bool PushState(CHTLParseState newState, const std::string& context = "");
    
    /**
     * 弹出状态（恢复上一状态）
     * @return 是否弹出成功
     */
    bool PopState();
    
    /**
     * 获取当前状态
     * @return 当前状态
     */
    CHTLParseState GetCurrentState() const;
    
    /**
     * 获取当前上下文
     * @return 当前上下文引用
     */
    const StateContext& GetCurrentContext() const;
    
    /**
     * 更新上下文信息
     * @param elementName 元素名称
     * @param blockType 块类型
     * @param templateName 模板名称
     */
    void UpdateContext(const std::string& elementName = "", 
                      const std::string& blockType = "",
                      const std::string& templateName = "");
    
    /**
     * 进入大括号块
     */
    void EnterBrace();
    
    /**
     * 退出大括号块
     */
    void ExitBrace();
    
    /**
     * 进入方括号块
     */
    void EnterBracket();
    
    /**
     * 退出方括号块
     */
    void ExitBracket();
    
    /**
     * 进入字符串
     * @param quote 引号字符
     */
    void EnterString(char quote);
    
    /**
     * 退出字符串
     */
    void ExitString();
    
    /**
     * 进入注释
     */
    void EnterComment();
    
    /**
     * 退出注释
     */
    void ExitComment();
    
    /**
     * 检查状态转换是否有效
     * @param from 源状态
     * @param to 目标状态
     * @return 是否有效
     */
    bool IsValidTransition(CHTLParseState from, CHTLParseState to);
    
    /**
     * 重置状态机
     */
    void Reset();
    
    /**
     * 检查是否有错误
     * @return 是否有错误
     */
    bool HasError() const;
    
    /**
     * 获取错误信息
     * @return 错误信息
     */
    std::string GetErrorMessage() const;
    
    /**
     * 获取状态名称
     * @param state 状态
     * @return 状态名称字符串
     */
    std::string GetStateName(CHTLParseState state) const;

private:
    /**
     * 初始化有效状态转换表
     */
    void InitializeValidTransitions();
    
    /**
     * 设置错误状态
     * @param message 错误信息
     */
    void SetError(const std::string& message);
    
    /**
     * 验证上下文一致性
     * @return 是否一致
     */
    bool ValidateContextConsistency();
};

/**
 * RAII状态管理器
 * 自动管理状态的推入和弹出
 */
class CHTLStateGuard {
private:
    CHTLStateMachine* m_StateMachine;
    bool m_ShouldPop;

public:
    /**
     * 构造函数 - 自动推入状态
     * @param stateMachine 状态机指针
     * @param newState 新状态
     * @param context 上下文信息
     */
    CHTLStateGuard(CHTLStateMachine* stateMachine, CHTLParseState newState, const std::string& context = "");
    
    /**
     * 析构函数 - 自动弹出状态
     */
    ~CHTLStateGuard();
    
    // 禁止拷贝和赋值
    CHTLStateGuard(const CHTLStateGuard&) = delete;
    CHTLStateGuard& operator=(const CHTLStateGuard&) = delete;
    
    // 支持移动
    CHTLStateGuard(CHTLStateGuard&& other) noexcept;
    CHTLStateGuard& operator=(CHTLStateGuard&& other) noexcept;
    
    /**
     * 手动释放状态管理
     */
    void Release();
};

} // namespace CHTL