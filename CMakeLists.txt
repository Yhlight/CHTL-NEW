cmake_minimum_required(VERSION 3.16)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# 设置编译器警告
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(src)

# CHTL核心组件源文件
set(CHTL_CORE_SOURCES
    src/CHTL/CHTLLexer/CHTLToken.cpp
    src/CHTL/CHTLLexer/CHTLGlobalMap.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLState/CHTLState.cpp
    src/CHTL/CHTLContext/CHTLContext.cpp
    src/CHTL/CHTLNode/CHTLBaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ConfigNode.cpp
    src/CHTL/CHTLNode/NamespaceNode.cpp
    src/CHTL/CHTLNode/Visitor.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
)

# CHTL JS核心组件源文件（完全独立）
set(CHTLJS_CORE_SOURCES
    "src/CHTL JS/CHTLJSLexer/CHTLJSToken.cpp"
    "src/CHTL JS/CHTLJSLexer/CHTLJSGlobalMap.cpp"
    "src/CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    "src/CHTL JS/CHTLJSState/CHTLJSState.cpp"
    "src/CHTL JS/CHTLJSContext/CHTLJSContext.cpp"
    "src/CHTL JS/CHTLJSNode/CHTLJSBaseNode.cpp"
    "src/CHTL JS/CHTLJSNode/EnhanceSelectorNode.cpp"
    "src/CHTL JS/CHTLJSNode/VirObjectNode.cpp"
    "src/CHTL JS/CHTLJSNode/AnimateNode.cpp"
    "src/CHTL JS/CHTLJSParser/CHTLJSParser.cpp"
    "src/CHTL JS/CHTLJSGenerator/CHTLJSGenerator.cpp"
)

# 扫描器源文件
set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
)

# 调度器源文件
set(DISPATCHER_SOURCES
    src/CompilerDispatcher/CompilerDispatcher.cpp
)

# 约束系统源文件
set(CONSTRAINT_SOURCES
    src/ConstraintSystem/CHTLConstraintValidator.cpp
)

# Import导入系统源文件 (已移动到CHTL/CHTLSystem内部)
set(IMPORT_SOURCES
    src/CHTL/CHTLSystem/ImportSystem/CHTLImportManager.cpp
)

# CSS编译器源文件（包含官方ANTLR生成的代码）
set(CSS_SOURCES
    src/CSS/CSSCompiler.cpp
    src/CSS/generated/css3Lexer.cpp
    src/CSS/generated/css3Parser.cpp
    src/CSS/generated/css3ParserBaseListener.cpp
)

# JavaScript编译器源文件（包含官方ANTLR生成的代码）
set(JS_SOURCES
    src/JS/JavaScriptCompiler.cpp
    src/JS/generated/JavaScriptLexer.cpp
    src/JS/generated/JavaScriptParser.cpp
    src/JS/generated/JavaScriptParserBaseListener.cpp
    src/JS/generated/JavaScriptLexerBase.cpp
    src/JS/generated/JavaScriptParserBase.cpp
)

# CJMOD API源文件 (已移动到CHTL JS/CJMODSystem内部)
set(CJMOD_SOURCES
    "src/CHTL JS/CJMODSystem/CJMODSyntax.cpp"
    "src/CHTL JS/CJMODSystem/CJMODScanner.cpp"
    "src/CHTL JS/CJMODSystem/CJMODGenerator.cpp"
    "src/CHTL JS/CJMODSystem/CJMODIntegration.cpp"
    "src/CHTL JS/CJMODSystem/ChthollyOfficialModule.cpp"
)

# CMOD模块系统源文件 (新增)
set(CMOD_SOURCES
    src/CHTL/CMODSystem/CMODManager.cpp
)

# 代码合并器源文件
set(CODE_MERGER_SOURCES
    src/CodeMerger/CHTLCodeMerger.cpp
)

# 错误处理系统源文件 (新增)
set(ERROR_SOURCES
    src/Error/Error.cpp
    src/Error/ErrorHandler.cpp
)

# 工具模块源文件 (新增)  
set(UTIL_SOURCES
    src/Util/StringUtil/StringUtil.cpp
    src/Util/FileSystem/FileSystem.cpp
)

# 官方模块加载器源文件 (新增)
set(MODULE_SOURCES
    src/Module/ModuleLoader.cpp
)

# 核心组件系统源文件 (新增)
set(CORE_SOURCES
    src/Core/ComponentManager.cpp
    src/Core/CompilerComponentBase.cpp
    src/Core/ServiceLocator.cpp
)

# 模块化组件源文件 (新增)
set(COMPONENT_SOURCES
    src/Components/CHTLCompilerComponent.cpp
    src/Components/ModularCompilerDispatcher.cpp
)

# 创建CHTL核心库
add_library(CHTLCore STATIC ${CHTL_CORE_SOURCES})
add_library(CHTLCMODSystem STATIC ${CMOD_SOURCES})
add_library(CHTLErrorSystem STATIC ${ERROR_SOURCES})
add_library(CHTLUtilSystem STATIC ${UTIL_SOURCES})
add_library(CHTLModuleSystem STATIC ${MODULE_SOURCES})
# 暂时隔离核心组件系统和模块化组件 - 等待后续完善
# add_library(CHTLCoreSystem STATIC ${CORE_SOURCES})
# add_library(CHTLComponentSystem STATIC ${COMPONENT_SOURCES})
target_include_directories(CHTLCore PUBLIC src/CHTL)

