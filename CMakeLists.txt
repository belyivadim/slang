cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME slang)

project(${PROJECT_NAME})

file(GLOB HEADERS "./src/*.hpp")
file(GLOB SOURCES "./src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17 -pedantic-errors -Wall -Wextra -g)
