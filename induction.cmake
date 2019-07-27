set(INDUCTION_ALG_ROOT ./induction CACHE STRING "Induction algorithms source root")

include_directories(SYSTEM
    ${INDUCTION_ALG_ROOT}
	${INDUCTION_ALG_ROOT}/include
)

set(INDUCTION_SOURCES
    ${INDUCTION_ALG_ROOT}/src/misc-all.cpp
    ${INDUCTION_ALG_ROOT}/src/induction/algebra.cpp
)

add_library(induction ${INDUCTION_SOURCES})

add_dependencies(induction structures)