set(SEQUENCE_ALGORITHM_ROOT ${PROJECT_SOURCE_DIR}/sequence CACHE STRING "Sequence algorithm source root")

include_directories(SYSTEM
    ${SEQUENCE_ALGORITHM_ROOT}
	${SEQUENCE_ALGORITHM_ROOT}/include
)

set(SEQUENCE_ALGORITHM_SOURCES
	${SEQUENCE_ALGORITHM_ROOT}/sequence-all.cpp
	${SEQUENCE_ALGORITHM_ROOT}/src/sequence/Huffman.cpp
)

add_library(sequence ${SEQUENCE_ALGORITHM_SOURCES})
add_dependencies(sequence structures)