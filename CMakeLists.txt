# CHTL Compiler Core-Only Build Configuration
# For testing core functionality without ANTLR dependencies

cmake_minimum_required(VERSION 3.16)

project(CHTL_Compiler_Core 
    VERSION 1.0.0
    DESCRIPTION "CHTL Core Compiler (without ANTLR dependencies)"
    LANGUAGES CXX
)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific configurations
if(WIN32)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g -fsanitize=address)
        add_link_options(-fsanitize=address)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
    add_compile_options(-Wall -Wextra -pedantic)
elseif(APPLE)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(src)

# Core source files (without ANTLR dependencies)
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

set(CJMOD_SOURCES
    "src/CHTL JS/CJMODSystem/CJMODCore.cpp"
    "src/CHTL JS/CJMODSystem/CJMODIntegration.cpp"
    "src/CHTL JS/CJMODSystem/ChthollyOfficialModule.cpp"
)

set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
)

set(DISPATCHER_SOURCES
    src/CompilerDispatcher/CompilerDispatcher_Core.cpp
)

set(CODE_MERGER_SOURCES
    src/CodeMerger/CHTLCodeMerger.cpp
)

set(ERROR_SOURCES
    src/Error/Error.cpp
    src/Error/ErrorHandler.cpp
)

set(UTIL_SOURCES
    src/Util/StringUtil/StringUtil.cpp
    src/Util/FileSystem/FileSystem.cpp
)

# MODULE_SOURCES removed - ModuleLoader not used in core system

set(CMOD_SOURCES
    src/CHTL/CMODSystem/CMODManager.cpp
)

set(CONSTRAINT_SOURCES
    src/ConstraintSystem/CHTLConstraintValidator.cpp
)

set(IMPORT_SOURCES
    src/CHTL/CHTLSystem/ImportSystem/CHTLImportManager.cpp
)

# Create static libraries
add_library(CHTLCore STATIC ${CHTL_CORE_SOURCES})
target_include_directories(CHTLCore PUBLIC src/CHTL)
target_compile_features(CHTLCore PUBLIC cxx_std_17)

add_library(CHTLJSCore STATIC ${CHTLJS_CORE_SOURCES})
target_include_directories(CHTLJSCore PUBLIC "src/CHTL JS")
target_compile_features(CHTLJSCore PUBLIC cxx_std_17)

add_library(CJMOD STATIC ${CJMOD_SOURCES})
target_include_directories(CJMOD PUBLIC "src/CHTL JS/CJMODSystem")
target_compile_features(CJMOD PUBLIC cxx_std_17)

add_library(CHTLScanner STATIC ${SCANNER_SOURCES})
target_include_directories(CHTLScanner PUBLIC src/Scanner)
target_compile_features(CHTLScanner PUBLIC cxx_std_17)

add_library(CHTLCodeMerger STATIC ${CODE_MERGER_SOURCES})
target_include_directories(CHTLCodeMerger PUBLIC src/CodeMerger)
target_compile_features(CHTLCodeMerger PUBLIC cxx_std_17)

add_library(CHTLErrorSystem STATIC ${ERROR_SOURCES})
target_include_directories(CHTLErrorSystem PUBLIC src/Error)
target_compile_features(CHTLErrorSystem PUBLIC cxx_std_17)

add_library(CHTLUtilSystem STATIC ${UTIL_SOURCES})
target_include_directories(CHTLUtilSystem PUBLIC src/Util)
target_compile_features(CHTLUtilSystem PUBLIC cxx_std_17)

# CHTLModuleSystem removed - ModuleLoader not used

add_library(CHTLCMODSystem STATIC ${CMOD_SOURCES})
target_include_directories(CHTLCMODSystem PUBLIC src/CHTL/CMODSystem)
target_compile_features(CHTLCMODSystem PUBLIC cxx_std_17)

add_library(CHTLConstraint STATIC ${CONSTRAINT_SOURCES})
target_include_directories(CHTLConstraint PUBLIC src/ConstraintSystem)
target_compile_features(CHTLConstraint PUBLIC cxx_std_17)

add_library(CHTLImport STATIC ${IMPORT_SOURCES})
target_include_directories(CHTLImport PUBLIC src/CHTL/CHTLSystem/ImportSystem)
target_compile_features(CHTLImport PUBLIC cxx_std_17)

# Main executable (professional version)
add_executable(chtl src/main_professional.cpp)
target_compile_features(chtl PUBLIC cxx_std_17)
target_link_libraries(chtl 
    CJMOD 
    CHTLCodeMerger 
    CHTLImport 
    CHTLConstraint 
    CHTLCore 
    CHTLJSCore 
    CHTLScanner
)

# Test executables (Debug mode)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # CJMOD tests
    add_executable(chtl_cjmod_test src/Test/CJMODCompleteTest.cpp)
    target_link_libraries(chtl_cjmod_test CJMOD CHTLJSCore CHTLScanner)
    
    # add_executable(chtl_integration_test src/Test/FinalCJMODIntegrationTest.cpp)
    # target_link_libraries(chtl_integration_test CJMOD CHTLDispatcher CHTLJSCore CHTLScanner)
    
    # add_executable(chtl_architecture_test src/Test/ArchitectureTest.cpp)
    # target_link_libraries(chtl_architecture_test CHTLDispatcher)
    
    # End-to-end test (simplified)
    add_executable(chtl_end_to_end_test src/Test/EndToEndTestSimplified.cpp)
    target_link_libraries(chtl_end_to_end_test CJMOD CHTLJSCore CHTLScanner)
    
    # Official module tests
    add_executable(chtl_official_module_test src/Test/OfficialModuleCompleteTest.cpp)
    target_link_libraries(chtl_official_module_test CJMOD CHTLUtilSystem CHTLJSCore CHTLScanner)
endif()

# Installation configuration
install(TARGETS chtl 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Build information
message(STATUS "=== CHTL Core Compiler Build Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Note: ANTLR dependencies disabled for core testing")
message(STATUS "===============================================")