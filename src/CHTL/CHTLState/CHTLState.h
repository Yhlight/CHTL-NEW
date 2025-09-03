#pragma once

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

    // CHTL解析状态枚举
    // 严格按照CHTL语法文档定义的语法结构
    enum class CHTLParseState {
        INITIAL,                    // 初始状态
        
        // 基础节点状态
        PARSING_ELEMENT,            // 解析HTML元素
        PARSING_TEXT,               // 解析文本节点
        PARSING_ATTRIBUTE,          // 解析属性
        PARSING_ATTRIBUTE_VALUE,    // 解析属性值
        
        // 样式相关状态
        PARSING_LOCAL_STYLE,        // 解析局部样式块
        PARSING_GLOBAL_STYLE,       // 解析全局样式块
        PARSING_STYLE_PROPERTY,     // 解析样式属性
        PARSING_STYLE_VALUE,        // 解析样式值
        PARSING_CSS_SELECTOR,       // 解析CSS选择器
        
        // 脚本相关状态
        PARSING_LOCAL_SCRIPT,       // 解析局部脚本块
        PARSING_GLOBAL_SCRIPT,      // 解析全局脚本块
        
        // 模板相关状态
        PARSING_TEMPLATE,           // 解析模板块
        PARSING_TEMPLATE_STYLE,     // 解析样式组模板
        PARSING_TEMPLATE_ELEMENT,   // 解析元素模板
        PARSING_TEMPLATE_VAR,       // 解析变量组模板
        
        // 自定义相关状态
        PARSING_CUSTOM,             // 解析自定义块
        PARSING_CUSTOM_STYLE,       // 解析自定义样式组
        PARSING_CUSTOM_ELEMENT,     // 解析自定义元素
        PARSING_CUSTOM_VAR,         // 解析自定义变量组
        
        // 原始嵌入相关状态
        PARSING_ORIGIN,             // 解析原始嵌入块
        PARSING_ORIGIN_HTML,        // 解析HTML原始嵌入
        PARSING_ORIGIN_STYLE,       // 解析Style原始嵌入
        PARSING_ORIGIN_JS,          // 解析JavaScript原始嵌入
        PARSING_CUSTOM_ORIGIN,      // 解析自定义类型原始嵌入
        
        // 导入相关状态
        PARSING_IMPORT,             // 解析导入块
        PARSING_IMPORT_PATH,        // 解析导入路径
        PARSING_IMPORT_ALIAS,       // 解析导入别名
        
        // 配置相关状态
        PARSING_CONFIGURATION,      // 解析配置块
        PARSING_CONFIG_ITEM,        // 解析配置项
        PARSING_NAME_CONFIG,        // 解析Name配置
        PARSING_ORIGIN_TYPE_CONFIG, // 解析OriginType配置
        
        // 命名空间相关状态
        PARSING_NAMESPACE,          // 解析命名空间
        PARSING_NAMESPACE_CONTENT,  // 解析命名空间内容
        
        // 特殊操作状态
        PARSING_INHERIT,            // 解析继承操作
        PARSING_DELETE,             // 解析删除操作
        PARSING_INSERT,             // 解析插入操作
        PARSING_EXCEPT,             // 解析约束操作
        PARSING_USE,                // 解析use语句
        
        // 注释状态
        PARSING_COMMENT,            // 解析注释
        
        // 错误状态
        ERROR_STATE,                // 错误状态
        
        // 完成状态
        COMPLETED                   // 解析完成
    };

    // CHTL上下文状态
    // 用于跟踪当前解析的上下文信息
    struct CHTLContextState {
        std::string currentElement;         // 当前元素名
        std::string currentNamespace;       // 当前命名空间
        std::string currentTemplate;        // 当前模板名
        std::string currentCustom;          // 当前自定义名
        std::string currentOrigin;          // 当前原始嵌入名
        std::string currentConfig;          // 当前配置组名
        
        int braceLevel;                     // 大括号嵌套级别
        int bracketLevel;                   // 中括号嵌套级别
        int parenLevel;                     // 小括号嵌套级别
        
        bool inLocalStyle;                  // 是否在局部样式块中
        bool inGlobalStyle;                 // 是否在全局样式块中
        bool inLocalScript;                 // 是否在局部脚本块中
        bool inGlobalScript;                // 是否在全局脚本块中
        bool inTemplate;                    // 是否在模板块中
        bool inCustom;                      // 是否在自定义块中
        bool inOrigin;                      // 是否在原始嵌入块中
        bool inNamespace;                   // 是否在命名空间中
        bool inConfiguration;               // 是否在配置块中
        
        CHTLContextState() : braceLevel(0), bracketLevel(0), parenLevel(0),
                            inLocalStyle(false), inGlobalStyle(false),
                            inLocalScript(false), inGlobalScript(false),
                            inTemplate(false), inCustom(false), inOrigin(false),
                            inNamespace(false), inConfiguration(false) {}
    };

    // CHTL状态管理器
    // 基于RAII自动化管理模式的状态机
    // 严格按照目标规划.ini要求实现
    class CHTLStateManager {
    private:
        std::stack<CHTLParseState> stateStack;              // 状态栈
        std::stack<CHTLContextState> contextStack;          // 上下文栈
        CHTLParseState currentState;                        // 当前状态
        CHTLContextState currentContext;                    // 当前上下文
        
        std::vector<std::string> errorMessages;             // 错误消息列表
        
        // 状态转换映射表
        std::unordered_map<CHTLParseState, std::vector<CHTLParseState>> validTransitions;
        
        void InitializeValidTransitions();                  // 初始化有效状态转换

    public:
        CHTLStateManager();
        ~CHTLStateManager() = default;

        // 状态管理
        void PushState(CHTLParseState newState);            // 压入新状态
        void PopState();                                    // 弹出状态
        CHTLParseState GetCurrentState() const;            // 获取当前状态
        CHTLParseState GetPreviousState() const;           // 获取上一个状态
        
        // 上下文管理
        void PushContext(const CHTLContextState& context);  // 压入上下文
        void PopContext();                                  // 弹出上下文
        CHTLContextState& GetCurrentContext();             // 获取当前上下文
        const CHTLContextState& GetCurrentContext() const; // 获取当前上下文（常量版本）
        
        // 状态验证
        bool IsValidTransition(CHTLParseState fromState, CHTLParseState toState) const;  // 验证状态转换是否有效
        bool CanTransitionTo(CHTLParseState targetState) const;                         // 检查是否可以转换到目标状态
        
        // 错误处理
        void AddError(const std::string& message);          // 添加错误消息
        bool HasErrors() const;                             // 检查是否有错误
        std::vector<std::string> GetErrors() const;         // 获取所有错误消息
        void ClearErrors();                                 // 清空错误消息
        
        // 上下文操作
        void EnterBrace();                                  // 进入大括号
        void ExitBrace();                                   // 退出大括号
        void EnterBracket();                                // 进入中括号
        void ExitBracket();                                 // 退出中括号
        void EnterParen();                                  // 进入小括号
        void ExitParen();                                   // 退出小括号
        
        void EnterLocalStyle();                             // 进入局部样式块
        void ExitLocalStyle();                              // 退出局部样式块
        void EnterGlobalStyle();                            // 进入全局样式块
        void ExitGlobalStyle();                             // 退出全局样式块
        
        void EnterLocalScript();                            // 进入局部脚本块
        void ExitLocalScript();                             // 退出局部脚本块
        void EnterGlobalScript();                           // 进入全局脚本块
        void ExitGlobalScript();                            // 退出全局脚本块
        
        void EnterTemplate(const std::string& templateName);// 进入模板块
        void ExitTemplate();                               // 退出模板块
        void EnterCustom(const std::string& customName);   // 进入自定义块
        void ExitCustom();                                 // 退出自定义块
        void EnterOrigin(const std::string& originName);   // 进入原始嵌入块
        void ExitOrigin();                                 // 退出原始嵌入块
        
        void EnterNamespace(const std::string& namespaceName);  // 进入命名空间
        void ExitNamespace();                               // 退出命名空间
        void EnterConfiguration(const std::string& configName = "");  // 进入配置块
        void ExitConfiguration();                           // 退出配置块
        
        // 工具方法
        void Reset();                                       // 重置状态管理器
        bool IsInValidState() const;                        // 检查是否处于有效状态
        std::string GetStateString(CHTLParseState state) const;  // 获取状态字符串表示
        
        // 调试方法
        void PrintState() const;                            // 打印当前状态（调试用）
        void PrintStateStack() const;                      // 打印状态栈（调试用）
    };

} // namespace CHTL