cmake_minimum_required(VERSION 3.20)
project(snake VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(snake_lib src/game.cpp)
target_include_directories(snake_lib PUBLIC src)

add_executable(snake src/main.cpp src/term.cpp)
target_link_libraries(snake PRIVATE snake_lib)

# Tests: dependency-free assert runner
add_executable(snake_tests tests/test_game.cpp)
target_link_libraries(snake_tests PRIVATE snake_lib)

enable_testing()
add_test(NAME game_tests COMMAND snake_tests)
