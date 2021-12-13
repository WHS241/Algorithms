project("AlgebraAlgorithms")

include_directories(SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

set (SOURCE_FILES
    src/algebra/algebra.cpp
    )

add_library(algebra ${SOURCE_FILES})
