#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ImportNode.h"

void TestBasicFileImports() {
    std::cout << "\n=== 基础文件导入测试 ===" << std::endl;
    
    // 测试HTML文件导入（必须有as）
    auto htmlImport = CHTL::CHTLImportNodeFactory::CreateHtmlImportNode("index.html", "indexPage");
    
    std::cout << "HTML导入: " << htmlImport->ToString() << std::endl;
    std::cout << "是否有as: " << (htmlImport->HasAs() ? "是" : "否") << std::endl;
    std::cout << "别名: " << htmlImport->GetAliasName() << std::endl;
    std::cout << "应该跳过: " << (htmlImport->ShouldSkipImport() ? "是" : "否") << std::endl;
    std::cout << "应该创建带名原始嵌入节点: " << (htmlImport->ShouldCreateNamedOriginNode() ? "是" : "否") << std::endl;
    
    // 测试CSS文件导入（无as，应该跳过）
    auto styleImportNoAs = CHTL::CHTLImportNodeFactory::CreateStyleImportNode("styles.css", "");
    
    std::cout << "\nCSS导入（无as）: " << styleImportNoAs->ToString() << std::endl;
    std::cout << "应该跳过: " << (styleImportNoAs->ShouldSkipImport() ? "是" : "否") << std::endl;
    
    // 测试JavaScript文件导入（有as）
    auto jsImport = CHTL::CHTLImportNodeFactory::CreateJavaScriptImportNode("utils.js", "utilities");
    
    std::cout << "\nJS导入: " << jsImport->ToString() << std::endl;
    std::cout << "路径类型: " << CHTL::CHTLImportNode::PathTypeToString(jsImport->GetPathType()) << std::endl;
    std::cout << "应该创建带名原始嵌入节点: " << (jsImport->ShouldCreateNamedOriginNode() ? "是" : "否") << std::endl;
}

void TestModuleImports() {
    std::cout << "\n=== 模块导入测试 ===" << std::endl;
    
    // 测试Chtl模块导入
    auto chtlImport = CHTL::CHTLImportNodeFactory::CreateChtlImportNode("Chtholly.Space");
    
    std::cout << "Chtl模块导入: " << chtlImport->ToString() << std::endl;
    std::cout << "导入路径: " << chtlImport->GetImportPath() << std::endl;
    std::cout << "路径类型: " << CHTL::CHTLImportNode::PathTypeToString(chtlImport->GetPathType()) << std::endl;
    
    // 测试CJmod模块导入
    auto cjmodImport = CHTL::CHTLImportNodeFactory::CreateCJmodImportNode("AudioPlayer");
    
    std::cout << "\nCJmod模块导入: " << cjmodImport->ToString() << std::endl;
    std::cout << "导入类型: " << CHTL::CHTLImportNode::ImportTypeToString(cjmodImport->GetImportType()) << std::endl;
}

void TestPreciseImports() {
    std::cout << "\n=== 精确导入测试 ===" << std::endl;
    
    // 测试自定义元素导入
    auto customElementImport = CHTL::CHTLImportNodeFactory::CreateCustomImportNode("@Element", "Box", "components.chtl");
    
    std::cout << "自定义元素导入: " << customElementImport->ToString() << std::endl;
    std::cout << "前缀: " << customElementImport->GetPrefix() << std::endl;
    std::cout << "项目名称: " << customElementImport->GetItemName() << std::endl;
    std::cout << "目标类型: " << CHTL::CHTLImportNode::TargetTypeToString(customElementImport->GetTargetType()) << std::endl;
    
    // 测试模板样式组导入
    auto templateStyleImport = CHTL::CHTLImportNodeFactory::CreateTemplateImportNode("@Style", "DefaultButton", "theme.chtl");
    
    std::cout << "\n模板样式导入: " << templateStyleImport->ToString() << std::endl;
    std::cout << "是否使用全缀名: " << (templateStyleImport->IsFullQualified() ? "是" : "否") << std::endl;
    
    // 测试原始嵌入导入
    auto originImport = CHTL::CHTLImportNodeFactory::CreateOriginImportNode("@Vue", "component", "vue-components.chtl");
    
    std::cout << "\n原始嵌入导入: " << originImport->ToString() << std::endl;
}

