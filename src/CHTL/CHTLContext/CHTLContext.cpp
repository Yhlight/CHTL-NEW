#include "CHTLContext.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLContextManager::CHTLContextManager() {
    CreateDefaultGlobalContext();
}

void CHTLContextManager::CreateDefaultGlobalContext() {
    m_CurrentContext = std::make_unique<ContextInfo>(
        CHTLContextType::GLOBAL_CONTEXT, 
        CHTLScopeType::FILE_SCOPE, 
        "global"
    );
}

bool CHTLContextManager::EnterContext(CHTLContextType type, CHTLScopeType scope, const std::string& name) {
    // 验证上下文转换的有效性
    if (m_CurrentContext && !IsValidContextTransition(m_CurrentContext->Type, type)) {
        return false;
    }
    
    // 保存当前上下文
    if (m_CurrentContext) {
        m_ContextStack.push(m_CurrentContext.release());
    }
    
    // 创建新上下文
    auto newContext = std::make_unique<ContextInfo>(type, scope, name);
    
    // 设置父上下文名称
    if (!m_ContextStack.empty()) {
        newContext->ParentName = m_ContextStack.top()->Name;
        
        // 继承父上下文信息
        InheritFromParentContext(m_ContextStack.top(), newContext.get());
    }
    
    // 应用作用域规则
    ApplyScopeRules(newContext.get());
    
    m_CurrentContext = std::move(newContext);
    return true;
}

bool CHTLContextManager::ExitContext() {
    if (m_ContextStack.empty()) {
        return false; // 无法退出全局上下文
    }
    
    m_CurrentContext.reset(m_ContextStack.top());
    m_ContextStack.pop();
    return true;
}

ContextInfo* CHTLContextManager::GetCurrentContext() {
    return m_CurrentContext.get();
}

CHTLContextType CHTLContextManager::GetCurrentContextType() const {
    return m_CurrentContext ? m_CurrentContext->Type : CHTLContextType::GLOBAL_CONTEXT;
}

CHTLScopeType CHTLContextManager::GetCurrentScopeType() const {
    return m_CurrentContext ? m_CurrentContext->Scope : CHTLScopeType::FILE_SCOPE;
}

void CHTLContextManager::SetVariable(const std::string& name, const std::string& value, bool isGlobal) {
    if (isGlobal) {
        m_GlobalVariables[name] = value;
    }
    else if (m_CurrentContext) {
        m_CurrentContext->Variables[name] = value;
    }
}

std::string CHTLContextManager::GetVariable(const std::string& name) {
    // 首先在当前上下文查找
    if (m_CurrentContext) {
        auto it = m_CurrentContext->Variables.find(name);
        if (it != m_CurrentContext->Variables.end()) {
            return it->second;
        }
    }
    
    // 在作用域链中查找
    std::string value = FindInScopeChain(name);
    if (!value.empty()) {
        return value;
    }
    
    // 在全局变量中查找
    auto globalIt = m_GlobalVariables.find(name);
    if (globalIt != m_GlobalVariables.end()) {
        return globalIt->second;
    }
    
    return ""; // 未找到
}

bool CHTLContextManager::HasVariable(const std::string& name) {
    return !GetVariable(name).empty();
}

void CHTLContextManager::SetProperty(const std::string& name, const std::string& value) {
    if (m_CurrentContext) {
        m_CurrentContext->Properties[name] = value;
    }
}

std::string CHTLContextManager::GetProperty(const std::string& name) {
    if (m_CurrentContext) {
        auto it = m_CurrentContext->Properties.find(name);
        if (it != m_CurrentContext->Properties.end()) {
            return it->second;
        }
    }
    
    return "";
}

bool CHTLContextManager::HasProperty(const std::string& name) {
    return !GetProperty(name).empty();
}

void CHTLContextManager::AddAutoClass(const std::string& className) {
    if (m_CurrentContext) {
        m_CurrentContext->ClassNames.push_back(className);
        m_CurrentContext->HasAutoClass = true;
        
        // 添加到全局映射
        std::string contextPath = GetFullContextPath();
        m_AutoClassMap[contextPath] = className;
    }
}

