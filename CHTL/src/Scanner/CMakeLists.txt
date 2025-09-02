# Scanner库
add_library(CHTLUnifiedScanner STATIC
    CHTLUnifiedScanner.cpp
)

target_include_directories(CHTLUnifiedScanner PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 如果需要链接其他库，在这里添加
# target_link_libraries(CHTLUnifiedScanner PUBLIC ...)