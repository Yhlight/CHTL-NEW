#include "CHTLJS/Core/State.h"
#include "Common/Error.h"

namespace CHTL {
namespace JS {

StateMachine::StateMachine() : currentState(ParseState::TopLevel) {
    stateStack.push(StateContext(ParseState::TopLevel, "TopLevel"));
}

void StateMachine::EnterState(ParseState newState, const std::string& contextName) {
    StateContext newContext(newState, contextName);
    stateStack.push(newContext);
    currentState = newState;
}

void StateMachine::ExitState() {
    if (stateStack.size() <= 1) {
        throw RuntimeError("状态栈下溢：无法退出顶层状态");
    }
    
    stateStack.pop();
    currentState = stateStack.top().state;
}

const StateContext& StateMachine::GetCurrentContext() const {
    if (stateStack.empty()) {
        throw RuntimeError("状态栈为空");
    }
    return stateStack.top();
}

bool StateMachine::IsInState(ParseState state) const {
    return currentState == state;
}

void StateMachine::Reset() {
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    
    currentState = ParseState::TopLevel;
    stateStack.push(StateContext(ParseState::TopLevel, "TopLevel"));
}

std::string StateMachine::GetStateString(ParseState state) {
    switch (state) {
        case ParseState::TopLevel: return "TopLevel";
        case ParseState::InModule: return "InModule";
        case ParseState::InLoad: return "InLoad";
        case ParseState::InListen: return "InListen";
        case ParseState::InDelegate: return "InDelegate";
        case ParseState::InAnimate: return "InAnimate";
        case ParseState::InVirtualObject: return "InVirtualObject";
        case ParseState::InINeverAway: return "InINeverAway";
        case ParseState::InUtil: return "InUtil";
        case ParseState::InSelector: return "InSelector";
        case ParseState::InFunction: return "InFunction";
        case ParseState::InObject: return "InObject";
        case ParseState::InArray: return "InArray";
        case ParseState::InExpression: return "InExpression";
        case ParseState::InString: return "InString";
        case ParseState::InComment: return "InComment";
        default: return "Unknown";
    }
}

} // namespace JS
} // namespace CHTL