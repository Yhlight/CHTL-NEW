#ifndef CHTL_CHTL_CORE_CONTEXT_H
#define CHTL_CHTL_CORE_CONTEXT_H

#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include "CHTL/Core/GlobalMap.h"
#include "CHTL/Core/State.h"
#include "Common/Error.h"

namespace CHTL {

// 前向声明
class ASTNode;
class ElementNode;
class Configuration;

/**
 * 编译上下文
 * 管理编译过程中的所有状态和数据
 */
class CompileContext {
private:
    // 全局映射表
    std::shared_ptr<GlobalMap> globalMap;
    
    // 状态机
    std::shared_ptr<StateMachine> stateMachine;
    
    // 当前文件信息
    std::string currentFile;
    std::string moduleDirectory;
    
    // 配置信息
    std::shared_ptr<Configuration> activeConfiguration;
    
    // 局部作用域栈
    struct LocalScope {
        std::unordered_map<std::string, std::string> variables;  // 局部变量
        std::unordered_map<std::string, std::string> styles;     // 局部样式
        std::shared_ptr<ElementNode> currentElement;             // 当前元素
        std::string currentClass;                                 // 当前类名
        std::string currentId;                                    // 当前ID
    };
    std::stack<LocalScope> scopeStack;
    
    // 编译选项
    struct CompileOptions {
        bool debugMode;
        bool useHTML5;
        bool disableStyleAutoAddClass;
        bool disableStyleAutoAddId;
        bool disableScriptAutoAddClass;
        bool disableScriptAutoAddId;
        bool disableDefaultNamespace;
        
        CompileOptions() : debugMode(false), useHTML5(false),
                          disableStyleAutoAddClass(false),
                          disableStyleAutoAddId(false),
                          disableScriptAutoAddClass(true),
                          disableScriptAutoAddId(true),
                          disableDefaultNamespace(false) {}
    } options;
    
    // 导入管理
    std::unordered_map<std::string, bool> importedFiles;  // 已导入的文件
    std::vector<std::string> importPath;                  // 导入路径栈
    
    // 命名空间管理
    std::vector<std::string> namespaceStack;              // 命名空间栈
    
public:
    CompileContext();
    ~CompileContext() = default;
    
    // 全局映射表访问
    std::shared_ptr<GlobalMap> GetGlobalMap() { return globalMap; }
    const std::shared_ptr<GlobalMap> GetGlobalMap() const { return globalMap; }
    
    // 状态机访问
    std::shared_ptr<StateMachine> GetStateMachine() { return stateMachine; }
    const std::shared_ptr<StateMachine> GetStateMachine() const { return stateMachine; }
    
    // 文件信息
    void SetCurrentFile(const std::string& file) { currentFile = file; }
    const std::string& GetCurrentFile() const { return currentFile; }
    
    void SetModuleDirectory(const std::string& dir) { moduleDirectory = dir; }
    const std::string& GetModuleDirectory() const { return moduleDirectory; }
    
    // 配置管理
    void SetActiveConfiguration(std::shared_ptr<Configuration> config);
    std::shared_ptr<Configuration> GetActiveConfiguration() const { return activeConfiguration; }
    
    // 作用域管理
    void EnterScope();
    void ExitScope();
    LocalScope& GetCurrentScope();
    const LocalScope& GetCurrentScope() const;
    
    // 局部变量管理
    void SetLocalVariable(const std::string& name, const std::string& value);
    std::string GetLocalVariable(const std::string& name) const;
    bool HasLocalVariable(const std::string& name) const;
    
    // 当前元素管理
    void SetCurrentElement(std::shared_ptr<ElementNode> element);
    std::shared_ptr<ElementNode> GetCurrentElement() const;
    
    void SetCurrentClass(const std::string& className);
    const std::string& GetCurrentClass() const;
    
    void SetCurrentId(const std::string& id);
    const std::string& GetCurrentId() const;
    
    // 编译选项
    void SetDebugMode(bool enable) { options.debugMode = enable; }
    bool IsDebugMode() const { return options.debugMode; }
    
    void SetUseHTML5(bool enable) { options.useHTML5 = enable; }
    bool UseHTML5() const { return options.useHTML5; }
    
    void SetDisableStyleAutoAddClass(bool disable) { options.disableStyleAutoAddClass = disable; }
    bool IsStyleAutoAddClassDisabled() const { return options.disableStyleAutoAddClass; }
    
    void SetDisableStyleAutoAddId(bool disable) { options.disableStyleAutoAddId = disable; }
    bool IsStyleAutoAddIdDisabled() const { return options.disableStyleAutoAddId; }
    
    void SetDisableScriptAutoAddClass(bool disable) { options.disableScriptAutoAddClass = disable; }
    bool IsScriptAutoAddClassDisabled() const { return options.disableScriptAutoAddClass; }
    
    void SetDisableScriptAutoAddId(bool disable) { options.disableScriptAutoAddId = disable; }
    bool IsScriptAutoAddIdDisabled() const { return options.disableScriptAutoAddId; }
    
    void SetDisableDefaultNamespace(bool disable) { options.disableDefaultNamespace = disable; }
    bool IsDefaultNamespaceDisabled() const { return options.disableDefaultNamespace; }
    
    // 导入管理
    bool IsFileImported(const std::string& filePath) const;
    void MarkFileImported(const std::string& filePath);
    void PushImportPath(const std::string& path);
    void PopImportPath();
    bool IsInImportCycle(const std::string& filePath) const;
    
    // 命名空间管理
    void EnterNamespace(const std::string& namespaceName);
    void ExitNamespace();
    std::string GetCurrentNamespace() const;
    std::string GetFullNamespacePath() const;
    
    // 重置上下文
    void Reset();
    
    /**
     * RAII作用域管理器
     */
    class ScopeGuard {
    private:
        CompileContext& context;
        bool exited;
        
    public:
        explicit ScopeGuard(CompileContext& ctx) : context(ctx), exited(false) {
            context.EnterScope();
        }
        
        ~ScopeGuard() {
            if (!exited) {
                context.ExitScope();
            }
        }
        
        void Exit() {
            if (!exited) {
                context.ExitScope();
                exited = true;
            }
        }
        
        // 禁止拷贝
        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
    };
};

} // namespace CHTL

#endif // CHTL_CHTL_CORE_CONTEXT_H