set(GRAPH_ALG_ROOT ./graph CACHE STRING "Graph algorithms source root")

include_directories(SYSTEM
    ${GRAPH_ALG_ROOT}
	${GRAPH_ALG_ROOT}/include
)

set(INDUCTION_SOURCES
    ${GRAPH_ALG_ROOT}/src/graph-all.cpp
)

add_library(graph ${INDUCTION_SOURCES})
add_dependencies(graph structures)