void CHTLContextManager::AddAutoId(const std::string& idName) {
    if (m_CurrentContext) {
        m_CurrentContext->IdNames.push_back(idName);
        m_CurrentContext->HasAutoId = true;
        
        // 添加到全局映射
        std::string contextPath = GetFullContextPath();
        m_AutoIdMap[contextPath] = idName;
    }
}

std::string CHTLContextManager::GetFirstAutoClass() {
    if (m_CurrentContext && !m_CurrentContext->ClassNames.empty()) {
        return m_CurrentContext->ClassNames[0];
    }
    
    return "";
}

std::string CHTLContextManager::GetFirstAutoId() {
    if (m_CurrentContext && !m_CurrentContext->IdNames.empty()) {
        return m_CurrentContext->IdNames[0];
    }
    
    return "";
}

ReferenceResolution CHTLContextManager::ResolveContextReference(const std::string& referenceType) {
    if (!m_CurrentContext) {
        return ReferenceResolution();
    }
    
    // 根据上下文类型决定引用优先级
    if (m_CurrentContext->Type == CHTLContextType::STYLE_CONTEXT) {
        // 局部style优先选择class
        if (referenceType.empty() || referenceType == "class") {
            std::string className = GetFirstAutoClass();
            if (!className.empty()) {
                return ReferenceResolution(className, "class");
            }
            
            // 如果没有自动类名，尝试ID
            std::string idName = GetFirstAutoId();
            if (!idName.empty()) {
                return ReferenceResolution(idName, "id");
            }
        }
        else if (referenceType == "id") {
            std::string idName = GetFirstAutoId();
            if (!idName.empty()) {
                return ReferenceResolution(idName, "id");
            }
        }
    }
    else if (m_CurrentContext->Type == CHTLContextType::SCRIPT_CONTEXT) {
        // 局部script优先选择id
        if (referenceType.empty() || referenceType == "id") {
            std::string idName = GetFirstAutoId();
            if (!idName.empty()) {
                return ReferenceResolution(idName, "id");
            }
            
            // 如果没有自动ID，尝试class
            std::string className = GetFirstAutoClass();
            if (!className.empty()) {
                return ReferenceResolution(className, "class");
            }
        }
        else if (referenceType == "class") {
            std::string className = GetFirstAutoClass();
            if (!className.empty()) {
                return ReferenceResolution(className, "class");
            }
        }
    }
    
    ReferenceResolution result;
    result.IsResolved = false;
    result.ErrorMessage = "Cannot resolve context reference in current context";
    return result;
}

void CHTLContextManager::AddConstraint(const std::string& constraint, bool isGlobal) {
    if (isGlobal) {
        m_GlobalConstraints.push_back(constraint);
    }
    else if (m_CurrentContext) {
        m_CurrentContext->Constraints.push_back(constraint);
    }
}

bool CHTLContextManager::CheckConstraint(const std::string& elementType) {
    // 检查全局约束
    for (const auto& constraint : m_GlobalConstraints) {
        if (constraint == elementType || 
            constraint == "[Template]" || 
            constraint == "[Custom]" ||
            constraint == "@Html") {
            return false; // 违反全局约束
        }
    }
    
    // 检查当前上下文约束
    if (m_CurrentContext) {
        for (const auto& constraint : m_CurrentContext->Constraints) {
            if (constraint == elementType) {
                return false; // 违反局部约束
            }
        }
    }
    
    // 检查父上下文约束
    std::stack<ContextInfo*> tempStack = m_ContextStack;
    while (!tempStack.empty()) {
        auto context = tempStack.top();
        for (const auto& constraint : context->Constraints) {
            if (constraint == elementType) {
                return false; // 违反父上下文约束
            }
        }
        tempStack.pop();
    }
    
    return true; // 通过所有约束检查
}

std::vector<std::string> CHTLContextManager::GetCurrentConstraints() {
    std::vector<std::string> allConstraints = m_GlobalConstraints;
    
    if (m_CurrentContext) {
        allConstraints.insert(allConstraints.end(), 
                            m_CurrentContext->Constraints.begin(),
                            m_CurrentContext->Constraints.end());
    }
    
    return allConstraints;
}

