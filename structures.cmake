set(DATA_STRUCTURES_ROOT ./structures CACHE STRING "Data structures source root")

include_directories(SYSTEM
    ${DATA_STRUCTURES_ROOT}
)

set(DATA_STRUCTURES_SOURCES
	${DATA_STRUCTURES_ROOT}/structures-all.cpp
	${DATA_STRUCTURES_ROOT}/src/AdjacencyList.cpp
	${DATA_STRUCTURES_ROOT}/src/AdjacencyMatrix.cpp
)

add_library(structures ${DATA_STRUCTURES_SOURCES})