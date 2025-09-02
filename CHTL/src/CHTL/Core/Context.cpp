#include "CHTL/Core/Context.h"
#include "CHTL/AST/TemplateNodes.h"
#include <algorithm>
#include <set>

namespace CHTL {

CompileContext::CompileContext() {
    globalMap = std::make_shared<GlobalMap>();
    stateMachine = std::make_shared<StateMachine>();
    
    // 初始化时压入一个全局作用域
    EnterScope();
}

void CompileContext::SetActiveConfiguration(std::shared_ptr<Configuration> config) {
    activeConfiguration = config;
    
    // TODO: 根据配置更新编译选项
    // 这里需要解析配置并应用到options中
}

void CompileContext::EnterScope() {
    scopeStack.push(LocalScope());
}

void CompileContext::ExitScope() {
    if (scopeStack.empty()) {
        throw RuntimeError("作用域栈下溢：无法退出全局作用域");
    }
    scopeStack.pop();
}

CompileContext::LocalScope& CompileContext::GetCurrentScope() {
    if (scopeStack.empty()) {
        throw RuntimeError("作用域栈为空");
    }
    return scopeStack.top();
}

const CompileContext::LocalScope& CompileContext::GetCurrentScope() const {
    if (scopeStack.empty()) {
        throw RuntimeError("作用域栈为空");
    }
    return scopeStack.top();
}

void CompileContext::SetLocalVariable(const std::string& name, const std::string& value) {
    GetCurrentScope().variables[name] = value;
}

std::string CompileContext::GetLocalVariable(const std::string& name) const {
    // 从当前作用域开始向上查找
    auto tempStack = scopeStack;
    while (!tempStack.empty()) {
        const auto& scope = tempStack.top();
        auto it = scope.variables.find(name);
        if (it != scope.variables.end()) {
            return it->second;
        }
        tempStack.pop();
    }
    return "";
}

bool CompileContext::HasLocalVariable(const std::string& name) const {
    auto tempStack = scopeStack;
    while (!tempStack.empty()) {
        const auto& scope = tempStack.top();
        if (scope.variables.find(name) != scope.variables.end()) {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

void CompileContext::SetCurrentElement(std::shared_ptr<ElementNode> element) {
    GetCurrentScope().currentElement = element;
}

std::shared_ptr<ElementNode> CompileContext::GetCurrentElement() const {
    if (!scopeStack.empty()) {
        return scopeStack.top().currentElement;
    }
    return nullptr;
}

void CompileContext::SetCurrentClass(const std::string& className) {
    GetCurrentScope().currentClass = className;
}

const std::string& CompileContext::GetCurrentClass() const {
    static const std::string empty;
    if (!scopeStack.empty()) {
        return scopeStack.top().currentClass;
    }
    return empty;
}

void CompileContext::SetCurrentId(const std::string& id) {
    GetCurrentScope().currentId = id;
}

const std::string& CompileContext::GetCurrentId() const {
    static const std::string empty;
    if (!scopeStack.empty()) {
        return scopeStack.top().currentId;
    }
    return empty;
}

bool CompileContext::IsFileImported(const std::string& filePath) const {
    return importedFiles.find(filePath) != importedFiles.end();
}

void CompileContext::MarkFileImported(const std::string& filePath) {
    importedFiles[filePath] = true;
}

void CompileContext::PushImportPath(const std::string& path) {
    importPath.push_back(path);
}

void CompileContext::PopImportPath() {
    if (!importPath.empty()) {
        importPath.pop_back();
    }
}

bool CompileContext::IsInImportCycle(const std::string& filePath) const {
    return std::find(importPath.begin(), importPath.end(), filePath) != importPath.end();
}

void CompileContext::Reset() {
    // 清空所有状态
    globalMap->Clear();
    stateMachine->Reset();
    
    // 清空作用域栈
    while (!scopeStack.empty()) {
        scopeStack.pop();
    }
    
    // 重新初始化
    EnterScope();
    
    // 清空导入记录
    importedFiles.clear();
    importPath.clear();
    
    // 清空命名空间栈
    namespaceStack.clear();
    
    // 清空约束
    constraints.clear();
    
    // 重置选项为默认值
    options = CompileOptions();
    
    // 清空其他状态
    currentFile.clear();
    activeConfiguration.reset();
}

void CompileContext::EnterNamespace(const std::string& namespaceName) {
    namespaceStack.push_back(namespaceName);
}

void CompileContext::ExitNamespace() {
    if (!namespaceStack.empty()) {
        namespaceStack.pop_back();
    }
}

std::string CompileContext::GetCurrentNamespace() const {
    return namespaceStack.empty() ? "" : namespaceStack.back();
}

std::string CompileContext::GetFullNamespacePath() const {
    std::string path;
    for (size_t i = 0; i < namespaceStack.size(); ++i) {
        if (i > 0) path += ".";
        path += namespaceStack[i];
    }
    return path;
}

void CompileContext::AddConstraint(std::shared_ptr<ExceptNode> constraint) {
    if (constraint) {
        constraints.push_back(constraint);
    }
}

bool CompileContext::IsElementAllowed(const std::string& elementName) const {
    // 检查所有约束
    for (const auto& constraint : constraints) {
        if (!constraint) continue;
        
        for (const auto& c : constraint->GetConstraints()) {
            if (c.type == ExceptNode::ConstraintType::Exact && 
                c.value == elementName) {
                return false; // 精确约束匹配，不允许
            }
            
            // 检查@Html类型约束
            if (c.type == ExceptNode::ConstraintType::Type && 
                c.value == "Html" && c.modifier.empty()) {
                // 检查是否是HTML元素（简单判断）
                // TODO: 完善HTML元素列表
                static const std::set<std::string> htmlElements = {
                    "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
                    "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody",
                    "a", "img", "form", "input", "button", "select", "textarea",
                    "header", "footer", "nav", "section", "article", "aside",
                    "html", "head", "body", "meta", "title", "link", "script", "style"
                };
                
                if (htmlElements.find(elementName) != htmlElements.end()) {
                    return false; // 是HTML元素，不允许
                }
            }
        }
    }
    
    return true; // 默认允许
}

bool CompileContext::IsTypeAllowed(const std::string& typeName, const std::string& modifier) const {
    // 检查所有约束
    for (const auto& constraint : constraints) {
        if (!constraint) continue;
        
        for (const auto& c : constraint->GetConstraints()) {
            if (c.type == ExceptNode::ConstraintType::Type) {
                // 检查修饰符匹配
                if (c.modifier == modifier) {
                    // 如果约束的值为空，表示禁止整个类型
                    if (c.value.empty()) {
                        return false;
                    }
                    // 否则检查具体类型
                    if (c.value == typeName) {
                        return false;
                    }
                }
            }
        }
    }
    
    return true; // 默认允许
}

} // namespace CHTL