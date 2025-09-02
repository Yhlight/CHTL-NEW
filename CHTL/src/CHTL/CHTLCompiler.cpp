#include "CHTL/CHTLCompiler.h"
#include "CHTL/Lexer/Lexer.h"
#include "CHTL/Parser/Parser.h"
#include "CHTL/Generator/Generator.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"
#include "Common/CMODHandler.h"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

CHTLCompiler::CHTLCompiler() 
    : debugMode(false), hasHTML5Declaration(false) {
    InitializeComponents();
}

CHTLCompiler::~CHTLCompiler() = default;

void CHTLCompiler::InitializeComponents() {
    context = std::make_shared<CompileContext>();
    lexer = std::make_unique<Lexer>("", "");
    parser = std::make_unique<Parser>(context);
    generator = std::make_unique<Generator>(context);
}

std::string CHTLCompiler::Compile(const CodeFragmentPtr& fragment) {
    if (!fragment || fragment->IsEmpty()) {
        return "";
    }
    
    LOG_INFO("CHTL编译器处理片段，类型: " + fragment->GetTypeString());
    
    try {
        // 设置源文件信息
        context->SetCurrentFile(fragment->sourcePath);
        
        // 预处理
        std::string processedCode = PreprocessFragment(fragment->content);
        
        // 词法分析
        lexer->SetSource(processedCode, fragment->sourcePath);
        lexer->Tokenize();
        
        // 语法分析
        auto ast = parser->Parse(lexer->GetTokens());
        
        // 处理导入
        ProcessImports(ast);
        
        // 检查HTML5声明
        if (context->UseHTML5()) {
            hasHTML5Declaration = true;
        }
        
        // 代码生成
        generator->Generate(ast, lastResult);
        
        // 后处理
        PostprocessResult(lastResult);
        
        return lastResult.htmlContent;
        
    } catch (const std::exception& e) {
        LOG_ERROR("CHTL编译错误: " + std::string(e.what()));
        throw;
    }
}

void CHTLCompiler::SetModuleDirectory(const std::string& dir) {
    moduleDirectory = dir;
    context->SetModuleDirectory(dir);
}

void CHTLCompiler::SetDebugMode(bool enable) {
    debugMode = enable;
    context->SetDebugMode(enable);
    Logger::SetDebugMode(enable);
}

void CHTLCompiler::Reset() {
    context->Reset();
    lexer->Reset();
    parser->Reset();
    hasHTML5Declaration = false;
    lastResult.Clear();
}

void CHTLCompiler::SetSourceFile(const std::string& file) {
    sourceFile = file;
    context->SetCurrentFile(file);
}

std::string CHTLCompiler::PreprocessFragment(const std::string& code) {
    // TODO: 实现预处理逻辑
    return code;
}

void CHTLCompiler::PostprocessResult(CompileResult& result) {
    // TODO: 实现后处理逻辑
}

void CHTLCompiler::ProcessImports(std::shared_ptr<ProgramNode> ast) {
    if (!ast) return;
    
    LOG_DEBUG("处理导入，AST子节点数: " + std::to_string(ast->GetChildren().size()));
    
    // 遍历AST寻找Import节点
    for (const auto& child : ast->GetChildren()) {
        if (child) {
            LOG_DEBUG("节点类型: " + std::to_string(static_cast<int>(child->GetType())));
            if (child->GetType() == ASTNodeType::Import) {
                auto importNode = std::dynamic_pointer_cast<ImportNode>(child);
                if (importNode) {
                    LOG_INFO("找到Import节点: " + importNode->ToString());
                    ProcessImportNode(importNode);
                }
            }
        }
    }
}

