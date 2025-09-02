#include "CHTLState.h"
#include <algorithm>

namespace CHTL {

CHTLStateMachine::CHTLStateMachine() : m_HasError(false) {
    InitializeValidTransitions();
    Reset();
}

void CHTLStateMachine::InitializeValidTransitions() {
    // 初始状态可以转换到的状态
    m_ValidTransitions[CHTLParseState::INITIAL] = {
        CHTLParseState::PARSING_ELEMENT,
        CHTLParseState::PARSING_TEMPLATE_BLOCK,
        CHTLParseState::PARSING_CUSTOM_BLOCK,
        CHTLParseState::PARSING_ORIGIN_BLOCK,
        CHTLParseState::PARSING_CONFIGURATION,
        CHTLParseState::PARSING_IMPORT_BLOCK,
        CHTLParseState::PARSING_NAMESPACE_BLOCK
    };
    
    // 解析元素状态可以转换到的状态
    m_ValidTransitions[CHTLParseState::PARSING_ELEMENT] = {
        CHTLParseState::PARSING_ATTRIBUTES,
        CHTLParseState::PARSING_TEXT,
        CHTLParseState::PARSING_STYLE_BLOCK,
        CHTLParseState::PARSING_SCRIPT_BLOCK,
        CHTLParseState::PARSING_ELEMENT, // 嵌套元素
        CHTLParseState::PARSING_EXCEPT,
        CHTLParseState::END_STATE
    };
    
    // 解析属性状态可以转换到的状态
    m_ValidTransitions[CHTLParseState::PARSING_ATTRIBUTES] = {
        CHTLParseState::PARSING_ELEMENT,
        CHTLParseState::PARSING_TEXT,
        CHTLParseState::PARSING_STYLE_BLOCK,
        CHTLParseState::PARSING_SCRIPT_BLOCK,
        CHTLParseState::END_STATE
    };
    
    // 解析样式块状态可以转换到的状态
    m_ValidTransitions[CHTLParseState::PARSING_STYLE_BLOCK] = {
        CHTLParseState::PARSING_CLASS_SELECTOR,
        CHTLParseState::PARSING_ID_SELECTOR,
        CHTLParseState::PARSING_CONTEXT_REFERENCE,
        CHTLParseState::PARSING_PSEUDO_CLASS,
        CHTLParseState::PARSING_PSEUDO_ELEMENT,
        CHTLParseState::PARSING_TEMPLATE_STYLE,
        CHTLParseState::PARSING_CUSTOM_STYLE,
        CHTLParseState::PARSING_INHERIT,
        CHTLParseState::PARSING_DELETE,
        CHTLParseState::PARSING_ELEMENT
    };
    
    // 解析脚本块状态可以转换到的状态（局部script属于CHTL）
    m_ValidTransitions[CHTLParseState::PARSING_SCRIPT_BLOCK] = {
        CHTLParseState::PARSING_TEMPLATE_VAR,  // 支持模板变量
        CHTLParseState::PARSING_CUSTOM_VAR,    // 支持自定义变量组
        CHTLParseState::PARSING_SPECIALIZATION, // 支持变量组特例化
        CHTLParseState::PARSING_IMPORT_FROM,   // 支持命名空间from
        CHTLParseState::PARSING_ELEMENT        // 返回元素解析
    };
    
    // 模板块状态转换
    m_ValidTransitions[CHTLParseState::PARSING_TEMPLATE_BLOCK] = {
        CHTLParseState::PARSING_TEMPLATE_STYLE,
        CHTLParseState::PARSING_TEMPLATE_ELEMENT,
        CHTLParseState::PARSING_TEMPLATE_VAR
    };
    
    // 自定义块状态转换
    m_ValidTransitions[CHTLParseState::PARSING_CUSTOM_BLOCK] = {
        CHTLParseState::PARSING_CUSTOM_STYLE,
        CHTLParseState::PARSING_CUSTOM_ELEMENT,
        CHTLParseState::PARSING_CUSTOM_VAR
    };
    
    // 原始嵌入块状态转换
    m_ValidTransitions[CHTLParseState::PARSING_ORIGIN_BLOCK] = {
        CHTLParseState::PARSING_ORIGIN_HTML,
        CHTLParseState::PARSING_ORIGIN_STYLE,
        CHTLParseState::PARSING_ORIGIN_JAVASCRIPT,
        CHTLParseState::PARSING_ORIGIN_CUSTOM
    };
    
    // 配置状态转换
    m_ValidTransitions[CHTLParseState::PARSING_CONFIGURATION] = {
        CHTLParseState::PARSING_NAME_CONFIG,
        CHTLParseState::PARSING_ORIGIN_TYPE_CONFIG
    };
    
    // 导入状态转换
    m_ValidTransitions[CHTLParseState::PARSING_IMPORT_BLOCK] = {
        CHTLParseState::PARSING_IMPORT_PATH,
        CHTLParseState::PARSING_IMPORT_AS,
        CHTLParseState::PARSING_IMPORT_FROM
    };
    
    // 命名空间状态转换
    m_ValidTransitions[CHTLParseState::PARSING_NAMESPACE_BLOCK] = {
        CHTLParseState::PARSING_NAMESPACE_CONTENT,
        CHTLParseState::PARSING_TEMPLATE_BLOCK,
        CHTLParseState::PARSING_CUSTOM_BLOCK,
        CHTLParseState::PARSING_EXCEPT
    };
    
    // 选择器状态转换
    m_ValidTransitions[CHTLParseState::PARSING_CLASS_SELECTOR] = {
        CHTLParseState::PARSING_STYLE_BLOCK,
        CHTLParseState::PARSING_PSEUDO_CLASS,
        CHTLParseState::PARSING_PSEUDO_ELEMENT
    };
    
    m_ValidTransitions[CHTLParseState::PARSING_ID_SELECTOR] = {
        CHTLParseState::PARSING_STYLE_BLOCK,
        CHTLParseState::PARSING_PSEUDO_CLASS,
        CHTLParseState::PARSING_PSEUDO_ELEMENT
    };
    
    // 上下文引用状态转换
    m_ValidTransitions[CHTLParseState::PARSING_CONTEXT_REFERENCE] = {
        CHTLParseState::PARSING_PSEUDO_CLASS,
        CHTLParseState::PARSING_PSEUDO_ELEMENT,
        CHTLParseState::PARSING_STYLE_BLOCK
    };
    
    // 特殊操作状态转换
    m_ValidTransitions[CHTLParseState::PARSING_INHERIT] = {
        CHTLParseState::PARSING_TEMPLATE_STYLE,
        CHTLParseState::PARSING_TEMPLATE_ELEMENT,
        CHTLParseState::PARSING_TEMPLATE_VAR,
        CHTLParseState::PARSING_CUSTOM_STYLE,
        CHTLParseState::PARSING_CUSTOM_ELEMENT,
        CHTLParseState::PARSING_CUSTOM_VAR
    };
    
    m_ValidTransitions[CHTLParseState::PARSING_DELETE] = {
        CHTLParseState::PARSING_ATTRIBUTES,
        CHTLParseState::PARSING_INHERIT,
        CHTLParseState::PARSING_ELEMENT,
        CHTLParseState::PARSING_TEMPLATE_STYLE,
        CHTLParseState::PARSING_CUSTOM_STYLE
    };
    
    m_ValidTransitions[CHTLParseState::PARSING_INSERT] = {
        CHTLParseState::PARSING_ELEMENT,
        CHTLParseState::PARSING_TEMPLATE_ELEMENT,
        CHTLParseState::PARSING_CUSTOM_ELEMENT
    };
}

bool CHTLStateMachine::TransitionTo(CHTLParseState newState, const std::string& context) {
    if (!IsValidTransition(m_CurrentContext.State, newState)) {
        SetError("Invalid state transition from " + GetStateName(m_CurrentContext.State) + 
                " to " + GetStateName(newState) + " in context: " + context);
        return false;
    }
    
    m_CurrentContext.State = newState;
    
    // 根据新状态更新上下文信息
    switch (newState) {
        case CHTLParseState::PARSING_ELEMENT:
            m_CurrentContext.ElementName = context;
            break;
        case CHTLParseState::PARSING_TEMPLATE_BLOCK:
        case CHTLParseState::PARSING_TEMPLATE_STYLE:
        case CHTLParseState::PARSING_TEMPLATE_ELEMENT:
        case CHTLParseState::PARSING_TEMPLATE_VAR:
            m_CurrentContext.TemplateName = context;
            m_CurrentContext.BlockType = "Template";
            break;
        case CHTLParseState::PARSING_CUSTOM_BLOCK:
        case CHTLParseState::PARSING_CUSTOM_STYLE:
        case CHTLParseState::PARSING_CUSTOM_ELEMENT:
        case CHTLParseState::PARSING_CUSTOM_VAR:
            m_CurrentContext.CustomName = context;
            m_CurrentContext.BlockType = "Custom";
            break;
        case CHTLParseState::PARSING_NAMESPACE_BLOCK:
            m_CurrentContext.NamespaceName = context;
            break;
        default:
            break;
    }
    
    return true;
}

bool CHTLStateMachine::PushState(CHTLParseState newState, const std::string& context) {
    if (!IsValidTransition(m_CurrentContext.State, newState)) {
        SetError("Invalid state push from " + GetStateName(m_CurrentContext.State) + 
                " to " + GetStateName(newState) + " in context: " + context);
        return false;
    }
    
    // 保存当前状态
    m_StateStack.push(m_CurrentContext);
    
    // 创建新的上下文
    StateContext newContext = m_CurrentContext;
    newContext.State = newState;
    m_CurrentContext = newContext;
    
    return TransitionTo(newState, context);
}

bool CHTLStateMachine::PopState() {
    if (m_StateStack.empty()) {
        SetError("Cannot pop state: state stack is empty");
        return false;
    }
    
    m_CurrentContext = m_StateStack.top();
    m_StateStack.pop();
    return true;
}

CHTLParseState CHTLStateMachine::GetCurrentState() const {
    return m_CurrentContext.State;
}

const StateContext& CHTLStateMachine::GetCurrentContext() const {
    return m_CurrentContext;
}

void CHTLStateMachine::UpdateContext(const std::string& elementName, 
                                   const std::string& blockType,
                                   const std::string& templateName) {
    if (!elementName.empty()) {
        m_CurrentContext.ElementName = elementName;
    }
    if (!blockType.empty()) {
        m_CurrentContext.BlockType = blockType;
    }
    if (!templateName.empty()) {
        m_CurrentContext.TemplateName = templateName;
    }
}

void CHTLStateMachine::EnterBrace() {
    m_CurrentContext.BraceLevel++;
}

void CHTLStateMachine::ExitBrace() {
    if (m_CurrentContext.BraceLevel > 0) {
        m_CurrentContext.BraceLevel--;
    }
}

void CHTLStateMachine::EnterBracket() {
    m_CurrentContext.BracketLevel++;
}

void CHTLStateMachine::ExitBracket() {
    if (m_CurrentContext.BracketLevel > 0) {
        m_CurrentContext.BracketLevel--;
    }
}

void CHTLStateMachine::EnterString(char quote) {
    m_CurrentContext.InString = true;
    m_CurrentContext.StringChar = quote;
}

void CHTLStateMachine::ExitString() {
    m_CurrentContext.InString = false;
    m_CurrentContext.StringChar = '\0';
}

void CHTLStateMachine::EnterComment() {
    m_CurrentContext.InComment = true;
}

void CHTLStateMachine::ExitComment() {
    m_CurrentContext.InComment = false;
}

bool CHTLStateMachine::IsValidTransition(CHTLParseState from, CHTLParseState to) {
    auto it = m_ValidTransitions.find(from);
    if (it != m_ValidTransitions.end()) {
        const auto& validStates = it->second;
        return std::find(validStates.begin(), validStates.end(), to) != validStates.end();
    }
    
    return false;
}

void CHTLStateMachine::Reset() {
    while (!m_StateStack.empty()) {
        m_StateStack.pop();
    }
    
    m_CurrentContext = StateContext();
    m_CurrentContext.State = CHTLParseState::INITIAL;
    m_HasError = false;
    m_ErrorMessage.clear();
}

bool CHTLStateMachine::HasError() const {
    return m_HasError;
}

std::string CHTLStateMachine::GetErrorMessage() const {
    return m_ErrorMessage;
}

std::string CHTLStateMachine::GetStateName(CHTLParseState state) const {
    switch (state) {
        case CHTLParseState::INITIAL: return "INITIAL";
        case CHTLParseState::PARSING_ELEMENT: return "PARSING_ELEMENT";
        case CHTLParseState::PARSING_TEXT: return "PARSING_TEXT";
        case CHTLParseState::PARSING_ATTRIBUTES: return "PARSING_ATTRIBUTES";
        case CHTLParseState::PARSING_STYLE_BLOCK: return "PARSING_STYLE_BLOCK";
        case CHTLParseState::PARSING_SCRIPT_BLOCK: return "PARSING_SCRIPT_BLOCK";
        case CHTLParseState::PARSING_TEMPLATE_BLOCK: return "PARSING_TEMPLATE_BLOCK";
        case CHTLParseState::PARSING_TEMPLATE_STYLE: return "PARSING_TEMPLATE_STYLE";
        case CHTLParseState::PARSING_TEMPLATE_ELEMENT: return "PARSING_TEMPLATE_ELEMENT";
        case CHTLParseState::PARSING_TEMPLATE_VAR: return "PARSING_TEMPLATE_VAR";
        case CHTLParseState::PARSING_CUSTOM_BLOCK: return "PARSING_CUSTOM_BLOCK";
        case CHTLParseState::PARSING_CUSTOM_STYLE: return "PARSING_CUSTOM_STYLE";
        case CHTLParseState::PARSING_CUSTOM_ELEMENT: return "PARSING_CUSTOM_ELEMENT";
        case CHTLParseState::PARSING_CUSTOM_VAR: return "PARSING_CUSTOM_VAR";
        case CHTLParseState::PARSING_ORIGIN_BLOCK: return "PARSING_ORIGIN_BLOCK";
        case CHTLParseState::PARSING_ORIGIN_HTML: return "PARSING_ORIGIN_HTML";
        case CHTLParseState::PARSING_ORIGIN_STYLE: return "PARSING_ORIGIN_STYLE";
        case CHTLParseState::PARSING_ORIGIN_JAVASCRIPT: return "PARSING_ORIGIN_JAVASCRIPT";
        case CHTLParseState::PARSING_ORIGIN_CUSTOM: return "PARSING_ORIGIN_CUSTOM";
        case CHTLParseState::PARSING_CONFIGURATION: return "PARSING_CONFIGURATION";
        case CHTLParseState::PARSING_NAME_CONFIG: return "PARSING_NAME_CONFIG";
        case CHTLParseState::PARSING_ORIGIN_TYPE_CONFIG: return "PARSING_ORIGIN_TYPE_CONFIG";
        case CHTLParseState::PARSING_IMPORT_BLOCK: return "PARSING_IMPORT_BLOCK";
        case CHTLParseState::PARSING_IMPORT_PATH: return "PARSING_IMPORT_PATH";
        case CHTLParseState::PARSING_IMPORT_AS: return "PARSING_IMPORT_AS";
        case CHTLParseState::PARSING_IMPORT_FROM: return "PARSING_IMPORT_FROM";
        case CHTLParseState::PARSING_NAMESPACE_BLOCK: return "PARSING_NAMESPACE_BLOCK";
        case CHTLParseState::PARSING_NAMESPACE_CONTENT: return "PARSING_NAMESPACE_CONTENT";
        case CHTLParseState::PARSING_INHERIT: return "PARSING_INHERIT";
        case CHTLParseState::PARSING_DELETE: return "PARSING_DELETE";
        case CHTLParseState::PARSING_INSERT: return "PARSING_INSERT";
        case CHTLParseState::PARSING_EXCEPT: return "PARSING_EXCEPT";
        case CHTLParseState::PARSING_CLASS_SELECTOR: return "PARSING_CLASS_SELECTOR";
        case CHTLParseState::PARSING_ID_SELECTOR: return "PARSING_ID_SELECTOR";
        case CHTLParseState::PARSING_CONTEXT_REFERENCE: return "PARSING_CONTEXT_REFERENCE";
        case CHTLParseState::PARSING_PSEUDO_CLASS: return "PARSING_PSEUDO_CLASS";
        case CHTLParseState::PARSING_PSEUDO_ELEMENT: return "PARSING_PSEUDO_ELEMENT";
        case CHTLParseState::PARSING_INDEX_ACCESS: return "PARSING_INDEX_ACCESS";
        case CHTLParseState::PARSING_SPECIALIZATION: return "PARSING_SPECIALIZATION";
        case CHTLParseState::ERROR_STATE: return "ERROR_STATE";
        case CHTLParseState::END_STATE: return "END_STATE";
        default: return "UNKNOWN_STATE";
    }
}

void CHTLStateMachine::SetError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = message;
    m_CurrentContext.State = CHTLParseState::ERROR_STATE;
}