std::vector<std::string> CHTLContextManager::BuildScopeChain() {
    std::vector<std::string> chain;
    
    // 添加当前上下文
    if (m_CurrentContext) {
        chain.push_back(m_CurrentContext->Name);
    }
    
    // 添加父上下文
    std::stack<ContextInfo*> tempStack = m_ContextStack;
    std::vector<std::string> parentChain;
    
    while (!tempStack.empty()) {
        parentChain.push_back(tempStack.top()->Name);
        tempStack.pop();
    }
    
    // 反转父链（从根到当前）
    std::reverse(parentChain.begin(), parentChain.end());
    chain.insert(chain.begin(), parentChain.begin(), parentChain.end());
    
    return chain;
}

std::string CHTLContextManager::FindInScopeChain(const std::string& name) {
    // 在作用域链中从当前向上查找变量
    std::stack<ContextInfo*> tempStack = m_ContextStack;
    
    while (!tempStack.empty()) {
        auto context = tempStack.top();
        auto it = context->Variables.find(name);
        if (it != context->Variables.end()) {
            return it->second;
        }
        tempStack.pop();
    }
    
    return "";
}

std::string CHTLContextManager::GetFullContextPath() {
    auto scopeChain = BuildScopeChain();
    std::string path;
    
    for (size_t i = 0; i < scopeChain.size(); ++i) {
        if (i > 0) {
            path += ".";
        }
        path += scopeChain[i];
    }
    
    return path;
}

bool CHTLContextManager::IsStyleAutoAddClassDisabled() {
    return GetConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS", "false") == "true";
}

bool CHTLContextManager::IsStyleAutoAddIdDisabled() {
    return GetConfigValue("DISABLE_STYLE_AUTO_ADD_ID", "false") == "true";
}

bool CHTLContextManager::IsScriptAutoAddClassDisabled() {
    return GetConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true") == "true";
}

bool CHTLContextManager::IsScriptAutoAddIdDisabled() {
    return GetConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID", "true") == "true";
}

int CHTLContextManager::GetIndexInitialCount() {
    std::string value = GetConfigValue("INDEX_INITIAL_COUNT", "0");
    return std::stoi(value);
}

std::string CHTLContextManager::GetContextStatistics() const {
    std::ostringstream oss;
    oss << "CHTL Context Statistics:\n";
    oss << "Context Stack Depth: " << m_ContextStack.size() << "\n";
    oss << "Global Variables: " << m_GlobalVariables.size() << "\n";
    oss << "Auto Classes: " << m_AutoClassMap.size() << "\n";
    oss << "Auto IDs: " << m_AutoIdMap.size() << "\n";
    oss << "Global Constraints: " << m_GlobalConstraints.size() << "\n";
    
    if (m_CurrentContext) {
        oss << "Current Context: " << m_CurrentContext->Name << "\n";
        oss << "Current Variables: " << m_CurrentContext->Variables.size() << "\n";
        oss << "Current Properties: " << m_CurrentContext->Properties.size() << "\n";
        oss << "Current Constraints: " << m_CurrentContext->Constraints.size() << "\n";
    }
    
    return oss.str();
}

std::string CHTLContextManager::PrintContextStack() const {
    std::ostringstream oss;
    oss << "Context Stack (top to bottom):\n";
    
    if (m_CurrentContext) {
        oss << "  [Current] " << m_CurrentContext->Name 
            << " (" << static_cast<int>(m_CurrentContext->Type) << ")\n";
    }
    
    std::stack<ContextInfo*> tempStack = m_ContextStack;
    int level = 0;
    
    while (!tempStack.empty()) {
        auto context = tempStack.top();
        oss << "  [" << level << "] " << context->Name 
            << " (" << static_cast<int>(context->Type) << ")\n";
        tempStack.pop();
        level++;
    }
    
    return oss.str();
}