void CHTLCompiler::ProcessImportNode(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) return;
    
    std::string importPath = importNode->GetFromPath();
    ImportNode::ImportType importType = importNode->GetImportType();
    
    // 对于@Chtl导入，检查是否是模块名
    if (importType == ImportNode::ImportType::Chtl) {
        // 检查是否是模块名（不含路径分隔符和后缀）
        bool isModuleName = (importPath.find('/') == std::string::npos && 
                           importPath.find('\\') == std::string::npos &&
                           importPath.find('.') == std::string::npos);
        
        if (isModuleName) {
            // 尝试作为CMOD模块处理
            if (ProcessCMODImport(importPath, importNode->GetAsName())) {
                return;
            }
            // 如果CMOD导入失败，继续尝试作为普通文件
        }
    }
    
    // 解析导入路径
    std::string resolvedPath = ResolveImportPath(
        importNode->GetFromPath(), 
        context->GetCurrentFile()
    );
    
    LOG_INFO("导入路径: " + importNode->GetFromPath() + " -> " + resolvedPath);
    
    // 检查循环导入
    if (context->IsInImportCycle(resolvedPath)) {
        LOG_ERROR("检测到循环导入: " + resolvedPath);
        return;
    }
    
    // 检查是否已经导入
    if (context->IsFileImported(resolvedPath)) {
        LOG_INFO("文件已导入，跳过: " + resolvedPath);
        return;
    }
    
    // 标记文件为已导入
    context->MarkFileImported(resolvedPath);
    context->PushImportPath(resolvedPath);
    
    try {
        // 加载并合并文件
        LoadAndMergeFile(
            resolvedPath,
            static_cast<int>(importNode->GetImportType()),
            importNode->GetItemName(),
            importNode->GetAsName()
        );
    } catch (const std::exception& e) {
        LOG_ERROR("导入文件失败 " + resolvedPath + ": " + e.what());
    }
    
    context->PopImportPath();
}

std::string CHTLCompiler::ResolveImportPath(const std::string& importPath, const std::string& currentFile) {
    // 如果是绝对路径，直接返回
    if (!importPath.empty() && importPath[0] == '/') {
        return importPath;
    }
    
    // 获取当前文件的目录
    std::string currentDir;
    size_t lastSlash = currentFile.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        currentDir = currentFile.substr(0, lastSlash + 1);
    } else {
        currentDir = "./";
    }
    
    // 处理点号分隔的路径（转换为斜杠）
    std::string normalizedPath = importPath;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '.', '/');
    
    // 根据导入类型添加后缀
    if (normalizedPath.find('.') == std::string::npos) {
        // 没有后缀，根据上下文添加
        normalizedPath += ".chtl";
    }
    
    // 组合路径
    return currentDir + normalizedPath;
}

void CHTLCompiler::LoadAndMergeFile(const std::string& filePath, int importType,
                                    const std::string& itemName, const std::string& asName) {
    // 读取文件内容
    LOG_INFO("尝试打开文件: " + filePath);
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filePath);
    }
    LOG_INFO("成功打开文件: " + filePath);
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // 根据导入类型处理
    // 需要与ImportNode::ImportType的枚举值对应
    auto type = static_cast<ImportNode::ImportType>(importType);
    switch (type) {
        case ImportNode::ImportType::Chtl: {
            // 编译CHTL文件
            auto fragment = std::make_shared<CodeFragment>();
            fragment->content = content;
            fragment->sourcePath = filePath;
            fragment->type = FragmentType::CHTL;
            
            // 保存当前状态
            std::string oldFile = context->GetCurrentFile();
            
            // 递归编译
            context->SetCurrentFile(filePath);
            
            // 预处理
            std::string processedCode = PreprocessFragment(content);
            
            // 词法分析
            lexer->SetSource(processedCode, filePath);
            lexer->Tokenize();
            
            // 语法分析
            auto importedAst = parser->Parse(lexer->GetTokens());
            
            // 递归处理导入
            ProcessImports(importedAst);
            
            LOG_INFO("导入的AST子节点数: " + std::to_string(importedAst->GetChildren().size()));
            
            // 合并AST中的模板和自定义元素到全局映射表
            MergeImportedAST(importedAst, asName);
            
            // 恢复状态
            context->SetCurrentFile(oldFile);
            break;
        }
        
        case ImportNode::ImportType::Html:
        case ImportNode::ImportType::Style:
        case ImportNode::ImportType::JavaScript:
            // TODO: 处理其他类型的导入
            LOG_INFO("导入类型尚未实现: " + filePath);
            break;
            
        case ImportNode::ImportType::TemplateStyle:
        case ImportNode::ImportType::TemplateElement:
        case ImportNode::ImportType::TemplateVar:
            // TODO: 导入特定的模板
            LOG_INFO("模板导入尚未实现: " + itemName + " from " + filePath);
            break;
            
        default:
            LOG_INFO("未知的导入类型: " + filePath);
            break;
    }
}

