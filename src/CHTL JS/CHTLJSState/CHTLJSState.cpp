#include "CHTLJSState.h"
#include <algorithm>

namespace CHTLJS {

CHTLJSStateMachine::CHTLJSStateMachine() : m_HasError(false) {
    InitializeValidTransitions();
    Reset();
}

void CHTLJSStateMachine::InitializeValidTransitions() {
    // 初始状态可以转换到的状态
    m_ValidTransitions[CHTLJSParseState::INITIAL] = {
        CHTLJSParseState::PARSING_MODULE_BLOCK,
        CHTLJSParseState::PARSING_ENHANCED_SELECTOR,
        CHTLJSParseState::PARSING_VIR_DECLARATION,
        CHTLJSParseState::PARSING_LISTEN_BLOCK,
        CHTLJSParseState::PARSING_DELEGATE_BLOCK,
        CHTLJSParseState::PARSING_ANIMATE_BLOCK,
        CHTLJSParseState::PARSING_EVENT_BIND
    };
    
    // 模块块状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_MODULE_BLOCK] = {
        CHTLJSParseState::PARSING_MODULE_LOAD,
        CHTLJSParseState::INITIAL
    };
    
    // 增强选择器状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_ENHANCED_SELECTOR] = {
        CHTLJSParseState::PARSING_SELECTOR_CONTENT,
        CHTLJSParseState::PARSING_ARROW_OPERATION,
        CHTLJSParseState::PARSING_EVENT_BIND
    };
    
    m_ValidTransitions[CHTLJSParseState::PARSING_SELECTOR_CONTENT] = {
        CHTLJSParseState::PARSING_ARROW_OPERATION,
        CHTLJSParseState::PARSING_EVENT_BIND,
        CHTLJSParseState::INITIAL
    };
    
    m_ValidTransitions[CHTLJSParseState::PARSING_ARROW_OPERATION] = {
        CHTLJSParseState::PARSING_LISTEN_BLOCK,
        CHTLJSParseState::PARSING_DELEGATE_BLOCK,
        CHTLJSParseState::INITIAL
    };
    
    // listen块状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_LISTEN_BLOCK] = {
        CHTLJSParseState::PARSING_LISTEN_EVENTS,
        CHTLJSParseState::INITIAL
    };
    
    // delegate块状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_DELEGATE_BLOCK] = {
        CHTLJSParseState::PARSING_DELEGATE_TARGET,
        CHTLJSParseState::PARSING_DELEGATE_EVENTS,
        CHTLJSParseState::INITIAL
    };
    
    // 动画块状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_ANIMATE_BLOCK] = {
        CHTLJSParseState::PARSING_ANIMATE_TARGET,
        CHTLJSParseState::PARSING_ANIMATE_DURATION,
        CHTLJSParseState::PARSING_ANIMATE_EASING,
        CHTLJSParseState::PARSING_ANIMATE_BEGIN,
        CHTLJSParseState::PARSING_ANIMATE_WHEN,
        CHTLJSParseState::PARSING_ANIMATE_END,
        CHTLJSParseState::PARSING_ANIMATE_PROPERTIES,
        CHTLJSParseState::INITIAL
    };
    
    // 虚对象状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_VIR_DECLARATION] = {
        CHTLJSParseState::PARSING_VIR_ASSIGNMENT,
        CHTLJSParseState::PARSING_VIR_FUNCTION
    };
    
    m_ValidTransitions[CHTLJSParseState::PARSING_VIR_ASSIGNMENT] = {
        CHTLJSParseState::PARSING_VIR_FUNCTION,
        CHTLJSParseState::PARSING_LISTEN_BLOCK,
        CHTLJSParseState::PARSING_DELEGATE_BLOCK,
        CHTLJSParseState::PARSING_ANIMATE_BLOCK
    };
    
    m_ValidTransitions[CHTLJSParseState::PARSING_VIR_FUNCTION] = {
        CHTLJSParseState::PARSING_VIR_KEYS,
        CHTLJSParseState::INITIAL
    };
    
    m_ValidTransitions[CHTLJSParseState::PARSING_VIR_REFERENCE] = {
        CHTLJSParseState::INITIAL
    };
    
    // 事件绑定状态转换
    m_ValidTransitions[CHTLJSParseState::PARSING_EVENT_BIND] = {
        CHTLJSParseState::PARSING_EVENT_HANDLER,
        CHTLJSParseState::INITIAL
    };
}

