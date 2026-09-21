cmake_minimum_required(VERSION 3.20)
project(snake VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Quality gate (used by .github/workflows/lint.yml, off by default):
#   -DSNAKE_CLANG_TIDY=ON  run clang-tidy during the build; .clang-tidy sets
#   WarningsAsErrors, so any hit fails the build. Requires clang-tidy installed.
option(SNAKE_CLANG_TIDY "Run clang-tidy during the build (warnings fail)" OFF)
if(SNAKE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
  # NOTE: --warnings-as-errors must be passed on the command line: clang-tidy
  # gives CLI options (even defaults) precedence over the .clang-tidy file, so
  # WarningsAsErrors set there alone would not fail the build.
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--warnings-as-errors=*")
endif()

add_library(snake_lib src/game.cpp src/bot.cpp)
target_include_directories(snake_lib PUBLIC src)

add_executable(snake src/main.cpp src/term.cpp)
target_link_libraries(snake PRIVATE snake_lib)

# Release knobs (used by .github/workflows/release.yml, off by default):
#   -DSNAKE_STATIC=ON     fully static binary (stdlib-only: no NSS/dlopen use, safe)
#   -DSNAKE_VERSION=v1.2.3  baked into --help/--version (defaults to "dev")
option(SNAKE_STATIC "Statically link the snake binary" OFF)
if(SNAKE_STATIC)
  target_link_options(snake PRIVATE -static)
endif()
set(SNAKE_VERSION "dev" CACHE STRING "Version string baked into the binary")
target_compile_definitions(snake PRIVATE "SNAKE_VERSION=\"${SNAKE_VERSION}\"")

# Tests: dependency-free assert runner
add_executable(snake_tests tests/test_game.cpp)
target_link_libraries(snake_tests PRIVATE snake_lib)

enable_testing()
add_test(NAME game_tests COMMAND snake_tests)