void CHTLCompiler::MergeImportedAST(std::shared_ptr<ProgramNode> importedAst, const std::string& namespaceName) {
    if (!importedAst) return;
    
    auto globalMap = context->GetGlobalMap();
    if (!globalMap) return;
    
    // 遍历导入的AST，提取模板和自定义定义
    for (const auto& node : importedAst->GetChildren()) {
        if (!node) continue;
        
        switch (node->GetType()) {
            case ASTNodeType::TemplateStyle: {
                auto templateNode = std::dynamic_pointer_cast<TemplateStyleNode>(node);
                if (templateNode) {
                    std::string name = templateNode->GetName();
                    if (!namespaceName.empty()) {
                        name = namespaceName + "." + name;
                    }
                    globalMap->AddTemplateStyle(name, templateNode);
                    LOG_DEBUG("导入样式模板: " + name);
                }
                break;
            }
            
            case ASTNodeType::TemplateElement: {
                auto templateNode = std::dynamic_pointer_cast<TemplateElementNode>(node);
                if (templateNode) {
                    std::string name = templateNode->GetName();
                    if (!namespaceName.empty()) {
                        name = namespaceName + "." + name;
                    }
                    globalMap->AddTemplateElement(name, templateNode);
                    LOG_DEBUG("导入元素模板: " + name);
                }
                break;
            }
            
            case ASTNodeType::TemplateVar: {
                auto templateNode = std::dynamic_pointer_cast<TemplateVarNode>(node);
                if (templateNode) {
                    std::string name = templateNode->GetName();
                    if (!namespaceName.empty()) {
                        name = namespaceName + "." + name;
                    }
                    globalMap->AddTemplateVar(name, templateNode);
                    LOG_DEBUG("导入变量组模板: " + name);
                }
                break;
            }
            
            // TODO: 处理其他类型的节点（Custom, Configuration等）
            
            default:
                // 忽略其他节点类型
                break;
        }
    }
}

std::string CHTLCompiler::FindCMODModule(const std::string& moduleName) {
    namespace fs = std::filesystem;
    CMODHandler cmodHandler;
    
    // 如果是官方模块前缀
    if (moduleName.substr(0, 6) == "chtl::") {
        std::string actualName = moduleName.substr(6);
        cmodHandler.SetWorkingDirectory(cmodHandler.GetOfficialModuleDirectory());
        auto found = cmodHandler.FindModules(actualName);
        if (!found.empty()) {
            return found[0].string();
        }
        return "";
    }
    
    // 设置工作目录为源文件所在目录
    if (!sourceFile.empty()) {
        fs::path sourcePath(sourceFile);
        cmodHandler.SetWorkingDirectory(sourcePath.parent_path());
    }
    
    // 查找模块
    auto found = cmodHandler.FindModules(moduleName);
    
    // 优先返回.cmod文件
    for (const auto& path : found) {
        if (path.extension() == ".cmod") {
            return path.string();
        }
    }
    
    // 如果没有.cmod文件，返回第一个找到的
    if (!found.empty()) {
        return found[0].string();
    }
    
    return "";
}

bool CHTLCompiler::ProcessCMODImport(const std::string& moduleName, const std::string& namespaceName) {
    namespace fs = std::filesystem;
    
    LOG_INFO("处理CMOD导入: " + moduleName);
    
    // 查找CMOD模块
    std::string modulePath = FindCMODModule(moduleName);
    if (modulePath.empty()) {
        LOG_ERROR("找不到CMOD模块: " + moduleName);
        return false;
    }
    
    LOG_INFO("找到CMOD模块: " + modulePath);
    
    fs::path modPath(modulePath);
    
    // 如果是.cmod文件，需要先解包到临时目录
    if (modPath.extension() == ".cmod") {
        CMODHandler cmodHandler;
        
        // 创建临时目录
        fs::path tempDir = fs::temp_directory_path() / ("chtl_cmod_" + std::to_string(std::hash<std::string>{}(modulePath)));
        fs::create_directories(tempDir);
        
        // 解包模块
        if (!cmodHandler.UnpackModule(modPath, tempDir)) {
            LOG_ERROR("解包CMOD模块失败: " + modulePath);
            return false;
        }
        
        // 更新模块路径到解包后的目录
        modPath = tempDir;
    }
    
    // 检查模块结构
    fs::path srcDir = modPath / "src";
    fs::path infoDir = modPath / "info";
    
    if (!fs::exists(srcDir) || !fs::exists(infoDir)) {
        LOG_ERROR("无效的CMOD模块结构: " + modPath.string());
        return false;
    }
    
    // 导入src目录下的所有.chtl文件
    for (const auto& entry : fs::recursive_directory_iterator(srcDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
            LOG_INFO("导入CMOD文件: " + entry.path().string());
            
            // 使用LoadAndMergeFile导入文件
            LoadAndMergeFile(entry.path().string(), 
                           static_cast<int>(ImportNode::ImportType::Chtl),
                           "", namespaceName);
        }
    }
    
    return true;
}

} // namespace CHTL