# 创建CHTL JS核心库（完全独立）
add_library(CHTLJSCore STATIC ${CHTLJS_CORE_SOURCES})
target_include_directories(CHTLJSCore PUBLIC "src/CHTL JS")

# 创建扫描器库
add_library(CHTLScanner STATIC ${SCANNER_SOURCES})
target_include_directories(CHTLScanner PUBLIC src/Scanner)

# 创建约束系统库
add_library(CHTLConstraint STATIC ${CONSTRAINT_SOURCES})
target_include_directories(CHTLConstraint PUBLIC src/ConstraintSystem)
target_link_libraries(CHTLConstraint CHTLCore CHTLJSCore)

# 创建Import导入系统库
add_library(CHTLImport STATIC ${IMPORT_SOURCES})
target_include_directories(CHTLImport PUBLIC src/ImportSystem)

# 添加ANTLR库路径
link_directories(${CMAKE_SOURCE_DIR}/thirdparty/antlr/lib)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/antlr/include)

# 创建CSS编译器库
add_library(CHTLCSSCompiler STATIC ${CSS_SOURCES})
target_include_directories(CHTLCSSCompiler PUBLIC src/CSS)
target_include_directories(CHTLCSSCompiler PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/antlr/include)
target_link_libraries(CHTLCSSCompiler antlr4-runtime)

# 创建JavaScript编译器库
add_library(CHTLJSCompiler STATIC ${JS_SOURCES})
target_include_directories(CHTLJSCompiler PUBLIC src/JS)
target_include_directories(CHTLJSCompiler PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/antlr/include)
target_link_libraries(CHTLJSCompiler antlr4-runtime)

# 创建CJMOD API库
add_library(CJMOD STATIC ${CJMOD_SOURCES})
target_include_directories(CJMOD PUBLIC "src/CHTL JS/CJMODSystem")

# 创建代码合并器库
add_library(CHTLCodeMerger STATIC ${CODE_MERGER_SOURCES})
target_include_directories(CHTLCodeMerger PUBLIC src/CodeMerger)

# 创建调度器库
add_library(CHTLDispatcher STATIC ${DISPATCHER_SOURCES})
target_include_directories(CHTLDispatcher PUBLIC src/CompilerDispatcher)
target_link_libraries(CHTLDispatcher CJMOD CHTLCore CHTLJSCore CHTLScanner CHTLConstraint CHTLImport CHTLCodeMerger CHTLCSSCompiler CHTLJSCompiler)

# 主程序可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl CHTLDispatcher CJMOD CHTLCodeMerger CHTLCSSCompiler CHTLJSCompiler CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner antlr4-runtime)