void TestBatchImports() {
    std::cout << "\n=== 批量导入测试 ===" << std::endl;
    
    // 测试导入所有模板
    auto batchTemplateImport = CHTL::CHTLImportNodeFactory::CreateBatchTemplateImportNode("templates.chtl");
    
    std::cout << "批量模板导入: " << batchTemplateImport->ToString() << std::endl;
    std::cout << "目标类型: " << CHTL::CHTLImportNode::TargetTypeToString(batchTemplateImport->GetTargetType()) << std::endl;
    
    // 测试导入所有自定义
    auto batchCustomImport = CHTL::CHTLImportNodeFactory::CreateBatchCustomImportNode("custom-components.chtl");
    
    std::cout << "\n批量自定义导入: " << batchCustomImport->ToString() << std::endl;
    
    // 测试配置导入
    auto configImport = CHTL::CHTLImportNodeFactory::CreateConfigImportNode("Basic", "config.chtl");
    
    std::cout << "\n配置导入: " << configImport->ToString() << std::endl;
    std::cout << "配置名称: " << configImport->GetItemName() << std::endl;
}

void TestPathResolver() {
    std::cout << "\n=== 路径解析器测试 ===" << std::endl;
    
    // 测试路径类型分析
    std::vector<std::string> testPaths = {
        "index",                    // 文件名（不带后缀）
        "style.css",                // 具体文件名（带后缀）
        "components/Box.chtl",      // 具体路径（含文件信息）
        "modules/",                 // 具体路径（不含文件信息）
        "path/to/files.*",          // 通配符路径
        "Chtholly.Space"            // 子模块路径
    };
    
    for (const auto& path : testPaths) {
        CHTL::CHTLPathType pathType = CHTL::CHTLPathResolver::AnalyzePathType(path);
        std::cout << "路径 \"" << path << "\": " << CHTL::CHTLImportNode::PathTypeToString(pathType) << std::endl;
    }
    
    // 测试HTML路径解析
    std::cout << "\nHTML路径解析测试:" << std::endl;
    auto htmlPaths = CHTL::CHTLPathResolver::ResolveHtmlPath("index");
    for (const auto& resolvedPath : htmlPaths) {
        std::cout << "  解析路径: " << resolvedPath << std::endl;
    }
    
    // 测试Chtl路径解析
    std::cout << "\nChtl路径解析测试:" << std::endl;
    auto chtlPaths = CHTL::CHTLPathResolver::ResolveChtlPath("Chtholly");
    for (const auto& resolvedPath : chtlPaths) {
        std::cout << "  解析路径: " << resolvedPath << std::endl;
    }
    
    // 测试通配符展开
    std::cout << "\n通配符展开测试:" << std::endl;
    auto wildcardPaths = CHTL::CHTLPathResolver::ExpandWildcardPath("components.*");
    for (const auto& expandedPath : wildcardPaths) {
        std::cout << "  展开路径: " << expandedPath << std::endl;
    }
    
    // 测试路径转换
    std::cout << "\n路径转换测试:" << std::endl;
    std::string dotPath = "Chtholly.Space.Component";
    std::string slashPath = CHTL::CHTLPathResolver::ConvertDotSlashPath(dotPath);
    std::cout << "原路径: " << dotPath << " → 转换后: " << slashPath << std::endl;
}

void TestCircularDependencyDetector() {
    std::cout << "\n=== 循环依赖检测器测试 ===" << std::endl;
    
    // 测试正常导入
    CHTL::CHTLCircularDependencyDetector::PushImport("components.chtl");
    CHTL::CHTLCircularDependencyDetector::PushImport("theme.chtl");
    
    std::cout << "导入components.chtl和theme.chtl" << std::endl;
    std::cout << "components.chtl是否已导入: " << (CHTL::CHTLCircularDependencyDetector::IsAlreadyImported("components.chtl") ? "是" : "否") << std::endl;
    
    // 标记为已导入
    CHTL::CHTLCircularDependencyDetector::MarkAsImported("components.chtl");
    CHTL::CHTLCircularDependencyDetector::MarkAsImported("theme.chtl");
    
    std::cout << "标记后，components.chtl是否已导入: " << (CHTL::CHTLCircularDependencyDetector::IsAlreadyImported("components.chtl") ? "是" : "否") << std::endl;
    
    // 测试循环依赖
    bool hasCircular = CHTL::CHTLCircularDependencyDetector::HasCircularDependency("components.chtl");
    std::cout << "components.chtl是否有循环依赖: " << (hasCircular ? "是" : "否") << std::endl;
    
    // 测试路径别名
    CHTL::CHTLCircularDependencyDetector::RegisterPathAlias("comp", "components.chtl");
    std::string resolvedPath = CHTL::CHTLCircularDependencyDetector::ResolvePath("comp");
    std::cout << "别名 \"comp\" 解析为: " << resolvedPath << std::endl;
    
    // 清理
    CHTL::CHTLCircularDependencyDetector::PopImport();
    CHTL::CHTLCircularDependencyDetector::PopImport();
}

