cmake_minimum_required(VERSION 3.25)

SET(CMAKE_CXX_COMPILER "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

project(Sudoku)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(SAT)
add_subdirectory(X_Sudoku)

add_executable(Sudoku main.cpp)
target_link_libraries(Sudoku SAT X_Sudoku)