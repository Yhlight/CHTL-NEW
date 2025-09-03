# CHTL Compiler Professional Build Configuration
cmake_minimum_required(VERSION 3.16)

project(CHTL_Compiler 
    VERSION 1.0.0
    DESCRIPTION "CHTL (Chtholly HyperText Language) Compiler"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific configurations
if(WIN32)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)
elseif(UNIX)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    else()
        add_compile_options(-O0 -g)
    endif()
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(src)

# Core source files
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
    src/CompilerDispatcher/CompilerDispatcher_Simple.cpp
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

add_library(CHTLConstraint STATIC ${CONSTRAINT_SOURCES})
target_include_directories(CHTLConstraint PUBLIC src/ConstraintSystem)
target_compile_features(CHTLConstraint PUBLIC cxx_std_17)

add_library(CHTLImport STATIC ${IMPORT_SOURCES})
target_include_directories(CHTLImport PUBLIC src/CHTL/CHTLSystem/ImportSystem)
target_compile_features(CHTLImport PUBLIC cxx_std_17)

# Dispatcher library
add_library(CHTLDispatcher STATIC ${DISPATCHER_SOURCES})
target_include_directories(CHTLDispatcher PUBLIC src/CompilerDispatcher)
target_compile_features(CHTLDispatcher PUBLIC cxx_std_17)
target_link_libraries(CHTLDispatcher 
    CJMOD 
    CHTLCore 
    CHTLJSCore 
    CHTLScanner 
    CHTLConstraint 
    CHTLImport 
    CHTLCodeMerger
)

# Main executable
add_executable(chtl src/main.cpp)
target_compile_features(chtl PUBLIC cxx_std_17)
target_link_libraries(chtl 
    CHTLDispatcher 
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
    add_executable(chtl_cjmod_test src/Test/CJMODCompleteTest.cpp)
    target_link_libraries(chtl_cjmod_test CJMOD CHTLJSCore CHTLScanner)
    
    add_executable(chtl_official_module_test src/Test/OfficialModuleCompleteTest.cpp)
    target_link_libraries(chtl_official_module_test CJMOD CHTLUtilSystem CHTLJSCore CHTLScanner)
endif()

# Installation
install(TARGETS chtl 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

message(STATUS "=== CHTL Compiler Build Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "===============================================")