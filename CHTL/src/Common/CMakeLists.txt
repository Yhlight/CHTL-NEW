# Common库
add_library(SyntaxConstraint STATIC
    SyntaxConstraint.cpp
)

target_include_directories(SyntaxConstraint PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# ImportManager库
add_library(ImportManager STATIC
    ImportManager.cpp
)

target_include_directories(ImportManager PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)