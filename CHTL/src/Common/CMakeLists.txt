# Common库
add_library(SyntaxConstraint STATIC
    SyntaxConstraint.cpp
)

target_include_directories(SyntaxConstraint PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)