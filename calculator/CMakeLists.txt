cmake_minimum_required(VERSION 3.5)
project(calc)

if(MSVC)
    # 禁用C4819警告
    add_compile_options(/wd4819)
    # 或者指定源文件编码为UTF-8
    add_compile_options(/utf-8)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/linenoise/include)

set(SRC_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Calculator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Scanner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/strext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/src/linenoise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/src/wcwidth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/src/ConvertUTF.cpp
)

add_executable(calc ${SRC_FILE})