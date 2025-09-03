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
    if(MSVC)
        add_compile_options(/utf-8)
    elseif(MINGW)
        add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    endif()
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
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

# ANTLR4 Configuration
set(ANTLR4_ROOT ${CMAKE_SOURCE_DIR}/thirdparty/antlr/antlr4-4.13.2)
set(ANTLR4_RUNTIME_DIR ${ANTLR4_ROOT}/runtime/Cpp)

# Check if ANTLR4 runtime is already built
find_path(ANTLR4_INCLUDE_DIR 
    NAMES antlr4-runtime.h
    PATHS 
        ${ANTLR4_RUNTIME_DIR}/runtime/src
        ${ANTLR4_RUNTIME_DIR}/build/runtime/src
        ${CMAKE_SOURCE_DIR}/thirdparty/antlr/include
    NO_DEFAULT_PATH
)

find_library(ANTLR4_LIBRARY
    NAMES antlr4-runtime antlr4_static libantlr4-runtime.a
    PATHS 
        ${ANTLR4_RUNTIME_DIR}/build/runtime
        ${ANTLR4_RUNTIME_DIR}/dist
        ${CMAKE_SOURCE_DIR}/thirdparty/antlr/lib
    NO_DEFAULT_PATH
)

# Build ANTLR4 runtime if not found
if(NOT ANTLR4_INCLUDE_DIR OR NOT ANTLR4_LIBRARY)
    message(STATUS "Building ANTLR4 C++ Runtime...")
    
    # Configure ANTLR4 runtime build
    execute_process(
        COMMAND ${CMAKE_COMMAND} 
            -B ${ANTLR4_RUNTIME_DIR}/build
            -S ${ANTLR4_RUNTIME_DIR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_STANDARD=17
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DWITH_LIBCXX=OFF
            -DWITH_STATIC_CRT=ON
        WORKING_DIRECTORY ${ANTLR4_RUNTIME_DIR}
        RESULT_VARIABLE ANTLR4_CONFIG_RESULT
        OUTPUT_VARIABLE ANTLR4_CONFIG_OUTPUT
        ERROR_VARIABLE ANTLR4_CONFIG_ERROR
    )
    
    if(ANTLR4_CONFIG_RESULT EQUAL 0)
        # Build ANTLR4 runtime
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build ${ANTLR4_RUNTIME_DIR}/build --config ${CMAKE_BUILD_TYPE} --parallel
            WORKING_DIRECTORY ${ANTLR4_RUNTIME_DIR}
            RESULT_VARIABLE ANTLR4_BUILD_RESULT
            OUTPUT_VARIABLE ANTLR4_BUILD_OUTPUT
            ERROR_VARIABLE ANTLR4_BUILD_ERROR
        )
        
        if(ANTLR4_BUILD_RESULT EQUAL 0)
            message(STATUS "ANTLR4 C++ Runtime built successfully")
            set(ANTLR4_INCLUDE_DIR ${ANTLR4_RUNTIME_DIR}/runtime/src)
            set(ANTLR4_LIBRARY ${ANTLR4_RUNTIME_DIR}/build/runtime/libantlr4-runtime.a)
        else()
            message(WARNING "Failed to build ANTLR4 runtime: ${ANTLR4_BUILD_ERROR}")
        endif()
    else()
        message(WARNING "Failed to configure ANTLR4 runtime: ${ANTLR4_CONFIG_ERROR}")
    endif()
endif()

# Include directories
include_directories(src)

# Add ANTLR4 include directory if found and not disabled
if(ANTLR4_INCLUDE_DIR AND NOT DISABLE_ANTLR)
    include_directories(${ANTLR4_INCLUDE_DIR})
    message(STATUS "ANTLR4 Include Directory: ${ANTLR4_INCLUDE_DIR}")
endif()

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
    "src/CHTL JS/CJMODSystem/CHTLJSFunction.cpp"
)

set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
    src/Scanner/CJMODScannerPlugin.cpp
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

# CSS Compiler (ANTLR-based)
set(CSS_COMPILER_SOURCES
    src/CSS/CSSCompiler.cpp
    src/CSS/generated/css3Lexer.cpp
    src/CSS/generated/css3Parser.cpp
    src/CSS/generated/css3ParserBaseListener.cpp
    src/CSS/generated/css3ParserBaseVisitor.cpp
    src/CSS/generated/css3ParserListener.cpp
    src/CSS/generated/css3ParserVisitor.cpp
)

# JavaScript Compiler (ANTLR-based)  
set(JS_COMPILER_SOURCES
    src/JS/JavaScriptCompiler.cpp
    src/JS/generated/JavaScriptLexer.cpp
    src/JS/generated/JavaScriptParser.cpp
    src/JS/generated/JavaScriptLexerBase.cpp
    src/JS/generated/JavaScriptParserBase.cpp
    src/JS/generated/JavaScriptParserBaseListener.cpp
    src/JS/generated/JavaScriptParserBaseVisitor.cpp
    src/JS/generated/JavaScriptParserListener.cpp
    src/JS/generated/JavaScriptParserVisitor.cpp
)