bool CHTLJSStateMachine::TransitionTo(CHTLJSParseState newState, const std::string& context) {
    if (!IsValidTransition(m_CurrentContext.State, newState)) {
        SetError("Invalid state transition from " + GetStateName(m_CurrentContext.State) + 
                " to " + GetStateName(newState) + " in context: " + context);
        return false;
    }
    
    m_CurrentContext.State = newState;
    
    // 根据新状态更新上下文信息
    switch (newState) {
        case CHTLJSParseState::PARSING_MODULE_BLOCK:
            m_CurrentContext.FunctionName = "module";
            break;
        case CHTLJSParseState::PARSING_LISTEN_BLOCK:
            m_CurrentContext.FunctionName = "listen";
            break;
        case CHTLJSParseState::PARSING_DELEGATE_BLOCK:
            m_CurrentContext.FunctionName = "delegate";
            break;
        case CHTLJSParseState::PARSING_ANIMATE_BLOCK:
            m_CurrentContext.FunctionName = "animate";
            m_CurrentContext.AnimationName = context;
            break;
        case CHTLJSParseState::PARSING_VIR_DECLARATION:
            m_CurrentContext.VirtualObjectName = context;
            break;
        case CHTLJSParseState::PARSING_ENHANCED_SELECTOR:
            m_CurrentContext.SelectorContent = context;
            break;
        case CHTLJSParseState::PARSING_DELEGATE_TARGET:
            m_CurrentContext.DelegateParent = context;
            break;
        default:
            break;
    }
    
    return true;
}

bool CHTLJSStateMachine::PushState(CHTLJSParseState newState, const std::string& context) {
    if (!IsValidTransition(m_CurrentContext.State, newState)) {
        SetError("Invalid state push from " + GetStateName(m_CurrentContext.State) + 
                " to " + GetStateName(newState) + " in context: " + context);
        return false;
    }
    
    // 保存当前状态
    m_StateStack.push(m_CurrentContext);
    
    // 创建新的上下文
    CHTLJSStateContext newContext = m_CurrentContext;
    newContext.State = newState;
    m_CurrentContext = newContext;
    
    return TransitionTo(newState, context);
}

bool CHTLJSStateMachine::PopState() {
    if (m_StateStack.empty()) {
        SetError("Cannot pop state: state stack is empty");
        return false;
    }
    
    m_CurrentContext = m_StateStack.top();
    m_StateStack.pop();
    return true;
}

CHTLJSParseState CHTLJSStateMachine::GetCurrentState() const {
    return m_CurrentContext.State;
}

const CHTLJSStateContext& CHTLJSStateMachine::GetCurrentContext() const {
    return m_CurrentContext;
}

void CHTLJSStateMachine::UpdateContext(const std::string& functionName, 
                                      const std::string& selectorContent,
                                      const std::string& virObjectName) {
    if (!functionName.empty()) {
        m_CurrentContext.FunctionName = functionName;
    }
    if (!selectorContent.empty()) {
        m_CurrentContext.SelectorContent = selectorContent;
    }
    if (!virObjectName.empty()) {
        m_CurrentContext.VirtualObjectName = virObjectName;
    }
}

void CHTLJSStateMachine::EnterBrace() {
    m_CurrentContext.BraceLevel++;
}

void CHTLJSStateMachine::ExitBrace() {
    if (m_CurrentContext.BraceLevel > 0) {
        m_CurrentContext.BraceLevel--;
    }
}

void CHTLJSStateMachine::EnterBracket() {
    m_CurrentContext.BracketLevel++;
}

void CHTLJSStateMachine::ExitBracket() {
    if (m_CurrentContext.BracketLevel > 0) {
        m_CurrentContext.BracketLevel--;
    }
}

void CHTLJSStateMachine::EnterParen() {
    m_CurrentContext.ParenLevel++;
}

void CHTLJSStateMachine::ExitParen() {
    if (m_CurrentContext.ParenLevel > 0) {
        m_CurrentContext.ParenLevel--;
    }
}

