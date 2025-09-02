#ifndef CHTL_CHTL_CORE_STATE_H
#define CHTL_CHTL_CORE_STATE_H

#include <stack>
#include <string>
#include <memory>

namespace CHTL {

/**
 * CHTL解析状态
 */
enum class ParseState {
    // 顶层状态
    TopLevel,              // 顶层（文件级别）
    
    // 声明状态
    InTemplate,            // 在模板声明中
    InCustom,              // 在自定义声明中
    InOrigin,              // 在原始嵌入中
    InImport,              // 在导入声明中
    InNamespace,           // 在命名空间中
    InConfiguration,       // 在配置组中
    
    // 元素状态
    InElement,             // 在元素中
    InText,                // 在文本节点中
    InAttribute,           // 在属性中
    InLocalStyle,          // 在局部样式块中
    InLocalScript,         // 在局部脚本块中
    
    // 样式状态
    InStyleRule,           // 在样式规则中
    InStyleSelector,       // 在样式选择器中
    InStyleProperty,       // 在样式属性中
    
    // 特例化状态
    InSpecialization,      // 在特例化块中
    InDelete,              // 在删除操作中
    InInsert,              // 在插入操作中
    
    // 表达式状态
    InExpression,          // 在表达式中
    InFunctionCall,        // 在函数调用中
    
    // 字符串状态
    InString,              // 在字符串中
    InComment              // 在注释中
};

/**
 * 状态上下文
 */
struct StateContext {
    ParseState state;           // 当前状态
    std::string contextName;    // 上下文名称（如元素名、模板名等）
    size_t startLine;          // 开始行号
    size_t startColumn;        // 开始列号
    int braceDepth;            // 花括号深度
    void* extraData;           // 额外数据指针
    
    StateContext(ParseState s = ParseState::TopLevel, const std::string& name = "")
        : state(s), contextName(name), startLine(0), startColumn(0), 
          braceDepth(0), extraData(nullptr) {}
};

/**
 * CHTL状态机
 * 使用RAII管理解析状态
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
     * 检查是否在任一状态中
     */
    template<typename... States>
    bool IsInAnyState(States... states) const {
        return ((currentState == states) || ...);
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

} // namespace CHTL

#endif // CHTL_CHTL_CORE_STATE_H