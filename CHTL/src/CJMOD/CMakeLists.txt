# CJMOD处理库
add_library(CJMODHandler STATIC
    CJMODHandler.cpp
    API.cpp
)

target_include_directories(CJMODHandler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# CJMOD命令行工具
add_executable(cjmod_tool cjmod_tool.cpp)

target_link_libraries(cjmod_tool
    CJMODHandler
)

target_include_directories(cjmod_tool PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 链接动态库加载库
if(UNIX)
    target_link_libraries(cjmod_tool dl)
endif()

# 安装到bin目录
install(TARGETS cjmod_tool
    RUNTIME DESTINATION bin
)