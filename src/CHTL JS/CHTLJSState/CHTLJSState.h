#pragma once

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTLJS {

    // CHTL JS解析状态枚举
    // 严格按照CHTL语法文档中的CHTL JS部分定义
    // 完全独立于CHTL状态系统
    enum class CHTLJSParseState {
        INITIAL,                        // 初始状态
        
        // 模块相关状态
        PARSING_MODULE,                 // 解析module块
        PARSING_MODULE_LOAD,            // 解析load语句
        PARSING_LOAD_PATH,              // 解析加载路径
        
        // 选择器相关状态
        PARSING_SELECTOR,               // 解析{{选择器}}
        PARSING_SELECTOR_CONTENT,       // 解析选择器内容
        PARSING_SELECTOR_INDEX,         // 解析选择器索引[index]
        
        // 事件监听相关状态
        PARSING_LISTEN,                 // 解析listen块
        PARSING_LISTEN_EVENT,           // 解析事件名
        PARSING_LISTEN_HANDLER,         // 解析事件处理器
        
        // 事件委托相关状态
        PARSING_DELEGATE,               // 解析delegate块
        PARSING_DELEGATE_TARGET,        // 解析委托目标
        PARSING_DELEGATE_EVENT,         // 解析委托事件
        
        // 动画相关状态
        PARSING_ANIMATE,                // 解析animate块
        PARSING_ANIMATE_TARGET,         // 解析动画目标
        PARSING_ANIMATE_DURATION,       // 解析动画持续时间
        PARSING_ANIMATE_EASING,         // 解析缓动函数
        PARSING_ANIMATE_BEGIN,          // 解析起始状态
        PARSING_ANIMATE_WHEN,           // 解析when数组
        PARSING_ANIMATE_END,            // 解析结束状态
        PARSING_ANIMATE_PROPERTIES,     // 解析动画属性
        
        // 虚对象相关状态
        PARSING_VIRTUAL_OBJECT,         // 解析vir虚对象
        PARSING_VIR_ASSIGNMENT,         // 解析vir赋值
        PARSING_VIR_FUNCTION,           // 解析vir函数体
        PARSING_VIR_KEY,                // 解析vir键
        PARSING_VIR_VALUE,              // 解析vir值
        
        // CHTL JS函数相关状态
        PARSING_CHTLJS_FUNCTION,        // 解析CHTL JS函数
        PARSING_FUNCTION_PARAMS,        // 解析函数参数
        PARSING_FUNCTION_BODY,          // 解析函数体
        
        // util...then表达式状态
        PARSING_UTIL,                   // 解析util关键字
        PARSING_UTIL_CONDITION,        // 解析util条件
        PARSING_UTIL_CHANGE,           // 解析change部分
        PARSING_UTIL_THEN,             // 解析then部分
        
        // 事件绑定操作符状态
        PARSING_EVENT_BINDING,          // 解析&->事件绑定
        PARSING_EVENT_NAME,             // 解析事件名称
        PARSING_EVENT_HANDLER,          // 解析事件处理器
        
        // 链式访问状态
        PARSING_CHAIN_ACCESS,           // 解析->链式访问
        PARSING_METHOD_CALL,            // 解析方法调用
        PARSING_PROPERTY_ACCESS,        // 解析属性访问
        
        // 表达式状态
        PARSING_EXPRESSION,             // 解析表达式
        PARSING_ASSIGNMENT,             // 解析赋值
        PARSING_FUNCTION_CALL,          // 解析函数调用
        
        // 注释状态
        PARSING_COMMENT,                // 解析注释
        
        // 错误状态
        ERROR_STATE,                    // 错误状态
        
        // 完成状态
        COMPLETED                       // 解析完成
    };

    // CHTL JS上下文状态
    // 用于跟踪CHTL JS解析的上下文信息
    struct CHTLJSContextState {
        std::string currentModule;          // 当前模块名
        std::string currentSelector;        // 当前选择器
        std::string currentVirtualObject;   // 当前虚对象名
        std::string currentFunction;        // 当前函数名
        std::string currentEvent;           // 当前事件名
        
        int braceLevel;                     // 大括号嵌套级别
        int bracketLevel;                   // 中括号嵌套级别
        int parenLevel;                     // 小括号嵌套级别
        
        bool inModule;                      // 是否在模块块中
        bool inSelector;                    // 是否在选择器中
        bool inListen;                      // 是否在listen块中
        bool inDelegate;                    // 是否在delegate块中
        bool inAnimate;                     // 是否在animate块中
        bool inVirtualObject;               // 是否在虚对象中
        bool inUtilExpression;              // 是否在util表达式中
        bool inEventBinding;                // 是否在事件绑定中
        bool inChainAccess;                 // 是否在链式访问中
        
        CHTLJSContextState() : braceLevel(0), bracketLevel(0), parenLevel(0),
                              inModule(false), inSelector(false), inListen(false),
                              inDelegate(false), inAnimate(false), inVirtualObject(false),
                              inUtilExpression(false), inEventBinding(false), inChainAccess(false) {}
    };

    // CHTL JS状态管理器
    // 基于RAII自动化管理模式的状态机
    // 完全独立于CHTL状态管理器
    class CHTLJSStateManager {
    private:
        std::stack<CHTLJSParseState> stateStack;            // 状态栈
        std::stack<CHTLJSContextState> contextStack;        // 上下文栈
        CHTLJSParseState currentState;                      // 当前状态
        CHTLJSContextState currentContext;                  // 当前上下文
        
        std::vector<std::string> errorMessages;             // 错误消息列表
        
        // 状态转换映射表
        std::unordered_map<CHTLJSParseState, std::vector<CHTLJSParseState>> validTransitions;
        
        void InitializeValidTransitions();                  // 初始化有效状态转换

    public:
        CHTLJSStateManager();
        ~CHTLJSStateManager() = default;

        // 状态管理
        void PushState(CHTLJSParseState newState);          // 压入新状态
        void PopState();                                    // 弹出状态
        CHTLJSParseState GetCurrentState() const;          // 获取当前状态
        CHTLJSParseState GetPreviousState() const;         // 获取上一个状态
        
        // 上下文管理
        void PushContext(const CHTLJSContextState& context); // 压入上下文
        void PopContext();                                  // 弹出上下文
        CHTLJSContextState& GetCurrentContext();           // 获取当前上下文
        const CHTLJSContextState& GetCurrentContext() const; // 获取当前上下文（常量版本）
        
        // 状态验证
        bool IsValidTransition(CHTLJSParseState fromState, CHTLJSParseState toState) const;  // 验证状态转换是否有效
        bool CanTransitionTo(CHTLJSParseState targetState) const;                           // 检查是否可以转换到目标状态
        
        // 错误处理
        void AddError(const std::string& message);          // 添加错误消息
        bool HasErrors() const;                             // 检查是否有错误
        std::vector<std::string> GetErrors() const;         // 获取所有错误消息
        void ClearErrors();                                 // 清空错误消息
        
        // 上下文操作
        void EnterBrace();                                  // 进入大括号
        void ExitBrace();                                   // 退出大括号
        void EnterBracket();                                // 进入中括号
        void ExitBracket();                                 // 退出中括号
        void EnterParen();                                  // 进入小括号
        void ExitParen();                                   // 退出小括号
        
        // CHTL JS特有上下文操作
        void EnterModule(const std::string& moduleName);    // 进入模块块
        void ExitModule();                                  // 退出模块块
        
        void EnterSelector(const std::string& selectorText); // 进入选择器
        void ExitSelector();                                // 退出选择器
        
        void EnterListen();                                 // 进入listen块
        void ExitListen();                                  // 退出listen块
        
        void EnterDelegate();                               // 进入delegate块
        void ExitDelegate();                                // 退出delegate块
        
        void EnterAnimate();                                // 进入animate块
        void ExitAnimate();                                 // 退出animate块
        
        void EnterVirtualObject(const std::string& objName); // 进入虚对象
        void ExitVirtualObject();                           // 退出虚对象
        
        void EnterUtilExpression();                         // 进入util表达式
        void ExitUtilExpression();                          // 退出util表达式
        
        void EnterEventBinding();                           // 进入事件绑定
        void ExitEventBinding();                            // 退出事件绑定
        
        void EnterChainAccess();                            // 进入链式访问
        void ExitChainAccess();                             // 退出链式访问
        
        // 工具方法
        void Reset();                                       // 重置状态管理器
        bool IsInValidState() const;                        // 检查是否处于有效状态
        std::string GetStateString(CHTLJSParseState state) const;  // 获取状态字符串表示
        
        // 调试方法
        void PrintState() const;                            // 打印当前状态（调试用）
        void PrintStateStack() const;                      // 打印状态栈（调试用）
    };

} // namespace CHTLJS