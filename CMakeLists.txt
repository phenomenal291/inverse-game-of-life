cmake_minimum_required(VERSION 3.10)
project(gameoflife LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/GameOfLife.cpp
    src/Bitboard.cpp
    src/Agents.cpp
)

add_executable(gameoflife src/main.cpp ${SOURCES})

target_link_libraries(gameoflife sfml-graphics sfml-window sfml-system)
