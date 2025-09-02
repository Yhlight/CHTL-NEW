#pragma once

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS解析状态枚举（完全独立于CHTL状态）
 * 定义CHTL JS解析过程中的各种状态
 */
enum class CHTLJSParseState {
    // 基础状态
    INITIAL,                        // 初始状态
    PARSING_MODULE_BLOCK,          // 解析module{}块
    PARSING_MODULE_LOAD,           // 解析模块加载语句
    
    // 增强选择器状态
    PARSING_ENHANCED_SELECTOR,     // 解析{{}}增强选择器
    PARSING_SELECTOR_CONTENT,      // 解析选择器内容
    PARSING_ARROW_OPERATION,       // 解析->操作
    
    // CHTL JS函数状态
    PARSING_LISTEN_BLOCK,          // 解析listen{}块
    PARSING_LISTEN_EVENTS,         // 解析listen事件映射
    PARSING_DELEGATE_BLOCK,        // 解析delegate{}块
    PARSING_DELEGATE_TARGET,       // 解析delegate目标
    PARSING_DELEGATE_EVENTS,       // 解析delegate事件
    
    // 动画状态
    PARSING_ANIMATE_BLOCK,         // 解析animate{}块
    PARSING_ANIMATE_TARGET,        // 解析动画目标
    PARSING_ANIMATE_DURATION,      // 解析动画持续时间
    PARSING_ANIMATE_EASING,        // 解析缓动函数
    PARSING_ANIMATE_BEGIN,         // 解析动画起始状态
    PARSING_ANIMATE_WHEN,          // 解析动画关键帧
    PARSING_ANIMATE_END,           // 解析动画结束状态
    PARSING_ANIMATE_PROPERTIES,    // 解析动画属性
    
    // 虚对象状态
    PARSING_VIR_DECLARATION,       // 解析vir声明
    PARSING_VIR_ASSIGNMENT,        // 解析vir赋值
    PARSING_VIR_FUNCTION,          // 解析vir函数内容
    PARSING_VIR_KEYS,              // 解析vir键值对
    PARSING_VIR_REFERENCE,         // 解析vir对象引用
    
    // 事件绑定状态
    PARSING_EVENT_BIND,            // 解析&->事件绑定
    PARSING_EVENT_HANDLER,         // 解析事件处理器
    
    // 错误和结束状态
    ERROR_STATE,                   // 错误状态
    END_STATE                      // 结束状态
};

/**
 * CHTL JS状态上下文结构（完全独立于CHTL）
 */
struct CHTLJSStateContext {
    CHTLJSParseState State;                 // 当前状态
    std::string FunctionName;               // 当前CHTL JS函数名称
    std::string SelectorContent;            // 当前选择器内容
    std::string VirtualObjectName;          // 当前虚对象名称
    std::string AnimationName;              // 当前动画名称
    std::string DelegateParent;             // 当前委托父元素
    int BraceLevel;                         // 大括号层级
    int BracketLevel;                       // 方括号层级
    int ParenLevel;                         // 圆括号层级
    bool InString;                          // 是否在字符串内
    char StringChar;                        // 字符串引号字符
    bool InSelector;                        // 是否在{{}}选择器内
    
    CHTLJSStateContext() : State(CHTLJSParseState::INITIAL), BraceLevel(0), 
                          BracketLevel(0), ParenLevel(0), InString(false), 
                          StringChar('\0'), InSelector(false) {}
};

/**
 * CHTL JS状态机（完全独立于CHTL状态机）
 * 管理CHTL JS解析过程中的状态转换和上下文
 * 使用RAII模式自动管理状态生命周期
 */
class CHTLJSStateMachine {
private:
    std::stack<CHTLJSStateContext> m_StateStack;           // 状态栈
    CHTLJSStateContext m_CurrentContext;                   // 当前上下文
    std::unordered_map<CHTLJSParseState, std::vector<CHTLJSParseState>> m_ValidTransitions; // 有效状态转换表
    std::string m_ErrorMessage;                            // 错误信息
    bool m_HasError;                                      // 是否有错误

public:
    /**
     * 构造函数
     */
    CHTLJSStateMachine();
    
    /**
     * 析构函数
     */
    ~CHTLJSStateMachine() = default;
    
    /**
     * 状态转换
     * @param newState 新状态
     * @param context 上下文信息
     * @return 是否转换成功
     */
    bool TransitionTo(CHTLJSParseState newState, const std::string& context = "");
    
    /**
     * 推入新状态（保存当前状态）
     * @param newState 新状态
     * @param context 上下文信息
     * @return 是否推入成功
     */
    bool PushState(CHTLJSParseState newState, const std::string& context = "");
    
    /**
     * 弹出状态（恢复上一状态）
     * @return 是否弹出成功
     */
    bool PopState();
    
    /**
     * 获取当前状态
     * @return 当前状态
     */
    CHTLJSParseState GetCurrentState() const;
    
    /**
     * 获取当前上下文
     * @return 当前上下文引用
     */
    const CHTLJSStateContext& GetCurrentContext() const;
    
    /**
     * 更新上下文信息
     * @param functionName CHTL JS函数名称
     * @param selectorContent 选择器内容
     * @param virObjectName 虚对象名称
     */
    void UpdateContext(const std::string& functionName = "", 
                      const std::string& selectorContent = "",
                      const std::string& virObjectName = "");
    
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
     * 进入圆括号块
     */
    void EnterParen();
    
    /**
     * 退出圆括号块
     */
    void ExitParen();
    
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
     * 进入增强选择器
     */
    void EnterSelector();
    
    /**
     * 退出增强选择器
     */
    void ExitSelector();
    
    /**
     * 检查状态转换是否有效
     * @param from 源状态
     * @param to 目标状态
     * @return 是否有效
     */
    bool IsValidTransition(CHTLJSParseState from, CHTLJSParseState to);
    
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
    std::string GetStateName(CHTLJSParseState state) const;

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
 * RAII CHTL JS状态管理器（完全独立）
 * 自动管理CHTL JS状态的推入和弹出
 */
class CHTLJSStateGuard {
private:
    CHTLJSStateMachine* m_StateMachine;
    bool m_ShouldPop;

public:
    /**
     * 构造函数 - 自动推入状态
     * @param stateMachine 状态机指针
     * @param newState 新状态
     * @param context 上下文信息
     */
    CHTLJSStateGuard(CHTLJSStateMachine* stateMachine, CHTLJSParseState newState, const std::string& context = "");
    
    /**
     * 析构函数 - 自动弹出状态
     */
    ~CHTLJSStateGuard();
    
    // 禁止拷贝和赋值
    CHTLJSStateGuard(const CHTLJSStateGuard&) = delete;
    CHTLJSStateGuard& operator=(const CHTLJSStateGuard&) = delete;
    
    // 支持移动
    CHTLJSStateGuard(CHTLJSStateGuard&& other) noexcept;
    CHTLJSStateGuard& operator=(CHTLJSStateGuard&& other) noexcept;
    
    /**
     * 手动释放状态管理
     */
    void Release();
};

} // namespace CHTLJS