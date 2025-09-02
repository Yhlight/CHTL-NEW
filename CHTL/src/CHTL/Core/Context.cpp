#include "CHTL/Core/Context.h"
#include <algorithm>

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
    
    // 重置选项为默认值
    options = CompileOptions();
    
    // 清空其他状态
    currentFile.clear();
    activeConfiguration.reset();
}

} // namespace CHTL