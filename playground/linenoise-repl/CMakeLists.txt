cmake_minimum_required(VERSION 3.5)
project(repl)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/linenoise/include)

set(SRC_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/src/linenoise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/src/wcwidth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/src/ConvertUTF.cpp
)

add_executable(repl ${SRC_FILE})