bool CHTLStateMachine::ValidateContextConsistency() {
    // 验证上下文一致性
    
    // 检查括号平衡
    if (m_CurrentContext.BraceLevel < 0 || m_CurrentContext.BracketLevel < 0) {
        SetError("Unbalanced brackets detected");
        return false;
    }
    
    // 检查字符串状态一致性
    if (m_CurrentContext.InString && m_CurrentContext.StringChar == '\0') {
        SetError("Invalid string state: in string but no quote character");
        return false;
    }
    
    // 检查状态和上下文的匹配性
    switch (m_CurrentContext.State) {
        case CHTLParseState::PARSING_ELEMENT:
            if (m_CurrentContext.ElementName.empty()) {
                SetError("Parsing element but no element name in context");
                return false;
            }
            break;
        case CHTLParseState::PARSING_TEMPLATE_STYLE:
        case CHTLParseState::PARSING_TEMPLATE_ELEMENT:
        case CHTLParseState::PARSING_TEMPLATE_VAR:
            if (m_CurrentContext.TemplateName.empty()) {
                SetError("Parsing template but no template name in context");
                return false;
            }
            break;
        case CHTLParseState::PARSING_CUSTOM_STYLE:
        case CHTLParseState::PARSING_CUSTOM_ELEMENT:
        case CHTLParseState::PARSING_CUSTOM_VAR:
            if (m_CurrentContext.CustomName.empty()) {
                SetError("Parsing custom but no custom name in context");
                return false;
            }
            break;
        case CHTLParseState::PARSING_NAMESPACE_BLOCK:
            if (m_CurrentContext.NamespaceName.empty()) {
                SetError("Parsing namespace but no namespace name in context");
                return false;
            }
            break;
        default:
            break;
    }
    
    return true;
}

// RAII状态管理器实现
CHTLStateGuard::CHTLStateGuard(CHTLStateMachine* stateMachine, CHTLParseState newState, const std::string& context)
    : m_StateMachine(stateMachine), m_ShouldPop(false) {
    
    if (m_StateMachine && m_StateMachine->PushState(newState, context)) {
        m_ShouldPop = true;
    }
}

CHTLStateGuard::~CHTLStateGuard() {
    if (m_ShouldPop && m_StateMachine) {
        m_StateMachine->PopState();
    }
}

CHTLStateGuard::CHTLStateGuard(CHTLStateGuard&& other) noexcept
    : m_StateMachine(other.m_StateMachine), m_ShouldPop(other.m_ShouldPop) {
    other.m_ShouldPop = false;
}

CHTLStateGuard& CHTLStateGuard::operator=(CHTLStateGuard&& other) noexcept {
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

void CHTLStateGuard::Release() {
    m_ShouldPop = false;
}

} // namespace CHTL