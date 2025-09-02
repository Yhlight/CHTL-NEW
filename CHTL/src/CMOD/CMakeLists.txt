# CMOD处理库
add_library(CMODHandler STATIC
    CMODHandler.cpp
)

target_include_directories(CMODHandler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# CMOD命令行工具
add_executable(cmod_tool cmod_tool.cpp)

target_link_libraries(cmod_tool
    CMODHandler
)

target_include_directories(cmod_tool PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 安装到bin目录
install(TARGETS cmod_tool
    RUNTIME DESTINATION bin
)