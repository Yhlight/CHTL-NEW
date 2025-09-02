#pragma once

#include <vector>
#include <stdexcept> // For std::runtime_error

namespace CHTL_JS
{
    // 定义 CHTL JS 编译器在解析过程中的所有可能状态
    enum class CHTLJSCompileState
    {
        GLOBAL_SCOPE,          // 全局作用域
        MODULE_BLOCK,          // 在 module {} 块内
        LISTEN_BLOCK,          // 在 listen {} 块内
        DELEGATE_BLOCK,        // 在 delegate {} 块内
        ANIMATE_BLOCK,         // 在 animate {} 块内
        CSS_SELECTOR,          // 在 {{...}} 内
    };

    // CHTLJSState 类用于管理一个状态栈，以跟踪当前的解析上下文
    class CHTLJSState
    {
    public:
        CHTLJSState();

        void PushState(CHTLJSCompileState state);
        void PopState();
        CHTLJSCompileState GetCurrentState() const;
        bool IsInState(CHTLJSCompileState state) const;

    private:
        std::vector<CHTLJSCompileState> m_stateStack;
    };
}