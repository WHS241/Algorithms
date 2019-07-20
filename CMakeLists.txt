# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("Algorithms")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Make sure we have cloned googletest
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR} submodule update --init --recursive)
#    execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR}/lib/googletest checkout tags/release-1.8.1)
endif()

include_directories("./include")

# Make sure libraries built
include(gtest.cmake)
include(induction.cmake)
include(sequence.cmake)
include(structures.cmake)

set(SOURCE_FILES
		testing/generator.cpp
        testing/main.cpp
		testing/misc/AlgebraTest.cpp
		testing/misc/GraphicTest.cpp
		testing/misc/GraphMiscTest.cpp
		testing/misc/FunctionTest.cpp
		testing/structures/BinarySearchTreeTest.cpp
		testing/structures/UnionFindTest.cpp
        )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(testing ${SOURCE_FILES})

add_dependencies(testing gtest)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(testing gtest ${CMAKE_THREAD_LIBS_INIT})
else ()
    target_link_libraries(testing gtest pthread)
endif()

add_dependencies(testing structures)
target_link_libraries(testing structures ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(induction structures)
add_dependencies(testing induction)
target_link_libraries(testing induction ${CMAKE_THREAD_LIBS_INIT})

add_dependencies(testing sequence)
target_link_libraries(testing sequence ${CMAKE_THREAD_LIBS_INIT})