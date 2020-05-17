# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("Algorithms")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Make sure we have cloned googletest
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive)
#    execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR}/lib/googletest checkout tags/release-1.8.1)
endif()

# Make sure libraries built
include(gtest.cmake)
set(ALGEBRA_ALG_ROOT ${PROJECT_SOURCE_DIR}/algebra CACHE STRING "Algebraic algorithms source root")
set(SEQUENCE_ALGORITHM_ROOT ${PROJECT_SOURCE_DIR}/sequence CACHE STRING "Sequence algorithm source root")
set(DATA_STRUCTURES_ROOT ${PROJECT_SOURCE_DIR}/structures CACHE STRING "Data structures source root")
set(GRAPH_ALG_ROOT ./graph CACHE STRING "Graph algorithms source root")
set(APPROX_ALG_ROOT ./approx CACHE STRING "Approximation algorithms source root")

include_directories(SYSTEM
	${ALGEBRA_ALG_ROOT}/include
	${SEQUENCE_ALGORITHM_ROOT}/include
    ${DATA_STRUCTURES_ROOT}/include
	${GRAPH_ALG_ROOT}/include
	${APPROX_ALG_ROOT}/include
)

set(ALGEBRA_SOURCES
    ${ALGEBRA_ALG_ROOT}/src/misc-all.cpp
    ${ALGEBRA_ALG_ROOT}/src/algebra/algebra.cpp
)
set(SEQUENCE_ALGORITHM_SOURCES
	${SEQUENCE_ALGORITHM_ROOT}/sequence-all.cpp
	${SEQUENCE_ALGORITHM_ROOT}/src/sequence/Huffman.cpp
)
set(DATA_STRUCTURES_SOURCES
    ${DATA_STRUCTURES_ROOT}/structures-all.cpp
    ${DATA_STRUCTURES_ROOT}/src/structures/van_Emde_Boas_tree.cpp
)
set(GRAPH_SOURCES
    ${GRAPH_ALG_ROOT}/src/graph-all.cpp
)
set(APPROX_SOURCES
    ${APPROX_ALG_ROOT}/src/approx/approx-all.cpp
)

set(SOURCE_FILES
        testing/main.cpp
        testing/AlgorithmTest.cpp
        )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_library(algebra ${ALGEBRA_SOURCES})
add_library(sequence ${SEQUENCE_ALGORITHM_SOURCES})
add_library(structures ${DATA_STRUCTURES_SOURCES})
add_library(graph ${GRAPH_SOURCES})
add_library(approx ${APPROX_SOURCES})
add_executable(testing ${SOURCE_FILES})

add_dependencies(approx structures)
add_dependencies(algebra structures)
add_dependencies(graph structures)
add_dependencies(sequence structures)

add_dependencies(testing gtest)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(testing gtest ${CMAKE_THREAD_LIBS_INIT})
else ()
    target_link_libraries(testing gtest pthread)
endif()

add_dependencies(testing structures)
target_link_libraries(testing structures ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(testing algebra)
target_link_libraries(testing algebra ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(testing sequence)
target_link_libraries(testing sequence ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(testing graph)
target_link_libraries(testing graph ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(testing approx)
target_link_libraries(testing approx ${CMAKE_THREAD_LIBS_INIT})
