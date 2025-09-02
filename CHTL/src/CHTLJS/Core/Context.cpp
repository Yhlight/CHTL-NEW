#include "CHTLJS/Core/Context.h"
#include "CHTLJS/AST/Nodes.h"

namespace CHTL {
namespace JS {

CompileContext::CompileContext() {
    globalMap = std::make_shared<GlobalMap>();
    stateMachine = std::make_shared<StateMachine>();
    
    // 初始化时压入一个全局作用域
    EnterScope();
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

void CompileContext::SetCurrentVirtualObject(std::shared_ptr<VirtualObjectNode> vobj) {
    GetCurrentScope().currentVirtualObject = vobj;
}

std::shared_ptr<VirtualObjectNode> CompileContext::GetCurrentVirtualObject() const {
    if (!scopeStack.empty()) {
        return scopeStack.top().currentVirtualObject;
    }
    return nullptr;
}

void CompileContext::SetCurrentSelector(const std::string& selector) {
    GetCurrentScope().currentSelector = selector;
}

const std::string& CompileContext::GetCurrentSelector() const {
    static const std::string empty;
    if (!scopeStack.empty()) {
        return scopeStack.top().currentSelector;
    }
    return empty;
}

void CompileContext::SetInCHTLJSFunction(bool inFunc) {
    GetCurrentScope().inCHTLJSFunction = inFunc;
}

bool CompileContext::IsInCHTLJSFunction() const {
    if (!scopeStack.empty()) {
        return scopeStack.top().inCHTLJSFunction;
    }
    return false;
}

void CompileContext::Reset() {
    globalMap->Clear();
    stateMachine->Reset();
    
    while (!scopeStack.empty()) {
        scopeStack.pop();
    }
    
    EnterScope();
    
    options = CompileOptions();
    stats = CodeStats();
    
    currentFile.clear();
    selectorCache.reset();
}

} // namespace JS
} // namespace CHTL