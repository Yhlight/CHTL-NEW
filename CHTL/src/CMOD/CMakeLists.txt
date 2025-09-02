# CMOD处理库
add_library(CMODHandler STATIC
    CMODHandler.cpp
)

target_include_directories(CMODHandler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)