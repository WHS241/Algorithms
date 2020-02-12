set(ALGEBRA_ALG_ROOT ./algebra CACHE STRING "Algebraic algorithms source root")

include_directories(SYSTEM
    ${ALGEBRA_ALG_ROOT}
	${ALGEBRA_ALG_ROOT}/include
)

set(ALGEBRA_SOURCES
    ${ALGEBRA_ALG_ROOT}/src/misc-all.cpp
    ${ALGEBRA_ALG_ROOT}/src/algebra/algebra.cpp
)

add_library(algebra ${ALGEBRA_SOURCES})

add_dependencies(algebra structures)