void CHTLJSStateMachine::EnterString(char quote) {
    m_CurrentContext.InString = true;
    m_CurrentContext.StringChar = quote;
}

void CHTLJSStateMachine::ExitString() {
    m_CurrentContext.InString = false;
    m_CurrentContext.StringChar = '\0';
}

void CHTLJSStateMachine::EnterSelector() {
    m_CurrentContext.InSelector = true;
}

void CHTLJSStateMachine::ExitSelector() {
    m_CurrentContext.InSelector = false;
}

bool CHTLJSStateMachine::IsValidTransition(CHTLJSParseState from, CHTLJSParseState to) {
    auto it = m_ValidTransitions.find(from);
    if (it != m_ValidTransitions.end()) {
        const auto& validStates = it->second;
        return std::find(validStates.begin(), validStates.end(), to) != validStates.end();
    }
    
    return false;
}

void CHTLJSStateMachine::Reset() {
    while (!m_StateStack.empty()) {
        m_StateStack.pop();
    }
    
    m_CurrentContext = CHTLJSStateContext();
    m_CurrentContext.State = CHTLJSParseState::INITIAL;
    m_HasError = false;
    m_ErrorMessage.clear();
}

bool CHTLJSStateMachine::HasError() const {
    return m_HasError;
}

std::string CHTLJSStateMachine::GetErrorMessage() const {
    return m_ErrorMessage;
}

std::string CHTLJSStateMachine::GetStateName(CHTLJSParseState state) const {
    switch (state) {
        case CHTLJSParseState::INITIAL: return "INITIAL";
        case CHTLJSParseState::PARSING_MODULE_BLOCK: return "PARSING_MODULE_BLOCK";
        case CHTLJSParseState::PARSING_MODULE_LOAD: return "PARSING_MODULE_LOAD";
        case CHTLJSParseState::PARSING_ENHANCED_SELECTOR: return "PARSING_ENHANCED_SELECTOR";
        case CHTLJSParseState::PARSING_SELECTOR_CONTENT: return "PARSING_SELECTOR_CONTENT";
        case CHTLJSParseState::PARSING_ARROW_OPERATION: return "PARSING_ARROW_OPERATION";
        case CHTLJSParseState::PARSING_LISTEN_BLOCK: return "PARSING_LISTEN_BLOCK";
        case CHTLJSParseState::PARSING_LISTEN_EVENTS: return "PARSING_LISTEN_EVENTS";
        case CHTLJSParseState::PARSING_DELEGATE_BLOCK: return "PARSING_DELEGATE_BLOCK";
        case CHTLJSParseState::PARSING_DELEGATE_TARGET: return "PARSING_DELEGATE_TARGET";
        case CHTLJSParseState::PARSING_DELEGATE_EVENTS: return "PARSING_DELEGATE_EVENTS";
        case CHTLJSParseState::PARSING_ANIMATE_BLOCK: return "PARSING_ANIMATE_BLOCK";
        case CHTLJSParseState::PARSING_ANIMATE_TARGET: return "PARSING_ANIMATE_TARGET";
        case CHTLJSParseState::PARSING_ANIMATE_DURATION: return "PARSING_ANIMATE_DURATION";
        case CHTLJSParseState::PARSING_ANIMATE_EASING: return "PARSING_ANIMATE_EASING";
        case CHTLJSParseState::PARSING_ANIMATE_BEGIN: return "PARSING_ANIMATE_BEGIN";
        case CHTLJSParseState::PARSING_ANIMATE_WHEN: return "PARSING_ANIMATE_WHEN";
        case CHTLJSParseState::PARSING_ANIMATE_END: return "PARSING_ANIMATE_END";
        case CHTLJSParseState::PARSING_ANIMATE_PROPERTIES: return "PARSING_ANIMATE_PROPERTIES";
        case CHTLJSParseState::PARSING_VIR_DECLARATION: return "PARSING_VIR_DECLARATION";
        case CHTLJSParseState::PARSING_VIR_ASSIGNMENT: return "PARSING_VIR_ASSIGNMENT";
        case CHTLJSParseState::PARSING_VIR_FUNCTION: return "PARSING_VIR_FUNCTION";
        case CHTLJSParseState::PARSING_VIR_KEYS: return "PARSING_VIR_KEYS";
        case CHTLJSParseState::PARSING_VIR_REFERENCE: return "PARSING_VIR_REFERENCE";
        case CHTLJSParseState::PARSING_EVENT_BIND: return "PARSING_EVENT_BIND";
        case CHTLJSParseState::PARSING_EVENT_HANDLER: return "PARSING_EVENT_HANDLER";
        case CHTLJSParseState::ERROR_STATE: return "ERROR_STATE";
        case CHTLJSParseState::END_STATE: return "END_STATE";
        default: return "UNKNOWN_STATE";
    }
}