# Dispatcher sources
set(DISPATCHER_SOURCES
    src/CompilerDispatcher/CompilerDispatcher.cpp
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

# ANTLR-based compilers (only if ANTLR4 is available and not disabled)
if(ANTLR4_INCLUDE_DIR AND ANTLR4_LIBRARY AND NOT DISABLE_ANTLR)
    add_library(CHTLCSSCompiler STATIC ${CSS_COMPILER_SOURCES})
    target_include_directories(CHTLCSSCompiler PUBLIC src/CSS ${ANTLR4_INCLUDE_DIR})
    target_compile_features(CHTLCSSCompiler PUBLIC cxx_std_17)
    target_link_libraries(CHTLCSSCompiler ${ANTLR4_LIBRARY})

    add_library(CHTLJSCompiler STATIC ${JS_COMPILER_SOURCES})
    target_include_directories(CHTLJSCompiler PUBLIC src/JS ${ANTLR4_INCLUDE_DIR})
    target_compile_features(CHTLJSCompiler PUBLIC cxx_std_17)
    target_link_libraries(CHTLJSCompiler ${ANTLR4_LIBRARY})

    # Dispatcher with full ANTLR support
    add_library(CHTLDispatcher STATIC ${DISPATCHER_SOURCES})
    target_include_directories(CHTLDispatcher PUBLIC src/CompilerDispatcher ${ANTLR4_INCLUDE_DIR})
    target_compile_features(CHTLDispatcher PUBLIC cxx_std_17)
    target_link_libraries(CHTLDispatcher 
        CJMOD 
        CHTLCore 
        CHTLJSCore 
        CHTLScanner 
        CHTLConstraint 
        CHTLImport 
        CHTLCodeMerger
        CHTLCSSCompiler
        CHTLJSCompiler
        ${ANTLR4_LIBRARY}
    )
    
    set(CHTL_FULL_SUPPORT ON)
    add_compile_definitions(CHTL_WITH_ANTLR)
    message(STATUS "✅ Full CHTL compiler with ANTLR support enabled")
else()
    # Simplified dispatcher without ANTLR
    add_library(CHTLDispatcher STATIC src/CompilerDispatcher/CompilerDispatcher_Simple.cpp)
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
    
    set(CHTL_FULL_SUPPORT OFF)
    message(STATUS "⚠️  Simplified CHTL compiler without ANTLR support")
endif()

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
    CHTLErrorSystem
    CHTLUtilSystem
)

# Professional main executable
add_executable(chtl-professional src/main_professional.cpp)
target_compile_features(chtl-professional PUBLIC cxx_std_17)
target_link_libraries(chtl-professional 
    CHTLDispatcher 
    CJMOD 
    CHTLCodeMerger 
    CHTLImport 
    CHTLConstraint 
    CHTLCore 
    CHTLJSCore 
    CHTLScanner
    CHTLErrorSystem
    CHTLUtilSystem
)

# Link ANTLR libraries if available
if(CHTL_FULL_SUPPORT)
    target_link_libraries(chtl CHTLCSSCompiler CHTLJSCompiler ${ANTLR4_LIBRARY})
    target_link_libraries(chtl-professional CHTLCSSCompiler CHTLJSCompiler ${ANTLR4_LIBRARY})
    
    # Add threading support for ANTLR
    find_package(Threads REQUIRED)
    target_link_libraries(chtl Threads::Threads)
    target_link_libraries(chtl-professional Threads::Threads)
endif()

# Test executables
add_executable(chtl_minimal_test src/main_minimal_test.cpp)
target_link_libraries(chtl_minimal_test CHTLCore)

add_executable(chtl_safe_test src/main_safe_test.cpp)
target_link_libraries(chtl_safe_test CHTLCore)

add_executable(chtl_cjmod_real_api_test src/Test/CJMODRealAPITest.cpp)
target_link_libraries(chtl_cjmod_real_api_test CJMOD)

add_executable(chtl_cjmod_integration_test src/Test/CJMODComprehensiveIntegrationTest.cpp)
target_link_libraries(chtl_cjmod_integration_test CJMOD CHTLScanner)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(chtl_cjmod_test src/Test/CJMODCompleteTest.cpp)
    target_link_libraries(chtl_cjmod_test CJMOD CHTLJSCore CHTLScanner)
    
    add_executable(chtl_official_module_test src/Test/OfficialModuleCompleteTest.cpp)
    target_link_libraries(chtl_official_module_test CJMOD CHTLUtilSystem CHTLJSCore CHTLScanner)
endif()

# Installation
install(TARGETS chtl chtl-professional
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install modules
install(DIRECTORY ${CMAKE_SOURCE_DIR}/module/
    DESTINATION share/chtl/modules
    PATTERN "*.cmod"
)

# Install documentation
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs/
    DESTINATION share/chtl/docs
    FILES_MATCHING PATTERN "*.md"
)

# Install examples
install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples/
    DESTINATION share/chtl/examples
    FILES_MATCHING PATTERN "*.chtl" PATTERN "*.cjjs" PATTERN "*.html"
)

message(STATUS "=== CHTL Compiler Build Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Full ANTLR Support: ${CHTL_FULL_SUPPORT}")
if(ANTLR4_INCLUDE_DIR)
    message(STATUS "ANTLR4 Include: ${ANTLR4_INCLUDE_DIR}")
endif()
if(ANTLR4_LIBRARY)
    message(STATUS "ANTLR4 Library: ${ANTLR4_LIBRARY}")
endif()
message(STATUS "===============================================")