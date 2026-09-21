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

## Podman build container

```bash
podman build -t snake .   # compiles + runs tests inside the build
podman run --rm -it snake --width 30 --height 16
```

Pushes to `main` (and PRs) trigger the same via `.github/workflows/build.yml`.

Game logic lives in `src/game.{hpp,cpp}` (no I/O); terminal handling in `src/term.*`.