void CHTLJSStateMachine::SetError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = message;
    m_CurrentContext.State = CHTLJSParseState::ERROR_STATE;
}

bool CHTLJSStateMachine::ValidateContextConsistency() {
    // 验证CHTL JS上下文一致性
    
    // 检查括号平衡
    if (m_CurrentContext.BraceLevel < 0 || 
        m_CurrentContext.BracketLevel < 0 || 
        m_CurrentContext.ParenLevel < 0) {
        SetError("Unbalanced brackets detected in CHTL JS context");
        return false;
    }
    
    // 检查字符串状态一致性
    if (m_CurrentContext.InString && m_CurrentContext.StringChar == '\0') {
        SetError("Invalid string state in CHTL JS context");
        return false;
    }
    
    // 检查选择器状态一致性
    if (m_CurrentContext.InSelector && m_CurrentContext.SelectorContent.empty()) {
        SetError("In selector state but no selector content");
        return false;
    }
    
    // 检查状态和上下文的匹配性
    switch (m_CurrentContext.State) {
        case CHTLJSParseState::PARSING_LISTEN_BLOCK:
        case CHTLJSParseState::PARSING_DELEGATE_BLOCK:
        case CHTLJSParseState::PARSING_ANIMATE_BLOCK:
            if (m_CurrentContext.FunctionName.empty()) {
                SetError("Parsing CHTL JS function but no function name in context");
                return false;
            }
            break;
        case CHTLJSParseState::PARSING_VIR_DECLARATION:
        case CHTLJSParseState::PARSING_VIR_ASSIGNMENT:
        case CHTLJSParseState::PARSING_VIR_FUNCTION:
            if (m_CurrentContext.VirtualObjectName.empty()) {
                SetError("Parsing virtual object but no object name in context");
                return false;
            }
            break;
        case CHTLJSParseState::PARSING_ENHANCED_SELECTOR:
        case CHTLJSParseState::PARSING_SELECTOR_CONTENT:
            if (m_CurrentContext.SelectorContent.empty()) {
                SetError("Parsing selector but no selector content in context");
                return false;
            }
            break;
        default:
            break;
    }
    
    return true;
}

// RAII CHTL JS状态管理器实现
CHTLJSStateGuard::CHTLJSStateGuard(CHTLJSStateMachine* stateMachine, 
                                  CHTLJSParseState newState, 
                                  const std::string& context)
    : m_StateMachine(stateMachine), m_ShouldPop(false) {
    
    if (m_StateMachine && m_StateMachine->PushState(newState, context)) {
        m_ShouldPop = true;
    }
}

CHTLJSStateGuard::~CHTLJSStateGuard() {
    if (m_ShouldPop && m_StateMachine) {
        m_StateMachine->PopState();
    }
}

CHTLJSStateGuard::CHTLJSStateGuard(CHTLJSStateGuard&& other) noexcept
    : m_StateMachine(other.m_StateMachine), m_ShouldPop(other.m_ShouldPop) {
    other.m_ShouldPop = false;
}

CHTLJSStateGuard& CHTLJSStateGuard::operator=(CHTLJSStateGuard&& other) noexcept {
    if (this != &other) {
        if (m_ShouldPop && m_StateMachine) {
            m_StateMachine->PopState();
        }
        
        m_StateMachine = other.m_StateMachine;
        m_ShouldPop = other.m_ShouldPop;
        other.m_ShouldPop = false;
    }
    return *this;
}

void CHTLJSStateGuard::Release() {
    m_ShouldPop = false;
}

} // namespace CHTLJS