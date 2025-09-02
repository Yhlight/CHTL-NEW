#include "CHTL/Core/State.h"
#include "Common/Error.h"

namespace CHTL {

StateMachine::StateMachine() : currentState(ParseState::TopLevel) {
    // 初始化时压入顶层状态
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
    // 清空栈
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    
    // 重新初始化
    currentState = ParseState::TopLevel;
    stateStack.push(StateContext(ParseState::TopLevel, "TopLevel"));
}

std::string StateMachine::GetStateString(ParseState state) {
    switch (state) {
        case ParseState::TopLevel: return "TopLevel";
        case ParseState::InTemplate: return "InTemplate";
        case ParseState::InCustom: return "InCustom";
        case ParseState::InOrigin: return "InOrigin";
        case ParseState::InImport: return "InImport";
        case ParseState::InNamespace: return "InNamespace";
        case ParseState::InConfiguration: return "InConfiguration";
        case ParseState::InElement: return "InElement";
        case ParseState::InText: return "InText";
        case ParseState::InAttribute: return "InAttribute";
        case ParseState::InLocalStyle: return "InLocalStyle";
        case ParseState::InLocalScript: return "InLocalScript";
        case ParseState::InStyleRule: return "InStyleRule";
        case ParseState::InStyleSelector: return "InStyleSelector";
        case ParseState::InStyleProperty: return "InStyleProperty";
        case ParseState::InSpecialization: return "InSpecialization";
        case ParseState::InDelete: return "InDelete";
        case ParseState::InInsert: return "InInsert";
        case ParseState::InExpression: return "InExpression";
        case ParseState::InFunctionCall: return "InFunctionCall";
        case ParseState::InString: return "InString";
        case ParseState::InComment: return "InComment";
        default: return "Unknown";
    }
}

} // namespace CHTL