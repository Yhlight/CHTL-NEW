cmake_minimum_required(VERSION 3.10)
project(ANTLR4Runtime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集所有源文件
file(GLOB_RECURSE ANTLR4_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp"
)

# 创建静态库
add_library(antlr4-runtime-static STATIC ${ANTLR4_SOURCES})

# 设置包含目录
target_include_directories(antlr4-runtime-static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(antlr4-runtime-static PRIVATE -fPIC)
endif()

# 安装库
install(TARGETS antlr4-runtime-static
    ARCHIVE DESTINATION lib
)