cmake_minimum_required(VERSION 3.10)

project(sudoku)
set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(3RD_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
add_library(sudoku STATIC ${SOURCES})


Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

add_executable(runTests ${CMAKE_CURRENT_LIST_DIR}/tests/tests.cpp)
target_link_libraries(runTests sudoku)
target_link_libraries(runTests Catch2::Catch2)
target_include_directories(runTests PRIVATE ${SRC_DIR})

add_custom_command(TARGET runTests PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/tests/data $<TARGET_FILE_DIR:runTests>/testdata)
