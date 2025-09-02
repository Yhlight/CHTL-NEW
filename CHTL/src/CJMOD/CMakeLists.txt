# CJMOD处理库
add_library(CJMODHandler STATIC
    CJMODHandler.cpp
)

target_include_directories(CJMODHandler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)