void CHTLContextManager::Reset() {
    while (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
    
    m_GlobalVariables.clear();
    m_ScopeChain.clear();
    m_AutoClassMap.clear();
    m_AutoIdMap.clear();
    m_GlobalConstraints.clear();
    m_ScopeConstraints.clear();
    
    CreateDefaultGlobalContext();
}

bool CHTLContextManager::IsValidContextTransition(CHTLContextType fromType, CHTLContextType toType) {
    // 定义有效的上下文转换规则
    switch (fromType) {
        case CHTLContextType::GLOBAL_CONTEXT:
            return true; // 全局上下文可以转换到任何上下文
            
        case CHTLContextType::ELEMENT_CONTEXT:
            return toType == CHTLContextType::STYLE_CONTEXT ||
                   toType == CHTLContextType::SCRIPT_CONTEXT ||
                   toType == CHTLContextType::ELEMENT_CONTEXT; // 嵌套元素
                   
        case CHTLContextType::STYLE_CONTEXT:
            return toType == CHTLContextType::TEMPLATE_CONTEXT ||
                   toType == CHTLContextType::CUSTOM_CONTEXT;
                   
        case CHTLContextType::SCRIPT_CONTEXT:
            // 局部script属于CHTL，支持模板变量、自定义变量组、变量组特例化、命名空间from
            return toType == CHTLContextType::TEMPLATE_CONTEXT ||
                   toType == CHTLContextType::CUSTOM_CONTEXT;
                   
        case CHTLContextType::NAMESPACE_CONTEXT:
            return toType != CHTLContextType::GLOBAL_CONTEXT; // 命名空间内不能有全局上下文
            
        case CHTLContextType::TEMPLATE_CONTEXT:
        case CHTLContextType::CUSTOM_CONTEXT:
            return toType == CHTLContextType::ELEMENT_CONTEXT ||
                   toType == CHTLContextType::STYLE_CONTEXT;
                   
        default:
            return false;
    }
}

void CHTLContextManager::InheritFromParentContext(ContextInfo* parentContext, ContextInfo* childContext) {
    if (!parentContext || !childContext) {
        return;
    }
    
    // 继承父上下文的变量（但不覆盖子上下文的变量）
    for (const auto& var : parentContext->Variables) {
        if (childContext->Variables.find(var.first) == childContext->Variables.end()) {
            childContext->Variables[var.first] = var.second;
        }
    }
    
    // 继承约束
    childContext->Constraints.insert(childContext->Constraints.end(),
                                   parentContext->Constraints.begin(),
                                   parentContext->Constraints.end());
}

void CHTLContextManager::ApplyScopeRules(ContextInfo* context) {
    if (!context) {
        return;
    }
    
    // 根据作用域类型应用特定规则
    switch (context->Scope) {
        case CHTLScopeType::STYLE_SCOPE:
            // 样式作用域允许所有CHTL语法元素
            break;
            
        case CHTLScopeType::SCRIPT_SCOPE:
            // 脚本作用域只允许部分CHTL语法元素
            context->Constraints.push_back("no_template_element");
            context->Constraints.push_back("no_custom_element");
            break;
            
        case CHTLScopeType::NAMESPACE_SCOPE:
            // 命名空间作用域根据约束限制
            break;
            
        default:
            break;
    }
}

std::string CHTLContextManager::GetConfigValue(const std::string& key, const std::string& defaultValue) {
    // 这里应该从活动配置中获取值
    // 暂时返回默认值，后续与CHTLGlobalMap集成
    return defaultValue;
}

// RAII上下文管理器实现
CHTLContextGuard::CHTLContextGuard(CHTLContextManager* contextManager, 
                                 CHTLContextType type, 
                                 CHTLScopeType scope, 
                                 const std::string& name)
    : m_ContextManager(contextManager), m_ShouldExit(false) {
    
    if (m_ContextManager && m_ContextManager->EnterContext(type, scope, name)) {
        m_ShouldExit = true;
    }
}

CHTLContextGuard::~CHTLContextGuard() {
    if (m_ShouldExit && m_ContextManager) {
        m_ContextManager->ExitContext();
    }
}

CHTLContextGuard::CHTLContextGuard(CHTLContextGuard&& other) noexcept
    : m_ContextManager(other.m_ContextManager), m_ShouldExit(other.m_ShouldExit) {
    other.m_ShouldExit = false;
}

CHTLContextGuard& CHTLContextGuard::operator=(CHTLContextGuard&& other) noexcept {
    if (this != &other) {
        if (m_ShouldExit && m_ContextManager) {
            m_ContextManager->ExitContext();
        }
        
        m_ContextManager = other.m_ContextManager;
        m_ShouldExit = other.m_ShouldExit;
        other.m_ShouldExit = false;
    }
    return *this;
}

void CHTLContextGuard::Release() {
    m_ShouldExit = false;
}

} // namespace CHTL