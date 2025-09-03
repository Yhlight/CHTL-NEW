#include "CHTLJSState.h"
#include <iostream>
#include <algorithm>

namespace CHTLJS {

    CHTLJSStateManager::CHTLJSStateManager() : currentState(CHTLJSParseState::INITIAL) {
        InitializeValidTransitions();
    }

    void CHTLJSStateManager::InitializeValidTransitions() {
        // 初始化CHTL JS有效状态转换映射表
        validTransitions[CHTLJSParseState::INITIAL] = {
            CHTLJSParseState::PARSING_MODULE,
            CHTLJSParseState::PARSING_SELECTOR,
            CHTLJSParseState::PARSING_VIRTUAL_OBJECT,
            CHTLJSParseState::PARSING_UTIL,
            CHTLJSParseState::COMPLETED
        };
    }

    // 状态管理基础实现
    void CHTLJSStateManager::PushState(CHTLJSParseState newState) {
        if (!IsValidTransition(currentState, newState)) {
            AddError("无效的CHTL JS状态转换: " + GetStateString(currentState) + " -> " + GetStateString(newState));
            currentState = CHTLJSParseState::ERROR_STATE;
            return;
        }
        stateStack.push(currentState);
        currentState = newState;
    }

    void CHTLJSStateManager::PopState() {
        if (!stateStack.empty()) {
            currentState = stateStack.top();
            stateStack.pop();
        } else {
            currentState = CHTLJSParseState::INITIAL;
        }
    }

    CHTLJSParseState CHTLJSStateManager::GetCurrentState() const {
        return currentState;
    }

    // 其他方法的基础实现...
    void CHTLJSStateManager::Reset() {
        while (!stateStack.empty()) stateStack.pop();
        while (!contextStack.empty()) contextStack.pop();
        currentState = CHTLJSParseState::INITIAL;
        currentContext = CHTLJSContextState();
        errorMessages.clear();
    }

    bool CHTLJSStateManager::IsValidTransition(CHTLJSParseState fromState, CHTLJSParseState toState) const {
        auto it = validTransitions.find(fromState);
        if (it != validTransitions.end()) {
            const auto& validStates = it->second;
            return std::find(validStates.begin(), validStates.end(), toState) != validStates.end();
        }
        return false;
    }

    std::string CHTLJSStateManager::GetStateString(CHTLJSParseState state) const {
        switch (state) {
            case CHTLJSParseState::INITIAL: return "INITIAL";
            case CHTLJSParseState::PARSING_MODULE: return "PARSING_MODULE";
            case CHTLJSParseState::PARSING_SELECTOR: return "PARSING_SELECTOR";
            case CHTLJSParseState::PARSING_VIRTUAL_OBJECT: return "PARSING_VIRTUAL_OBJECT";
            case CHTLJSParseState::ERROR_STATE: return "ERROR_STATE";
            case CHTLJSParseState::COMPLETED: return "COMPLETED";
            default: return "UNKNOWN_STATE";
        }
    }

} // namespace CHTLJS