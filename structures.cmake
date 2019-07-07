set(DATA_STRUCTURES_ROOT ./structures CACHE STRING "Data structures source root")

include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/${DATA_STRUCTURES_ROOT}
)

set(DATA_STRUCTURES_SOURCES
	${PROJECT_SOURCE_DIR}/${DATA_STRUCTURES_ROOT}/structures-all.cpp
	${PROJECT_SOURCE_DIR}/${DATA_STRUCTURES_ROOT}/src/AdjacencyList.cpp
	${PROJECT_SOURCE_DIR}/${DATA_STRUCTURES_ROOT}/src/AdjacencyMatrix.cpp
)

add_library(structures ${DATA_STRUCTURES_SOURCES})