#include "CHTLState.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

    CHTLStateManager::CHTLStateManager() : currentState(CHTLParseState::INITIAL) {
        InitializeValidTransitions();
    }

    void CHTLStateManager::InitializeValidTransitions() {
        // 初始化有效状态转换映射表
        // 严格按照CHTL语法文档定义的语法结构
        
        // INITIAL状态可以转换到的状态
        validTransitions[CHTLParseState::INITIAL] = {
            CHTLParseState::PARSING_ELEMENT,
            CHTLParseState::PARSING_TEXT,
            CHTLParseState::PARSING_TEMPLATE,
            CHTLParseState::PARSING_CUSTOM,
            CHTLParseState::PARSING_ORIGIN,
            CHTLParseState::PARSING_IMPORT,
            CHTLParseState::PARSING_CONFIGURATION,
            CHTLParseState::PARSING_NAMESPACE,
            CHTLParseState::PARSING_USE,
            CHTLParseState::PARSING_COMMENT,
            CHTLParseState::PARSING_GLOBAL_STYLE,
            CHTLParseState::PARSING_GLOBAL_SCRIPT
        };
        
        // PARSING_ELEMENT状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_ELEMENT] = {
            CHTLParseState::PARSING_ATTRIBUTE,
            CHTLParseState::PARSING_LOCAL_STYLE,
            CHTLParseState::PARSING_LOCAL_SCRIPT,
            CHTLParseState::PARSING_TEXT,
            CHTLParseState::PARSING_ELEMENT,  // 嵌套元素
            CHTLParseState::COMPLETED
        };
        
        // PARSING_ATTRIBUTE状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_ATTRIBUTE] = {
            CHTLParseState::PARSING_ATTRIBUTE_VALUE,
            CHTLParseState::PARSING_ATTRIBUTE,  // 多个属性
            CHTLParseState::PARSING_LOCAL_STYLE,
            CHTLParseState::PARSING_LOCAL_SCRIPT,
            CHTLParseState::PARSING_TEXT,
            CHTLParseState::PARSING_ELEMENT
        };
        
        // PARSING_LOCAL_STYLE状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_LOCAL_STYLE] = {
            CHTLParseState::PARSING_STYLE_PROPERTY,
            CHTLParseState::PARSING_CSS_SELECTOR,
            CHTLParseState::PARSING_ELEMENT  // 退出样式块
        };
        
        // PARSING_CSS_SELECTOR状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_CSS_SELECTOR] = {
            CHTLParseState::PARSING_STYLE_PROPERTY,
            CHTLParseState::PARSING_LOCAL_STYLE  // 退出选择器
        };
        
        // PARSING_TEMPLATE状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_TEMPLATE] = {
            CHTLParseState::PARSING_TEMPLATE_STYLE,
            CHTLParseState::PARSING_TEMPLATE_ELEMENT,
            CHTLParseState::PARSING_TEMPLATE_VAR,
            CHTLParseState::INITIAL  // 退出模板
        };
        
        // PARSING_CUSTOM状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_CUSTOM] = {
            CHTLParseState::PARSING_CUSTOM_STYLE,
            CHTLParseState::PARSING_CUSTOM_ELEMENT,
            CHTLParseState::PARSING_CUSTOM_VAR,
            CHTLParseState::INITIAL  // 退出自定义
        };
        
        // PARSING_ORIGIN状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_ORIGIN] = {
            CHTLParseState::PARSING_ORIGIN_HTML,
            CHTLParseState::PARSING_ORIGIN_STYLE,
            CHTLParseState::PARSING_ORIGIN_JS,
            CHTLParseState::PARSING_CUSTOM_ORIGIN,
            CHTLParseState::INITIAL  // 退出原始嵌入
        };
        
        // PARSING_NAMESPACE状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_NAMESPACE] = {
            CHTLParseState::PARSING_NAMESPACE_CONTENT,
            CHTLParseState::PARSING_TEMPLATE,
            CHTLParseState::PARSING_CUSTOM,
            CHTLParseState::PARSING_ELEMENT,
            CHTLParseState::PARSING_EXCEPT,  // 全局约束
            CHTLParseState::INITIAL  // 退出命名空间
        };
        
        // PARSING_CONFIGURATION状态可以转换到的状态
        validTransitions[CHTLParseState::PARSING_CONFIGURATION] = {
            CHTLParseState::PARSING_CONFIG_ITEM,
            CHTLParseState::PARSING_NAME_CONFIG,
            CHTLParseState::PARSING_ORIGIN_TYPE_CONFIG,
            CHTLParseState::INITIAL  // 退出配置
        };
    }

    // 状态管理实现
    void CHTLStateManager::PushState(CHTLParseState newState) {
        // 验证状态转换是否有效
        if (!IsValidTransition(currentState, newState)) {
            AddError("无效的状态转换: " + GetStateString(currentState) + " -> " + GetStateString(newState));
            currentState = CHTLParseState::ERROR_STATE;
            return;
        }
        
        stateStack.push(currentState);
        currentState = newState;
    }

    void CHTLStateManager::PopState() {
        if (!stateStack.empty()) {
            currentState = stateStack.top();
            stateStack.pop();
        } else {
            currentState = CHTLParseState::INITIAL;
        }
    }

    CHTLParseState CHTLStateManager::GetCurrentState() const {
        return currentState;
    }

    CHTLParseState CHTLStateManager::GetPreviousState() const {
        return stateStack.empty() ? CHTLParseState::INITIAL : stateStack.top();
    }

    // 上下文管理实现
    void CHTLStateManager::PushContext(const CHTLContextState& context) {
        contextStack.push(currentContext);
        currentContext = context;
    }

    void CHTLStateManager::PopContext() {
        if (!contextStack.empty()) {
            currentContext = contextStack.top();
            contextStack.pop();
        } else {
            currentContext = CHTLContextState();
        }
    }

    CHTLContextState& CHTLStateManager::GetCurrentContext() {
        return currentContext;
    }

    const CHTLContextState& CHTLStateManager::GetCurrentContext() const {
        return currentContext;
    }

    // 状态验证实现
    bool CHTLStateManager::IsValidTransition(CHTLParseState fromState, CHTLParseState toState) const {
        auto it = validTransitions.find(fromState);
        if (it != validTransitions.end()) {
            const auto& validStates = it->second;
            return std::find(validStates.begin(), validStates.end(), toState) != validStates.end();
        }
        return false;
    }

    bool CHTLStateManager::CanTransitionTo(CHTLParseState targetState) const {
        return IsValidTransition(currentState, targetState);
    }

    // 错误处理实现
    void CHTLStateManager::AddError(const std::string& message) {
        errorMessages.push_back(message);
    }

    bool CHTLStateManager::HasErrors() const {
        return !errorMessages.empty();
    }

    std::vector<std::string> CHTLStateManager::GetErrors() const {
        return errorMessages;
    }

    void CHTLStateManager::ClearErrors() {
        errorMessages.clear();
    }

    // 上下文操作实现
    void CHTLStateManager::EnterBrace() {
        currentContext.braceLevel++;
    }

    void CHTLStateManager::ExitBrace() {
        if (currentContext.braceLevel > 0) {
            currentContext.braceLevel--;
        } else {
            AddError("大括号不匹配：尝试退出但当前级别为0");
        }
    }

    void CHTLStateManager::EnterBracket() {
        currentContext.bracketLevel++;
    }

    void CHTLStateManager::ExitBracket() {
        if (currentContext.bracketLevel > 0) {
            currentContext.bracketLevel--;
        } else {
            AddError("中括号不匹配：尝试退出但当前级别为0");
        }
    }

    void CHTLStateManager::EnterParen() {
        currentContext.parenLevel++;
    }

    void CHTLStateManager::ExitParen() {
        if (currentContext.parenLevel > 0) {
            currentContext.parenLevel--;
        } else {
            AddError("小括号不匹配：尝试退出但当前级别为0");
        }
    }

    void CHTLStateManager::EnterLocalStyle() {
        currentContext.inLocalStyle = true;
        PushState(CHTLParseState::PARSING_LOCAL_STYLE);
    }

    void CHTLStateManager::ExitLocalStyle() {
        currentContext.inLocalStyle = false;
        PopState();
    }

    void CHTLStateManager::EnterGlobalStyle() {
        currentContext.inGlobalStyle = true;
        PushState(CHTLParseState::PARSING_GLOBAL_STYLE);
    }

    void CHTLStateManager::ExitGlobalStyle() {
        currentContext.inGlobalStyle = false;
        PopState();
    }

    void CHTLStateManager::EnterLocalScript() {
        currentContext.inLocalScript = true;
        PushState(CHTLParseState::PARSING_LOCAL_SCRIPT);
    }

    void CHTLStateManager::ExitLocalScript() {
        currentContext.inLocalScript = false;
        PopState();
    }

    void CHTLStateManager::EnterGlobalScript() {
        currentContext.inGlobalScript = true;
        PushState(CHTLParseState::PARSING_GLOBAL_SCRIPT);
    }

    void CHTLStateManager::ExitGlobalScript() {
        currentContext.inGlobalScript = false;
        PopState();
    }

    void CHTLStateManager::EnterTemplate(const std::string& templateName) {
        currentContext.inTemplate = true;
        currentContext.currentTemplate = templateName;
        PushState(CHTLParseState::PARSING_TEMPLATE);
    }

    void CHTLStateManager::ExitTemplate() {
        currentContext.inTemplate = false;
        currentContext.currentTemplate.clear();
        PopState();
    }

    void CHTLStateManager::EnterCustom(const std::string& customName) {
        currentContext.inCustom = true;
        currentContext.currentCustom = customName;
        PushState(CHTLParseState::PARSING_CUSTOM);
    }

    void CHTLStateManager::ExitCustom() {
        currentContext.inCustom = false;
        currentContext.currentCustom.clear();
        PopState();
    }

    void CHTLStateManager::EnterOrigin(const std::string& originName) {
        currentContext.inOrigin = true;
        currentContext.currentOrigin = originName;
        PushState(CHTLParseState::PARSING_ORIGIN);
    }

    void CHTLStateManager::ExitOrigin() {
        currentContext.inOrigin = false;
        currentContext.currentOrigin.clear();
        PopState();
    }

    void CHTLStateManager::EnterNamespace(const std::string& namespaceName) {
        currentContext.inNamespace = true;
        currentContext.currentNamespace = namespaceName;
        PushState(CHTLParseState::PARSING_NAMESPACE);
    }

    void CHTLStateManager::ExitNamespace() {
        currentContext.inNamespace = false;
        currentContext.currentNamespace.clear();
        PopState();
    }

    void CHTLStateManager::EnterConfiguration(const std::string& configName) {
        currentContext.inConfiguration = true;
        currentContext.currentConfig = configName;
        PushState(CHTLParseState::PARSING_CONFIGURATION);
    }

    void CHTLStateManager::ExitConfiguration() {
        currentContext.inConfiguration = false;
        currentContext.currentConfig.clear();
        PopState();
    }

    // 工具方法实现
    void CHTLStateManager::Reset() {
        while (!stateStack.empty()) {
            stateStack.pop();
        }
        while (!contextStack.empty()) {
            contextStack.pop();
        }
        currentState = CHTLParseState::INITIAL;
        currentContext = CHTLContextState();
        errorMessages.clear();
    }

    bool CHTLStateManager::IsInValidState() const {
        return currentState != CHTLParseState::ERROR_STATE;
    }

    std::string CHTLStateManager::GetStateString(CHTLParseState state) const {
        switch (state) {
            case CHTLParseState::INITIAL: return "INITIAL";
            case CHTLParseState::PARSING_ELEMENT: return "PARSING_ELEMENT";
            case CHTLParseState::PARSING_TEXT: return "PARSING_TEXT";
            case CHTLParseState::PARSING_ATTRIBUTE: return "PARSING_ATTRIBUTE";
            case CHTLParseState::PARSING_ATTRIBUTE_VALUE: return "PARSING_ATTRIBUTE_VALUE";
            case CHTLParseState::PARSING_LOCAL_STYLE: return "PARSING_LOCAL_STYLE";
            case CHTLParseState::PARSING_GLOBAL_STYLE: return "PARSING_GLOBAL_STYLE";
            case CHTLParseState::PARSING_STYLE_PROPERTY: return "PARSING_STYLE_PROPERTY";
            case CHTLParseState::PARSING_STYLE_VALUE: return "PARSING_STYLE_VALUE";
            case CHTLParseState::PARSING_CSS_SELECTOR: return "PARSING_CSS_SELECTOR";
            case CHTLParseState::PARSING_LOCAL_SCRIPT: return "PARSING_LOCAL_SCRIPT";
            case CHTLParseState::PARSING_GLOBAL_SCRIPT: return "PARSING_GLOBAL_SCRIPT";
            case CHTLParseState::PARSING_TEMPLATE: return "PARSING_TEMPLATE";
            case CHTLParseState::PARSING_TEMPLATE_STYLE: return "PARSING_TEMPLATE_STYLE";
            case CHTLParseState::PARSING_TEMPLATE_ELEMENT: return "PARSING_TEMPLATE_ELEMENT";
            case CHTLParseState::PARSING_TEMPLATE_VAR: return "PARSING_TEMPLATE_VAR";
            case CHTLParseState::PARSING_CUSTOM: return "PARSING_CUSTOM";
            case CHTLParseState::PARSING_CUSTOM_STYLE: return "PARSING_CUSTOM_STYLE";
            case CHTLParseState::PARSING_CUSTOM_ELEMENT: return "PARSING_CUSTOM_ELEMENT";
            case CHTLParseState::PARSING_CUSTOM_VAR: return "PARSING_CUSTOM_VAR";
            case CHTLParseState::PARSING_ORIGIN: return "PARSING_ORIGIN";
            case CHTLParseState::PARSING_ORIGIN_HTML: return "PARSING_ORIGIN_HTML";
            case CHTLParseState::PARSING_ORIGIN_STYLE: return "PARSING_ORIGIN_STYLE";
            case CHTLParseState::PARSING_ORIGIN_JS: return "PARSING_ORIGIN_JS";
            case CHTLParseState::PARSING_CUSTOM_ORIGIN: return "PARSING_CUSTOM_ORIGIN";
            case CHTLParseState::PARSING_IMPORT: return "PARSING_IMPORT";
            case CHTLParseState::PARSING_IMPORT_PATH: return "PARSING_IMPORT_PATH";
            case CHTLParseState::PARSING_IMPORT_ALIAS: return "PARSING_IMPORT_ALIAS";
            case CHTLParseState::PARSING_CONFIGURATION: return "PARSING_CONFIGURATION";
            case CHTLParseState::PARSING_CONFIG_ITEM: return "PARSING_CONFIG_ITEM";
            case CHTLParseState::PARSING_NAME_CONFIG: return "PARSING_NAME_CONFIG";
            case CHTLParseState::PARSING_ORIGIN_TYPE_CONFIG: return "PARSING_ORIGIN_TYPE_CONFIG";
            case CHTLParseState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
            case CHTLParseState::PARSING_NAMESPACE_CONTENT: return "PARSING_NAMESPACE_CONTENT";
            case CHTLParseState::PARSING_INHERIT: return "PARSING_INHERIT";
            case CHTLParseState::PARSING_DELETE: return "PARSING_DELETE";
            case CHTLParseState::PARSING_INSERT: return "PARSING_INSERT";
            case CHTLParseState::PARSING_EXCEPT: return "PARSING_EXCEPT";
            case CHTLParseState::PARSING_USE: return "PARSING_USE";
            case CHTLParseState::PARSING_COMMENT: return "PARSING_COMMENT";
            case CHTLParseState::ERROR_STATE: return "ERROR_STATE";
            case CHTLParseState::COMPLETED: return "COMPLETED";
            default: return "UNKNOWN_STATE";
        }
    }

    void CHTLStateManager::PrintState() const {
        std::cout << "当前状态: " << GetStateString(currentState) << std::endl;
        std::cout << "当前元素: " << currentContext.currentElement << std::endl;
        std::cout << "当前命名空间: " << currentContext.currentNamespace << std::endl;
        std::cout << "大括号级别: " << currentContext.braceLevel << std::endl;
        std::cout << "是否在局部样式块: " << (currentContext.inLocalStyle ? "是" : "否") << std::endl;
        std::cout << "是否在模板块: " << (currentContext.inTemplate ? "是" : "否") << std::endl;
    }

    void CHTLStateManager::PrintStateStack() const {
        std::cout << "状态栈深度: " << stateStack.size() << std::endl;
        if (!stateStack.empty()) {
            std::cout << "栈顶状态: " << GetStateString(stateStack.top()) << std::endl;
        }
    }

} // namespace CHTL