# 测试可执行文件
add_executable(chtl_test src/Test/BasicCompilationTest.cpp)
target_link_libraries(chtl_test CHTLDispatcher CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# 简化测试可执行文件
add_executable(chtl_simple_test src/Test/SimpleCompilerTest.cpp)
target_link_libraries(chtl_simple_test CHTLDispatcher CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# 解析器生成器测试可执行文件
add_executable(chtl_parser_test src/Test/ParserGeneratorTest.cpp)
target_link_libraries(chtl_parser_test CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# 架构验证测试可执行文件
add_executable(chtl_architecture_test src/Test/ArchitectureTest.cpp)
target_link_libraries(chtl_architecture_test CHTLDispatcher CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# 约束系统测试可执行文件
add_executable(chtl_constraint_test src/Test/ConstraintTest.cpp)
target_link_libraries(chtl_constraint_test CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# Import导入系统测试可执行文件
add_executable(chtl_import_test src/Test/ImportTest.cpp)
target_link_libraries(chtl_import_test CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)



# 项目结构修正报告可执行文件
add_executable(chtl_structure_fix_report src/Test/ProjectStructureFixReport.cpp)

# 官方模块测试可执行文件
add_executable(chtl_official_module_test src/Test/OfficialModuleTest.cpp)
target_link_libraries(chtl_official_module_test CHTLModuleSystem CHTLCMODSystem CHTLUtilSystem CHTLErrorSystem)

# 文档遵循验证报告可执行文件
add_executable(chtl_document_compliance src/Test/DocumentComplianceReport.cpp)

# 官方ANTLR集成测试可执行文件
add_executable(chtl_official_antlr_test src/Test/OfficialANTLRTest.cpp)

# 最终完成验证报告可执行文件
add_executable(chtl_final_completion_verification src/Test/FinalCompletionVerification.cpp)

# 项目结构优化报告可执行文件
add_executable(chtl_project_optimization_report src/Test/ProjectOptimizationReport.cpp)

# 严格模块文档遵循验证测试可执行文件
add_executable(chtl_strict_module_compliance_test src/Test/StrictModuleComplianceTest.cpp)
target_link_libraries(chtl_strict_module_compliance_test CHTLUtilSystem)

# Module分类结构验证测试可执行文件
add_executable(chtl_module_classification_test src/Test/ModuleClassificationTest.cpp)
target_link_libraries(chtl_module_classification_test CHTLUtilSystem)

# 最终严格文档遵循验证报告可执行文件
add_executable(chtl_final_strict_compliance src/Test/FinalStrictComplianceReport.cpp)

# 仓库清理报告可执行文件
add_executable(chtl_repository_cleanup_report src/Test/RepositoryCleanupReport.cpp)

# 最终完成验证可执行文件
add_executable(chtl_final_completion_report src/Test/FinalCompletionVerification.cpp)

# 最终编译成功报告可执行文件
add_executable(chtl_final_compilation_success src/Test/FinalCompilationSuccessReport.cpp)

# 高级功能完善报告可执行文件
add_executable(chtl_advanced_features_report src/Test/AdvancedFeaturesReport.cpp)

# 项目最终状态报告可执行文件
add_executable(chtl_project_final_status src/Test/ProjectFinalStatus.cpp)

# 词法分析器测试可执行文件
add_executable(chtl_lexer_test src/Test/LexerTest.cpp)
target_link_libraries(chtl_lexer_test CHTLCore CHTLJSCore CHTLScanner)



# 模板系统完善测试可执行文件
add_executable(chtl_template_test src/Test/TemplateSystemTest.cpp)
target_link_libraries(chtl_template_test CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# 自定义系统完善测试可执行文件
add_executable(chtl_custom_test src/Test/CustomSystemTest.cpp)
target_link_libraries(chtl_custom_test CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)




# 原始嵌入和配置系统测试可执行文件
add_executable(chtl_origin_config_test src/Test/OriginConfigTest.cpp)
target_link_libraries(chtl_origin_config_test CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)

# CJMOD API系统测试可执行文件
add_executable(chtl_cjmod_test src/Test/CJMODTest.cpp)
target_link_libraries(chtl_cjmod_test CJMOD CHTLImport CHTLConstraint CHTLCore CHTLJSCore CHTLScanner)




# ANTLR真实集成测试可执行文件
add_executable(chtl_antlr_real_test src/Test/ANTLRRealIntegrationTest.cpp)
target_link_libraries(chtl_antlr_real_test CHTLCSSCompiler CHTLJSCompiler antlr4-runtime)

# ANTLR构建报告可执行文件
add_executable(chtl_antlr_build_report src/Test/ANTLRBuildReport.cpp)

# 代码合并器测试可执行文件
add_executable(chtl_code_merger_test src/Test/CodeMergerTest.cpp)
target_link_libraries(chtl_code_merger_test CHTLCodeMerger)

# 最终系统演示可执行文件
add_executable(chtl_final_system_demo src/Test/FinalSystemDemo.cpp)
target_link_libraries(chtl_final_system_demo CHTLCodeMerger CHTLCSSCompiler CHTLJSCompiler antlr4-runtime)

# 完整编译流程测试可执行文件
add_executable(chtl_complete_flow_test src/Test/CompleteCompilationFlowTest.cpp)
target_link_libraries(chtl_complete_flow_test CHTLDispatcher CHTLCodeMerger CHTLCSSCompiler CHTLJSCompiler antlr4-runtime)

# 项目完成度审计可执行文件
add_executable(chtl_project_audit src/Test/ProjectCompletionAudit.cpp)

# 性能基准测试可执行文件
add_executable(chtl_performance_benchmark src/Test/PerformanceBenchmark.cpp)
target_link_libraries(chtl_performance_benchmark CHTLDispatcher CHTLCodeMerger CHTLCSSCompiler CHTLJSCompiler antlr4-runtime)



# CJMOD真实集成测试可执行文件
add_executable(chtl_cjmod_real_test src/Test/CJMODRealIntegrationTest.cpp)
target_link_libraries(chtl_cjmod_real_test CJMOD CHTLJSCore CHTLScanner)



# 设置调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(CHTL_DEBUG_MODE)
    if(MSVC)
        add_compile_options(/Zi)
    else()
        add_compile_options(-g)
    endif()
endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(TARGETS CHTLCore CHTLJSCore CHTLScanner CHTLDispatcher DESTINATION lib)

# 创建module目录（用于模块管理）
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/CMOD)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/CJMOD)

# 复制示例文件
configure_file(${CMAKE_SOURCE_DIR}/CHTL语法文档.md ${CMAKE_BINARY_DIR}/CHTL语法文档.md COPYONLY)

# 打印配置信息
message(STATUS "CHTL编译器配置完成")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "UTF-8支持: 已启用")
message(STATUS "架构设计: CHTL和CHTL JS完全分离")

# 自定义目标：运行测试
add_custom_target(run_tests
    COMMAND chtl_test
    DEPENDS chtl_test
    COMMENT "运行CHTL编译器基础测试"
)

# 自定义目标：编译示例
add_custom_target(compile_example
    COMMAND chtl example.chtl example.html
    DEPENDS chtl
    COMMENT "编译CHTL示例文件"
)