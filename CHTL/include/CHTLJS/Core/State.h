#ifndef CHTL_CHTLJS_CORE_STATE_H
#define CHTL_CHTLJS_CORE_STATE_H

#include <stack>
#include <string>
#include <memory>

namespace CHTL {
namespace JS {

/**
 * CHTL JS解析状态
 */
enum class ParseState {
    // 顶层状态
    TopLevel,              // 顶层
    
    // 模块状态
    InModule,              // 在module块中
    InLoad,                // 在load语句中
    
    // CHTL JS特有状态
    InListen,              // 在listen块中
    InDelegate,            // 在delegate块中
    InAnimate,             // 在animate块中
    InVirtualObject,       // 在虚对象定义中
    InINeverAway,          // 在iNeverAway块中
    InUtil,                // 在util表达式中
    
    // 选择器状态
    InSelector,            // 在{{选择器}}中
    
    // JavaScript状态
    InFunction,            // 在函数中
    InObject,              // 在对象字面量中
    InArray,               // 在数组中
    InExpression,          // 在表达式中
    
    // 字符串和注释
    InString,              // 在字符串中
    InComment              // 在注释中
};

/**
 * 状态上下文
 */
struct StateContext {
    ParseState state;           // 当前状态
    std::string contextName;    // 上下文名称
    size_t startLine;          // 开始行号
    size_t startColumn;        // 开始列号
    int braceDepth;            // 花括号深度
    void* extraData;           // 额外数据指针
    
    StateContext(ParseState s = ParseState::TopLevel, const std::string& name = "")
        : state(s), contextName(name), startLine(0), startColumn(0), 
          braceDepth(0), extraData(nullptr) {}
};

/**
 * CHTL JS状态机
 */
class StateMachine {
private:
    std::stack<StateContext> stateStack;
    ParseState currentState;
    
public:
    StateMachine();
    ~StateMachine() = default;
    
    /**
     * 进入新状态
     */
    void EnterState(ParseState newState, const std::string& contextName = "");
    
    /**
     * 退出当前状态
     */
    void ExitState();
    
    /**
     * 获取当前状态
     */
    ParseState GetCurrentState() const { return currentState; }
    
    /**
     * 获取当前状态上下文
     */
    const StateContext& GetCurrentContext() const;
    
    /**
     * 检查是否在特定状态中
     */
    bool IsInState(ParseState state) const;
    
    /**
     * 检查是否在CHTL JS特有状态中
     */
    bool IsInCHTLJSState() const {
        return currentState >= ParseState::InModule && 
               currentState <= ParseState::InSelector;
    }
    
    /**
     * 获取状态栈深度
     */
    size_t GetStackDepth() const { return stateStack.size(); }
    
    /**
     * 重置状态机
     */
    void Reset();
    
    /**
     * 获取状态字符串表示
     */
    static std::string GetStateString(ParseState state);
    
    /**
     * RAII状态管理器
     */
    class StateGuard {
    private:
        StateMachine& machine;
        bool exited;
        
    public:
        StateGuard(StateMachine& m, ParseState state, const std::string& context = "")
            : machine(m), exited(false) {
            machine.EnterState(state, context);
        }
        
        ~StateGuard() {
            if (!exited) {
                machine.ExitState();
            }
        }
        
        void Exit() {
            if (!exited) {
                machine.ExitState();
                exited = true;
            }
        }
        
        // 禁止拷贝
        StateGuard(const StateGuard&) = delete;
        StateGuard& operator=(const StateGuard&) = delete;
        
        // 允许移动
        StateGuard(StateGuard&& other) noexcept 
            : machine(other.machine), exited(other.exited) {
            other.exited = true;
        }
    };
};

} // namespace JS
} // namespace CHTL

#endif // CHTL_CHTLJS_CORE_STATE_H