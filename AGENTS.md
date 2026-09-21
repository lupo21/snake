# AGENTS.md

C++20 CLI snake. Stdlib + POSIX `termios`/ANSI only — no ncurses, no external deps.

## Commands

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
./build/snake --width 20 --height 12 --fps 10
ctest --test-dir build --output-on-failure   # builds `snake_tests`, runs game-logic asserts
podman build -t snake .                      # same compile+test inside container; CI runs this on push/PR
```

## Structure

- `src/game.{hpp,cpp}` — pure logic (`Game`: grid, snake deque, food, score, collisions). No I/O; test via `setFoodForTest` / `setSnakeForTest` hooks.
- `src/term.{hpp,cpp}` — RAII raw mode + non-blocking `pollKey()` (arrows parse `ESC [ A/B/C/D`; WASD mapped to directions).
- `src/main.cpp` — argparse (`--width/--height/--fps`), fixed-timestep loop with score-based speedup (`max(40, 1000/fps - score*2)` ms), ANSI render (`\x1b[H` home-cursor, no full clear per frame).
- `tests/test_game.cpp` — dependency-free assert runner, wired via `add_test`.
- `docs/index.html` — GitHub Pages site source (`main` + `/docs`). Owner enables Pages in repo Settings; agent does not.

## Gotchas

- `RawTerminal` hides cursor / restores termios in dtor — game must exit through it (Ctrl-C arrives as `0x03` → `Key::Quit` since `ISIG` is off).
- `Game::setDirection` ignores 180° reversals vs committed `dir_`, not pending — don't "fix" this.
- Tail cell is walkable on non-eating moves (`checkLen = size - 1`); eating moves check full body.
- `pollKey()` drains all pending keys per frame in `main`; keep that drain loop or fast input drops.
