set(APPROX_ALG_ROOT ./approx CACHE STRING "Approximation algorithms source root")

include_directories(SYSTEM
    ${APPROX_ALG_ROOT}
	${APPROX_ALG_ROOT}/include
)

set(APPROX_SOURCES
    ${APPROX_ALG_ROOT}/src/approx/approx-all.cpp
)

add_library(approx ${APPROX_SOURCES})

add_dependencies(approx structures)