void TestImportValidation() {
    std::cout << "\n=== 导入验证测试 ===" << std::endl;
    
    // 测试as语法要求
    std::vector<CHTL::CHTLImportType> importTypes = {
        CHTL::CHTLImportType::IMPORT_HTML,
        CHTL::CHTLImportType::IMPORT_STYLE,
        CHTL::CHTLImportType::IMPORT_JAVASCRIPT,
        CHTL::CHTLImportType::IMPORT_CHTL,
        CHTL::CHTLImportType::IMPORT_CUSTOM
    };
    
    for (const auto& type : importTypes) {
        bool requiresAs = CHTL::CHTLImportNodeFactory::RequiresAs(type);
        std::cout << CHTL::CHTLImportNode::ImportTypeToString(type) << " 是否需要as: " << (requiresAs ? "是" : "否") << std::endl;
    }
    
    // 测试路径验证
    std::vector<std::string> testPaths = {
        "valid/path.chtl",
        "",                         // 空路径
        "components.*",             // 通配符路径
        "invalid\\path"             // 无效路径
    };
    
    for (const auto& path : testPaths) {
        bool isValid = CHTL::CHTLImportNodeFactory::IsValidImportPath(path);
        std::cout << "路径 \"" << path << "\": " << (isValid ? "有效" : "无效") << std::endl;
    }
    
    // 测试别名验证
    std::vector<std::string> testAliases = {
        "validAlias",
        "_validAlias",
        "123invalid",
        "",                         // 空别名（有效）
        "valid_alias_123"
    };
    
    for (const auto& alias : testAliases) {
        bool isValid = CHTL::CHTLImportNodeFactory::IsValidAliasName(alias);
        std::cout << "别名 \"" << alias << "\": " << (isValid ? "有效" : "无效") << std::endl;
    }
}

void TestComplexImportScenarios() {
    std::cout << "\n=== 复杂导入场景测试 ===" << std::endl;
    
    // 测试通配符导入
    auto wildcardImport = CHTL::CHTLImportNodeFactory::CreateChtlImportNode("components.*");
    
    std::cout << "通配符导入: " << wildcardImport->ToString() << std::endl;
    std::cout << "是否为通配符: " << (wildcardImport->IsWildcard() ? "是" : "否") << std::endl;
    
    // 测试路径分隔符转换
    auto dotPathImport = CHTL::CHTLImportNodeFactory::CreateChtlImportNode("Chtholly.Space.Component");
    std::string convertedPath = CHTL::CHTLPathResolver::ConvertDotSlashPath(dotPathImport->GetImportPath());
    
    std::cout << "\n路径分隔符转换:" << std::endl;
    std::cout << "原路径: " << dotPathImport->GetImportPath() << std::endl;
    std::cout << "转换后: " << convertedPath << std::endl;
    
    // 测试无修饰字符串支持
    auto unquotedImport = CHTL::CHTLImportNodeFactory::CreateChtlImportNode("module-name");
    unquotedImport->SetSupportsUnquotedString(true);
    
    std::cout << "\n无修饰字符串导入: " << unquotedImport->ToString() << std::endl;
    std::cout << "支持无修饰字符串: " << (unquotedImport->SupportsUnquotedString() ? "是" : "否") << std::endl;
}

int main() {
    try {
        std::cout << "CHTL第六阶段导入系统测试开始..." << std::endl;
        
        // 执行第六阶段导入系统测试
        TestBasicFileImports();
        TestModuleImports();
        TestPreciseImports();
        TestBatchImports();
        TestPathResolver();
        TestCircularDependencyDetector();
        TestImportValidation();
        TestComplexImportScenarios();
        
        std::cout << "\n第六阶段导入系统测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}