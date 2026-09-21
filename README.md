# snake — C++20 CLI snake

No dependencies beyond a C++20 compiler + CMake. Terminal via POSIX `termios` + ANSI codes (no ncurses).

## Build & run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/snake                    # play
./build/snake --width 30 --height 16 --fps 12
```

## Keys

Arrows or WASD move · `p`/space pause · `r` restart · `q` quit (Ctrl-C too).

## Test

```bash
cmake -B build && cmake --build build -j && ctest --test-dir build --output-on-failure
```

## Quality

Google style + clang-tidy, enforced by `.github/workflows/lint.yml` on push/PR
(tidy warnings fail the build via `WarningsAsErrors`):

```bash
clang-format --dry-run --Werror src/*.cpp src/*.hpp tests/*.cpp
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSNAKE_CLANG_TIDY=ON && cmake --build build -j
```

## Podman build container

```bash
podman build -t snake .   # compiles + runs tests inside the build
podman run --rm -it snake --width 30 --height 16
```

Pushes to `main` (and PRs) trigger the same via `.github/workflows/build.yml`,
which also uploads the built binary as an artifact.

## Release

```bash
git tag v0.1.0 && git push origin v0.1.0   # builds static binary, publishes GitHub Release
```

`--version` prints the tag baked in at configure time (`-DSNAKE_VERSION=...`, default `dev`).
Release binaries are fully static (`-DSNAKE_STATIC=ON`).

Game logic lives in `src/game.{hpp,cpp}` (no I/O); terminal handling in `src/term.*`.
